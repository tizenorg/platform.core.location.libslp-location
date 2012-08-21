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

#include "location-types.h"
#include "location-route.h"
#include "location-route-ext.h"
#include "location-boundary.h"
#include "map-service.h"


struct _LocationRoute {
//	gint req_id;				//< Request id : for expandability
	LocationPosition *origin;		//< Coordinate StartCoord
	LocationPosition *destination;		//< Coordinates destCoord
	LocationBoundary *bbox;			//< a rectangular geographical area
	gdouble total_distance;			//< Total distance
	gchar *distance_unit;			//< Distance Unit
	glong total_duration;			//< Total duration
	GHashTable *properties;			//<Key/Value>
	GList *segment;
};

//Waypoints
struct _LocationRouteSegment {
	LocationPosition *start;	//< Coordinate StartCoord;
	LocationPosition *end;		//< Coordinates destCoord;
	LocationBoundary *bbox;		//< a rectangular geographical area
	gdouble distance;
	glong duration;
	GHashTable *properties;		//<Key/Value>
	GList *step;
};

// Each instruction
struct _LocationRouteStep {
	LocationPosition *start;	//< Coordinate StartCoord;
	LocationPosition *end;		//< Coordinates destCoord;
	LocationBoundary *bbox;		//< a rectangular geographical area
	gdouble distance;
	glong duration;
	gchar *transport_mode;
	gchar *instruction;
	GList *geometry;

	GHashTable *properties;
};

struct _LocationRoutePreference {
	GList* addr_to_avoid;
	GList* area_to_avoid;
	GList* feature_to_avoid;
	GList* freeformed_addr_to_avoid;
	LocationBoundary* bbox;
	guint max_matches_count;
	gchar *distance_unit;

	gchar *route_type;
	gchar *transport_mode;
	gboolean is_geometry_used;
	gboolean is_instruction_bounding_box_used;
	gboolean is_instruction_geometry_used;
	gboolean is_instruction_used;
	gboolean is_traffic_data_used;

	// AvoidFreeways, Easy, Fastest, MoreFreeways, NoFreeways, Pedestrian, Shortest
	GHashTable *properties;

};

static void route_pref_addr_to_avoid_copy_cb (gpointer data, gpointer user_data)
{
	g_return_if_fail(data);
	g_return_if_fail(user_data);

	LocationAddress *address = (LocationAddress *)data;
	LocationRoutePreference *pref = (LocationRoutePreference *)user_data;

	pref->addr_to_avoid = g_list_append (pref->addr_to_avoid, location_address_copy(address));
}

static void addr_to_avoid_free_cb (gpointer data)
{
	g_return_if_fail (data);

	LocationAddress *addr = (LocationAddress *)data;

	location_address_free (addr);
}

EXPORT_API gboolean
location_route_pref_set_addr_to_avoid (LocationRoutePreference *pref, GList *addr)
{
	g_return_val_if_fail(pref, FALSE);

	if (pref->addr_to_avoid) {
		g_list_free_full (pref->addr_to_avoid, addr_to_avoid_free_cb);
		pref->addr_to_avoid = NULL;
	}

	if (addr) g_list_foreach (addr, route_pref_addr_to_avoid_copy_cb, pref);

	return TRUE;
}

static void route_pref_area_to_avoid_copy_cb (gpointer data, gpointer user_data)
{
	g_return_if_fail(data);
	g_return_if_fail(user_data);

	LocationBoundary *area = (LocationBoundary *)data;
	LocationRoutePreference *pref = (LocationRoutePreference *)user_data;

	pref->area_to_avoid = g_list_append (pref->area_to_avoid, location_boundary_copy((const LocationBoundary *)area));
}

static void route_pref_area_to_avoid_free_cb (gpointer data)
{
	g_return_if_fail (data);

	LocationBoundary *boundary = (LocationBoundary *)data;

	location_boundary_free (boundary);
}

EXPORT_API gboolean
location_route_pref_set_area_to_avoid (LocationRoutePreference *pref, GList *area)
{
	g_return_val_if_fail(pref, FALSE);

	if (pref->area_to_avoid) {
		g_list_free_full (pref->area_to_avoid, route_pref_area_to_avoid_free_cb);
		pref->area_to_avoid = NULL;
	}

	if (area) g_list_foreach (area, route_pref_area_to_avoid_copy_cb, pref);

	return TRUE;
}

static void route_pref_feature_to_avoid_copy_cb (gpointer data, gpointer user_data)
{
	g_return_if_fail (data);
	g_return_if_fail (user_data);

	gchar *feature = (gchar *)data;
	LocationRoutePreference *pref = (LocationRoutePreference *) user_data;

	pref->feature_to_avoid = g_list_append (pref->feature_to_avoid, g_strdup (feature));
}

EXPORT_API gboolean
location_route_pref_set_feature_to_avoid (LocationRoutePreference *pref, GList * feature)
{
	g_return_val_if_fail(pref, FALSE);

	if (pref->feature_to_avoid) {
		g_list_free_full (pref->feature_to_avoid, g_free);
		pref->feature_to_avoid = NULL;
	}

	g_list_foreach (feature, route_pref_feature_to_avoid_copy_cb, pref);

	return TRUE;
}

static void route_pref_freeforemd_addr_to_avoid_foreach_copy (gpointer data, gpointer user_data)
{
	g_return_if_fail(data);
	g_return_if_fail(user_data);

	gchar *freeformed_addr = (gchar *)data;
	LocationRoutePreference *pref = (LocationRoutePreference *)user_data;

	pref->freeformed_addr_to_avoid = g_list_append (pref->freeformed_addr_to_avoid, g_strdup(freeformed_addr));
}

EXPORT_API gboolean
location_route_pref_set_freeformed_addr_to_avoid (LocationRoutePreference *pref, GList *freeformed_addr)
{
	g_return_val_if_fail(pref, FALSE);

	if (pref->freeformed_addr_to_avoid) {
		g_list_free_full (pref->freeformed_addr_to_avoid, g_free);
		pref->freeformed_addr_to_avoid = NULL;
	}

	if (freeformed_addr) g_list_foreach (freeformed_addr, route_pref_freeforemd_addr_to_avoid_foreach_copy, pref);

	return TRUE;
}

EXPORT_API gboolean
location_route_pref_set_bounding_box (LocationRoutePreference *pref, const LocationBoundary *bbox)
{
	g_return_val_if_fail(pref, FALSE);

	if (pref->bbox) {
		location_boundary_free (pref->bbox);
		pref->bbox = NULL;
	}

	if (bbox) pref->bbox = location_boundary_copy (bbox);

	return TRUE;
}

EXPORT_API gboolean
location_route_pref_set_max_result (LocationRoutePreference *pref, guint max_num)
{
	g_return_val_if_fail(pref, FALSE);
	g_return_val_if_fail(max_num > 0, FALSE);

	pref->max_matches_count = max_num;

	return TRUE;
}

EXPORT_API gboolean
location_route_pref_set_route_type (LocationRoutePreference *pref, const gchar *type)
{
	g_return_val_if_fail(pref, FALSE);

	if (pref->route_type) {
		g_free(pref->route_type);
		pref->route_type = NULL;
	}

	if (type) pref->route_type = g_strdup (type);

	return TRUE;
}

EXPORT_API gboolean
location_route_pref_set_transport_mode (LocationRoutePreference *pref, const gchar *mode)
{
	g_return_val_if_fail(pref, FALSE);

	if (pref->transport_mode) {
		g_free(pref->transport_mode);
		pref->transport_mode = NULL;
	}

	if (mode) pref->transport_mode = g_strdup (mode);

	return TRUE;
}

EXPORT_API gboolean
location_route_pref_set_geometry_used (LocationRoutePreference *pref, gboolean is_used)
{
	g_return_val_if_fail(pref, FALSE);

	pref->is_geometry_used = is_used;

	return TRUE;
}

EXPORT_API gboolean
location_route_pref_set_instruction_bounding_box_used (LocationRoutePreference *pref, gboolean is_used)
{
	g_return_val_if_fail(pref, FALSE);

	pref->is_instruction_bounding_box_used = is_used;

	return TRUE;
}

EXPORT_API gboolean
location_route_pref_set_instruction_geometry_used (LocationRoutePreference *pref, gboolean is_used)
{
	g_return_val_if_fail(pref, FALSE);

	pref->is_instruction_geometry_used = is_used;

	return TRUE;
}

EXPORT_API gboolean
location_route_pref_set_instruction_used (LocationRoutePreference *pref, gboolean is_used)
{
	g_return_val_if_fail(pref, FALSE);

	pref->is_instruction_used = is_used;

	return TRUE;
}

EXPORT_API gboolean
location_route_pref_set_traffic_data_used (LocationRoutePreference *pref, gboolean is_used)
{
	g_return_val_if_fail(pref, FALSE);

	pref->is_traffic_data_used = is_used;

	return TRUE;
}

EXPORT_API gboolean
location_route_pref_set_property (LocationRoutePreference *pref, gconstpointer key, gconstpointer value)
{
	g_return_val_if_fail(pref, FALSE);
	g_return_val_if_fail(key, FALSE);

	if (value) {
		gchar *re_key = g_strdup(key);
		gchar *re_val = g_strdup(value);
		g_hash_table_insert (pref->properties, re_key, re_val);
	} else g_hash_table_remove (pref->properties, key);

	return TRUE;
}

EXPORT_API GList *
location_route_pref_get_addr_to_avoid (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, NULL);

	return pref->addr_to_avoid;
}

EXPORT_API GList *
location_route_pref_get_area_to_avoid (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, NULL);

	return pref->area_to_avoid;
}

EXPORT_API GList *
location_route_pref_get_feature_to_avoid (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, NULL);

	return pref->feature_to_avoid;
}

EXPORT_API GList *
location_route_pref_get_freeformed_addr_to_avoid (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, NULL);

	return pref->freeformed_addr_to_avoid;
}

EXPORT_API LocationBoundary *
location_route_pref_get_bounding_box (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, NULL);

	return pref->bbox;
}

EXPORT_API guint
location_route_pref_get_max_result (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, 0);

	return pref->max_matches_count;
}

EXPORT_API gchar *
location_route_pref_get_route_type (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, NULL);

	return pref->route_type;
}

EXPORT_API gchar *
location_route_pref_get_transport_mode (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, NULL);

	return pref->transport_mode;
}

EXPORT_API gboolean
location_route_pref_get_geometry_used (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, FALSE);

	return pref->is_geometry_used;
}

EXPORT_API gboolean
location_route_pref_get_instruction_bounding_box_used (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, FALSE);

	return pref->is_instruction_bounding_box_used;
}

EXPORT_API gboolean
location_route_pref_get_instruction_geometry_used (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, FALSE);

	return pref->is_instruction_geometry_used;
}

EXPORT_API gboolean
location_route_pref_get_instruction_used (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, FALSE);

	return pref->is_instruction_used;
}

EXPORT_API gboolean
location_route_pref_get_traffic_data_used (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, FALSE);

	return pref->is_traffic_data_used;
}

EXPORT_API GList*
location_route_pref_get_property_key (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, NULL);

	return g_hash_table_get_keys(pref->properties);
}

EXPORT_API gpointer
location_route_pref_get_property (const LocationRoutePreference *pref, gconstpointer key)
{
	g_return_val_if_fail(pref, NULL);
	g_return_val_if_fail(key, NULL);

	return g_hash_table_lookup(pref->properties, key);
}

EXPORT_API LocationRoutePreference *
location_route_pref_new (void)
{
	LocationRoutePreference *pref = g_slice_new0(LocationRoutePreference);
	g_return_val_if_fail (pref, NULL);

	pref->properties = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	return pref;
}

static void route_pref_property_copy_cb (gpointer key, gpointer value, gpointer user_data)
{
	g_return_if_fail(key);
	g_return_if_fail(value);
	g_return_if_fail(user_data);

	LocationRoutePreference *pref = (LocationRoutePreference *)user_data;

	if (pref->properties) {
		gchar *re_key = g_strdup (key);
		gchar *re_val = g_strdup (value);
		g_hash_table_insert (pref->properties, re_key, re_val);
	}
}

EXPORT_API LocationRoutePreference *
location_route_pref_copy (const LocationRoutePreference *pref)
{
	g_return_val_if_fail(pref, NULL);

	LocationRoutePreference *new_pref = location_route_pref_new();
	g_return_val_if_fail (pref, NULL);

	location_route_pref_set_addr_to_avoid(new_pref, location_route_pref_get_addr_to_avoid (pref));
	location_route_pref_set_area_to_avoid(new_pref, location_route_pref_get_area_to_avoid (pref));
	location_route_pref_set_feature_to_avoid(new_pref, location_route_pref_get_feature_to_avoid (pref));
	location_route_pref_set_freeformed_addr_to_avoid(new_pref, location_route_pref_get_freeformed_addr_to_avoid (pref));
	location_route_pref_set_bounding_box(new_pref, location_route_pref_get_bounding_box (pref));
	location_route_pref_set_max_result(new_pref, location_route_pref_get_max_result (pref));
	location_route_pref_set_route_type(new_pref, location_route_pref_get_route_type (pref));
	location_route_pref_set_transport_mode(new_pref, location_route_pref_get_transport_mode (pref));
	location_route_pref_set_geometry_used(new_pref, location_route_pref_get_geometry_used (pref));
	location_route_pref_set_instruction_bounding_box_used(new_pref, location_route_pref_get_instruction_bounding_box_used (pref));
	location_route_pref_set_instruction_geometry_used(new_pref, location_route_pref_get_instruction_geometry_used (pref));
	location_route_pref_set_instruction_used(new_pref, location_route_pref_get_instruction_used (pref));
	location_route_pref_set_traffic_data_used(new_pref, location_route_pref_get_traffic_data_used (pref));
	location_route_pref_set_traffic_data_used(new_pref, location_route_pref_get_traffic_data_used (pref));

	if (new_pref->properties) g_hash_table_foreach (pref->properties, route_pref_property_copy_cb, new_pref);

	return new_pref;
}


EXPORT_API void
location_route_pref_free (LocationRoutePreference *pref)
{
	g_return_if_fail(pref);

	location_route_pref_set_addr_to_avoid(pref, NULL);
	location_route_pref_set_area_to_avoid(pref, NULL);
	location_route_pref_set_feature_to_avoid(pref, NULL);
	location_route_pref_set_freeformed_addr_to_avoid(pref, NULL);
	location_route_pref_set_bounding_box(pref, NULL);
	location_route_pref_set_route_type(pref, NULL);
	location_route_pref_set_transport_mode(pref, NULL);

	if (pref->properties) {
		g_hash_table_destroy (pref->properties);
		pref->properties = NULL;
	}

	g_slice_free (LocationRoutePreference, pref);
}

/* for expandability
EXPORT_API gint
location_route_get_req_id (const LocationRoute *route)
{
	g_return_val_if_fail(route, 0);

	return route->req_id;
}
*/

EXPORT_API LocationPosition *
location_route_get_origin (const LocationRoute *route)
{
	g_return_val_if_fail(route, NULL);

	return route->origin;
}

EXPORT_API LocationPosition *
location_route_get_destination (const LocationRoute *route)
{
	g_return_val_if_fail(route, NULL);

	return route->destination;
}

EXPORT_API LocationBoundary *
location_route_get_bounding_box (const LocationRoute *route)
{
	g_return_val_if_fail(route, NULL);

	return route->bbox;
}

EXPORT_API gdouble
location_route_get_total_distance (const LocationRoute *route)
{
	g_return_val_if_fail(route, 0.0);

	return route->total_distance;
}

EXPORT_API gchar *
location_route_get_distance_unit (const LocationRoute *route)
{
	g_return_val_if_fail(route, NULL);

	return route->distance_unit;
}

EXPORT_API glong
location_route_get_total_duration (const LocationRoute *route)
{
	g_return_val_if_fail(route, 0);

	return route->total_duration;
}

EXPORT_API GList *
location_route_get_property_key (const LocationRoute *route)
{
	g_return_val_if_fail(route, NULL);

	return g_hash_table_get_keys(route->properties);
}

EXPORT_API gpointer
location_route_get_property (const LocationRoute *route, gconstpointer key)
{
	g_return_val_if_fail(route, NULL);
	g_return_val_if_fail(key, NULL);

	return g_hash_table_lookup(route->properties, key);
}

EXPORT_API GList *
location_route_get_route_segment (const LocationRoute *route)
{
	g_return_val_if_fail(route, NULL);

	return route->segment;
}

/* for expandability
EXPORT_API gboolean
location_route_set_req_id (LocationRoute *route, gint req_id)
{
	g_return_val_if_fail(route, FALSE);

	route->req_id = req_id;

	return TRUE;
}
*/

EXPORT_API gboolean
location_route_set_origin (LocationRoute *route, const LocationPosition* origin)
{
	g_return_val_if_fail(route, FALSE);

	if (route->origin) {
		location_position_free(route->origin);
		route->origin = NULL;
	}

	if (origin) route->origin = location_position_copy(origin);

	return TRUE;
}

EXPORT_API gboolean
location_route_set_destination (LocationRoute *route, const LocationPosition* destination)
{
	g_return_val_if_fail(route, FALSE);

	if (route->destination) {
		location_position_free(route->destination);
		route->destination = NULL;
	}

	if (destination) route->destination = location_position_copy(destination);

	return TRUE;
}

EXPORT_API gboolean
location_route_set_bounding_box (LocationRoute *route, const LocationBoundary* bbox)
{
	g_return_val_if_fail(route, FALSE);

	if (route->bbox) {
		location_boundary_free(route->bbox);
		route->bbox = NULL;
	}

	if (bbox) route->bbox = location_boundary_copy(bbox);

	return TRUE;
}

EXPORT_API
gboolean location_route_set_total_distance (LocationRoute *route, gdouble total_distance)
{
	g_return_val_if_fail(route, FALSE);

	route->total_distance = total_distance;

	return TRUE;
}

EXPORT_API
gboolean location_route_set_distance_unit (LocationRoute *route, const gchar* distance_unit)
{
	g_return_val_if_fail(route, FALSE);

	if (route->distance_unit) {
		g_free(route->distance_unit);
		route->distance_unit = NULL;
	}

	if (distance_unit) route->distance_unit = g_strdup(distance_unit);

	return TRUE;
}

EXPORT_API gboolean
location_route_set_total_duration (LocationRoute *route, glong total_duration)
{
	g_return_val_if_fail(route, FALSE);

	route->total_duration = total_duration;

	return TRUE;
}

EXPORT_API gboolean
location_route_set_property (LocationRoute *route, gconstpointer key, gconstpointer value)
{
	g_return_val_if_fail(route, FALSE);
	g_return_val_if_fail(key, FALSE);
	if (!route->properties) return FALSE;

	if (value) {
		gchar *re_key = g_strdup (key);
		gchar *re_val = g_strdup (value);
		g_hash_table_insert(route->properties, re_key, re_val);
	} else g_hash_table_remove (route->properties, key);

	return TRUE;
}

static void route_segment_foreach_copy (gpointer data, gpointer user_data)
{
	g_return_if_fail (data);

	LocationRouteSegment *segment = (LocationRouteSegment *) data;
	LocationRoute *route = (LocationRoute *) user_data;

	route->segment = g_list_append (route->segment, location_route_segment_copy (segment));
}

static void route_segment_foreach_free (gpointer data)
{
	g_return_if_fail (data);

	LocationRouteSegment *segment = (LocationRouteSegment *)data;

	location_route_segment_free(segment);
}


EXPORT_API gboolean
location_route_set_route_segment (LocationRoute *route, GList* segment)
{
	g_return_val_if_fail(route, FALSE);

	if (route->segment) {
		g_list_free_full (route->segment, route_segment_foreach_free);
		route->segment = NULL;
	}

	if (segment) g_list_foreach (segment, route_segment_foreach_copy, route);

	return TRUE;
}

EXPORT_API LocationRoute *
location_route_new (void)
{
	LocationRoute *route = g_slice_new0(LocationRoute);
	g_return_val_if_fail (route, NULL);

	route->properties = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

	return route;
}

static void route_property_copy_cb (gpointer key, gpointer value, gpointer user_data)
{
	g_return_if_fail (key);
	g_return_if_fail (value);
	g_return_if_fail (user_data);

	LocationRoute *route = (LocationRoute *)user_data;

	if (route->properties) {
		gchar *re_key = g_strdup (key);
		gchar *re_val = g_strdup (value);
		g_hash_table_insert (route->properties, re_key, re_val);
	}
}

EXPORT_API LocationRoute *
location_route_copy (const LocationRoute *route)
{
	g_return_val_if_fail(route, NULL);
	LocationRoute *new_route = location_route_new();
	g_return_val_if_fail(new_route, NULL);

	location_route_set_origin (new_route, location_route_get_origin(route));
	location_route_set_destination (new_route, location_route_get_destination(route));
	location_route_set_bounding_box (new_route, location_route_get_bounding_box(route));
	location_route_set_total_distance (new_route, location_route_get_total_distance(route));
	location_route_set_distance_unit (new_route, location_route_get_distance_unit(route));
	location_route_set_total_duration (new_route, location_route_get_total_duration(route));

	g_list_foreach(route->segment, route_segment_foreach_copy, new_route);

	if (route->properties) g_hash_table_foreach (route->properties, route_property_copy_cb, new_route);

	return new_route;
}

EXPORT_API void
location_route_free (LocationRoute *route)
{
	g_return_if_fail (route);

	location_route_set_origin (route, NULL);
	location_route_set_destination (route, NULL);
	location_route_set_bounding_box (route, NULL);
	location_route_set_distance_unit (route, NULL);
	location_route_set_route_segment (route, NULL);

	if (route->properties) {
		g_hash_table_destroy (route->properties);
		route->properties = NULL;
	}

	g_slice_free (LocationRoute, route);
}

EXPORT_API LocationPosition *
location_route_segment_get_start_point (const LocationRouteSegment *segment)
{
	g_return_val_if_fail(segment, NULL);

	return segment->start;
}

EXPORT_API LocationPosition *
location_route_segment_get_end_point (const LocationRouteSegment *segment)
{
	g_return_val_if_fail(segment, NULL);

	return segment->end;
}

EXPORT_API LocationBoundary *
location_route_segment_get_bounding_box (const LocationRouteSegment *segment)
{
	g_return_val_if_fail(segment, NULL);

	return segment->bbox;
}

EXPORT_API gdouble
location_route_segment_get_distance (const LocationRouteSegment *segment)
{
	g_return_val_if_fail(segment, 0.0);

	return segment->distance;
}

EXPORT_API glong
location_route_segment_get_duration (const LocationRouteSegment *segment)
{
	g_return_val_if_fail(segment, 0);

	return segment->duration;
}

EXPORT_API GList*
location_route_segment_get_property_key (const LocationRouteSegment *segment)
{
	g_return_val_if_fail(segment, NULL);

	return g_hash_table_get_keys(segment->properties);
}

EXPORT_API gpointer
location_route_segment_get_property (const LocationRouteSegment *segment, gconstpointer key)
{
	g_return_val_if_fail(segment, NULL);
	g_return_val_if_fail(key, NULL);

	return g_hash_table_lookup(segment->properties, (gpointer) key);
}

EXPORT_API GList*
location_route_segment_get_route_step (const LocationRouteSegment *segment)
{
	g_return_val_if_fail(segment, NULL);

	return segment->step;
}

EXPORT_API gboolean
location_route_segment_set_start_point (LocationRouteSegment *segment, const LocationPosition *start)
{
	g_return_val_if_fail (segment, FALSE);

	if (segment->start) {
		location_position_free(segment->start);
		segment->start = NULL;
	}

	if (start) segment->start = location_position_copy (start);

	return TRUE;
}

EXPORT_API gboolean
location_route_segment_set_end_point (LocationRouteSegment *segment, const LocationPosition *end)
{
	g_return_val_if_fail (segment, FALSE);

	if (segment->end) {
		location_position_free(segment->end);
		segment->end = NULL;
	}

	if (end) segment->end = location_position_copy (end);

	return TRUE;

}

EXPORT_API gboolean
location_route_segment_set_bounding_box (LocationRouteSegment *segment, const LocationBoundary *bbox)
{
	g_return_val_if_fail (segment, FALSE);

	if (segment->bbox) {
		location_boundary_free(segment->bbox);
		segment->bbox = NULL;
	}

	if (bbox) segment->bbox = location_boundary_copy (bbox);

	return TRUE;
}

EXPORT_API gboolean
location_route_segment_set_distance (LocationRouteSegment *segment, gdouble distance)
{
	g_return_val_if_fail (segment, FALSE);

	segment->distance = distance;

	return TRUE;
}

EXPORT_API gboolean
location_route_segment_set_duration (LocationRouteSegment *segment, glong duration)
{
	g_return_val_if_fail (segment, FALSE);

	segment->duration = duration;

	return TRUE;
}

EXPORT_API gboolean
location_route_segment_set_property (LocationRouteSegment *segment, gconstpointer key, gconstpointer value)
{
	g_return_val_if_fail (segment, FALSE);
	g_return_val_if_fail (key, FALSE);
	if (!segment->properties) return FALSE;

	if (value) {
		gchar *re_key = g_strdup (key);
		gchar *re_val = g_strdup (value);
		g_hash_table_insert(segment->properties, re_key, re_val);
	} else g_hash_table_remove (segment->properties, key);

	return TRUE;
}

static void route_step_foreach_free (gpointer data)
{
	g_return_if_fail (data);

	LocationRouteStep *step = (LocationRouteStep *) data;

	location_route_step_free(step);
}

static void route_step_foreach_copy (gpointer data, gpointer user_data)
{
	g_return_if_fail (data);
	g_return_if_fail (user_data);

	LocationRouteStep *step = (LocationRouteStep *) data;
	LocationRouteSegment *segment = (LocationRouteSegment *) user_data;

	LocationRouteStep *step_new = location_route_step_copy(step);
	segment->step = g_list_append(segment->step, step_new);

}

EXPORT_API gboolean
location_route_segment_set_route_step (LocationRouteSegment *segment, GList* step)
{
	g_return_val_if_fail (segment, FALSE);

	if (segment->step) {
		g_list_free_full (segment->step, route_step_foreach_free);
		segment->step = NULL;
	}

	if (step) g_list_foreach (step, route_step_foreach_copy, segment);

	return TRUE;

}

EXPORT_API LocationRouteSegment *
location_route_segment_new (void)
{
	LocationRouteSegment *segment = g_slice_new0 (LocationRouteSegment);
	g_return_val_if_fail (segment, NULL);

	segment->properties = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

	return segment;
}

static void segment_property_copy_cb (gpointer key, gpointer value, gpointer user_data)
{
	g_return_if_fail (key);
	g_return_if_fail (value);
	g_return_if_fail (user_data);

	LocationRouteSegment *segment = (LocationRouteSegment *)user_data;

	if (segment->properties) {
		gchar *re_key = g_strdup (key);
		gchar *re_val = g_strdup (value);
		g_hash_table_insert (segment->properties, re_key, re_val);
	}
}

EXPORT_API LocationRouteSegment *
location_route_segment_copy (LocationRouteSegment *segment)
{
	g_return_val_if_fail(segment, NULL);

	LocationRouteSegment *new_segment = location_route_segment_new();
	g_return_val_if_fail(new_segment, NULL);

	location_route_segment_set_start_point(new_segment, location_route_segment_get_start_point(segment));
	location_route_segment_set_end_point(new_segment, location_route_segment_get_end_point(segment));
	location_route_segment_set_bounding_box(new_segment, location_route_segment_get_bounding_box(segment));
	location_route_segment_set_distance(new_segment, location_route_segment_get_distance(segment));
	location_route_segment_set_duration(new_segment, location_route_segment_get_duration(segment));
	location_route_segment_set_route_step(new_segment, location_route_segment_get_route_step(segment));

	if (segment->properties) g_hash_table_foreach (segment->properties, segment_property_copy_cb, new_segment);

	return new_segment;
}

EXPORT_API void
location_route_segment_free (LocationRouteSegment *segment)
{
	g_return_if_fail(segment);

	location_route_segment_set_start_point(segment, NULL);
	location_route_segment_set_end_point(segment, NULL);
	location_route_segment_set_bounding_box(segment, NULL);
	location_route_segment_set_distance(segment, 0.0);
	location_route_segment_set_duration(segment, 0);
	location_route_segment_set_route_step(segment, NULL);

	if (segment->properties) {
		g_hash_table_destroy (segment->properties);
		segment->properties = NULL;
	}

	g_slice_free(LocationRouteSegment, segment);
}

EXPORT_API LocationPosition*
location_route_step_get_start_point (const LocationRouteStep *step)
{
	g_return_val_if_fail(step, NULL);

	return step->start;
}

EXPORT_API LocationPosition*
location_route_step_get_end_point (const LocationRouteStep *step)
{
	g_return_val_if_fail(step, NULL);

	return step->end;
}

EXPORT_API LocationBoundary*
location_route_step_get_bounding_box (const LocationRouteStep *step)
{
	g_return_val_if_fail(step, NULL);

	return step->bbox;
}

EXPORT_API gdouble
location_route_step_get_distance (const LocationRouteStep *step)
{
	g_return_val_if_fail(step, 0.0);

	return step->distance;
}

EXPORT_API glong
location_route_step_get_duration (const LocationRouteStep *step)
{
	g_return_val_if_fail(step, 0);

	return step->duration;
}

EXPORT_API gchar*
location_route_step_get_transport_mode (const LocationRouteStep *step)
{
	g_return_val_if_fail(step, NULL);

	return step->transport_mode;
}

EXPORT_API gchar*
location_route_step_get_instruction (const LocationRouteStep *step)
{
	g_return_val_if_fail(step, NULL);

	return step->instruction;
}

EXPORT_API GList *
location_route_step_get_geometry (const LocationRouteStep *step)
{
	g_return_val_if_fail(step, NULL);

	return step->geometry;
}

EXPORT_API GList*
location_route_step_get_property_key (const LocationRouteStep *step)
{
	g_return_val_if_fail(step, NULL);

	return g_hash_table_get_keys (step->properties);
}

EXPORT_API gpointer
location_route_step_get_property (const LocationRouteStep *step, gconstpointer key)
{
	g_return_val_if_fail(step, NULL);
	g_return_val_if_fail(key, NULL);

	return g_hash_table_lookup (step->properties, key);
}

EXPORT_API gboolean
location_route_step_set_start_point (LocationRouteStep *step, const LocationPosition *start)
{
	g_return_val_if_fail (step, FALSE);

	if (step->start) {
		location_position_free (step->start);
		step->start = NULL;
	}

	if (start) step->start = location_position_copy (start);

	return TRUE;
}

EXPORT_API gboolean
location_route_step_set_end_point (LocationRouteStep *step, const LocationPosition *end)
{
	g_return_val_if_fail (step, FALSE);

	if (step->end) {
		location_position_free (step->end);
		step->end = NULL;
	}

	if (end) step->end = location_position_copy (end);

	return TRUE;
}

EXPORT_API gboolean
location_route_step_set_bounding_box (LocationRouteStep *step, const LocationBoundary *bbox)
{
	g_return_val_if_fail (step, FALSE);

	if (step->bbox) {
		location_boundary_free (step->bbox);
		step->bbox = NULL;
	}

	if (bbox) step->bbox = location_boundary_copy (bbox);

	return TRUE;
}

EXPORT_API gboolean
location_route_step_set_distance (LocationRouteStep *step, gdouble distance)
{
	g_return_val_if_fail (step, FALSE);

	step->distance = distance;

	return TRUE;

}

EXPORT_API gboolean
location_route_step_set_duration (LocationRouteStep *step, glong duration)
{
	g_return_val_if_fail (step, FALSE);

	step->duration = duration;

	return TRUE;
}

EXPORT_API gboolean
location_route_step_set_transport_mode (LocationRouteStep *step, const gchar *transport_mode)
{
	g_return_val_if_fail (step, FALSE);

	if (step->transport_mode) {
		g_free(step->transport_mode);
		step->transport_mode = NULL;
	}

	if (transport_mode) step->transport_mode = g_strdup(transport_mode);

	return TRUE;

}

EXPORT_API gboolean
location_route_step_set_instruction (LocationRouteStep *step, const gchar *instruction)
{
	g_return_val_if_fail (step, FALSE);

	if (step->instruction) {
		g_free(step->instruction);
		step->instruction = NULL;
	}

	if (instruction) step->instruction = g_strdup(instruction);

	return TRUE;

}

static void route_step_geometry_foreach_copy (gpointer data, gpointer user_data)
{
	g_return_if_fail (data);
	g_return_if_fail (user_data);

	LocationPosition *pos = (LocationPosition *)data;
	LocationRouteStep *step = (LocationRouteStep *)user_data;

	step->geometry = g_list_append (step->geometry, location_position_copy (pos));
}

static void route_step_geometry_free (gpointer data)
{
	g_return_if_fail (data);

	LocationPosition *pos = (LocationPosition *)data;

	location_position_free(pos);
}

EXPORT_API gboolean
location_route_step_set_geometry (LocationRouteStep *step, GList *geometry)
{
	g_return_val_if_fail (step, FALSE);

	if (step->geometry) {
		g_list_free_full (step->geometry, route_step_geometry_free);
		step->geometry = NULL;
	}

	if (geometry) g_list_foreach (geometry, route_step_geometry_foreach_copy, step);

	return TRUE;

}

EXPORT_API gboolean
location_route_step_set_property (LocationRouteStep *step, gconstpointer key, gconstpointer value)
{
	g_return_val_if_fail(step, FALSE);
	g_return_val_if_fail(key, FALSE);

	if (!step->properties) return FALSE;

	if (value) {
		gchar *re_key = g_strdup (key);
		gchar *re_val = g_strdup (value);
		g_hash_table_insert (step->properties, re_key, re_val);
	}
	else g_hash_table_remove (step->properties, key);

	return TRUE;
}

EXPORT_API LocationRouteStep *
location_route_step_new (void)
{
	LocationRouteStep *step = g_slice_new0 (LocationRouteStep);
	g_return_val_if_fail(step, NULL);

	step->properties = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

	return step;
}

static void step_property_copy_cb (gpointer key, gpointer value, gpointer user_data)
{
	g_return_if_fail (key);
	g_return_if_fail (value);
	g_return_if_fail (user_data);

	LocationRouteStep *step = (LocationRouteStep *) user_data;

	if (step->properties) {
		gchar *re_key = g_strdup (key);
		gchar *re_val = g_strdup (value);
		g_hash_table_insert (step->properties, re_key, re_val);
	}
}


EXPORT_API LocationRouteStep *
location_route_step_copy (LocationRouteStep *step)
{
	g_return_val_if_fail(step, NULL);

	LocationRouteStep *new_step = location_route_step_new ();
	g_return_val_if_fail (new_step, NULL);

	location_route_step_set_start_point (new_step, location_route_step_get_start_point(step));
	location_route_step_set_end_point (new_step, location_route_step_get_end_point(step));
	location_route_step_set_bounding_box (new_step, location_route_step_get_bounding_box(step));
	location_route_step_set_distance (new_step, location_route_step_get_distance(step));
	location_route_step_set_duration (new_step, location_route_step_get_duration(step));
	location_route_step_set_instruction (new_step, location_route_step_get_instruction(step));
	location_route_step_set_geometry (new_step, location_route_step_get_geometry(step));

	if (step->properties) g_hash_table_foreach (step->properties, step_property_copy_cb, new_step);
	return new_step;
}

EXPORT_API void
location_route_step_free (LocationRouteStep *step)
{
	g_return_if_fail(step);

	location_route_step_set_start_point (step, NULL);
	location_route_step_set_end_point (step, NULL);
	location_route_step_set_bounding_box (step, NULL);
	location_route_step_set_distance (step, 0.0);
	location_route_step_set_duration (step, 0);
	location_route_step_set_instruction (step, NULL);
	location_route_step_set_geometry (step, NULL);

	if (step->properties) {
		g_hash_table_destroy (step->properties);
		step->properties = NULL;
	}
	g_slice_free(LocationRouteStep, step);
}
