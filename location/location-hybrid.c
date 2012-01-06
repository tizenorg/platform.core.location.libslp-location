/*
 * libslp-location
 *
 * Copyright (c) 2010-2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Youngae Kang <youngae.kang@samsung.com>, Yunhan Kim <yhan.kim@samsung.com>,
 *          Genie Kim <daejins.kim@samsung.com>, Minjune Kim <sena06.kim@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "location/location-setting.h"
#include "location/location-log.h"

#include "location/location-module-internal.h"

#include "location/location-hybrid.h"
#include "location/location-marshal.h"
#include "location/location-ielement.h"
#include "location/location-signaling-util.h"
#include "location/location-common-util.h"

#include "location/location-gps.h"
#include "location/location-wps.h"
#include "location/location-sps.h"

typedef struct _LocationHybridPrivate {
	gboolean is_started;
	gboolean gps_enabled;
	gboolean wps_enabled;
	gboolean sps_enabled;
	guint interval;
	LocationObject *gps;
	LocationObject *wps;
	LocationObject *sps;
	gboolean enabled;
	LocationMethod current_method;
	LocationPosition *pos;
	LocationVelocity *vel;
	LocationAccuracy *acc;
	GList* boundary_list;
	ZoneStatus zone_status;

} LocationHybridPrivate;

enum {
	PROP_0,
	PROP_METHOD_TYPE,
	PROP_LAST_POSITION,
	PROP_UPDATE_INTERVAL,
	PROP_BOUNDARY,
	PROP_REMOVAL_BOUNDARY,
	PROP_MAX
};

static guint32 signals[LAST_SIGNAL] = {0, };
static GParamSpec *properties[PROP_MAX] = {NULL, };

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), LOCATION_TYPE_HYBRID, LocationHybridPrivate))

static void location_ielement_interface_init (LocationIElementInterface *iface);

G_DEFINE_TYPE_WITH_CODE (LocationHybrid, location_hybrid, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (LOCATION_TYPE_IELEMENT,
                         location_ielement_interface_init));


static LocationObject*
hybrid_get_update_method_obj (LocationHybridPrivate* priv)
{
	if(!priv->sps && !priv->gps && !priv->wps) return NULL;

	LocationObject* obj = NULL;
	if (priv->sps_enabled) {
		LOCATION_LOGW("Current method is SPS");
		priv->current_method = LOCATION_METHOD_SPS;
		obj = priv->sps;
	} else if (priv->gps_enabled) {
		LOCATION_LOGW("Current method is GPS");
		priv->current_method = LOCATION_METHOD_GPS;
		obj = priv->gps;
	} else if (priv->wps_enabled) {
		LOCATION_LOGW("Current method is WPS");
		priv->current_method = LOCATION_METHOD_WPS;
		obj = priv->wps;
	} else {
		LOCATION_LOGW("No current method available");
		priv->current_method = LOCATION_METHOD_HYBRID;
		obj = NULL;
	}
	return obj;
}

static gboolean
hyrid_is_equal_g_type_method(GType g_type, LocationMethod method)
{
	if (g_type == LOCATION_TYPE_SPS && method == LOCATION_METHOD_SPS) return TRUE;
	if (g_type == LOCATION_TYPE_GPS && method == LOCATION_METHOD_GPS) return TRUE;
	if (g_type == LOCATION_TYPE_WPS && method == LOCATION_METHOD_WPS) return TRUE;
	return FALSE;
}

static void
hybrid_update_sps(LocationHybrid* self,
	guint type,
	gpointer data,
	gpointer accuracy)
{
	LocationHybridPrivate* priv = GET_PRIVATE(self);
	if (!priv->sps) return;
	LOCATION_LOGD ("hybrid_update_sps");

	switch (type) {
		case POSITION_UPDATED: {
			if (data) g_object_set(priv->sps, "position-base", data, NULL);
			if (priv->gps) {
				LocationSatellite* sat = NULL;
				g_object_get(priv->gps, "satellite", &sat, NULL);
				if (sat) {
					g_object_set(priv->sps, "satellite-info", sat, NULL);
					location_satellite_free (sat);
				}
			}
			if (accuracy) g_object_set(priv->sps, "accuracy-info", accuracy, NULL);
			break;
		}
		case VELOCITY_UPDATED:
			if (data) g_object_set(priv->sps, "velocity-base", data, NULL);
			if (accuracy) g_object_set(priv->sps, "accuracy-info", accuracy, NULL);
			break;
	}
}

static void
hybrid_service_updated (GObject *obj,
	guint type,
	gpointer data,
	gpointer accuracy,
	gpointer self)
{
	LOCATION_LOGD ("hybrid_service_updated");
	LocationHybridPrivate* priv = GET_PRIVATE((LocationHybrid*)self);
	GType g_type = G_TYPE_FROM_INSTANCE(obj);
	if (g_type == LOCATION_TYPE_GPS) hybrid_update_sps((LocationHybrid*)self, type, data, accuracy);
	if (hyrid_is_equal_g_type_method(g_type, priv->current_method)) {
		LocationAccuracy *acc = (LocationAccuracy*)accuracy;
		if (type == POSITION_UPDATED) {
			LocationPosition *pos = (LocationPosition*)data;
			position_signaling(self, signals, &(priv->enabled), priv->interval, &(priv->pos), &(priv->acc), priv->boundary_list, &(priv->zone_status), TRUE, pos, acc);
		} else if (type == VELOCITY_UPDATED) {
			LocationVelocity *vel = (LocationVelocity*) data;
			velocity_signaling(self, signals, &(priv->enabled), priv->interval, &(priv->vel), TRUE, vel, acc);
		} else LOCATION_LOGW ("Undefined GType updated");
	}

}

static void
hybrid_service_enabled (GObject *obj,
	guint status,
	gpointer self)
{
	LOCATION_LOGD ("hybrid_service_enabled");
	LocationHybridPrivate* priv = GET_PRIVATE((LocationHybrid*)self);
	GType g_type = G_TYPE_FROM_INSTANCE(obj);
	if (g_type == LOCATION_TYPE_SPS)     priv->sps_enabled = TRUE;
	else if(g_type == LOCATION_TYPE_GPS) priv->gps_enabled = TRUE;
	else if(g_type == LOCATION_TYPE_WPS) priv->wps_enabled = TRUE;
	else {
		LOCATION_LOGW("Undefined GType enabled");
		return;
	}
	hybrid_get_update_method_obj(priv);
	if(priv->sps_enabled || priv->gps_enabled || priv->wps_enabled)
		enable_signaling(self, signals, &(priv->enabled), TRUE, status);
}

static void
hybrid_service_disabled (GObject *obj,
	guint status,
	gpointer self)
{
	LOCATION_LOGD ("hybrid_service_disabled");
	LocationHybridPrivate* priv = GET_PRIVATE((LocationHybrid*)self);
	GType g_type = G_TYPE_FROM_INSTANCE(obj);
	if (g_type == LOCATION_TYPE_SPS)     priv->sps_enabled = FALSE;
	else if(g_type == LOCATION_TYPE_GPS) priv->gps_enabled = FALSE;
	else if(g_type == LOCATION_TYPE_WPS) priv->wps_enabled = FALSE;
	else {
		LOCATION_LOGW("Undefined GType disabled");
		return;
	}
	hybrid_get_update_method_obj(priv);
	if(!priv->sps_enabled && !priv->gps_enabled && !priv->wps_enabled)
		enable_signaling(self, signals, &(priv->enabled), FALSE, status);
}

static int
location_hybrid_start (LocationHybrid *self)
{
	LOCATION_LOGD("location_hybrid_start");
	setting_retval_if_fail(GPS_ENABLED);

	int ret_gps = LOCATION_ERROR_NONE;
	int ret_wps = LOCATION_ERROR_NONE;
	int ret_sps = LOCATION_ERROR_NONE;

	LocationHybridPrivate* priv = GET_PRIVATE(self);
	if (priv->is_started == TRUE)
		return LOCATION_ERROR_NONE;

	if(priv->gps) ret_gps = location_start(priv->gps);
	if(priv->wps) ret_wps = location_start(priv->wps);
	if(priv->sps) ret_sps = location_start(priv->sps);

	if (ret_gps != LOCATION_ERROR_NONE &&
		ret_wps != LOCATION_ERROR_NONE &&
		ret_sps != LOCATION_ERROR_NONE)
		return LOCATION_ERROR_NOT_AVAILABLE;

	priv->is_started = TRUE;
	return LOCATION_ERROR_NONE;
}

static int
location_hybrid_stop (LocationHybrid *self)
{
	LOCATION_LOGD("location_hybrid_stop");

	LocationHybridPrivate* priv = GET_PRIVATE(self);
	if( priv->is_started == FALSE)
		return LOCATION_ERROR_NONE;

	int ret_gps = LOCATION_ERROR_NONE;
	int ret_wps = LOCATION_ERROR_NONE;
	int ret_sps = LOCATION_ERROR_NONE;

	if(priv->gps) ret_gps = location_stop(priv->gps);
	if(priv->wps) ret_wps = location_stop(priv->wps);
	if(priv->sps) ret_sps = location_stop(priv->sps);

	if (ret_gps != LOCATION_ERROR_NONE &&
		ret_wps != LOCATION_ERROR_NONE &&
		ret_sps != LOCATION_ERROR_NONE)
		return LOCATION_ERROR_NOT_AVAILABLE;

	priv->is_started = FALSE;
	return LOCATION_ERROR_NONE;
}

static void
location_hybrid_dispose (GObject *gobject)
{
	LOCATION_LOGD("location_hybrid_dispose");
	G_OBJECT_CLASS (location_hybrid_parent_class)->dispose (gobject);
}

static void
location_hybrid_finalize (GObject *gobject)
{
	LOCATION_LOGD("location_hybrid_finalize");
	LocationHybridPrivate* priv = GET_PRIVATE(gobject);

	if (priv->gps) {
		g_signal_handlers_disconnect_by_func(priv->gps, G_CALLBACK (hybrid_service_enabled), gobject);
		g_signal_handlers_disconnect_by_func(priv->gps, G_CALLBACK (hybrid_service_disabled), gobject);
		g_signal_handlers_disconnect_by_func(priv->gps, G_CALLBACK (hybrid_service_updated), gobject);
		location_free(priv->gps);
	}
	if (priv->wps) {
		g_signal_handlers_disconnect_by_func(priv->wps, G_CALLBACK (hybrid_service_enabled), gobject);
		g_signal_handlers_disconnect_by_func(priv->wps, G_CALLBACK (hybrid_service_disabled), gobject);
		g_signal_handlers_disconnect_by_func(priv->wps, G_CALLBACK (hybrid_service_updated), gobject);
		location_free(priv->wps);
	}
	if (priv->sps) {
		g_signal_handlers_disconnect_by_func(priv->sps, G_CALLBACK (hybrid_service_enabled), gobject);
		g_signal_handlers_disconnect_by_func(priv->sps, G_CALLBACK (hybrid_service_disabled), gobject);
		g_signal_handlers_disconnect_by_func(priv->sps, G_CALLBACK (hybrid_service_updated), gobject);
		location_free(priv->sps);
	}

	G_OBJECT_CLASS (location_hybrid_parent_class)->finalize (gobject);
}

static void
location_hybrid_set_property (GObject *object,
	guint property_id,
	const GValue *value,
	GParamSpec *pspec)
{
	LocationHybridPrivate* priv = GET_PRIVATE(object);
	if (!priv->gps && !priv->wps && !priv->sps) {
		LOCATION_LOGW("Set property is not available now");
		return;
	}

	int ret = 0;
	switch (property_id){
		case PROP_BOUNDARY:{
			GList *boundary_list = (GList *)g_list_copy(g_value_get_pointer(value));
			ret = set_prop_boundary(&priv->boundary_list, boundary_list);
			if(ret != 0) LOCATION_LOGD("Set boundary. Error[%d]", ret);
		   break;
		}
		case PROP_REMOVAL_BOUNDARY: {
			LocationBoundary *req_boundary = (LocationBoundary*) g_value_dup_boxed(value);
			ret = set_prop_removal_boundary(&priv->boundary_list, req_boundary);
			if(ret != 0) LOCATION_LOGD("Removal boundary. Error[%d]", ret);
			break;
		}
		case PROP_UPDATE_INTERVAL: {
			guint interval = g_value_get_uint(value);
			if(interval > 0) {
				if(interval < LOCATION_UPDATE_INTERVAL_MAX)
					priv->interval = interval;
				else
					priv->interval = (guint) LOCATION_UPDATE_INTERVAL_MAX;

			}
			else
				priv->interval = LOCATION_UPDATE_INTERVAL_DEFAULT;
			break;
		}
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
location_hybrid_get_property (GObject *object,
	guint property_id,
	GValue *value,
	GParamSpec *pspec)
{
	LocationHybridPrivate *priv = GET_PRIVATE (object);
	if(!priv->gps && !priv->wps && !priv->sps){
		LOCATION_LOGW("Get property is not available now");
		return;
	}

	LOCATION_LOGW("Get Propery ID[%d]", property_id);

	switch (property_id){
	case PROP_METHOD_TYPE:
		g_value_set_int(value, priv->current_method);
		break;
	case PROP_LAST_POSITION:
		g_value_set_boxed(value, priv->pos);
		break;
	case PROP_BOUNDARY:
		g_value_set_pointer(value, g_list_first(priv->boundary_list));
		break;
	case PROP_UPDATE_INTERVAL:
		g_value_set_uint(value, priv->interval);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

static int
location_hybrid_get_position (LocationHybrid *self,
	LocationPosition **position,
	LocationAccuracy **accuracy)
{
	LOCATION_LOGD("location_hybrid_get_position");
	setting_retval_if_fail(GPS_ENABLED);

	LocationHybridPrivate *priv = GET_PRIVATE (self);
	LocationObject *obj = hybrid_get_update_method_obj(priv);
	if(!obj) return LOCATION_ERROR_NOT_AVAILABLE;
	return location_get_position (obj, position, accuracy);
}

static int
location_hybrid_get_velocity (LocationHybrid *self,
	LocationVelocity **velocity,
	LocationAccuracy **accuracy)
{
	LOCATION_LOGD("location_hybrid_get_velocity");
	setting_retval_if_fail(GPS_ENABLED);

	LocationHybridPrivate *priv = GET_PRIVATE (self);
	LocationObject *obj = hybrid_get_update_method_obj(priv);
	if(!obj) return LOCATION_ERROR_NOT_AVAILABLE;
	return location_get_velocity (obj, velocity, accuracy);
}

static void
location_ielement_interface_init (LocationIElementInterface *iface)
{
	iface->start = (TYPE_START_FUNC)location_hybrid_start;
	iface->stop = (TYPE_STOP_FUNC)location_hybrid_stop;
	iface->get_position = (TYPE_GET_POSITION)location_hybrid_get_position;
	iface->get_velocity = (TYPE_GET_VELOCITY)location_hybrid_get_velocity;
}

static void
location_hybrid_init (LocationHybrid *self)
{
	LOCATION_LOGD("location_hybrid_init");
	LocationHybridPrivate* priv = GET_PRIVATE(self);

	priv->is_started = FALSE;
	priv->interval = LOCATION_UPDATE_INTERVAL_DEFAULT;
	priv->gps_enabled = FALSE;
	priv->wps_enabled = FALSE;
	priv->sps_enabled = FALSE;
	priv->gps = NULL;
	priv->wps = NULL;
	priv->sps = NULL;

	if(location_is_supported_method(LOCATION_METHOD_GPS)) priv->gps = location_new (LOCATION_METHOD_GPS);
	if(location_is_supported_method(LOCATION_METHOD_WPS)) priv->wps = location_new (LOCATION_METHOD_WPS);
	if(location_is_supported_method(LOCATION_METHOD_SPS)) priv->sps = location_new (LOCATION_METHOD_SPS);

	if(priv->gps){
		g_signal_connect (priv->gps, "service-enabled", G_CALLBACK(hybrid_service_enabled), self);
		g_signal_connect (priv->gps, "service-disabled", G_CALLBACK(hybrid_service_disabled), self);
		g_signal_connect (priv->gps, "service-updated", G_CALLBACK(hybrid_service_updated), self);
	}
	if(priv->wps){
		g_signal_connect (priv->wps, "service-enabled", G_CALLBACK(hybrid_service_enabled), self);
		g_signal_connect (priv->wps, "service-disabled", G_CALLBACK(hybrid_service_disabled), self);
		g_signal_connect (priv->wps, "service-updated", G_CALLBACK(hybrid_service_updated), self);
	}
	if(priv->sps){
		g_signal_connect (priv->sps, "service-enabled", G_CALLBACK(hybrid_service_enabled), self);
		g_signal_connect (priv->sps, "service-disabled", G_CALLBACK(hybrid_service_disabled), self);
		g_signal_connect (priv->sps, "service-updated", G_CALLBACK(hybrid_service_updated), self);
	}

	priv->current_method = LOCATION_METHOD_HYBRID;
	priv->enabled= FALSE;

	priv->pos = NULL;
	priv->vel = NULL;
	priv->acc = NULL;
	priv->zone_status = ZONE_STATUS_NONE;
	priv->boundary_list = NULL;
}

static void
location_hybrid_class_init (LocationHybridClass *klass)
{
	LOCATION_LOGD("location_hybrid_class_init");
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->set_property = location_hybrid_set_property;
	gobject_class->get_property = location_hybrid_get_property;

	gobject_class->dispose = location_hybrid_dispose;
	gobject_class->finalize = location_hybrid_finalize;

	g_type_class_add_private (klass, sizeof (LocationHybridPrivate));

	signals[SERVICE_ENABLED] = g_signal_new ("service-enabled",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST |
			G_SIGNAL_NO_RECURSE,
			G_STRUCT_OFFSET (LocationHybridClass, enabled),
			NULL, NULL,
			location_VOID__UINT,
			G_TYPE_NONE, 1,
			G_TYPE_UINT);

	signals[SERVICE_DISABLED] = g_signal_new ("service-disabled",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST |
			G_SIGNAL_NO_RECURSE,
			G_STRUCT_OFFSET (LocationHybridClass, disabled),
			NULL, NULL,
			location_VOID__UINT,
			G_TYPE_NONE, 1,
			G_TYPE_UINT);

	signals[SERVICE_UPDATED] = g_signal_new ("service-updated",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST |
			G_SIGNAL_NO_RECURSE,
			G_STRUCT_OFFSET (LocationHybridClass, updated),
			NULL, NULL,
			location_VOID__UINT_POINTER_POINTER,
			G_TYPE_NONE, 3,
			G_TYPE_UINT,
			G_TYPE_POINTER,
			G_TYPE_POINTER);

	signals[ZONE_IN] = g_signal_new ("zone-in",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST |
			G_SIGNAL_NO_RECURSE,
			G_STRUCT_OFFSET (LocationHybridClass, zone_in),
			NULL, NULL,
			location_VOID__UINT_POINTER_POINTER,
			G_TYPE_NONE, 3,
			G_TYPE_UINT,
			G_TYPE_POINTER,
			G_TYPE_POINTER);

	signals[ZONE_OUT] = g_signal_new ("zone-out",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST |
			G_SIGNAL_NO_RECURSE,
			G_STRUCT_OFFSET (LocationHybridClass, zone_out),
			NULL, NULL,
			location_VOID__UINT_POINTER_POINTER,
			G_TYPE_NONE, 3,
			G_TYPE_UINT,
			G_TYPE_POINTER,
			G_TYPE_POINTER);

	properties[PROP_METHOD_TYPE] = g_param_spec_int ("method",
			"method type",
			"location method type name",
			LOCATION_METHOD_HYBRID,
			LOCATION_METHOD_HYBRID,
			LOCATION_METHOD_HYBRID,
			G_PARAM_READABLE);

	properties[PROP_LAST_POSITION] = g_param_spec_boxed ("last-position",
			"hybrid last position prop",
			"hybrid last position data",
			LOCATION_TYPE_POSITION,
			G_PARAM_READABLE);

	properties[PROP_UPDATE_INTERVAL] = g_param_spec_uint ("update-interval",
			"update interval prop",
			"update interval data",
			LOCATION_UPDATE_INTERVAL_MIN,
			LOCATION_UPDATE_INTERVAL_MAX,
			LOCATION_UPDATE_INTERVAL_DEFAULT,
			G_PARAM_READWRITE);

	properties[PROP_BOUNDARY]  = g_param_spec_pointer ("boundary",
			"hybrid boundary prop",
			"hybrid boundary data",
			G_PARAM_READWRITE);

	properties[PROP_REMOVAL_BOUNDARY] = g_param_spec_boxed("removal-boundary",
			"hybrid removal boundary prop",
			"hybrid removal boundary data",
			LOCATION_TYPE_BOUNDARY,
			G_PARAM_READWRITE);

	g_object_class_install_properties (gobject_class,
			PROP_MAX,
			properties);

}
