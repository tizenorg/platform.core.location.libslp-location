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

#ifndef __MAP_INTERNAL_H__
#define __MAP_INTERNAL_H__

#include "location-types.h"
#include "location-ielement.h"

/**
 * @file map-internal.h
 * @brief This file contains the internal definitions and structures related to geocode.
 */

G_BEGIN_DECLS

int map_service_get_geocode (LocationIElement *self, const LocationAddress *address, GList **position_list, GList **accuracy_list);

int map_service_get_geocode_freeform (LocationIElement *self, const gchar  *address, GList **position_list, GList **accuracy_list);

int map_service_get_reversegeocode (LocationIElement *self, const LocationPosition *position, LocationAddress **address, LocationAccuracy **accuracy);

int map_service_get_geocode_async (LocationIElement *self, LocationAddress *address, LocationPositionCB callback, gpointer userdata);

int map_service_get_geocode_freeform_async (LocationIElement *self, const gchar *address, LocationPositionCB callback, gpointer userdata);

int map_service_get_reversegeocode_async (LocationIElement *self, const LocationPosition *position, LocationAddressCB callback, gpointer userdata);

int map_service_search_poi (LocationIElement *self,
		const LocationPOIFilter *filter, const LocationPosition *position,
		const LocationPreference *svc_pref, const LocationPOIPreference *pref,
		LocationPOICB cb, const gpointer user_data, guint * req_id);

int map_service_search_poi_by_area (LocationIElement *self,
		const LocationPOIFilter *filter, const LocationBoundary *boundary,
		const LocationPreference *svc_pref, const LocationPOIPreference *pref,
		LocationPOICB cb, const gpointer user_data, guint * req_id);

int map_service_search_poi_by_address (LocationIElement *self,
		const LocationPOIFilter *filter, const LocationAddress *address,
		const LocationPreference *svc_pref, const LocationPOIPreference *pref,
		LocationPOICB cb, const gpointer user_data, guint * req_id);

int map_service_search_poi_by_freeform (LocationIElement *self,
		const LocationPOIFilter * filter, const gchar *freeform,
		const LocationPreference *svc_pref, const LocationPOIPreference *pref,
		LocationPOICB cb, const gpointer user_data, guint *req_id);

int map_service_cancel_poi_request (LocationIElement *self, guint req_id);

int map_service_request_route (LocationIElement *self,
		const LocationPosition *origin, const LocationPosition *destination, GList *waypoint,
		const LocationPreference *svc_pref, const LocationRoutePreference *pref,
		LocationRouteCB cb, const gpointer user_data, guint * req_id);

int map_service_cancel_route_request (LocationIElement *self, guint req_id);

gboolean map_service_is_supported_map_provider_capability (LocationIElement *self, LocationMapServiceType type);

int map_service_get_map_provider_capability_key (LocationIElement *self, LocationMapServiceType type, GList **key);

G_END_DECLS

#endif
