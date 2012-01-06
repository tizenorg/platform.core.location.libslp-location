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

#ifndef __LOCATION_POI_INFO_H__
#define __LOCATION_POI_INFO_H__

#include <location/location-types.h>
#include <location/location-position.h>
#include <location/location-address.h>

/**
 * @file location-poi-info.h
 * @brief This file contains the internal definitions and structures related to POI information.
 * @addtogroup LocationTypes
 * @{
 */

G_BEGIN_DECLS

/**
 * @brief This represents one POI information such as name, type of POI, etc.
 */
typedef struct _LocationPOIDetail {
	gchar *name;                   // Name of a POI
	gchar *type;                   // Type of a POI (e.g. cafe)
	LocationAddress *address;      // Address of a POI
	LocationPosition *position;    // Position of a POI
	gchar *phone_number;           // Phone number of a POI
	GHashTable *properties;        // GHashTable with detailed characteristics of a POI.
} LocationPOIDetail;

/**
 * @brief This represents a number of POI informations.
 */
struct _LocationPOIInfo
{
	guint num_of_poi;
	LocationPOIDetail *poi_detail;
};

GType location_poiinfo_get_type (void);
#define LOCATION_TYPE_POIINFO (location_poiinfo_get_type ())

/**
 * @brief   Create a new GHashTable for properties in a #LocationPOIDetail.
 * @remarks None.
 * @pre     #location_init should be called before.\n
 * @post    None.
 * @param   None.
 * @return  a new GHashTable.
 * @retval  NULL if error occured
 */
GHashTable *location_poiinfo_detail_properties_new (void);

/**
 * @brief   Free a GHashTable for properties in a #LocationPOIDetail.
 * @remarks None.
 * @pre     #location_init should be called before.\n
 * @post    None.
 * @param [in] properties - a #GHashTable.
 * @return None.
 */
void location_poiinfo_detail_properties_free (GHashTable* properties);

/**
 * @brief   Insert a new key and value into a GHashTable for properties in a #LocationPOIDetail.
 * @remarks None.
 * @pre     #location_init should be called before.\n
 * @post    None.
 * @param [in]  properties - a #GHashTable in a #LocationPOIDetail.
 * @param [in]  key - a key to insert (e.g. URL)
 * @param [in]  value - the value to associate with the key. (e.g. http://www.samsung.com)
 * @return gboolean
 * @retval\n
 * TRUE - if success\n
 * FALSE - if error occured\n
 */
gboolean location_poiinfo_detail_properties_insert (GHashTable* properties, const gchar* key, const gchar* value);

/**
 * @brief   Makes a copy of GHashTable for properties in a #LocationPOIDetail.
 * @remarks None.
 * @pre     #location_init should be called before.\n
 * @post    None.
 * @param [in]  properties - a #GHashTable
 * @return a new #GHashTable
 * @retval NULL if error occured
 */
GHashTable *location_poiinfo_detail_properties_copy (const GHashTable *properties);

/**
 * @brief   Create a new #LocationPOIInfo with given number of #LocationPOIDetail.
 * @remarks None.
 * @pre     #location_init should be called before.\n
 * @post    None.
 * @param [in]  num_of_poi - number of #LocationPOIDetail.
 * @return a new #LocationPOIInfo
 * @retval NULL if error occured
 */
LocationPOIInfo* location_poiinfo_new (int num_of_poi);

/**
 * @brief   Free a #LocationPOIInfo.
 * @remarks None.
 * @pre     #location_init should be called before.\n
 * @post    None.
 * @param [in] poi_info - a #LocationPOIInfo.
 * @return None.
 */
void location_poiinfo_free(LocationPOIInfo* poi_info);

/**
 * @brief   Makes a copy of #LocationPOIInfo
 * @remarks None.
 * @pre     #location_init should be called before.\n
 * @post    None.
 * @param [in]  poi_info - a #LocationPOIInfo
 * @return a new #LocationPOIInfo
 * @retval NULL if error occured
 */
LocationPOIInfo *location_poiinfo_copy (const LocationPOIInfo* poi_info);

/**
 * @brief   Get elements of #LocationPOIDetail with given index in #LocationPOIInfo.
 * @remarks None.
 * @pre     #location_init should be called before.\n
 * @post    None.
 * @param [in]   poi_info - a #LocationPOIInfo
 * @param [in]   index - index of #LocationPOIDetail in #LocationPOIInfo
 * @param [out]  name - a name of a #LocationPOIDetail
 * @param [out]  type - a type of a #LocationPOIDetail
 * @param [out]  address - a address of a #LocationPOIDetail
 * @param [out]  position - a position of a#LocationPOIDetail
 * @param [out]  phone_number - a phone number of #LocationPOIDetail
 * @param [out]  properties - a GHashTable with properties of #LocationPOIDetail
 * @return gboolean
 * @retval\n
 * TRUE - if success\n
 * FALSE - if error occured\n
 */
gboolean location_poiinfo_get_poi_detail(LocationPOIInfo *poi_info, guint index, gchar** name, gchar** type, LocationAddress** address, LocationPosition** position, gchar** phone_number, GHashTable** properties);

/**
 * @brief   Set elements of #LocationPOIDetail with given index in #LocationPOIInfo.
 * @remarks None.
 * @pre     #location_init should be called before.\n
 * @post    None.
 * @param [in]  poi_info - a #LocationPOIInfo
 * @param [in]  index - index of #LocationPOIDetail in #LocationPOIInfo
 * @param [in]  name - a name of a #LocationPOIDetail
 * @param [in]  type - a type of a #LocationPOIDetail
 * @param [in]  address - a address of a #LocationPOIDetail
 * @param [in]  position - a position of a #LocationPOIDetail
 * @param [in]  phone_number - a phone number of a #LocationPOIDetail
 * @param [in]  properties - a GHashTable with properties of a #LocationPOIDetail, can be NULL if a POI have no additional properties
 * @return gboolean
 * @retval\n
 * TRUE - if success\n
 * FALSE - if error occured\n
 */
gboolean location_poiinfo_set_poi_detail(LocationPOIInfo *poi_info, guint index, const gchar* name, const gchar* type, const LocationAddress* address, const LocationPosition* position, const gchar* phone_number, const GHashTable* properties);

/**
 * @}
 */

G_END_DECLS

#endif
