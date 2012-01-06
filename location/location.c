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

#include "location/location.h"
#include "location/location-log.h"
#include "location/location-setting.h"
#include "location/location-module-internal.h"
#include "location/location-ielement.h"
#include "location/location-hybrid.h"
#include "location/location-gps.h"
#include "location/location-wps.h"
#include "location/location-ips.h"
#include "location/location-cps.h"
#include "location/location-sps.h"
#include "location/location-geocode.h"
#include "location/location-poi.h"
#include "location/location-position.h"

static LocationObject *geocode = NULL;
static LocationObject *poi = NULL;

static gboolean
is_connected_network()
{
	gboolean is_connected = TRUE;
	int net_state = 0;

	net_state = location_setting_get_int(VCONFKEY_NETWORK_STATUS);

	LOCATION_LOGW("net_state[%d]", net_state);
	if(net_state == VCONFKEY_NETWORK_OFF) {
		is_connected = FALSE;
	}

	return is_connected;

}

EXPORT_API int location_init (void)
{
	LOCATION_LOGD("location_init");
	g_type_init ();
	if( FALSE == module_init() )
		return LOCATION_ERROR_NOT_AVAILABLE;

	return LOCATION_ERROR_NONE;
}

EXPORT_API LocationObject*
location_new (LocationMethod method)
{
	LocationObject *self = NULL;

	if (!geocode)
		geocode = g_object_new (LOCATION_TYPE_GEOCODE, NULL);

	/*if (!poi)
		poi = g_object_new (LOCATION_TYPE_POI, NULL);*/

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
	case LOCATION_METHOD_CPS:
		self = g_object_new (LOCATION_TYPE_CPS, NULL);
		break;
	case LOCATION_METHOD_IPS:
		self = g_object_new (LOCATION_TYPE_IPS, NULL);
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

	//location_ielement_stop(LOCATION_IELEMENT(obj));
	if (geocode) {
		g_object_unref (geocode);
		geocode = NULL;
	}
	/*if (poi) {
		g_object_unref (poi);
		poi = NULL;
	}*/
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
	case LOCATION_METHOD_CPS:
	case LOCATION_METHOD_IPS:
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
	return location_ielement_get_position (LOCATION_IELEMENT(obj), position, accuracy);
}

EXPORT_API int
location_get_last_known_position (LocationObject *obj,
	LocationLastPosition *last_position)
{
	g_return_val_if_fail (last_position, LOCATION_ERROR_PARAMETER);

	int ret = -1;
	int lat, lon, acc;
	char position[MAX_KEY_LENGTH + 1] = {0,};

	snprintf(position, MAX_KEY_LENGTH + 1, "%s", location_setting_get_string(LAST_POSITION));
	if (position[0] != '\0') {
		location_last_position_a2i(position, &lat, &lon);
		LOCATION_LOGD("location_last_position_a2i [%s][%d/%d]", position, lat, lon);
		acc = location_setting_get_int(LAST_ACCURACY);

		last_position->latitude = lat * 0.0001;
		last_position->longitude = lon * 0.0001;
		last_position->accuracy = acc * 0.0001;
		LOCATION_LOGD("last_position [%f/%f]", last_position->latitude, last_position->longitude);
		ret = LOCATION_ERROR_NONE;
	} else {
		LOCATION_LOGD("location_last_position is empty");
		ret = LOCATION_ERROR_UNKNOWN;
	}

	return ret;
}

EXPORT_API int
location_get_position_from_address (LocationObject *obj,
	const LocationAddress *address,
	LocationPosition **position,
	LocationAccuracy **accuracy)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (geocode, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_geocode (LOCATION_IELEMENT(geocode), address, position, accuracy);
}

EXPORT_API int
location_get_position_from_freeformed_address (LocationObject *obj,
	const gchar *address,
	LocationPosition **position,
	LocationAccuracy **accuracy)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (geocode, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_geocode_freeform (LOCATION_IELEMENT(geocode), address, position, accuracy);
}

EXPORT_API int
location_get_velocity (LocationObject *obj,
	LocationVelocity **velocity,
	LocationAccuracy **accuracy)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	return location_ielement_get_velocity (LOCATION_IELEMENT(obj), velocity, accuracy);
}

EXPORT_API int
location_get_address (LocationObject *obj,
	LocationAddress **address,
	LocationAccuracy **accuracy)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (geocode, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	LocationPosition *position = NULL;
	int ret = location_ielement_get_position(LOCATION_IELEMENT (obj), &position, accuracy);
	if (LOCATION_ERROR_NONE != ret) return ret;
	ret = location_ielement_get_reversegeocode (LOCATION_IELEMENT(geocode), position, address, accuracy);
	location_position_free (position);
	return ret;
}

EXPORT_API int
location_get_address_from_position (LocationObject *obj,
	const LocationPosition *position,
	LocationAddress **address,
	LocationAccuracy **accuracy)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (geocode, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_reversegeocode (LOCATION_IELEMENT(geocode), position, address, accuracy);
}

EXPORT_API int
location_get_position_from_address_async (LocationObject *obj,
	const LocationAddress *address,
	LocationPositionCB callback,
	gpointer userdata)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (geocode, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_geocode_async (LOCATION_IELEMENT(geocode), address, callback, userdata);
}


EXPORT_API int
location_get_position_from_freeformed_address_async (LocationObject *obj,
	const gchar *address,
	LocationPositionCB callback,
	gpointer userdata)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (geocode, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_geocode_freeform_async (LOCATION_IELEMENT(geocode), address, callback, userdata);
}

EXPORT_API int
location_get_address_async (LocationObject *obj,
	LocationAddressCB callback,
	gpointer userdata)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (geocode, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	LocationPosition *position = NULL;
	LocationAccuracy *acc = NULL;
	int ret = location_ielement_get_position(LOCATION_IELEMENT(obj), &position, &acc);
	if (LOCATION_ERROR_NONE != ret) return ret;
	ret = location_ielement_get_reversegeocode_async (LOCATION_IELEMENT(geocode), position, callback, userdata);
	location_position_free (position);
	return ret;
}

EXPORT_API int
location_get_address_from_position_async (LocationObject *obj,
	const LocationPosition *position,
	LocationAddressCB callback,
	gpointer userdata)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (geocode, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_reversegeocode_async (LOCATION_IELEMENT(geocode), position, callback, userdata);
}

EXPORT_API int
location_get_poi (LocationObject *obj,
	gdouble radius,
	const gchar *keyword,
	LocationPOIInfo **poi_info)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (poi, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_poi(LOCATION_IELEMENT(poi), radius, keyword, poi_info);
}

EXPORT_API int
location_get_poi_from_address(LocationObject *obj,
	const LocationAddress *address,
	gdouble radius,
	const gchar *keyword,
	LocationPOIInfo **poi_info)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (poi, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_poi_from_address(LOCATION_IELEMENT(poi), address, radius, keyword, poi_info);
}

EXPORT_API int
location_get_poi_from_position(LocationObject *obj,
	const LocationPosition *position,
	gdouble radius,
	const gchar *keyword,
	LocationPOIInfo **poi_info)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (poi, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_poi_from_position(LOCATION_IELEMENT(poi), position, radius, keyword, poi_info);
}

EXPORT_API int
location_get_poi_async (LocationObject *obj,
	gdouble radius,
	const gchar *keyword,
	LocationPOICB callback,
	gpointer userdata)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (poi, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_poi_async(LOCATION_IELEMENT(poi), radius, keyword, callback, userdata);
}

EXPORT_API int
location_get_poi_from_address_async (LocationObject *obj,
	const LocationAddress *address,
	gdouble radius,
	const gchar *keyword,
	LocationPOICB callback,
	gpointer userdata)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (poi, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_poi_from_address_async(LOCATION_IELEMENT(poi), address, radius, keyword, callback, userdata);
}

EXPORT_API int
location_get_poi_from_position_async (LocationObject *obj,
	const LocationPosition *position,
	gdouble radius,
	const gchar*keyword,
	LocationPOICB callback,
	gpointer userdata)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (poi, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_poi_from_position_async(LOCATION_IELEMENT(poi), position, radius, keyword, callback, userdata);
}

EXPORT_API int
location_send_command(const char *cmd)
{
	g_return_val_if_fail (cmd, LOCATION_ERROR_PARAMETER);

	return LOCATION_ERROR_NONE;
}

