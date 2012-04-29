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

#ifndef __LOCATION_IELEMENT_H__
#define __LOCATION_IELEMENT_H__

#include <glib-object.h>
#include <location-types.h>
#include <location-position.h>
#include <location-velocity.h>
#include <location-accuracy.h>
#include <location-address.h>
#include <location-boundary.h>
#include <location-satellite.h>
#include <location-poi.h>

/**
 * @file location-ielement.h
 * @brief This file contains the internal definitions and structures related to location interface.
 */

G_BEGIN_DECLS

enum {
	SERVICE_ENABLED,
	SERVICE_DISABLED,
	SERVICE_UPDATED,
	ZONE_IN,
	ZONE_OUT,
	LAST_SIGNAL
};

#define LOCATION_TYPE_IELEMENT                  (location_ielement_get_type ())
#define LOCATION_IELEMENT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), LOCATION_TYPE_IELEMENT, LocationIElement))
#define LOCATION_IS_IELEMENT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LOCATION_TYPE_IELEMENT))
#define LOCATION_IELEMENT_GET_INTERFACE(obj)    (G_TYPE_INSTANCE_GET_INTERFACE ((obj), LOCATION_TYPE_IELEMENT, LocationIElementInterface))

typedef struct _LocationIElement          LocationIElement;
typedef struct _LocationIElementInterface LocationIElementInterface;

typedef int (*TYPE_START_FUNC)(LocationIElement *self);
typedef int (*TYPE_STOP_FUNC) (LocationIElement *self);
typedef int (*TYPE_GET_POSITION)(LocationIElement *self, LocationPosition **position, LocationAccuracy **accuracy);
typedef int (*TYPE_GET_VELOCITY)(LocationIElement *self, LocationVelocity **velocity, LocationAccuracy **accuracy);
typedef int (*TYPE_GET_SATELLITE)(LocationIElement *self, LocationSatellite **satellite);
typedef int (*TYPE_GET_GEOCODE)(LocationIElement *self, const LocationAddress *address, GList **position_list, GList **accuracy_list);
typedef int (*TYPE_GET_GEOCODE_FREEFORM)(LocationIElement *self, const gchar *address, GList **position_list, GList **accuracy_list);
typedef int (*TYPE_GET_REVERSEGEOCODE)(LocationIElement *self, const LocationPosition *position, LocationAddress **address, LocationAccuracy **accuracy);
typedef int (*TYPE_GET_GEOCODE_ASYNC)(LocationIElement *self, const LocationAddress *address, LocationPositionCB callback, gpointer userdata);
typedef int (*TYPE_GET_GEOCODE_FREEFORM_ASYNC)(LocationIElement *self, const gchar *address,  LocationPositionCB callback, gpointer userdata);
typedef int (*TYPE_GET_REVERSEGEOCODE_ASYNC)(LocationIElement *self, const LocationPosition *position, LocationAddressCB callback, gpointer userdata);
typedef int (*TYPE_SEARCH_POI) (LocationIElement *self, const LocationPOIFilter * filter, const LocationPosition *position, const LocationPreference *svc_pref, const LocationPOIPreference * pref, LocationPOICB cb, const gpointer user_data, guint *req_id);
typedef int (*TYPE_SEARCH_POI_BY_AREA) (LocationIElement *self, const LocationPOIFilter *filter, const LocationBoundary * boundary, const LocationPreference *svc_pref, const LocationPOIPreference * pref, LocationPOICB cb, const gpointer user_data, guint *req_id);
typedef int (*TYPE_SEARCH_POI_BY_ADDR) (LocationIElement *self, const LocationPOIFilter *filter, const LocationAddress * address, const LocationPreference *svc_pref, const LocationPOIPreference * pref, LocationPOICB cb, const gpointer user_data, guint *req_id);
typedef int (*TYPE_SEARCH_POI_BY_FREEFORM) (LocationIElement *self, const LocationPOIFilter *filter, const gchar *freeform, const LocationPreference *svc_pref, const LocationPOIPreference * pref, LocationPOICB cb, const gpointer user_data, guint *req_id);
typedef int (*TYPE_CANCEL_POI_REQUEST) (LocationIElement *self, guint req_id);
typedef int (*TYPE_REQUEST_ROUTE) (LocationIElement *self, const LocationPosition *origin, const LocationPosition *destination, GList *waypoint, const LocationPreference *svc_pref, const LocationRoutePreference *pref, LocationRouteCB cb, const gpointer user_data, guint *req_id);
typedef int (*TYPE_CANCEL_ROUTE_REQUEST) (LocationIElement *self, guint req_id);
typedef gboolean (*TYPE_IS_SUPPORTED_MAP_PROVIDER_CAPABILITY) (LocationIElement *self, LocationMapServiceType type);
typedef int (*TYPE_GET_MAP_PROVIDER_CAPABILITY_KEY)(LocationIElement *self, LocationMapServiceType type, GList **key);

struct _LocationIElementInterface
{
	GTypeInterface parent_iface;

	TYPE_START_FUNC start;
	TYPE_STOP_FUNC stop;
	TYPE_GET_POSITION get_position;
	TYPE_GET_POSITION get_last_position;
	TYPE_GET_VELOCITY get_velocity;
	TYPE_GET_VELOCITY get_last_velocity;
	TYPE_GET_SATELLITE get_satellite;
	TYPE_GET_SATELLITE get_last_satellite;
	TYPE_GET_GEOCODE get_geocode;
	TYPE_GET_GEOCODE_FREEFORM get_geocode_freeform;
	TYPE_GET_REVERSEGEOCODE get_reversegeocode;
	TYPE_GET_GEOCODE_ASYNC get_geocode_async;
	TYPE_GET_GEOCODE_FREEFORM_ASYNC get_geocode_freeform_async;
	TYPE_GET_REVERSEGEOCODE_ASYNC get_reversegeocode_async;
	TYPE_SEARCH_POI search_poi;
	TYPE_SEARCH_POI_BY_AREA search_poi_by_area;
	TYPE_SEARCH_POI_BY_ADDR search_poi_by_address;
	TYPE_SEARCH_POI_BY_FREEFORM search_poi_by_freeform;
	TYPE_CANCEL_POI_REQUEST cancel_poi_request;
	TYPE_REQUEST_ROUTE request_route;
	TYPE_CANCEL_ROUTE_REQUEST cancel_route_request;
	TYPE_IS_SUPPORTED_MAP_PROVIDER_CAPABILITY is_supported_map_provider_capability;
	TYPE_GET_MAP_PROVIDER_CAPABILITY_KEY get_map_provider_capability_key;

};

GType location_ielement_get_type (void);

int location_ielement_start (LocationIElement *self);
int location_ielement_stop(LocationIElement *self);
int location_ielement_get_position (LocationIElement *self, LocationPosition **position, LocationAccuracy **accuracy);
int location_ielement_get_last_position (LocationIElement *self, LocationPosition **position, LocationAccuracy **accuracy);
int location_ielement_get_velocity (LocationIElement *self, LocationVelocity **velocity, LocationAccuracy **accuracy);
int location_ielement_get_last_velocity (LocationIElement *self, LocationVelocity **velocity, LocationAccuracy **accuracy);
int location_ielement_get_satellite (LocationIElement *self, LocationSatellite **satellite);
int location_ielement_get_last_satellite (LocationIElement *self, LocationSatellite **satellite);
int location_ielement_get_geocode (LocationIElement *self, const LocationAddress *address, GList **position_list, GList **accuracy_list);
int location_ielement_get_geocode_freeform (LocationIElement *self, const gchar *address, GList **position_list, GList **accuracy_list);
int location_ielement_get_reversegeocode (LocationIElement *self, const LocationPosition *position, LocationAddress **address, LocationAccuracy **accuracy);
int location_ielement_get_geocode_async (LocationIElement *self, const LocationAddress *address, LocationPositionCB callback, gpointer userdata);
int location_ielement_get_geocode_freeform_async (LocationIElement *self, const gchar *address, LocationPositionCB callback, gpointer userdata);
int location_ielement_get_reversegeocode_async (LocationIElement *self, const LocationPosition *position, LocationAddressCB callback, gpointer userdata);
int location_ielement_search_poi (LocationIElement *self, const LocationPOIFilter * filter, const LocationPosition *position, const LocationPreference *svc_pref, const LocationPOIPreference * pref, LocationPOICB cb, const gpointer user_data, guint * req_id);
int location_ielement_search_poi_by_area (LocationIElement *self, const LocationPOIFilter * filter, const LocationBoundary * boundary, const LocationPreference *svc_pref, const LocationPOIPreference * pref, LocationPOICB cb, const gpointer user_data, guint * req_id);
int location_ielement_search_poi_by_address (LocationIElement *self, const LocationPOIFilter * filter, const LocationAddress * address, const LocationPreference *svc_pref, const LocationPOIPreference * pref, LocationPOICB cb, const gpointer user_data, guint * req_id);
int location_ielement_search_poi_by_freeform (LocationIElement *self, const LocationPOIFilter * filter, const gchar * freeform, const LocationPreference *svc_pref, const LocationPOIPreference * pref, LocationPOICB cb, const gpointer user_data, guint * req_id);
int location_ielement_cancel_poi_request (LocationIElement *self, guint req_id);
int location_ielement_request_route (LocationIElement *self, const LocationPosition *origin, const LocationPosition *destination, GList *waypoint, const LocationPreference *svc_pref, const LocationRoutePreference * pref, LocationRouteCB cb, const gpointer user_data, guint * req_id);
int location_ielement_cancel_route_request (LocationIElement *self, guint req_id);
gboolean location_ielement_is_supported_map_provider_capability (LocationIElement *self, LocationMapServiceType type);
int location_ielement_get_map_provider_capability_key (LocationIElement *self, LocationMapServiceType type, GList **key);

G_END_DECLS

#endif
