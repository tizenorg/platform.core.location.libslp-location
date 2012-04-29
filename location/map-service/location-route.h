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

#ifndef __LOCATION_ROUTE_H__
#define __LOCATION_ROUTE_H__

#include <location-types.h>

G_BEGIN_DECLS

/**
 * @file location-route.h
 * @brief This file contains the internal definitions and structures related to Route.
 */

/**
 * @addtogroup LocationMapService
 * @{
 * @defgroup LocationMapServiceRoute Location Route
 * @brief This is a Location Route for providing location map services.
 * @addtogroup LocationMapServiceRoute
 * @{
 */

/**
 * @brief Create a new Location route preference
 * @remarks None.
 * @pre     #location_init should be called before.\n
 * @post    None.
 * @return a #LocationRoutePreference
 * @retval NULL if error occured
 * @see location_route_pref_free
 */
LocationRoutePreference *location_route_pref_new (void);

/**
 * @brief Copy Location route preference
 * @remarks None.
 * @pre     #location_init should be called before.\n
 * @post    None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return a new #LocationRoutePreference
 * @retval NULL if error occured
 */
LocationRoutePreference *location_route_pref_copy (const LocationRoutePreference *pref);

/**
 * @brief Free Location route preference
 * @remarks None.
 * @pre     #location_init should be called before.\n
 * @post    None.
 * @param [in]  pref - a #LocationRoutePreference
 * @see location_route_pref_new
 * @return None
 * @retval None
 */
void location_route_pref_free (LocationRoutePreference * pref);

/**
 * @brief Get a list of address structures to be avoided in Location route preference
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return GList
 * @retval list of #LocationAddress
 * @see location_route_pref_set_addr_to_avoid
 */
GList *location_route_pref_get_addr_to_avoid (const LocationRoutePreference *pref);

/**
 * @brief Get a list of area to be avoided in Location route preference
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return GList
 * @retval list of #LocationBoundary
 * @see location_route_pref_set_area_to_avoid
 */
GList *location_route_pref_get_area_to_avoid (const LocationRoutePreference *pref);

/**
 * @brief Get a list of features to be avoided in Location route preference
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return GList
 * @retval list of gchar
 * @see location_route_pref_set_feature_to_avoid
 */
GList *location_route_pref_get_feature_to_avoid (const LocationRoutePreference *pref);

/**
 * @brief Get a list of freeformed address to be avoided in Location route preference
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return GList
 * @retval list of gchar
 * @see location_route_pref_set_freeformed_to_avoid
 */
GList *location_route_pref_get_freeformed_addr_to_avoid (const LocationRoutePreference *pref);

/**
 * @brief Get a list of bounding box to be avoided in Location route preference
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return #LocationBoundary
 * @retval bounding box
 * @see location_route_pref_set_addr_to_avoid
 */
LocationBoundary *location_route_pref_get_bounding_box (const LocationRoutePreference *pref);

/**
 * @brief Get a maximum number of matches returned from route service provider
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return guint
 * @retval maximum number of matches
 * @see location_route_pref_set_max_result
 */
guint location_route_pref_get_max_result (const LocationRoutePreference *pref);

/**
 * @brief Get a route type to be used in route service
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return gchar
 * @retval route type
 * @see location_route_pref_set_route_type
 */
gchar *location_route_pref_get_route_type (const LocationRoutePreference *pref);

/**
 * @brief Get a transport mode to be used in route service
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return gchar
 * @retval transport mode
 * @see location_route_pref_set_transport_mode
 */
gchar *location_route_pref_get_transport_mode (const LocationRoutePreference *pref);

/**
 * @brief Get whether the route service provider must provide the route geometry in the service requests
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return gboolean
 * @retval TRUE			if used
 * @see location_route_pref_set_geometry_used
 */
gboolean location_route_pref_get_geometry_used (const LocationRoutePreference *pref);
/**
 * @brief Get whether the route service provider must provide the route instruction bounding box in the service requests
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return gboolean
 * @retval TRUE			if used
 * @see location_route_pref_set_instruction_bounding_box_used
 */
gboolean location_route_pref_get_instruction_bounding_box_used (const LocationRoutePreference *pref);
/**
 * @brief Get whether the route service provider must provide the route geometry in the service requests
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return gboolean
 * @retval TRUE			if used
 * @see location_route_pref_set_instruction_geometry_used
 */
gboolean location_route_pref_get_instruction_geometry_used (const LocationRoutePreference *pref);

/**
 * @brief Get whether the route service provider must provide the route instruction in the service requests
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return gboolean
 * @retval TRUE			if used
 * @see location_route_pref_set_instruction_used
 */
gboolean location_route_pref_get_instruction_used (const LocationRoutePreference *pref);
/**
 * @brief Get whether the route service provider must provide traffic data in the service requests
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return gboolean
 * @retval TRUE			if used
 * @see location_route_pref_set_traffic_data_used
 */
gboolean location_route_pref_get_traffic_data_used (const LocationRoutePreference *pref);
/**
 * @brief Get a list of Property Keys in #LocationRoutePreference
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return GList
 * @retval list of property key
 */
GList *location_route_pref_get_property_key (const LocationRoutePreference *pref);
/**
 * @brief Get Property value in #LocationRoutePreference
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @return GList
 * @retval property value
 * @see location_route_pref_set_property
 */
gpointer location_route_pref_get_property (const LocationRoutePreference *pref, gconstpointer key);

/**
 * @brief Set a list of address structures to be avoided in #LocationRoutePreference
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  addr - a list of #LocationAddress
 * @return gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_get_addr_to_avoid
 */
gboolean location_route_pref_set_addr_to_avoid (LocationRoutePreference * pref, GList *addr);

/**
 * @brief Set a list of area to be avoided in Location route preference
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  area - a list of #LocationBoundary
 * @return gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_get_area_to_avoid
 */
gboolean location_route_pref_set_area_to_avoid (LocationRoutePreference * pref, GList *area);

/**
 * @brief Set a list of features to be avoided in Location route preference
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  feature - a list of features
 * @return gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_get_feature_to_avoid
 */
gboolean location_route_pref_set_feature_to_avoid (LocationRoutePreference * pref, GList * feature);

/**
 * @brief Set a list of freeformed address to be avoided in Location route preference
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  addr - a list of #gchar
 * @return gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_get_freeformed_addr_to_avoid
 */
gboolean location_route_pref_set_freeformed_addr_to_avoid (LocationRoutePreference *pref, GList * freeformed_addr);

/**
 * @brief Set a bounding box #LocationBoundary in Location route preference
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  bbox - a #LocationBoundary
 * @return gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_get_bounding_box
 */
gboolean location_route_pref_set_bounding_box (LocationRoutePreference *pref, const LocationBoundary *boundary);

/**
 * @brief Set a maximum number of matches returned from route service provider
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  max_num - a #gint
 * @return gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_set_max_result
 */
gboolean location_route_pref_set_max_result (LocationRoutePreference *pref, guint max_num);

/**
 * @brief Set a route type to be used in route service
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  type - a #gchar
 * @return gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_get_route_type
 */
gboolean location_route_pref_set_route_type (LocationRoutePreference *pref, const gchar *type);

/**
 * @brief Set a transport mode to be used in route service
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  mode - a #gchar
 * @return gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_get_transport_mode
 */
gboolean location_route_pref_set_transport_mode (LocationRoutePreference *pref, const gchar * mode);

/**
 * @brief Set whether the route service provider must provide the route geometry in the service requests
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  is_used - a #gboolean
 * @return gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_get_geometry_used
 */
gboolean location_route_pref_set_geometry_used (LocationRoutePreference *pref, gboolean is_used);

/**
 * @brief Set whether the route service provider must provide the route instruction bounding box in the service requests
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  is_used - a #gboolean
 * @return gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_get_instruction_bounding_box_used
 */
gboolean location_route_pref_set_instruction_bounding_box_used (LocationRoutePreference *pref, gboolean is_used);

/**
 * @brief Set whether the route service provider must provide the route geometry in the service requests
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  is_used - a #gboolean
 * @return gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_get_instruction_geometry_used
 */
gboolean location_route_pref_set_instruction_geometry_used (LocationRoutePreference *pref, gboolean is_used);

/**
 * @brief Set whether the route service provider must provide the route instruction in the service requests
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  is_used - a #gboolean
 * @return gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_get_instruction_used
 */
gboolean location_route_pref_set_instruction_used (LocationRoutePreference *pref, gboolean is_used);

/**
 * @brief Set whether the route service provider must provide traffic data in the service requests
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  is_used - a #gboolean
 * @return #gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_get_traffic_data_used
 */
gboolean location_route_pref_set_traffic_data_used (LocationRoutePreference *pref, gboolean is_used);

/**
 * @brief Set Property in Location route proference
 * @remarks The service provider should support route service.
 * @pre #location_route_pref_new should be called before.
 * @post None.
 * @param [in]  pref - a #LocationRoutePreference
 * @param [in]  key - a #gconstpointer
 * @param [in]  value - a #gconstpointer
 * @return gboolean
 * @retval TRUE                              if success
 * @see location_route_pref_get_property
 */
gboolean location_route_pref_set_property (LocationRoutePreference *pref, gconstpointer key, gconstpointer value);

/**
 * @defgroup LocationMapServiceRouteResult Location Route Result
 * @brief This provides structure, enumeration and APIs for Route Result
 * @addtogroup LocationMapServiceRouteResult
 * @{
 */
/**
 * @brief Create a new Location route
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @return #LocationRoute
 * @retval a new location route
 * @see location_route_free
 */
LocationRoute *location_route_new (void);
/**
 * @brief Copy Location route
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  route - a #LocationRoute
 * @return #LocationRoute
 * @retval a copied location route
 */
LocationRoute *location_route_copy (const LocationRoute *route);

/**
 * @brief Free #LocationRoute
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  route - a #LocationRoute
 * @return void
 * @retval None
 */
void location_route_free (LocationRoute *route);

/**
 * @brief Get the origin of Location route
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  route - a #LocationRoute
 * @return #Location position
 * @retval origin
 * @see location_route_set_origin
 */
LocationPosition *location_route_get_origin (const LocationRoute *route);

/**
 * @brief Get the destination of Location route
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  route - a #LocationRoute
 * @return #LocationPosition
 * @retval destination
 * @see location_route_set_destination
 */
LocationPosition *location_route_get_destination (const LocationRoute *route);

/**
 * @brief Get the bounding box of Location route
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  route - a #LocationRoute
 * @return #LocationBoundary
 * @retval bounding box
 * @see location_route_set_bounding box
 */
LocationBoundary *location_route_get_bounding_box (const LocationRoute *route);

/**
 * @brief Get the total distance of Location route
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  route - a #LocationRoute
 * @return gdouble
 * @retval total distance
 * @see location_route_set_total_distance
 */
gdouble location_route_get_total_distance (const LocationRoute *route);

/**
 * @brief Get the distance unit of Location route
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  route - a #LocationRoute
 * @return gchar
 * @retval distance unit
 * @see location_route_set_distance_unit
 */
gchar *location_route_get_distance_unit (const LocationRoute *route);

/**
 * @brief Get the total duration of location route
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  route - a #LocationRoute
 * @return glong
 * @retval total duration
 * @see location_route_set_total_duration
 */
glong location_route_get_total_duration (const LocationRoute *route);

/**
 * @brief Get the list of property key of Location route
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  route - a #LocationRoute
 * @return GList
 * @retval list of property keys
 * @see location_route_set_property
 */
GList *location_route_get_property_key (const LocationRoute *route);

/**
 * @brief Get the property value of Location route
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  route - a #LocationRoute
 * @return gconstpointer
 * @retval property
 * @see location_route_set_property
 */
gpointer location_route_get_property (const LocationRoute *route, gconstpointer key);

/**
 * @brief Get the list of segments in Location route
 * @remarks The service provider should support route service.
 * @post None.
 * @pre None.
 * @param [in]  route - a #LocationRoute
 * @return GList
 * @retval list of #LocationRouteSegment
 * @see location_route_set_route_segment
 */
GList *location_route_get_route_segment (const LocationRoute *route);

/**
 * @brief Create a new Location route segment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @return #LocationPositionSegment
 * @retval Location route segment
 * @see location_route_segment_free
 */
LocationRouteSegment *location_route_segment_new (void);

/**
 * @brief Copy Location route segment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  segment - a #LocationRouteSegment
 * @return #LocationRouteSegment
 * @retval copied Location route segment
 */
LocationRouteSegment *location_route_segment_copy (LocationRouteSegment *segment);

/**
 * @brief Free Location route segment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  segment - a #LocationRouteSegment
 * @return void
 * @retval None.
 * @see location_route_segment_new
 */
void location_route_segment_free (LocationRouteSegment *segment);

/**
 * @brief Get the start point #LocationPosition of #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  segment - a #LocationRouteSegment
 * @return void
 * @retval None.
 * @see location_route_segment_set_start_point
 */
LocationPosition *location_route_segment_get_start_point (const LocationRouteSegment *segment);

/**
 * @brief Get the end point #LocationPosition of #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  segment - a #LocationRouteSegment
 * @return void
 * @retval None.
 * @see location_route_segment_set_end_point
 */
LocationPosition *location_route_segment_get_end_point (const LocationRouteSegment *segment);

/**
 * @brief Get the bounding box #LocationBoundary of #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  segment - a #LocationRouteSegment
 * @return void
 * @retval None.
 * @see location_route_segment_set_bounding_box
 */
LocationBoundary *location_route_segment_get_bounding_box (const LocationRouteSegment *segment);

/**
 * @brief Get the distance of #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  segment - a #LocationRouteSegment
 * @return void
 * @retval None.
 * @see location_route_segment_set_distance
 */
gdouble location_route_segment_get_distance (const LocationRouteSegment *segment);

/**
 * @brief Get the duration #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  segment - a #LocationRouteSegment
 * @return void
 * @retval None.
 * @see location_route_segment_set_duration
 */
glong location_route_segment_get_duration (const LocationRouteSegment *segment);

/**
 * @brief Get the list of property keys of #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  segment - a #LocationRouteSegment
 * @return void
 * @retval None.
 * @see location_route_segment_set_property_key
 */
GList *location_route_segment_get_property_key (const LocationRouteSegment *segment);

/**
 * @brief Get the property value of #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  segment - a #LocationRouteSegment
 * @return void
 * @retval None.
 * @see location_route_segment_set_property
 */
gpointer location_route_segment_get_property (const LocationRouteSegment *segment, gconstpointer key);

/**
 * @brief Get the list of route steps #LocationRouteStep in #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in]  segment - a #LocationRouteSegment
 * @return void
 * @retval None.
 * @see location_route_segment_set_route_step
 */
GList *location_route_segment_get_route_step (const LocationRouteSegment *segment);

/**
 * @brief Create a new #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param None.
 * @return #LocationRouteStep
 * @retval  route step
 * @see location_route_step_free
 */
LocationRouteStep *location_route_step_new (void);

/**
 * @brief Copy #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @return #LocationRouteStep
 * @retval  route step
 * @see location_route_step_new
 */
LocationRouteStep *location_route_step_copy (LocationRouteStep *step);

/**
 * @brief Free #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @return void
 * @retval  None
 * @see location_route_step_new
 */
void location_route_step_free (LocationRouteStep *step);

/**
 * @brief Get the start point #LocationPosition of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @return #LocationPosition
 * @retval  start point
 * @see location_route_step_set_start_point
 */
LocationPosition *location_route_step_get_start_point (const LocationRouteStep *step);

/**
 * @brief Get the end point #LocationPosition of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @return #LocationPosition
 * @retval  end point
 * @see location_route_step_set_end_point
 */
LocationPosition *location_route_step_get_end_point (const LocationRouteStep *step);

/**
 * @brief Get the bounding box #LocationBoundary of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @return #LocationBoundary
 * @retval  bounding box
 * @see location_route_step_set_bounding_box
 */
LocationBoundary *location_route_step_get_bounding_box (const LocationRouteStep *step);

/**
 * @brief Get the distance of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @return gboolean
 * @retval  distance
 * @see location_route_step_set_distance
 */
gdouble location_route_step_get_distance (const LocationRouteStep *step);

/**
 * @brief Get the duration of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @return glong
 * @retval  duration
 * @see location_route_step_set_duration
 */
glong location_route_step_get_duration (const LocationRouteStep *step);

/**
 * @brief Get the transport mode of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @return gchar
 * @retval  transport mode
 * @see location_route_step_set_transport_mode
 */
gchar *location_route_step_get_transport_mode (const LocationRouteStep *step);

/**
 * @brief Get the instruction of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @return gchar
 * @retval  instruction
 * @see location_route_step_set_instruction
 */
gchar *location_route_step_get_instruction (const LocationRouteStep *step);

/**
 * @brief Get the list of geometry #LocationPosition of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @return GList
 * @retval  a list of LocationPosition
 * @see location_route_step_set_geometry
 */
GList *location_route_step_get_geometry (const LocationRouteStep *step);

/**
 * @brief Get the list of property key of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @return GList
 * @retval  a list of property key
 * @see location_route_step_set_property_key
 */
GList *location_route_step_get_property_key (const LocationRouteStep *step);

/**
 * @brief Get the property value of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @param [in] key - a gconstpointer
 * @return gpointer
 * @retval  value
 * @see location_route_step_set_property
 */
gpointer location_route_step_get_property (const LocationRouteStep *step, gconstpointer key);

/**
 * @} @} @}
 */

G_END_DECLS

#endif
