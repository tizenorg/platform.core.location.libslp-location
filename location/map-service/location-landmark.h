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

#ifndef __LOCATION_LANDMARK_H__
#define __LOCATION_LANDMARK_H__

#include <location-map-types.h>

G_BEGIN_DECLS

/**
 * @file location-landmark.h
 * @brief This file contains the internal definitions and structures related to Landmark.
 */

/**
 * @addtogroup LocationMapServicePOI
 * @{
 * @defgroup LocationMapServiceLandmark Location Landmark
 * @brief This provides structure, enumeration and APIs for Landmark
 * @addtogroup LocationMapServiceLandmark
 * @{
 */

/**
 * @brief Create a new Landmark
 */
LocationLandmark *location_landmark_new (void);

/**
 * @brief Copy the given Landmark
 */
LocationLandmark *location_landmark_copy (const LocationLandmark *landmark);

/**
 * @brief Free the given Landmark
 */
void location_landmark_free (LocationLandmark *landmark);

/**
 * @brief Get the ID of the given landmark
 */
guint location_landmark_get_id (const LocationLandmark *landmark);

/**
 * @brief Get the name of the given landmark
 */
gchar *location_landmark_get_name (const LocationLandmark *landmark);
/**
 * @brief Get the position in the given landmark
 */
LocationPosition *location_landmark_get_position (const LocationLandmark *landmark);
/**
 * @brief Get the address of the given landmark
 */
LocationAddress *location_landmark_get_address (const LocationLandmark *landmark);
/**
 * @brief Get the description of the given landmark
 */
gchar *location_landmark_get_description (const LocationLandmark *landmark);
/**
 * @brief Get the timestamp of the given landmark
 */
guint location_landmark_get_timestamp (const LocationLandmark *landmark);
/**
 * @brief Get the priority of the given landmark
 */
guint location_landmark_get_priority (const LocationLandmark *landmark);
/**
 * @brief Get the bounding box of the given landmark
 */
LocationBoundary *location_landmark_get_bounding_box (const LocationLandmark *landmark);
/**
 * @brief Get the author of the given landmark
 */
gchar *location_landmark_get_author (const LocationLandmark *landmark);
/**
 * @brief Get the list of urls of the given landmark
 */
GList *location_landmark_get_url (const LocationLandmark *landmark);
/**
 * @brief Get the categories of the given landmark
 */
GList *location_landmark_get_category (const LocationLandmark *landmark);
/**
 * @brief Get the phone number of the given landmark
 */
gchar *location_landmark_get_phone_number (const LocationLandmark *landmark);
/**
 * @brief Get the property keys of the given landmark
 */
GList *location_landmark_get_property_key (const LocationLandmark *landmark);

/**
 * @brief Get the property data of the given landmark
 */
gconstpointer location_landmark_get_property (const LocationLandmark *landmark, gconstpointer key);

/**
 * @brief Create a new #LocationLandmarkUrl
 */
LocationLandmarkUrl * location_landmark_url_new (void);

/**
 * @brief Free a #LocationLandmarkUrl
 */
void location_landmark_url_free (LocationLandmarkUrl *url);

/**
 * @brief Copy a #LocationLandmarkUrl
 */
LocationLandmarkUrl *location_landmark_url_copy (const LocationLandmarkUrl *url);

/**
 * @brief Get the url path of the given #LocationLandmarkUrl
 */
gchar *location_landmark_url_get_url_path (const LocationLandmarkUrl *url);
/**
 * @brief Get the url's description of the given #LocationLandmarkUrl
 */
gchar *location_landmark_url_get_description (const LocationLandmarkUrl *url);

/**
  * @} @}
  */

G_END_DECLS

#endif
