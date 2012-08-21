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

#ifndef __LOCATION_LANDMARK_EXT_H__
#define __LOCATION_LANDMARK_EXT_H__

#include <location-map-types.h>

G_BEGIN_DECLS

/**
 * @file location-landmark-ext.h
 * @brief This file contains the extensional definitions and structures related to Landmark.
 */

/**
 * @addtogroup LocationMapServicePOI
 * @{
 * @defgroup LocationMapServiceLandmarkExt Location Landmark Ext
 * @brief This provides Location Landmark Ext APIs
 * @addtogroup LocationMapServiceLandmarkExt
 * @{
 */

/**
 * @brief Set the ID of the given #LocationLandmark
 * @remarks None.
 * @pre None.
 * @post None.
 * @param [in] landmark - a #LocationLandmark
 * @param [in] id - a guint
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_get_id
 */
gboolean location_landmark_set_id (LocationLandmark *landmark, guint id);

/**
 * @brief Set the name of the given #LocationLandmark
 * @remarks The previous name of the #LocationLandmark will be removed if a name is NULL.
 * @pre None.
 * @post None.
 * @param [in] landmark - a #LocationLandmark
 * @param [in] name - a gchar
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_get_name
 */
gboolean location_landmark_set_name (LocationLandmark *landmark, const gchar *name);

/**
 * @brief Set the position in the given #LocationLandmark
 * @remarks The previous position of the #LocationLandmark will be removed if a position is NULL.
 * @pre None.
 * @post None.
 * @param [in] landmark - a #LocationLandmark
 * @param [in] position - a #LocationPosition
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_get_position
 */
gboolean location_landmark_set_position (LocationLandmark *landmark, const LocationPosition *position);

/**
 * @brief Set the address of the given #LocationLandmark
 * @remarks The previous address of the #LocationLandmark will be removed if an addr is NULL.
 * @pre None.
 * @post None.
 * @param [in] landmark - a #LocationLandmark
 * @param [in] addr - a #LocationAddress
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_get_address
 */
gboolean location_landmark_set_address (LocationLandmark *landmark, const LocationAddress *addr);

/**
 * @brief Set the description of the given #LocationLandmark
 * @remarks The previous description of the #LocationLandmark will be removed if a desc is NULL.
 * @pre None.
 * @post None.
 * @param [in] landmark - a #LocationLandmark
 * @param [in] desc - a gchar
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_get_description
 */
gboolean location_landmark_set_description (LocationLandmark *landmark, const gchar *desc);

/**
 * @brief Set the timestamp of the given #LocationLandmark
 * @remarks None.
 * @pre None.
 * @post None.
 * @param [in] landmark - a #LocationLandmark
 * @param [in] timestamp - a guint
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_get_timestamp
 */
gboolean location_landmark_set_timestamp (LocationLandmark *landmark, guint timestamp);

/**
 * @brief Set the priority of the given #LocationLandmark
 * @remarks None.
 * @pre None.
 * @post None.
 * @param [in] landmark - a #LocationLandmark
 * @param [in] priority - a guint
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_get_priority
 */
gboolean location_landmark_set_priority (LocationLandmark *landmark, guint priority);

/**
 * @brief Set the boundary of the given #LocationLandmark
 * @remarks The previous bounding box of the #LocationLandmark will be removed if a bbox is NULL.
 * @pre None.
 * @post None.
 * @param [in] landmark - a #LocationLandmark
 * @param [in] bbox - a #LocationBoundary
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_get_bounding_box
 */
gboolean location_landmark_set_bounding_box (LocationLandmark *landmark, const LocationBoundary *bbox);

/**
 * @brief Set the author of the given #LocationLandmark
 * @remarks The previous author of the #LocationLandmark will be removed if a author is NULL.
 * @pre None.
 * @post None.
 * @param [in] landmark - a #LocationLandmark
 * @param [in] author - a gchar
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_get_author
 */
gboolean location_landmark_set_author (LocationLandmark *landmark, const gchar *author);

/**
 * @brief Set the urls of the given #LocationLandmark
 * @remarks The previous url of the #LocationLandmark will be removed if an url is NULL.
 * @pre None.
 * @post None.
 * @param [in] landmark - a #LocationLandmark
 * @param [in] url - a GList
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_get_url
 */
gboolean location_landmark_set_url (LocationLandmark *landmark, GList *url);

/**
 * @brief Set the categories of the given #LocationLandmark
 * @remarks The previous categories of the #LocationLandmark will be removed if a category is NULL.
 * @pre None.
 * @post None.
 * @param [in] landmark - a #LocationLandmark
 * @param [in] category - a GList
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_get_category
 */
gboolean location_landmark_set_category (const LocationLandmark *landmark, GList *category);

/**
 * @brief Set the phone number of the given #LocationLandmark
 * @remarks The previous phone number of the #LocationLandmark will be removed if a number is NULL.
 * @pre None.
 * @post None.
 * @param [in] landmark - a #LocationLandmark
 * @param [in] number - a gchar
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_get_phone_number
 */
gboolean location_landmark_set_phone_number (const LocationLandmark *landmark, const gchar *number);

/**
 * @brief Set the property data of the given #LocationLandmark
 * @remarks The previous value of the #LocationLandmark matching to key will be removed if a value is NULL.
 * @pre None.
 * @post None.
 * @param [in] landmark - a #LocationLandmark
 * @param [in] key - a gconstpointer
 * @param [in] value - a gconstpointer
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_get_property
 */
gboolean location_landmark_set_property (const LocationLandmark *landmark, gconstpointer key, gconstpointer value);

/**
 * @brief Set url of the given #LocationLandmarkUrl
 * @remarks The previous url path of the #LocationLandmarkUrl will be removed if a path is NULL.
 * @pre None.
 * @post None.
 * @param [in] url - a #LocationLandmarkUrl
 * @param [in] path - a gchar
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_url_get_url_path
 */
gboolean location_landmark_url_set_url_path (LocationLandmarkUrl *url, const gchar *path);

/**
 * @brief Set the description of the given #LocationLandmarkUrl
 * @remarks The previous description of the #LocationLandmarkUrl will be removed if a desc is NULL.
 * @pre None.
 * @post None.
 * @param [in] url - a #LocationLandmarkUrl
 * @param [in] desc - a gchar
 * @return gboolean
 * @retval  TRUE if success
 * @see location_landmark_url_get_description
 */
gboolean location_landmark_url_set_description (LocationLandmarkUrl *url, const gchar *desc);

/**
 * @} @}
 */

G_END_DECLS

#endif


