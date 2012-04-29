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

#include "location.h"
#include "location-log.h"
#include "location-setting.h"
#include "location-ielement.h"
#include "location-pref.h"
#include "map-service.h"

LocationObject *g_map_service = NULL;

//static LocationObject *poi = NULL;

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

EXPORT_API int
location_get_position_from_address (LocationObject *obj,
	const LocationAddress *address,
	GList **position_list,
	GList **accuracy_list)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_geocode (LOCATION_IELEMENT(g_map_service), address, position_list, accuracy_list);
}

EXPORT_API int
location_get_position_from_freeformed_address (LocationObject *obj,
	const gchar *address,
	GList **position_list,
	GList **accuracy_list)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	return location_ielement_get_geocode_freeform (LOCATION_IELEMENT(g_map_service), address, position_list, accuracy_list);
}

EXPORT_API int
location_get_address (LocationObject *obj,
	LocationAddress **address,
	LocationAccuracy **accuracy)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	LocationPosition *position = NULL;
	int ret = location_ielement_get_position(LOCATION_IELEMENT (obj), &position, accuracy);
	if (LOCATION_ERROR_NONE != ret) return ret;
	ret = location_ielement_get_reversegeocode (LOCATION_IELEMENT(g_map_service), position, address, accuracy);
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
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_reversegeocode (LOCATION_IELEMENT(g_map_service), position, address, accuracy);
}

EXPORT_API int
location_get_position_from_address_async (LocationObject *obj,
	const LocationAddress *address,
	LocationPositionCB callback,
	gpointer userdata)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_geocode_async (LOCATION_IELEMENT(g_map_service), address, callback, userdata);
}


EXPORT_API int
location_get_position_from_freeformed_address_async (LocationObject *obj,
	const gchar *address,
	LocationPositionCB callback,
	gpointer userdata)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);
	return location_ielement_get_geocode_freeform_async (LOCATION_IELEMENT(g_map_service), address, callback, userdata);
}

EXPORT_API int
location_get_address_async (LocationObject *obj,
	LocationAddressCB callback,
	gpointer userdata)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	LocationPosition *position = NULL;
	LocationAccuracy *acc = NULL;
	int ret = location_ielement_get_position(LOCATION_IELEMENT(obj), &position, &acc);
	if (LOCATION_ERROR_NONE != ret) return ret;
	ret = location_ielement_get_reversegeocode_async (LOCATION_IELEMENT(g_map_service), position, callback, userdata);
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
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	return location_ielement_get_reversegeocode_async (LOCATION_IELEMENT(g_map_service), position, callback, userdata);
}

EXPORT_API int
location_search_poi (LocationObject *obj,
		const LocationPOIFilter * filter,
		const LocationPosition *position,
		const LocationPOIPreference * pref,
		LocationPOICB cb, gpointer user_data, guint * req_id)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (filter, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (position, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (pref, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (cb, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (req_id, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	LocationPreference *svc_pref = location_get_map_service_pref (g_map_service);

	return location_ielement_search_poi (LOCATION_IELEMENT(g_map_service), filter, position, svc_pref, pref, cb, user_data, req_id);
}

EXPORT_API int
location_search_poi_by_area (LocationObject *obj,
		const LocationPOIFilter * filter,
		const LocationBoundary * boundary,
		const LocationPOIPreference * pref,
		LocationPOICB cb, gpointer user_data, guint * req_id)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (filter, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (boundary, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (pref, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (cb, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (req_id, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	LocationPreference *svc_pref = location_get_map_service_pref (g_map_service);

	return location_ielement_search_poi_by_area (LOCATION_IELEMENT(g_map_service), filter, boundary, svc_pref, pref, cb, user_data, req_id);
}

EXPORT_API int
location_search_poi_by_address (LocationObject *obj, const LocationPOIFilter * filter,
				const LocationAddress * address, const LocationPOIPreference * pref,
				LocationPOICB cb, gpointer user_data, guint * req_id)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (filter, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (address, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (pref, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (cb, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (req_id, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	LocationPreference *svc_pref = location_get_map_service_pref (g_map_service);

	return location_ielement_search_poi_by_address (LOCATION_IELEMENT(g_map_service), filter, address, svc_pref, pref, cb, user_data, req_id);
}

EXPORT_API int
location_search_poi_by_freeformed_address (LocationObject *obj,
		const LocationPOIFilter * filter,
		const gchar * address,
		const LocationPOIPreference * pref,
		LocationPOICB cb, gpointer user_data, guint * req_id)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (filter, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (address, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (pref, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (cb, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (req_id, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	LocationPreference *svc_pref = location_get_map_service_pref (g_map_service);

	return location_ielement_search_poi_by_freeform (LOCATION_IELEMENT(g_map_service), filter, address, svc_pref, pref, cb, user_data, req_id);
}

EXPORT_API int
location_cancel_poi_request (LocationObject *obj, guint req_id)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (req_id, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	return location_ielement_cancel_poi_request (LOCATION_IELEMENT(g_map_service), req_id);
}

EXPORT_API int
location_request_route (LocationObject *obj, LocationPosition *origin, LocationPosition *destination,
				GList *waypoint, const LocationRoutePreference * pref,
				LocationRouteCB cb, gpointer user_data, guint * req_id)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (origin, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (destination, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (pref, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (cb, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (req_id, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	LocationPreference *svc_pref = location_get_map_service_pref (g_map_service);

	return location_ielement_request_route (LOCATION_IELEMENT(g_map_service), origin, destination,
					waypoint, svc_pref, pref, cb, user_data, req_id);
}

EXPORT_API int
location_cancel_route_request (LocationObject *obj, guint req_id)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	return location_ielement_cancel_route_request (LOCATION_IELEMENT(g_map_service), req_id);
}

EXPORT_API gboolean
location_is_supported_map_provider_capability (LocationObject *obj, LocationMapServiceType type)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	return location_ielement_is_supported_map_provider_capability (LOCATION_IELEMENT(g_map_service), type);
}

EXPORT_API int
location_get_map_provider_capability_key (LocationObject *obj, LocationMapServiceType type, GList **key)
{
	g_return_val_if_fail (obj, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (key, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (g_map_service, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (is_connected_network(), LOCATION_ERROR_NETWORK_NOT_CONNECTED);

	return location_ielement_get_map_provider_capability_key (LOCATION_IELEMENT(g_map_service), type, key);
}

EXPORT_API LocationPreference *
location_get_map_service_pref (LocationObject *obj)
{
	g_return_val_if_fail (obj, NULL);
	g_return_val_if_fail (g_map_service, NULL);

	return map_service_get_pref (g_map_service);
}

EXPORT_API gboolean
location_set_map_service_pref (LocationObject *obj, LocationPreference *pref)
{
	g_return_val_if_fail (obj, FALSE);
	g_return_val_if_fail (pref, FALSE);
	g_return_val_if_fail (g_map_service, FALSE);

	return map_service_set_pref (g_map_service, pref);
}
