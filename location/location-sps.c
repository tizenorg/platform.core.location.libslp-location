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

#include "location/location-sps.h"
#include "location/location-marshal.h"
#include "location/location-ielement.h"
#include "location/location-signaling-util.h"
#include "location/location-common-util.h"

typedef struct _LocationSpsPrivate {
	LocationSpsMod* mod;
	gboolean is_started;
	gboolean enabled;
	guint 	 interval;
	LocationPosition *pos;
	LocationVelocity *vel;
	LocationAccuracy *acc;
	GList *boundary_list;
	ZoneStatus zone_status;
	LocationPosition *pos_base;
	LocationVelocity *vel_base;
	LocationAccuracy *acc_info;
	LocationSatellite *sat_info;
} LocationSpsPrivate;

enum {
	PROP_0,
	PROP_METHOD_TYPE,
	PROP_LAST_POSITION,
	PROP_UPDATE_INTERVAL,
	PROP_BOUNDARY,
	PROP_REMOVAL_BOUNDARY,
	PROP_POSITION_BASE,
	PROP_VELOCITY_BASE,
	PROP_ACCURACY_INFO,
	PROP_SATELLITE_INFO,
	PROP_MAX
};

static guint32 signals[LAST_SIGNAL] = {0, };
static GParamSpec *properties[PROP_MAX] = {NULL, };

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), LOCATION_TYPE_SPS, LocationSpsPrivate))

static void location_ielement_interface_init (LocationIElementInterface *iface);

G_DEFINE_TYPE_WITH_CODE (LocationSps, location_sps, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (LOCATION_TYPE_IELEMENT,
                         location_ielement_interface_init));

static void
sps_status_cb (gboolean enabled,
	LocationStatus status,
	gpointer self)
{
	LOCATION_LOGD("sps_status_cb");
	g_return_if_fail(self);
	LocationSpsPrivate* priv = GET_PRIVATE(self);
	enable_signaling(self, signals, &(priv->enabled), enabled, status);
}

static void
sps_position_cb (gboolean enabled,
	LocationPosition *pos,
	LocationAccuracy *acc,
	gpointer self)
{
	LOCATION_LOGD("sps_position_cb");
	g_return_if_fail(self);
	g_return_if_fail(pos);
	g_return_if_fail(acc);
	LocationSpsPrivate* priv = GET_PRIVATE(self);
	enable_signaling(self, signals, &(priv->enabled), enabled, pos->status);

	position_signaling(self, signals, &(priv->enabled), priv->interval, &(priv->pos), &(priv->acc), priv->boundary_list, &(priv->zone_status), enabled, pos, acc);
}

static void
sps_velocity_cb (gboolean enabled,
	LocationVelocity *vel,
	LocationAccuracy *acc,
	gpointer self)
{
	LOCATION_LOGD("sps_velocity_cb");
	g_return_if_fail(self);
	LocationSpsPrivate* priv = GET_PRIVATE(self);
	velocity_signaling(self, signals, &(priv->enabled), priv->interval, &(priv->vel), enabled, vel, acc);
}

static void
location_setting_sps_cb(keynode_t *key,
	gpointer self)
{
	LOCATION_LOGD("location_setting_sps_cb");
	g_return_if_fail(key);
	g_return_if_fail(self);
	LocationSpsPrivate* priv = GET_PRIVATE(self);
	g_return_if_fail (priv->mod);
	g_return_if_fail (priv->mod->handler);
	if (0 == location_setting_get_key_val(key) && priv->mod->ops.stop) {
		LOCATION_LOGD("location stopped by setting");
		priv->mod->ops.stop(priv->mod->handler);
	}
	else if (1 == location_setting_get_key_val(key) && priv->mod->ops.start) {
		LOCATION_LOGD("location resumed by setting");
		priv->mod->ops.start (priv->mod->handler, sps_status_cb, sps_position_cb, sps_velocity_cb, self);
	}
}

static int
location_sps_start (LocationSps *self)
{
	LOCATION_LOGD("location_sps_start");
	LocationSpsPrivate* priv = GET_PRIVATE(self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.start, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.update_data, LOCATION_ERROR_NOT_AVAILABLE);
	setting_retval_if_fail(GPS_ENABLED);
	setting_retval_if_fail(SENSOR_ENABLED);
	if( priv->is_started == TRUE) return LOCATION_ERROR_NONE;

	int ret = priv->mod->ops.start(priv->mod->handler, sps_status_cb, sps_position_cb, sps_velocity_cb, self);
	if(ret == LOCATION_ERROR_NONE){
		priv->is_started = TRUE;
		location_setting_add_notify (GPS_ENABLED, location_setting_sps_cb, self);
		location_setting_add_notify (SENSOR_ENABLED, location_setting_sps_cb, self);
		priv->mod->ops.update_data(priv->mod->handler, priv->pos_base, priv->vel_base, priv->acc_info, priv->sat_info);
	}
	return ret;
}

static int
location_sps_stop (LocationSps *self)
{
	LOCATION_LOGD("location_sps_stop");
	LocationSpsPrivate* priv = GET_PRIVATE(self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.stop, LOCATION_ERROR_NOT_AVAILABLE);
	if (priv->is_started == FALSE) return LOCATION_ERROR_NONE;

	int ret = priv->mod->ops.stop (priv->mod->handler);
	if (ret == LOCATION_ERROR_NONE){
		priv->is_started = FALSE;
		location_setting_ignore_notify (GPS_ENABLED, location_setting_sps_cb);
		location_setting_ignore_notify (SENSOR_ENABLED, location_setting_sps_cb);
	}
	return ret;
}

static void
location_sps_dispose (GObject *gobject)
{
	LOCATION_LOGD("location_sps_dispose");
	G_OBJECT_CLASS (location_sps_parent_class)->dispose (gobject);
}

static void
location_sps_finalize (GObject *gobject)
{
	LOCATION_LOGD("location_sps_finalize");
	LocationSpsPrivate* priv = GET_PRIVATE(gobject);
	module_free(priv->mod, "sps");
	priv->mod = NULL;
	G_OBJECT_CLASS (location_sps_parent_class)->finalize (gobject);
}

static void
location_sps_set_property (GObject *object,
	guint property_id,
	const GValue *value,
	GParamSpec *pspec)
{
	LocationSpsPrivate* priv = GET_PRIVATE(object);

	g_return_if_fail (priv->mod);
	g_return_if_fail (priv->mod->handler);
	LocModSpsOps ops = priv->mod->ops;
	g_return_if_fail (ops.update_data);

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
		case PROP_POSITION_BASE:{
			if(priv->pos_base) location_position_free(priv->pos_base);
			priv->pos_base = (LocationPosition*) g_value_dup_boxed (value);
			LOCATION_LOGD("Set prop>> base position: \t%lf, %lf, %lf, time: %d", priv->pos_base->latitude, priv->pos_base->longitude, priv->pos_base->altitude, priv->pos_base->timestamp);
			if (priv->is_started) ops.update_data (priv->mod->handler, priv->pos_base, priv->vel_base, priv->acc_info, priv->sat_info);
			break;
		}
		case PROP_VELOCITY_BASE:{
			if(priv->vel_base) location_velocity_free(priv->vel_base);
			priv->vel_base = (LocationVelocity*) g_value_dup_boxed (value);
			LOCATION_LOGD("Set prop>> base velocity: \t%lf, %lf, %lf, time: %d", priv->vel_base->speed, priv->vel_base->direction, priv->vel_base->climb, priv->vel_base->timestamp);
			if(priv->is_started) ops.update_data (priv->mod->handler, priv->pos_base, priv->vel_base, priv->acc_info, priv->sat_info);
			break;
		}
		case PROP_ACCURACY_INFO:{
			if(priv->acc_info) location_accuracy_free(priv->acc_info);
			priv->acc_info = (LocationAccuracy*) g_value_dup_boxed (value);
			LOCATION_LOGD("Set prop>> accuracy information: \t%d, %lf, %lf", priv->acc_info->level, priv->acc_info->horizontal_accuracy, priv->acc_info->vertical_accuracy);
			if(priv->is_started) ops.update_data (priv->mod->handler, priv->pos_base, priv->vel_base, priv->acc_info, priv->sat_info);
			break;
		}
		case PROP_SATELLITE_INFO:{
			if(priv->sat_info) location_satellite_free(priv->sat_info);
			priv->sat_info = (LocationSatellite*) g_value_dup_boxed (value);
			LOCATION_LOGD("Set prop>> satellite information: \tNofView:%d, NofUsed:%d", priv->sat_info->num_of_sat_inview, priv->sat_info->num_of_sat_used);
			if(priv->is_started) ops.update_data (priv->mod->handler, priv->pos_base, priv->vel_base, priv->acc_info, priv->sat_info);
			break;
		}
		case PROP_UPDATE_INTERVAL: {
			guint interval = g_value_get_uint(value);
			if(interval > 0) {
				if(interval < LOCATION_UPDATE_INTERVAL_MAX)
					priv->interval = interval;
				else
					priv->interval = (guint)LOCATION_UPDATE_INTERVAL_MAX;

			}
			else
				priv->interval = (guint)LOCATION_UPDATE_INTERVAL_DEFAULT;
			break;
		}
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
location_sps_get_property (GObject *object,
                           guint property_id,
                           GValue *value,
                           GParamSpec *pspec)
{
	LocationSpsPrivate *priv = GET_PRIVATE (object);

	switch (property_id){
		case PROP_METHOD_TYPE:
			g_value_set_int(value, LOCATION_METHOD_SPS);
			break;
		case PROP_LAST_POSITION:
			g_value_set_boxed (value, priv->pos);
			break;
		case PROP_BOUNDARY:
			g_value_set_pointer(value, g_list_first(priv->boundary_list));
			break;
		case PROP_POSITION_BASE:
			g_value_set_boxed (value, priv->pos_base);
			break;
		case PROP_VELOCITY_BASE:
			g_value_set_boxed (value, priv->vel_base);
			break;
		case PROP_ACCURACY_INFO:
			g_value_set_boxed (value, priv->acc_info);
			break;
		case PROP_SATELLITE_INFO:
			g_value_set_boxed (value, priv->sat_info);
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
location_sps_get_position (LocationSps *self,
	LocationPosition **position,
	LocationAccuracy **accuracy)
{
	LOCATION_LOGD("location_sps_get_position");

	LocationSpsPrivate *priv = GET_PRIVATE (self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	setting_retval_if_fail(GPS_ENABLED);
	setting_retval_if_fail(SENSOR_ENABLED);

	LocModSpsOps ops = priv->mod->ops;
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (ops.get_position, LOCATION_ERROR_NOT_AVAILABLE);
	return ops.get_position(priv->mod->handler, position, accuracy);
}

static int
location_sps_get_velocity (LocationSps *self,
	LocationVelocity **velocity,
	LocationAccuracy **accuracy)
{
	LOCATION_LOGD("location_sps_get_velocity");

	LocationSpsPrivate *priv = GET_PRIVATE (self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	setting_retval_if_fail(GPS_ENABLED);
	setting_retval_if_fail(SENSOR_ENABLED);

	LocModSpsOps ops = priv->mod->ops;
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (ops.get_velocity, LOCATION_ERROR_NOT_AVAILABLE);
	return ops.get_velocity(priv->mod->handler, velocity, accuracy);
}

static void
location_ielement_interface_init (LocationIElementInterface *iface)
{
	iface->start = (TYPE_START_FUNC)location_sps_start;
	iface->stop = (TYPE_STOP_FUNC)location_sps_stop;
	iface->get_position = (TYPE_GET_POSITION)location_sps_get_position;
	iface->get_velocity = (TYPE_GET_VELOCITY)location_sps_get_velocity;
}

static void
location_sps_init (LocationSps *self)
{
	LOCATION_LOGD("location_sps_init");
	LocationSpsPrivate* priv = GET_PRIVATE(self);

	priv->mod = (LocationSpsMod*)module_new("sps");
	if(!priv->mod)
		LOCATION_LOGW("module loading failed");

	priv->is_started = FALSE;
	priv->enabled= FALSE;
	priv->interval = LOCATION_UPDATE_INTERVAL_DEFAULT;
	priv->pos = NULL;
	priv->vel = NULL;
	priv->acc = NULL;
	priv->zone_status = ZONE_STATUS_NONE;
	priv->boundary_list = NULL;

	priv->pos_base = NULL;
	priv->vel_base = NULL;
	priv->acc_info = NULL;
	priv->sat_info = NULL;
}

static void
location_sps_class_init (LocationSpsClass *klass)
{
	LOCATION_LOGD("location_sps_class_init");
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->set_property = location_sps_set_property;
	gobject_class->get_property = location_sps_get_property;

	gobject_class->dispose = location_sps_dispose;
	gobject_class->finalize = location_sps_finalize;

	g_type_class_add_private (klass, sizeof (LocationSpsPrivate));

	signals[SERVICE_ENABLED] = g_signal_new ("service-enabled",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST |
			G_SIGNAL_NO_RECURSE,
			G_STRUCT_OFFSET (LocationSpsClass, enabled),
			NULL, NULL,
			location_VOID__UINT,
			G_TYPE_NONE, 1,
			G_TYPE_UINT);

	signals[SERVICE_DISABLED] = g_signal_new ("service-disabled",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST |
			G_SIGNAL_NO_RECURSE,
			G_STRUCT_OFFSET (LocationSpsClass, disabled),
			NULL, NULL,
			location_VOID__UINT,
			G_TYPE_NONE, 1,
			G_TYPE_UINT);

	signals[SERVICE_UPDATED] = g_signal_new ("service-updated",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST |
			G_SIGNAL_NO_RECURSE,
			G_STRUCT_OFFSET (LocationSpsClass, updated),
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
			G_STRUCT_OFFSET (LocationSpsClass, zone_in),
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
			G_STRUCT_OFFSET (LocationSpsClass, zone_out),
			NULL, NULL,
			location_VOID__UINT_POINTER_POINTER,
			G_TYPE_NONE, 3,
			G_TYPE_UINT,
			G_TYPE_POINTER,
			G_TYPE_POINTER);

	properties[PROP_METHOD_TYPE]  = g_param_spec_int ("method",
			"method type",
			"location method type name",
			LOCATION_METHOD_SPS,
			LOCATION_METHOD_SPS,
			LOCATION_METHOD_SPS,
			G_PARAM_READABLE);

	properties[PROP_LAST_POSITION] = g_param_spec_boxed ("last-position",
			"sps last position prop",
			"sps last position data",
			LOCATION_TYPE_POSITION,
			G_PARAM_READABLE);

	properties[PROP_UPDATE_INTERVAL] = g_param_spec_uint ("update-interval",
			"sps update interval prop",
			"sps update interval data",
			LOCATION_UPDATE_INTERVAL_MIN,
			LOCATION_UPDATE_INTERVAL_MAX,
			LOCATION_UPDATE_INTERVAL_DEFAULT,
			G_PARAM_READWRITE);

	properties[PROP_BOUNDARY] = g_param_spec_pointer ("boundary",
			"sps boundary prop",
			"sps boundary data",
			G_PARAM_READWRITE);

	properties[PROP_REMOVAL_BOUNDARY] = g_param_spec_boxed("removal-boundary",
			"sps removal boundary prop",
			"sps removal boundary data",
			LOCATION_TYPE_BOUNDARY,
			G_PARAM_READWRITE);

	properties[PROP_POSITION_BASE] = g_param_spec_boxed ("position-base",
			"sps position base prop",
			"sps position base data",
			LOCATION_TYPE_POSITION,
			G_PARAM_READWRITE);

	properties[PROP_VELOCITY_BASE] = g_param_spec_boxed ("velocity-base",
			"sps velocity base prop",
			"sps velocity base data",
			LOCATION_TYPE_VELOCITY,
			G_PARAM_READWRITE);

	properties[PROP_ACCURACY_INFO] = g_param_spec_boxed ("accuracy-info",
			"sps accuracy information prop",
			"sps accuracy information data",
			LOCATION_TYPE_ACCURACY,
			G_PARAM_READWRITE);

	properties[PROP_SATELLITE_INFO] = g_param_spec_boxed ("satellite-info",
			"sps satellite information prop",
			"sps satellite information data",
			LOCATION_TYPE_SATELLITE,
			G_PARAM_READWRITE);

	g_object_class_install_properties (gobject_class,
			PROP_MAX,
			properties);
}
