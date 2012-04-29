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

#include <stdio.h>
#include <pthread.h>
#include <dbus/dbus-glib.h>

#include "location.h"
#include "location-log.h"
#include "location-setting.h"
#include "location-ielement.h"
#include "location-hybrid.h"
#include "location-gps.h"
#include "location-wps.h"
#include "location-ips.h"
#include "location-cps.h"
#include "location-sps.h"
#include "location-position.h"
#include "map-service.h"
#include "module-internal.h"
#include "location-common-util.h"

extern LocationObject *g_map_service;

EXPORT_API
int location_init (void)
{
	LOCATION_LOGD("location_init");
	g_type_init ();
	if (!g_thread_supported()) g_thread_init (NULL);
	dbus_g_thread_init ();
	if( FALSE == module_init() )
		return LOCATION_ERROR_NOT_AVAILABLE;

	return LOCATION_ERROR_NONE;
}

EXPORT_API LocationObject*
location_new (LocationMethod method)
{
	LocationObject *self = NULL;

	if (!g_map_service)
		g_map_service = g_object_new (MAP_TYPE_SERVICE, NULL);

	switch (method) {
	case LOCATION_METHOD_HYBRID:
		self = g_object_new (LOCATION_TYPE_HYBRID, NULL);
		break;
	case LOCATION_METHOD_GPS:
		self = g_object_new (LOCATION_TYPE_GPS, NULL);
		break;
	case LOCATION_METHOD_WPS:
		self = g_object_new (LOCATION_TYPE_WPS, NULL);
		break;
	case LOCATION_METHOD_SPS:
		self = g_object_new (LOCATION_TYPE_SPS, NULL);
		break;
	default:
		break;
	}
	return self;
}

EXPORT_API int
location_free (LocationObject *obj)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);

	if (g_map_service) {
		g_object_unref (g_map_service);
		g_map_service = NULL;
	}

	g_object_unref (obj);
	return LOCATION_ERROR_NONE;
}

EXPORT_API int
location_start (LocationObject *obj)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	return location_ielement_start (LOCATION_IELEMENT(obj));
}

EXPORT_API int
location_stop (LocationObject *obj)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	return location_ielement_stop (LOCATION_IELEMENT(obj));
}

EXPORT_API gboolean
location_is_supported_method(LocationMethod method)
{
	gboolean is_supported = FALSE;

	switch(method) {
	case LOCATION_METHOD_HYBRID:
		if(module_is_supported("gps") || module_is_supported("wps") || module_is_supported("sps"))
			is_supported = TRUE;
		break;
	case LOCATION_METHOD_GPS:
		is_supported = module_is_supported("gps");
		break;
	case LOCATION_METHOD_WPS:
		is_supported = module_is_supported("wps");
		break;
	case LOCATION_METHOD_SPS:
		is_supported = module_is_supported("sps");
		break;
	case LOCATION_METHOD_CPS:	/* deprecated */
	case LOCATION_METHOD_IPS:	/* deprecated */
	default:
		break;
	}

	return is_supported;
}

EXPORT_API gboolean
location_is_enabled_gps(LocationObject *obj)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);

	return (gboolean) location_setting_get_int(GPS_ENABLED);
}

EXPORT_API int
location_get_position (LocationObject *obj,
	LocationPosition **position,
	LocationAccuracy **accuracy)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (position, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (accuracy, LOCATION_ERROR_PARAMETER);
	return location_ielement_get_position (LOCATION_IELEMENT(obj), position, accuracy);
}

EXPORT_API int
location_get_last_position (LocationObject *obj,
	LocationPosition **position,
	LocationAccuracy **accuracy)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (position, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (accuracy, LOCATION_ERROR_PARAMETER);
	return location_ielement_get_last_position (LOCATION_IELEMENT(obj), position, accuracy);
}

EXPORT_API int
location_get_last_known_position (LocationObject *obj, LocationMethod method,
	LocationLastPosition *last_position)
{
	return LOCATION_ERROR_NOT_SUPPORTED;
}

EXPORT_API int
location_get_satellite (LocationObject *obj, LocationSatellite **satellite)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (satellite, LOCATION_ERROR_PARAMETER);

	return location_ielement_get_satellite (LOCATION_IELEMENT(obj), satellite);
}

EXPORT_API int
location_get_last_satellite (LocationObject *obj, LocationSatellite **satellite)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (satellite, LOCATION_ERROR_PARAMETER);

	return location_ielement_get_last_satellite (LOCATION_IELEMENT(obj), satellite);
}

EXPORT_API int
location_get_velocity (LocationObject *obj,
	LocationVelocity **velocity,
	LocationAccuracy **accuracy)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (velocity, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (accuracy, LOCATION_ERROR_PARAMETER);

	return location_ielement_get_velocity (LOCATION_IELEMENT(obj), velocity, accuracy);
}

EXPORT_API int
location_get_last_velocity (LocationObject *obj,
	LocationVelocity **velocity,
	LocationAccuracy **accuracy)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (velocity, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (accuracy, LOCATION_ERROR_PARAMETER);

	return location_ielement_get_last_velocity (LOCATION_IELEMENT(obj), velocity, accuracy);
}


EXPORT_API int
location_send_command(const char *cmd)
{
	g_return_val_if_fail (cmd, LOCATION_ERROR_PARAMETER);

	return LOCATION_ERROR_NONE;
}

