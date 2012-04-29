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

#ifndef __LOCATION_ROUTE_EXT_H__
#define __LOCATION_ROUTE_EXT_H__

#include <location-types.h>

G_BEGIN_DECLS

/**
 * @file location-route-ext.h
 * @brief This file contains the extensional definitions and structures related to Route.
 */
/**
 * @addtogroup LocationMapServiceRoute
 * @{
 * @defgroup LocationMapServiceRouteExt Location Route Ext
 * @brief This provides Location Route Ext APIs.
 * @addtogroup LocationMapServiceRouteExt
 * @{
 */

/**
 * @brief Set the origin #LocationPosition of #LocationRoute
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] route - a #LocationRoute
 * @param [in] origin - a #LocationPosition
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_get_origin
 */
gboolean location_route_set_origin (LocationRoute *route, const LocationPosition* origin);

/**
 * @brief Set the destication #LocationPosition of #LocationRoute
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] route - a #LocationRoute
 * @param [in] destination - a #LocationPosition
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_get_destination
 */
gboolean location_route_set_destination (LocationRoute *route, const LocationPosition* destination);

/**
 * @brief Set the bounding box #LocationBoundary of #LocationRoute
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] route - a #LocationRoute
 * @param [in] bbox - a #LocationBoundary
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_get_bounding_box
 */
gboolean location_route_set_bounding_box (LocationRoute *route, const LocationBoundary* bbox);
/**
 * @brief Set the distance of #LocationRoute
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] route - a #LocationRoute
 * @param [in] total_distance - gdouble
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_get_total_distance
 */
gboolean location_route_set_total_distance (LocationRoute *route, gdouble total_distance);

/**
 * @brief Set the distance unit of #LocationRoute
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] route - a #LocationRoute
 * @param [in] distance_unit - gchar
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_get_distance_unit
 */
gboolean location_route_set_distance_unit (LocationRoute *route, const gchar* distance_unit);

/**
 * @brief Set the duration of #LocationRoute
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] route - a #LocationRoute
 * @param [in] total_duration - glong
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_get_total_duration
 */
gboolean location_route_set_total_duration (LocationRoute *route, glong total_duration);

/**
 * @brief Set the propery value of #LocationRoute
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] route - a #LocationRoute
 * @param [in] key - gconstpointer
 * @param [in] value - gconstpointer
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_get_property
 */
gboolean location_route_set_property (LocationRoute *route, gconstpointer key, gconstpointer value);

/**
 * @brief Set the list of segments #LocationRouteSegment of #LocationRoute
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] route - a #LocationRoute
 * @param [in] segment - a list of #LocationRouteSegment
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_get_route_segment
 */
gboolean location_route_set_route_segment (LocationRoute *route, GList* segment);

/**
 * @brief Set the start point #LocationPosition of #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] segment - a #LocationRouteSegment
 * @param [in] start - a #LocationPosition
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_segment_get_start_point
 */
gboolean location_route_segment_set_start_point (LocationRouteSegment *segment, const LocationPosition *start);

/**
 * @brief Set the end point #LocationPosition of #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] segment - a #LocationRouteSegment
 * @param [in] end - a #LocationPosition
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_segment_get_end_point
 */
gboolean location_route_segment_set_end_point (LocationRouteSegment *segment, const LocationPosition *end);

/**
 * @brief Set the bounding box #LocationBoundary of #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] segment - a #LocationRouteSegment
 * @param [in] bbox - a #LocationBoundary
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_segment_get_bounding_box
 */
gboolean location_route_segment_set_bounding_box (LocationRouteSegment *segment, const LocationBoundary *bbox);

/**
 * @brief Set the distance of #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] segment - a #LocationRouteSegment
 * @param [in] distance - gdouble
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_segment_get_distance
 */
gboolean location_route_segment_set_distance (LocationRouteSegment *segment, gdouble distance);

/**
 * @brief Set the duration of #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] segment - a #LocationRouteSegment
 * @param [in] duration - glong
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_segment_get_duration
 */
gboolean location_route_segment_set_duration (LocationRouteSegment *segment, glong duration);

/**
 * @brief Set the property value of #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] segment - a #LocationRouteSegment
 * @param [in] key - gconstpointer
 * @param [in] value - gconstpointer
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_segment_get_property
 */
gboolean location_route_segment_set_property (LocationRouteSegment *segment, gconstpointer key, gconstpointer value);

/**
 * @brief Set the list of route steps #LocationRouteStep in #LocationRouteSegment
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] segment - a #LocationRouteSegment
 * @param [in] step - a list of #locationRouteStep
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_segment_get_route_step
 */
gboolean location_route_segment_set_route_step (LocationRouteSegment *segment, GList* step);

/**
 * @brief Set the start point #LocationPosition of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @param [in] start - a #LocationPosition
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_step_get_start_point
 */
gboolean location_route_step_set_start_point (LocationRouteStep *step, const LocationPosition *start);

/**
 * @brief Set the end point #LocationPosition of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @param [in] end - a #LocationPosition
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_step_get_end_point
 */
gboolean location_route_step_set_end_point (LocationRouteStep *step, const LocationPosition *end);

/**
 * @brief Set the bounding box #LocationBoundary of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @param [in] bbox - a #LocationBoundary
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_step_get_bounding_box
 */
gboolean location_route_step_set_bounding_box (LocationRouteStep *step, const LocationBoundary *bbox);

/**
 * @brief Set the distance of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @param [in] distance - gdouble
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_step_get_distance
 */
gboolean location_route_step_set_distance (LocationRouteStep *step, gdouble distance);

/**
 * @brief Set the duration of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @param [in] duration - glong
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_step_get_duration
 */
gboolean location_route_step_set_duration (LocationRouteStep *step, glong duration);

/**
 * @brief Set the transport mode of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @param [in] transport_mode - gchar
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_step_get_transport_mode
 */
gboolean location_route_step_set_transport_mode (LocationRouteStep *step, const gchar *transport_mode);

/**
 * @brief Set the instruction of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @param [in] instruction - gchar
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_step_get_instruction
 */
gboolean location_route_step_set_instruction (LocationRouteStep *step, const gchar *instruction);

/**
 * @brief Set the list of geometry #LocationPosition in #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @param [in] geometry - a list of #LocationPosition
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_step_get_geometry
 */
gboolean location_route_step_set_geometry (LocationRouteStep *step, GList *geometry);

/**
 * @brief Set the property value of #LocationRouteStep
 * @remarks The service provider should support route service.
 * @pre None.
 * @post None.
 * @param [in] step - a #LocationRouteStep
 * @param [in] key - gconstpointer
 * @param [in] value - gconstpointer
 * @return gboolean
 * @retval  TRUE if success
 * @see location_route_step_get_property
 */
gboolean location_route_step_set_property (LocationRouteStep *step, gconstpointer key, gconstpointer value);

/**
  * @} @}
  */

G_END_DECLS

#endif
