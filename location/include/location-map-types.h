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

#ifndef __LOCATION_MAP_TYPES_H__
#define __LOCATION_MAP_TYPES_H__

#include <glib.h>
#include <glib-object.h>
#include <location-types.h>

G_BEGIN_DECLS

/**
 * @file location-map-types.h
 * @brief This file contains the Location related structure, enumeration, and asynchronous function definitions.
 * @addtogroup LocationFW
 * @{
 * @defgroup LocationMapTypes Location Map Types
 * @brief This sub module provides structure, enumeration, and asynchronous function definitions.
 * @addtogroup LocationMapTypes
 * @{
 */

typedef enum {
	MAP_SERVICE_PREF_LANGUAGE,
	MAP_SERVICE_PREF_DISTANCE_UNIT,

	MAP_SERVICE_PREF_PROPERTY,

	MAP_SERVICE_GEOCODE_TYPE,
	MAP_SERVICE_REVERSE_GEOCODE_TYPE,

	MAP_SERVICE_POI_TYPE,
	MAP_SERVICE_POI_SEARCH_BY_ADDRESS,
	MAP_SERVICE_POI_SEARCH_BY_FREEFORM_ADDRESS,
	MAP_SERVICE_POI_SEARCH_BY_CIRCLE_BOUNDARY,
	MAP_SERVICE_POI_SEARCH_BY_RECT_BOUNDARY,
	MAP_SERVICE_POI_SEARCH_BY_POLYGON_BOUNDARY,
	MAP_SERVICE_POI_PREF_SORT_BY,
	MAP_SERVICE_POI_PREF_PROPERTY,
	MAP_SERVICE_POI_FILTER,
	MAP_SERVICE_POI_FILTER_CATEGORY,

	MAP_SERVICE_ROUTE_REQUEST_FREEFORM_ADDR_TO_AVOID,
	MAP_SERVICE_ROUTE_REQUEST_STRUCTED_ADDR_TO_AVOID,
	MAP_SERVICE_ROUTE_REQUEST_CIRCLE_AREA_TO_AVOID,
	MAP_SERVICE_ROUTE_REQUEST_RECT_AREA_TO_AVOID,
	MAP_SERVICE_ROUTE_REQUEST_POLYGON_AREA_TO_AVOID,
	MAP_SERVICE_ROUTE_REQUEST_FEATURE_TO_AVOID,
	MAP_SERVICE_ROUTE_PREF_TYPE,
	MAP_SERVICE_ROUTE_PREF_TRANSPORT_MODE,
	MAP_SERVICE_ROUTE_PREF_GEOMETRY_BOUNDING_BOX,
	MAP_SERVICE_ROUTE_PREF_GEOMETRY_RETRIEVAL,
	MAP_SERVICE_ROUTE_PREF_INSTRUCTION_GEOMETRY,
	MAP_SERVICE_ROUTE_PREF_INSTRUCTION_BOUNDING_BOX,
	MAP_SERVICE_ROUTE_PREF_INSTRUCTION_RETRIEVAL,
	MAP_SERVICE_ROUTE_PREF_REALTIME_TRAFFIC,
	MAP_SERVICE_ROUTE_PREF_PROPERTY,
	MAP_SERVICE_ROUTE_DISTANCE_UNIT,
	MAP_SERVICE_ROUTE_PROPERTY,
	MAP_SERVICE_ROUTE_SEGMENT_PROPERTY,
	MAP_SERVICE_ROUTE_STEP_PROPERTY,
	MAP_SERVICE_TYPE_MAX

} LocationMapServiceType;

/**
 * @brief Location Map Object redefined by GObject.
 */
typedef GObject LocationMapObject;

/**
 * @brief This represents address information such as building number, street name, etc.
 */
typedef struct _LocationAddress    LocationAddress;

/**
 * @brief This represents a structure of Location Map Preference.
 */
typedef struct _LocationMapPref LocationMapPref;

/**
 * @brief This represents a structure of Location POI filter.
 */
typedef struct _LocationPOIFilter LocationPOIFilter;

/**
 * @brief This represents a structure of Location POI preference.
 */
typedef struct _LocationPOIPreference LocationPOIPreference;

/**
 * @brief This represents a structure of Landmark information.
 */
typedef struct _LocationLandmark    LocationLandmark;

/**
 * @brief This represents a structure of Landmark Url.
 */
typedef struct _LocationLandmarkUrl    LocationLandmarkUrl;

/**
 * @brief This represents a structure of Location route preference.
 */
typedef struct _LocationRoutePreference LocationRoutePreference;

/**
 * @brief This represents a structure of Location route.
 */
typedef struct _LocationRoute LocationRoute;

/**
 * @brief This represents a structure of Location route segment.
 */
typedef struct _LocationRouteSegment LocationRouteSegment;

/**
 * @brief This represents a structure of Location route step.
 */
typedef struct _LocationRouteStep LocationRouteStep;

/**
 * @brief This represents callback function which will be called to give position information.
 */
typedef void (*LocationPositionCB)(LocationError error, GList *position_list, GList *accuracy_list, gpointer userdata);

/**
 * @brief This represents callback function which will be called to give address information.
 */
typedef void  (*LocationAddressCB)(LocationError error, LocationAddress *address, LocationAccuracy *acc, gpointer userdata);

/**
 * @brief This represents callback function which will be called to give POI information.
 */
typedef void (*LocationPOICB)(LocationError error, guint req_id, GList *landmark_list, gchar *error_code, gchar *error_msg, gpointer userdata);

/**
 * @brief This represents callback function which will be called to give Route information.
 */
typedef void (*LocationRouteCB)(LocationError error, guint req_id, GList *route_list, gchar *error_code, gchar *error_msg, gpointer userdata);

/**
 * @}@}
 */

G_END_DECLS

#endif /* __LOCATION_MAP_TYPES_H__ */
