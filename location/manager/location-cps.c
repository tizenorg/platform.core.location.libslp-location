/*
 * libslp-location
 *
 * Copyright (c) 2010-2013 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Youngae Kang <youngae.kang@samsung.com>, Minjune Kim <sena06.kim@samsung.com>
 *          Genie Kim <daejins.kim@samsung.com>
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

#include "location-setting.h"
#include "location-log.h"

#include "module-internal.h"

#include "location-cps.h"
#include "location-marshal.h"
#include "location-ielement.h"
#include "location-signaling-util.h"
#include "location-common-util.h"

typedef struct _LocationCpsPrivate {
	LocationCpsMod *mod;
	gboolean is_started;
	gboolean set_noti;
	gboolean enabled;
	guint pos_updated_timestamp;
	guint pos_interval;
	guint vel_updated_timestamp;
	guint vel_interval;
	LocationPosition *pos;
	LocationVelocity *vel;
	LocationAccuracy *acc;
	GList *boundary_list;
	guint pos_timer;
	guint vel_timer;
} LocationCpsPrivate;

enum {
	PROP_0,
	PROP_METHOD_TYPE,
	PROP_POS_INTERVAL,
	PROP_VEL_INTERVAL,
	PROP_BOUNDARY,
	PROP_REMOVAL_BOUNDARY,
	PROP_MAX
};

static guint32 signals[LAST_SIGNAL] = {0, };
static GParamSpec *properties[PROP_MAX] = {NULL, };

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), LOCATION_TYPE_CPS, LocationCpsPrivate))

static void location_ielement_interface_init (LocationIElementInterface *iface);

G_DEFINE_TYPE_WITH_CODE (LocationCps, location_cps, G_TYPE_OBJECT,
			G_IMPLEMENT_INTERFACE (LOCATION_TYPE_IELEMENT,
			location_ielement_interface_init));

static gboolean
_position_timeout_cb (gpointer data)
{
	GObject *object = (GObject *)data;
	LocationCpsPrivate *priv = GET_PRIVATE(object);
	if (!priv) return FALSE;

	LocationPosition *pos = NULL;
	LocationAccuracy *acc = NULL;

	if (priv->pos) {
		pos = location_position_copy(priv->pos);
	} else {
		pos = location_position_new (0, 0.0, 0.0, 0.0, LOCATION_STATUS_NO_FIX);
	}

	if (priv->acc) {
		acc = location_accuracy_copy (priv->acc);
	} else {
		acc = location_accuracy_new (LOCATION_ACCURACY_LEVEL_NONE, 0.0, 0.0);
	}

	LOCATION_LOGD("VELOCITY SERVICE_UPDATED");
	g_signal_emit(object, signals[SERVICE_UPDATED], 0, POSITION_UPDATED, pos, acc);

	location_position_free (pos);
	location_accuracy_free (acc);

	return TRUE;
}

static gboolean
_velocity_timeout_cb (gpointer data)
{
	GObject *object = (GObject *)data;
	LocationCpsPrivate *priv = GET_PRIVATE(object);
	if (!priv) return FALSE;

	LocationVelocity *vel = NULL;
	LocationAccuracy *acc = NULL;

	if (priv->vel) {
		vel = location_velocity_copy(priv->vel);
	} else {
		vel = location_velocity_new (0, 0.0, 0.0, 0.0);
	}

	if (priv->acc) {
		acc = location_accuracy_copy (priv->acc);
	} else {
		acc = location_accuracy_new (LOCATION_ACCURACY_LEVEL_NONE, 0.0, 0.0);
	}

	LOCATION_LOGD("VELOCITY SERVICE_UPDATED");
	g_signal_emit(object, signals[SERVICE_UPDATED], 0, VELOCITY_UPDATED, vel, acc);

	location_velocity_free (vel);
	location_accuracy_free (acc);

	return TRUE;
}

static void
__reset_pos_data_from_priv(LocationCpsPrivate *priv)
{
	LOCATION_LOGD("__reset_pos_data_from_priv");
	g_return_if_fail(priv);

	if (priv->pos) {
		location_position_free(priv->pos);
		priv->pos = NULL;
	}

	if (priv->vel) {
		location_velocity_free(priv->vel);
		priv->vel = NULL;
	}

	if (priv->acc) {
		location_accuracy_free(priv->acc);
		priv->acc = NULL;
	}
}

static void
cps_status_cb (gboolean enabled,
	LocationStatus status,
	gpointer self)
{
	LOCATION_LOGD("cps_status_cb");
	g_return_if_fail(self);
	LocationCpsPrivate* priv = GET_PRIVATE(self);
	enable_signaling(self, signals, &(priv->enabled), enabled, status);
	if (!priv->enabled) {
		if (priv->pos_timer) {
			g_source_remove(g_source_remove(priv->pos_timer));
			priv->pos_timer = 0;
		}
		if (priv->vel_timer) {
			g_source_remove(g_source_remove(priv->vel_timer));
			priv->vel_timer = 0;
		}
	}
}

static void
cps_position_ext_cb (gboolean enabled,
	LocationPosition *pos,
	LocationVelocity *vel,
	LocationAccuracy *acc,
	gpointer self)
{
	LOCATION_LOGD("cps_position_ext_cb");
	g_return_if_fail(self);
	g_return_if_fail(pos);
	g_return_if_fail(vel);
	g_return_if_fail(acc);
	LocationCpsPrivate* priv = GET_PRIVATE(self);

	if (enabled && !priv->enabled) {
		if (!priv->pos_timer) priv->pos_timer = g_timeout_add_seconds (priv->pos_interval, _position_timeout_cb, self);
		if (!priv->vel_timer) priv->vel_timer = g_timeout_add_seconds (priv->vel_interval, _velocity_timeout_cb, self);
	}

	enable_signaling(self, signals, &(priv->enabled), enabled, pos->status);
	position_signaling(self, signals, &(priv->enabled), priv->pos_interval, FALSE, &(priv->pos_updated_timestamp), &(priv->pos), &(priv->acc), priv->boundary_list, pos, acc);
	velocity_signaling(self, signals, &(priv->enabled), priv->vel_interval, FALSE, &(priv->vel_updated_timestamp), &(priv->vel), vel, acc);
}

static void
location_setting_cps_cb (keynode_t *key,
		gpointer self)
{
	LOCATION_LOGD("location_setting_cps_cb");
	g_return_if_fail (key);
	g_return_if_fail (self);

	LocationCpsPrivate *priv = GET_PRIVATE(self);
	g_return_if_fail (priv->mod);
	g_return_if_fail (priv->mod->handler);

	int ret = LOCATION_ERROR_NONE;

	if (location_setting_get_key_val (key) == 0) {
		if (priv->mod->ops.stop && priv->is_started) {
			ret = priv->mod->ops.stop (priv->mod->handler);
			if (ret == LOCATION_ERROR_NONE) {
				priv->is_started = FALSE;
				__reset_pos_data_from_priv(priv);
			}
		}
	} else {
		if (1 == location_setting_get_int (VCONFKEY_LOCATION_NETWORK_ENABLED) && priv->mod->ops.start && !priv->is_started) {
			LOCATION_LOGD("location resumed by setting");
			ret = priv->mod->ops.start (priv->mod->handler, cps_status_cb, cps_position_ext_cb, NULL, self);
			if (ret == LOCATION_ERROR_NONE) {
				priv->is_started = TRUE;
			}
		}
	}
}

static void
location_cps_dispose (GObject *gobject)
{
	LOCATION_LOGD("location_cps_dispose");

	LocationCpsPrivate *priv = GET_PRIVATE(gobject);
	if (priv->set_noti == TRUE) {
		location_setting_ignore_notify (VCONFKEY_LOCATION_NETWORK_ENABLED, location_setting_cps_cb);
		priv->set_noti = FALSE;
	}

	if (priv->pos_timer) {
		g_source_remove (priv->pos_timer);
		priv->pos_timer = 0;
	}

	if (priv->vel_timer) {
		g_source_remove (priv->vel_timer);
		priv->vel_timer = 0;
	}

	G_OBJECT_CLASS (location_cps_parent_class)->dispose (gobject);
}

static void
location_cps_finalize (GObject *gobject)
{
	LOCATION_LOGD("location_cps_finalize");
	LocationCpsPrivate* priv = GET_PRIVATE(gobject);
	module_free(priv->mod, "cps");
	priv->mod = NULL;

	if (priv->boundary_list) {
		g_list_free_full (priv->boundary_list, free_boundary_list);
		priv->boundary_list = NULL;
	}

	if (priv->pos) {
		location_position_free(priv->pos);
		priv->pos = NULL;
	}

	if (priv->vel) {
		location_velocity_free(priv->vel);
		priv->vel = NULL;
	}

	if (priv->acc) {
		location_accuracy_free(priv->acc);
		priv->acc = NULL;
	}

	G_OBJECT_CLASS (location_cps_parent_class)->finalize (gobject);
}

static void
location_cps_set_property (GObject *object,
	guint property_id,
	GValue *value,
	GParamSpec *pspec)
{
	LocationCpsPrivate *priv = GET_PRIVATE(object);

	int ret = 0;

	switch (property_id) {
		case PROP_BOUNDARY:{
			GList *boundary_list = (GList *)g_list_copy(g_value_get_pointer(value));
			ret = set_prop_boundary(&priv->boundary_list, boundary_list);
			if(ret != 0) LOCATION_LOGD("Set boundary. Error[%d]", ret);
			break;
		}
		case PROP_REMOVAL_BOUNDARY: {
			LocationBoundary *req_boundary = (LocationBoundary*) g_value_dup_boxed(value);
			ret = set_prop_removal_boundary(&priv->boundary_list, req_boundary);
			if(ret != 0) LOCATION_LOGD("Set removal boundary. Error[%d]", ret);
			break;
		}
		case PROP_POS_INTERVAL: {
			guint interval = g_value_get_uint (value);

			if (interval > 0) {
				if (interval < LOCATION_UPDATE_INTERVAL_MAX)
					priv->pos_interval = interval;
				else
					priv->pos_interval = (guint) LOCATION_UPDATE_INTERVAL_MAX;
			} else {
				priv->pos_interval = (guint) LOCATION_UPDATE_INTERVAL_DEFAULT;
			}

			if (priv->pos_timer) {
				g_source_remove (priv->pos_timer);
				priv->pos_timer = g_timeout_add_seconds (priv->pos_interval, _position_timeout_cb, object);
			}

			break;
		}

		case PROP_VEL_INTERVAL: {
			guint interval = g_value_get_uint(value);
			if(interval > 0) {
				if(interval < LOCATION_UPDATE_INTERVAL_MAX)
					priv->vel_interval = interval;
				else
					priv->vel_interval = (guint)LOCATION_UPDATE_INTERVAL_MAX;
			} else
				priv->vel_interval = (guint)LOCATION_UPDATE_INTERVAL_DEFAULT;

			if (priv->vel_timer) {
				g_source_remove (priv->vel_timer);
				priv->vel_timer = g_timeout_add_seconds (priv->vel_interval, _velocity_timeout_cb, object);
			}

			break;
		}
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
location_cps_get_property (GObject *object,
	guint property_id,
	GValue *value,
	GParamSpec *pspec)
{
	LocationCpsPrivate *priv = GET_PRIVATE (object);

	switch (property_id){
		case PROP_METHOD_TYPE:
			g_value_set_int(value, LOCATION_METHOD_CPS);
			break;
		case PROP_BOUNDARY:
			g_value_set_pointer(value, g_list_first(priv->boundary_list));
			break;
		case PROP_POS_INTERVAL: {
			g_value_set_uint (value, priv->pos_interval);
			break;
		}
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static int
location_cps_start (LocationCps *self)
{
	LOCATION_LOGD("location_cps_start");

	LocationCpsPrivate* priv = GET_PRIVATE(self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.start, LOCATION_ERROR_NOT_AVAILABLE);

	if (priv->is_started == TRUE) return LOCATION_ERROR_NONE;

	int ret = LOCATION_ERROR_NOT_AVAILABLE;

	if (!location_setting_get_int (VCONFKEY_LOCATION_NETWORK_ENABLED)) {
		ret = LOCATION_ERROR_SETTING_OFF;
	} else {
		ret = priv->mod->ops.start (priv->mod->handler, cps_status_cb, cps_position_ext_cb, NULL, self);
		if (ret == LOCATION_ERROR_NONE) {
			priv->is_started = TRUE;
		}
		else {
			return ret;
		}
	}

	if (priv->set_noti == FALSE) {
		location_setting_add_notify (VCONFKEY_LOCATION_NETWORK_ENABLED, location_setting_cps_cb, self);
		priv->set_noti = TRUE;
	}

	return ret;
}

static int
location_cps_stop (LocationCps *self)
{
	LOCATION_LOGD("location_cps_stop");
	LocationCpsPrivate* priv = GET_PRIVATE(self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.stop, LOCATION_ERROR_NOT_AVAILABLE);

	int ret = LOCATION_ERROR_NONE;

	if (priv->is_started == TRUE) {
		ret = priv->mod->ops.stop (priv->mod->handler);
		if (ret == LOCATION_ERROR_NONE) {
			priv->is_started = FALSE;
			__reset_pos_data_from_priv(priv);
		}
		else {
			LOCATION_LOGD("Failed to stop. Error[%d]", ret);
		}
	}

	if (priv->set_noti == TRUE) {
		location_setting_ignore_notify (VCONFKEY_LOCATION_NETWORK_ENABLED, location_setting_cps_cb);
		priv->set_noti = FALSE;
	}

	return ret;
}

static int
location_cps_get_position (LocationCps *self,
		LocationPosition **position,
		LocationAccuracy **accuracy)
{
	int ret = LOCATION_ERROR_NOT_AVAILABLE;
	LOCATION_LOGD("location_cps_get_position");

	LocationCpsPrivate *priv = GET_PRIVATE (self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);

	setting_retval_if_fail(VCONFKEY_LOCATION_NETWORK_ENABLED);

	if (priv->is_started != TRUE) {
		LOCATION_LOGD("location is not started");
		return LOCATION_ERROR_NOT_AVAILABLE;
	}

	if (priv->pos) {
		*position = location_position_copy (priv->pos);
		if (priv->acc) *accuracy = location_accuracy_copy(priv->acc);
		else *accuracy = location_accuracy_new(LOCATION_ACCURACY_LEVEL_NONE, 0.0, 0.0);
		ret = LOCATION_ERROR_NONE;
	}

	return ret;
}

static int
location_cps_get_position_ext (LocationCps *self,
		LocationPosition **position,
		LocationVelocity **velocity,
		LocationAccuracy **accuracy)
{
	int ret = LOCATION_ERROR_NOT_AVAILABLE;
	LOCATION_LOGD("location_cps_get_position");

	LocationCpsPrivate *priv = GET_PRIVATE (self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);

	setting_retval_if_fail(VCONFKEY_LOCATION_NETWORK_ENABLED);

	if (priv->is_started != TRUE) {
		LOCATION_LOGD("location is not started");
		return LOCATION_ERROR_NOT_AVAILABLE;
	}

	if (priv->pos && priv->vel) {
		*position = location_position_copy (priv->pos);
		*velocity = location_velocity_copy (priv->vel);
		if (priv->acc) *accuracy = location_accuracy_copy(priv->acc);
		else *accuracy = location_accuracy_new(LOCATION_ACCURACY_LEVEL_NONE, 0.0, 0.0);
		ret = LOCATION_ERROR_NONE;
	}

	return ret;
}

static int
location_cps_get_last_position (LocationCps *self,
		LocationPosition **position,
		LocationAccuracy **accuracy)
{
	LOCATION_LOGD("location_cps_get_last_position");
	return LOCATION_ERROR_NOT_SUPPORTED;
}

static int
location_cps_get_last_position_ext (LocationCps *self,
		LocationPosition **position,
		LocationVelocity **velocity,
		LocationAccuracy **accuracy)
{
	LOCATION_LOGD("location_cps_get_last_position_ext");
	return LOCATION_ERROR_NOT_SUPPORTED;
}

static int
location_cps_get_velocity (LocationCps *self,
		LocationVelocity **velocity,
		LocationAccuracy **accuracy)
{
	LOCATION_LOGD("location_cps_get_velocity");
	return LOCATION_ERROR_NOT_SUPPORTED;
}

static int
location_cps_get_last_velocity (LocationCps *self,
		LocationVelocity **velocity,
		LocationAccuracy **accuracy)
{
	LOCATION_LOGD("location_cps_get_last_velocity");
	return LOCATION_ERROR_NOT_SUPPORTED;
}

static int
location_cps_get_satellite (LocationCps *self,
		LocationSatellite **satellite)
{
	LOCATION_LOGD("location_cps_get_satellite");
	return LOCATION_ERROR_NOT_SUPPORTED;
}

static int
location_cps_get_last_satellite (LocationCps *self)
{
	LOCATION_LOGD("location_cps_get_last_satellite");
	return LOCATION_ERROR_NOT_SUPPORTED;
}

static void
location_ielement_interface_init (LocationIElementInterface *iface)
{
	iface->start = (TYPE_START_FUNC) location_cps_start;
	iface->stop = (TYPE_STOP_FUNC) location_cps_stop;
	iface->get_position = (TYPE_GET_POSITION) location_cps_get_position;
	iface->get_position_ext = (TYPE_GET_POSITION_EXT) location_cps_get_position_ext;
	iface->get_last_position = (TYPE_GET_POSITION) location_cps_get_last_position;
	iface->get_last_position_ext = (TYPE_GET_POSITION_EXT) location_cps_get_last_position_ext;
	iface->get_velocity = (TYPE_GET_VELOCITY) location_cps_get_velocity;
	iface->get_last_velocity = (TYPE_GET_VELOCITY)location_cps_get_last_velocity;
	iface->get_satellite = (TYPE_GET_SATELLITE)location_cps_get_satellite;
	iface->get_last_satellite = (TYPE_GET_SATELLITE)location_cps_get_last_satellite;
}

static void
location_cps_init (LocationCps *self)
{
	LOCATION_LOGD("location_cps_init");
	LocationCpsPrivate* priv = GET_PRIVATE(self);

	priv->mod = (LocationCpsMod*)module_new("cps");
	if(!priv->mod) LOCATION_LOGW("module loading failed");

	priv->is_started = FALSE;
	priv->set_noti = FALSE;
	priv->enabled = FALSE;

	priv->pos_interval = LOCATION_UPDATE_INTERVAL_DEFAULT;
	priv->vel_interval = LOCATION_UPDATE_INTERVAL_DEFAULT;

	priv->pos_updated_timestamp = 0;
	priv->vel_updated_timestamp = 0;

	priv->pos = NULL;
	priv->acc = NULL;
	priv->vel = NULL;
	priv->boundary_list = NULL;

	priv->pos_timer = 0;
	priv->vel_timer = 0;
}

static void
location_cps_class_init (LocationCpsClass *klass)
{
	LOCATION_LOGD("location_cps_class_init");
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = location_cps_get_property;
	gobject_class->set_property = location_cps_set_property;

	gobject_class->dispose = location_cps_dispose;
	gobject_class->finalize = location_cps_finalize;

	g_type_class_add_private (klass, sizeof (LocationCpsPrivate));

	signals[SERVICE_ENABLED] = g_signal_new ("service-enabled",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST |
			G_SIGNAL_NO_RECURSE,
			G_STRUCT_OFFSET (LocationCpsClass, enabled),
			NULL, NULL,
			location_VOID__UINT,
			G_TYPE_NONE, 1,
			G_TYPE_UINT);

	signals[SERVICE_DISABLED] = g_signal_new ("service-disabled",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST |
			G_SIGNAL_NO_RECURSE,
			G_STRUCT_OFFSET (LocationCpsClass, disabled),
			NULL, NULL,
			location_VOID__UINT,
			G_TYPE_NONE, 1,
			G_TYPE_UINT);

	signals[SERVICE_UPDATED] = g_signal_new ("service-updated",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST |
			G_SIGNAL_NO_RECURSE,
			G_STRUCT_OFFSET (LocationCpsClass, updated),
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
			G_STRUCT_OFFSET (LocationCpsClass, zone_in),
			NULL, NULL,
			location_VOID__POINTER_POINTER_POINTER,
			G_TYPE_NONE, 3,
			G_TYPE_POINTER,
			G_TYPE_POINTER,
			G_TYPE_POINTER);

	signals[ZONE_OUT] = g_signal_new ("zone-out",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST |
			G_SIGNAL_NO_RECURSE,
			G_STRUCT_OFFSET (LocationCpsClass, zone_out),
			NULL, NULL,
			location_VOID__POINTER_POINTER_POINTER,
			G_TYPE_NONE, 3,
			G_TYPE_POINTER,
			G_TYPE_POINTER,
			G_TYPE_POINTER);

	properties[PROP_METHOD_TYPE] = g_param_spec_int ("method",
                                "method type",
                                "location method type name",
                                LOCATION_METHOD_CPS,
                                LOCATION_METHOD_CPS,
                                LOCATION_METHOD_CPS,
                                G_PARAM_READABLE);

	properties[PROP_POS_INTERVAL] = g_param_spec_uint ("pos-interval",
				"cps position interval prop",
				"cps position interval data",
				LOCATION_UPDATE_INTERVAL_MIN,
				LOCATION_UPDATE_INTERVAL_MAX,
				LOCATION_UPDATE_INTERVAL_DEFAULT,
				G_PARAM_READWRITE);

	properties[PROP_VEL_INTERVAL] = g_param_spec_uint ("vel-interval",
			"cps velocity interval prop",
			"cps velocity interval data",
			LOCATION_UPDATE_INTERVAL_MIN,
			LOCATION_UPDATE_INTERVAL_MAX,
			LOCATION_UPDATE_INTERVAL_DEFAULT,
			G_PARAM_READWRITE);

	properties[PROP_BOUNDARY] = g_param_spec_pointer ("boundary",
			"cps boundary prop",
			"cps boundary data",
			G_PARAM_READWRITE);

	properties[PROP_REMOVAL_BOUNDARY] = g_param_spec_boxed("removal-boundary",
			"cps removal boundary prop",
			"cps removal boundary data",
			LOCATION_TYPE_BOUNDARY,
			G_PARAM_READWRITE);

	g_object_class_install_properties (gobject_class,
			PROP_MAX,
			properties);
}
