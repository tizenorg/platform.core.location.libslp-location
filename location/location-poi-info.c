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

#include "location/location-poi-info.h"
#include "location/location-log.h"

GType
location_poiinfo_get_type (void)
{
	static volatile gsize type_volatile = 0;
	if(g_once_init_enter(&type_volatile)) {
		GType type = g_boxed_type_register_static (
			g_intern_static_string ("LocationPOIInfo"),
			(GBoxedCopyFunc) location_poiinfo_copy,
			(GBoxedFreeFunc) location_poiinfo_free);
		g_once_init_leave(&type_volatile, type);
	}
	return type_volatile;
}

GHashTable*
location_poiinfo_detail_properties_new (void)
{
	return g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
}

void
location_poiinfo_detail_properties_free (GHashTable* properties)
{
	g_return_if_fail(properties);
	g_hash_table_destroy(properties);
}

gboolean
location_poiinfo_detail_properties_insert (GHashTable* properties,
	const gchar* key,
	const gchar* value)
{
	g_return_val_if_fail(properties, FALSE);
	g_hash_table_insert (properties, g_strdup(key), g_strdup(value));
	return TRUE;
}

static void
copy_poiinfo_detail_properties (gchar *key,
	gchar *value,
	GHashTable *properties)
{
	location_poiinfo_detail_properties_insert(properties, key, value);
}

GHashTable *
location_poiinfo_detail_properties_copy (const GHashTable *properties)
{
	g_return_val_if_fail(properties, NULL);

	GHashTable *dup = location_poiinfo_detail_properties_new ();
	g_hash_table_foreach (properties,
						(GHFunc)copy_poiinfo_detail_properties,
						dup);
	return dup;
}

LocationPOIInfo *
location_poiinfo_new (int num_of_poi)
{
	LocationPOIInfo* poi_info = g_slice_new0(LocationPOIInfo);
	poi_info->num_of_poi = num_of_poi;
	poi_info->poi_detail = g_new0(LocationPOIDetail, poi_info->num_of_poi);
	return poi_info;
}

void
location_poiinfo_free (LocationPOIInfo* poi_info)
{
	g_return_if_fail(poi_info);
	if(poi_info->poi_detail){
		int i = 0;
		for (i = 0 ; i < poi_info->num_of_poi ; i++) {
			g_free(poi_info->poi_detail[i].name);
			g_free(poi_info->poi_detail[i].type);
			location_address_free(poi_info->poi_detail[i].address);
			location_position_free(poi_info->poi_detail[i].position);
			g_free(poi_info->poi_detail[i].phone_number);
			location_poiinfo_detail_properties_free(poi_info->poi_detail[i].properties);
		}
	}
	g_free(poi_info->poi_detail);
	g_slice_free(LocationPOIInfo, poi_info);
}

LocationPOIInfo *
location_poiinfo_copy (const LocationPOIInfo* poi_info)
{
	LocationPOIInfo* poi_info_dup = location_poiinfo_new(poi_info->num_of_poi);
	int i = 0;
	for (i = 0 ; i < poi_info_dup->num_of_poi ; i++)
		location_poiinfo_set_poi_detail (poi_info_dup, i,
										poi_info->poi_detail[i].name,
										poi_info->poi_detail[i].type,
										poi_info->poi_detail[i].address,
										poi_info->poi_detail[i].position,
										poi_info->poi_detail[i].phone_number,
										poi_info->poi_detail[i].properties);

	return poi_info_dup;
}

gboolean
location_poiinfo_get_poi_detail (LocationPOIInfo *poi_info,
	guint index,
	gchar** name,
	gchar** type,
	LocationAddress** address,
	LocationPosition** position,
	gchar** phone_number,
	GHashTable** properties)
{
	LOCATION_LOGD("location_poiinfo_get_poi_detail");
	g_return_val_if_fail(name, FALSE);
	g_return_val_if_fail(type, FALSE);
	g_return_val_if_fail(address, FALSE);
	g_return_val_if_fail(position, FALSE);
	g_return_val_if_fail(phone_number, FALSE);
	g_return_val_if_fail(properties, FALSE);
	g_return_val_if_fail(index < poi_info->num_of_poi, FALSE);

	*name = g_strdup(poi_info->poi_detail[index].name);
	*type = g_strdup(poi_info->poi_detail[index].type);
	*address = location_address_copy(poi_info->poi_detail[index].address);
	*position = location_position_copy(poi_info->poi_detail[index].position);
	*phone_number = g_strdup(poi_info->poi_detail[index].phone_number);
	*properties = location_poiinfo_detail_properties_copy(poi_info->poi_detail[index].properties);

	return TRUE;
}

gboolean
location_poiinfo_set_poi_detail (LocationPOIInfo *poi_info,
	guint index,
	const gchar* name,
	const gchar* type,
	const LocationAddress* address,
	const LocationPosition* position,
	const gchar* phone_number,
	const GHashTable* properties)
{
	LOCATION_LOGD("location_poiinfo_set_poi_detail");
	g_return_val_if_fail(name, FALSE);
	g_return_val_if_fail(type, FALSE);
	g_return_val_if_fail(address, FALSE);
	g_return_val_if_fail(position, FALSE);
	g_return_val_if_fail(phone_number, FALSE);
	g_return_val_if_fail( index < poi_info->num_of_poi, FALSE);

	g_free(poi_info->poi_detail[index].name);
	g_free(poi_info->poi_detail[index].type);
	location_address_free(poi_info->poi_detail[index].address);
	location_position_free(poi_info->poi_detail[index].position);
	g_free(poi_info->poi_detail[index].phone_number);
	location_poiinfo_detail_properties_free(poi_info->poi_detail[index].properties);

	poi_info->poi_detail[index].name = g_strdup(name);
	poi_info->poi_detail[index].type = g_strdup(type);
	poi_info->poi_detail[index].address = location_address_copy(address);
	poi_info->poi_detail[index].position = location_position_copy(position);
	poi_info->poi_detail[index].phone_number = g_strdup(phone_number);
	poi_info->poi_detail[index].properties = location_poiinfo_detail_properties_copy(properties);

	return TRUE;
}
