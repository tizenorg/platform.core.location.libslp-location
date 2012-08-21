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

#include <glib.h>
#include <string.h>
#include <glib/gprintf.h>
#include <gconf/gconf-client.h>
#include <location-map-service.h>
#include "location-api-test-util.h"

#define STR_MAX	128
LocationMapObject* map_obj = NULL;
static GMainLoop *g_mainloop = NULL;

#define LOCATION_API_TEST_JSON_FILE "/opt/data/location-api-test.json"

static gpointer GmainThread(gpointer data)
{
	g_mainloop = g_main_loop_new (NULL, FALSE);
	g_printf("\n...Entering GMain Loop to Receive Notifications....\n");
	g_main_loop_run (g_mainloop);
	g_main_loop_unref (g_mainloop);
	g_mainloop = NULL;
	return NULL;
}


static void GetLocationError(char str[STR_MAX], int ret)
{
	switch(ret)
	{
		case LOCATION_ERROR_NONE:
			g_utf8_strncpy(str, "LOCATION_ERROR_NONE", STR_MAX);
			break;
		case LOCATION_ERROR_NOT_ALLOWED:
			g_utf8_strncpy(str, "LOCATION_ERROR_NOT_ALLOWED", STR_MAX);
			break;
		case LOCATION_ERROR_NOT_AVAILABLE:
			g_utf8_strncpy(str, "LOCATION_ERROR_NOT_AVAILABLE", STR_MAX);
			break;
		case LOCATION_ERROR_NETWORK_FAILED:
			g_utf8_strncpy(str, "LOCATION_ERROR_NETWORK_FAILED", STR_MAX);
			break;
		case LOCATION_ERROR_NETWORK_NOT_CONNECTED:
			g_utf8_strncpy(str, "LOCATION_ERROR_NETWORK_NOT_CONNECTED", STR_MAX);
			break;
		case LOCATION_ERROR_CONFIGURATION:
			g_utf8_strncpy(str, "LOCATION_ERROR_CONFIGURATION", STR_MAX);
			break;
		case LOCATION_ERROR_PARAMETER:
			g_utf8_strncpy(str, "LOCATION_ERROR_PARAMETER", STR_MAX);
			break;
		case LOCATION_ERROR_UNKNOWN:
			g_utf8_strncpy(str, "LOCATION_ERROR_UNKNOWN", STR_MAX);
			break;
		case LOCATION_ERROR_NOT_SUPPORTED:
			g_utf8_strncpy(str, "LOCATION_ERROR_UNKNOWN", STR_MAX);
			break;
		default:
			g_utf8_strncpy(str, "Error: undefined error code", STR_MAX);
	}
}

static void GetAccuracyLevel(char str[STR_MAX], LocationAccuracyLevel acc_level)
{
	switch (acc_level) {
		case LOCATION_ACCURACY_LEVEL_NONE:
			g_utf8_strncpy(str, "LOCATION_ACCURACY_LEVEL_NONE", STR_MAX);
			break;
		case LOCATION_ACCURACY_LEVEL_COUNTRY:
			g_utf8_strncpy(str, "LOCATION_ACCURACY_LEVEL_COUNTRY", STR_MAX);
			break;
		case LOCATION_ACCURACY_LEVEL_REGION:
			g_utf8_strncpy(str, "LOCATION_ACCURACY_LEVEL_REGION", STR_MAX);
			break;
		case LOCATION_ACCURACY_LEVEL_LOCALITY:
			g_utf8_strncpy(str, "LOCATION_ACCURACY_LEVEL_LOCALITY", STR_MAX);
			break;
		case LOCATION_ACCURACY_LEVEL_POSTALCODE:
			g_utf8_strncpy(str, "LOCATION_ACCURACY_LEVEL_POSTALCODE", STR_MAX);
			break;
		case LOCATION_ACCURACY_LEVEL_STREET:
			g_utf8_strncpy(str, "LOCATION_ACCURACY_LEVEL_STREET", STR_MAX);
			break;
		case LOCATION_ACCURACY_LEVEL_DETAILED:
			g_utf8_strncpy(str, "LOCATION_ACCURACY_LEVEL_DETAILED", STR_MAX);
			break;
		default:
			g_utf8_strncpy(str, "Error: undefined accuracy level", STR_MAX);
	}
}

static void SelectOpt(char* buf)
{
	int iLen = 0;
	fgets(buf, 255, stdin);
	iLen = g_utf8_strlen(buf, -1);
	buf[iLen-1] = '\0';
}

static int PromptInt()
{
	char buf[255];
	int ret;
	fgets(buf, 255, stdin);
	buf[strlen(buf)-1]='\0';
	ret = g_ascii_strtoll(buf, NULL, 10);
	return ret;
}

static double PromptDB()
{
	char buf[255];
	double ret;
	fgets(buf, 255, stdin);
	buf[strlen(buf)-1]='\0';
	ret = g_ascii_strtod(buf, NULL);
	return ret;
}

static void PrintProperty (LocationObject* loc)
{
	if (!loc) return;
#if 0
	LocationMethod method = LOCATION_METHOD_NONE;
	gchar method_str[STR_MAX] = {0, };

	gchar* devname = NULL;

	g_object_get(loc, "method", &method, NULL);
	GetMethod(method_str, method);
	g_printf("method[%s] ", method_str);

	if (LOCATION_METHOD_GPS == method) {
		g_object_get(loc, "dev-name", &devname, NULL);
		if (devname) {
			g_printf("dev-name[%s] ", devname);
			g_free(devname);
		}
	}
#endif
}

static void _print_property (gpointer data, gpointer user_data)
{
	LocationLandmark *landmark = (LocationLandmark *)user_data;
	gpointer key = (gpointer)data;
	gpointer value = NULL;

	if (key) {
		value = (gpointer)location_landmark_get_property(landmark, key);
		g_printf(", [%s:%s]", (gchar*) key, (gchar*) value);
	}
}

static void
__plugin_print_poi_list (gpointer data, gpointer user_data)
{
	LocationLandmark *landmark = (LocationLandmark *)data;
	LocationPosition *pos = location_landmark_get_position (landmark);
	LocationAddress *addr = location_landmark_get_address (landmark);

	g_printf ("[ID:%d], [NAME:%s], phone[%s],pos[%f:%f], addr[%s:%s:%s:%s:%s]",
			location_landmark_get_id (landmark),
			location_landmark_get_name(landmark),
			location_landmark_get_phone_number (landmark),
			pos->latitude, pos->longitude,
			addr->building_number, addr->city, addr->country_code, addr->district,
			addr->postal_code);

	GList *key_list = location_landmark_get_property_key(landmark);
	if (key_list) {
		g_list_foreach(key_list, _print_property, landmark);
	}

	g_printf("\n");
}


static void cb_poi(LocationError error, guint req_id, GList * landmark_list, gchar * error_code, gchar * error_msg, gpointer userdata)
{
	g_printf("\n===== __location_POI_cb ======\n");
	if (error != LOCATION_ERROR_NONE) {
		g_printf("Failed :%d\n", error);
		return;
	}
	g_printf("Success, poi_list[0x%x] user_data[0x%x] req_id[%d]\n", (unsigned int)landmark_list, (unsigned int)userdata, req_id);
	g_list_foreach (landmark_list, __plugin_print_poi_list, NULL);

	if (error_code && error_msg) {
		g_printf("__location_POI_cb: error_code[%s], error_msg[%s]\n", error_code, error_msg);
	}
}
static void PrintStepInfo(const LocationRouteStep *step)
{
	const LocationPosition *start = location_route_step_get_start_point(step);
	gdouble start_lat = 0;
	gdouble start_lon = 0;
	if (start) {
		start_lat = start->latitude;
		start_lon = start->longitude;
	} else {
		g_printf("Step start position NULL\n");
	}
	const LocationPosition *end = location_route_step_get_end_point(step);
	gdouble end_lat = 0;
	gdouble end_lon = 0;
	if (end) {
		end_lat = end->latitude;
		end_lon = end->longitude;
	} else {
		g_printf("Step end position NULL\n");
	}
	const gchar *inst = location_route_step_get_instruction(step);

	g_printf("Step: start(%f/%f), end(%f/%f), instruction(%s)\n", start_lat, start_lon, end_lat, end_lon, inst);
}

static void cb_print_route_step (gpointer data, gpointer user_data)
{
	g_printf("+++Step begin\n");
	LocationRouteStep *step = (LocationRouteStep *)data;

	PrintStepInfo(step);
	g_printf("---Step end\n");
}

static void cb_print_route_segment (gpointer data, gpointer user_data)
{
	g_printf("++Segment begin\n");
	LocationRouteSegment *seg = (LocationRouteSegment *)data;
	gdouble seg_dist = location_route_segment_get_distance(seg);
	glong seg_duration = location_route_segment_get_duration(seg);
	const LocationPosition *start = location_route_segment_get_start_point(seg);
	gdouble start_lat = 0;
	gdouble start_lon = 0;
	if (start) {
		start_lat = start->latitude;
		start_lon = start->longitude;
	} else {
		g_printf("Segment start position NULL\n");
	}
	const LocationPosition *end = location_route_segment_get_end_point(seg);
	gdouble end_lat = 0;
	gdouble end_lon = 0;
	if (end) {
		end_lat = end->latitude;
		end_lon = end->longitude;
	} else {
		g_printf("Segment end postion NULL\n");
	}
	g_printf("Segment info: Distance[%f], Duration[%ld], start(%f/%f), end(%f/%f)\n", seg_dist, seg_duration,
			start_lat, start_lon, end_lat, end_lon);

	GList *step_list = location_route_segment_get_route_step(seg);
	GList *tmp_list = (GList *)step_list;
	if (tmp_list) {
		g_list_foreach(tmp_list, cb_print_route_step, NULL);
	}
	g_printf("--Segment end\n");
}


static void cb_print_route_list (gpointer data, gpointer user_data)
{
	g_printf("+Route begin\n");
	LocationRoute *route = (LocationRoute *)data;

	const LocationPosition *start = location_route_get_origin(route);
	gdouble start_lat = 0;
	gdouble start_lon = 0;
	if (start) {
		start_lat = start->latitude;
		start_lon = start->longitude;
	} else {
		g_printf("Route start position NULL\n");
	}
	const LocationPosition *end = location_route_get_destination(route);
	gdouble end_lat = 0;
	gdouble end_lon = 0;
	if (end) {
		end_lat = end->latitude;
		end_lon = end->longitude;
	} else {
		g_printf("Route end position NULL\n");
	}
	g_printf("Route: start(%f/%f), end(%f/%f)\n", start_lat, start_lon, end_lat, end_lon);

	gdouble distance = location_route_get_total_distance(route);
	const gchar *dis_unit = location_route_get_distance_unit(route);
	glong duration = location_route_get_total_duration(route);
	const LocationBoundary *bound = location_route_get_bounding_box(route);
	if (bound && bound->type == LOCATION_BOUNDARY_RECT) {
		g_printf("RECT left top[%f-%f], right bottom[%f-%f]\n", bound->rect.left_top->latitude, bound->rect.left_top->longitude,
			bound->rect.right_bottom->latitude, bound->rect.right_bottom->longitude);
	} else {
		g_printf("route boundary not exist, or not RECT\n");
	}
	g_printf ("Distance[%f], Distance unit[%s], Duration[%ld]\n", distance, dis_unit, duration);

	GList *seg_list = location_route_get_route_segment(route);
	if (seg_list) {
		g_list_foreach(seg_list, cb_print_route_segment, NULL);
	}

	g_printf("-Route end\n");
}

static void cb_route(LocationError error, guint req_id, GList * route_list, gchar * error_code, gchar * error_msg, gpointer userdata)
{
	g_printf("\n===== cb_route ======\n");
	if (error != LOCATION_ERROR_NONE) {
		g_printf("Failed :%d\n", error);
		return;
	}

	g_printf("Success, poi_list[0x%x] user_data[0x%x] req_id[%d]\n", (unsigned int)route_list, (unsigned int)userdata, req_id);

	if (route_list)
		g_list_foreach (route_list, cb_print_route_list, NULL);

	if (error_code && error_msg) {
		g_printf("cb_route: error_code[%s], error_msg[%s]\n", error_code, error_msg);
	}
}

static void cb_address(LocationError error, LocationAddress *addr, LocationAccuracy *acc, gpointer userdata)
{
	if (error != LOCATION_ERROR_NONE) {
		g_printf("cb_address failed: error=%d\n", error);
		return;
	}
	char str[STR_MAX];
	g_printf("userdata[0x%x] building number: [%s], street: [%s], state: [%s], country code: [%s], city: [%s], district: [%s], postal code: [%s]\n",
		(unsigned int)userdata, addr->building_number, addr->street, addr->state, addr->country_code, addr->city, addr->district, addr->postal_code);
	GetAccuracyLevel(str, acc->level);
	g_printf("level: [%s], horizontal_accuracy: [%f], vertical_accuracy: [%f]\n", str, acc->horizontal_accuracy, acc->vertical_accuracy);
}

typedef struct {
	LocationObject *obj;
	LocationPosition *pos;
	LocationAddress *addr;
	gchar *str_addr;
	LocationPositionCB pos_cb;
	LocationAddressCB addr_cb;
	gpointer user_data;
} IdleData;

static gboolean idle_address_from_position_async(gpointer data)
{
	IdleData* idle_data = (IdleData*)data;
	char str[STR_MAX];
	int ret = location_map_get_address_from_position_async(idle_data->obj, idle_data->pos, idle_data->addr_cb, idle_data->user_data);
	GetLocationError(str, ret);
	g_printf("location_map_get_address_from_position_async: returned value [%s]\n", str);
	location_position_free(idle_data->pos);
	g_free(idle_data);
	return FALSE;
}

void _print_keys(gpointer value)
{
	g_printf(" %s ", (gchar*) value);
}

static void print_map_service_keys (LocationObject *obj, int type)
{
	GList *key = NULL;

	location_map_get_provider_capability_key (obj, type, &key);

	if (key) {
		g_list_foreach (key, _print_keys, NULL);
	}

	g_list_free_full(key, g_free);
}

void _print_provider (gpointer data, gpointer user_data)
{
	gchar *provider = (gchar *)data;

	g_printf("[%s] is supported\n", provider);
}

static void print_menu()
{
	g_printf("\n================================= Location API Test =================================\n");
	g_printf("q.   Exit\n");
	g_printf("1.   location_init\n");
	g_printf("2.   location_map_new for default \n");
	g_printf("2a.   location_map_new for decarta \n");
	g_printf("2b.   location_map_new for osm \n");
	g_printf("3.   location_map_free\n");
	g_printf("4.   location_map_get_address_from_position\n");
	g_printf("4a.  location_map_get_address_from_position_async\n");
	g_printf("5.   location_map_search_poi_with_keyword\n");
	g_printf("5a.  location_map_search_poi_with_category\n");
	g_printf("5b.  location_map_search_poi_with_poi_name\n");
	g_printf("6.   location_map_search_poi_by_area_with_keyword\n");
	g_printf("6a.  location_map_search_poi_by_area_with_category\n");
	g_printf("6b.  location_map_search_poi_by_area_with_poi_name\n");
	g_printf("7.   location_map_search_poi_by_addr_with_keyword\n");
	g_printf("7a.  location_map_search_poi_by_addr_with_category\n");
	g_printf("7b.  location_map_search_poi_by_addr_with_poi_name\n");
	g_printf("8.   location_map_search_poi_by_freeformed_address_with_keyword\n");
	g_printf("8a.  location_map_search_poi_by_freeformed_address_with_category\n");
	g_printf("8b.  location_map_search_poi_by_freeformed_address_with_poi_name\n");
	g_printf("9.   location_map_cancel_poi_request\n");
	g_printf("10.  location_map_request_route\n");
	g_printf("10a. location_map_request_route for a long distance\n");
	g_printf("11.  location_map_cancel_route_request\n");
	g_printf("12.  location_map_is_supported_provider_capability\n");
	g_printf("13.  location_map_get_provider_capability_key\n");
	g_printf("14.  location_map_get_supported_providers \n");
	g_printf("15.  location_map_get_default_provider \n");
	g_printf("16.  location_map_set_provider \n");
	g_printf("99. change map provider to default\n");
	g_printf("99a. change map provider to decarta\n");
	g_printf("99b. change map provider to osm\n");

	g_printf("==================================== Property ====================================\n");
	PrintProperty(map_obj);
	g_printf("\n==================================================================================\n");
}

int main(int argc, char** argv)
{
	char strOpt[255];
	int ret;
	char str[STR_MAX];
	GError *gerr = NULL;
	guint req_id = 0;

	// If application is executed by AUL, this is not needed.
	g_setenv("PKG_NAME", "org.tizen.map-service-test", 1);

	g_type_init();

#if !GLIB_CHECK_VERSION (2, 31, 0)
	if( !g_thread_supported() )
	{
		g_thread_init(NULL);
	}
#endif

	GThread *g_main;
	g_main = g_thread_create(GmainThread, NULL, TRUE, &gerr);
	if (!g_main) {
		g_debug("Error create gmain thread: Err domain[%d] Err code[%d] Err msg[%s]",
			gerr->domain, gerr->code, gerr->message);
		g_error_free(gerr);
		return 0;
	}

	g_printf("--- Start LBS Test App ---\n");
	while(1)
	{
		print_menu();
		g_printf("Select option: ");
		SelectOpt(strOpt);
		g_printf("======================================================================================\n");
		if (0 == g_strcmp0 ("x", strOpt)) {

		}
		else if (0 == g_strcmp0("1",strOpt)) {
			ret = location_init();
			GetLocationError(str, ret);
			g_printf("location_init: returned value [%s]\n", str);
		} else if (0 == g_strcmp0("2",strOpt)) {
			if (map_obj) {
				g_printf("Location object already existed: [0x%x]", (unsigned int)map_obj);
				continue;
			}
			map_obj = location_map_new(NULL);
			if(map_obj) g_printf("Success\n");
			else             g_printf("Failed\n");
		} else if (0 == g_strcmp0("2a",strOpt)) {
			if (map_obj) {
				g_printf("Location object already existed: [0x%x]", (unsigned int)map_obj);
				continue;
			}
			map_obj = location_map_new("decarta");
			if(map_obj) g_printf("Success\n");
			else             g_printf("Failed\n");
		} else if (0 == g_strcmp0("2b",strOpt)) {
			if (map_obj) {
				g_printf("Location object already existed: [0x%x]", (unsigned int)map_obj);
				continue;
			}
			map_obj = location_map_new("osm");
			if(map_obj) g_printf("Success\n");
			else             g_printf("Failed\n");
		} else if (0 == g_strcmp0("3",strOpt)) {
			ret = location_map_free (map_obj);
			map_obj = NULL;
			GetLocationError(str, ret);
			g_printf("location_map_free: returned value [%s]\n", str);
		}else if(0 == g_strcmp0("4",strOpt) ){
			LocationPosition *pos = NULL;
			LocationAddress *addr = NULL;
			LocationAccuracy *acc = NULL;
			g_printf("[0].San jose [1].Suwon HQ [*].Custom\n");
			g_printf("Select Position: ");
			int opt = PromptInt();
			if(opt == 0)      pos = location_position_new(0, 37.335276, -121.890059, 0, LOCATION_STATUS_2D_FIX);
			else if(opt == 1) pos = location_position_new(0, 37.257809, 127.056383, 0, LOCATION_STATUS_2D_FIX);
			else{
				g_printf("Input latitude: ");
				gdouble lat = PromptDB();
				g_printf("Input longitude: ");
				gdouble lon = PromptDB();
				pos = location_position_new(0, lat, lon, 0, LOCATION_STATUS_2D_FIX);
			}
			ret = location_map_get_address_from_position(map_obj, pos, &addr, &acc);
			GetLocationError(str, ret);
			g_printf("location_map_get_address_from_position: returned value [%s]\n", str);
			if(ret == LOCATION_ERROR_NONE){
				g_printf("building number: [%s], street: [%s], state: [%s], country code: [%s], city: [%s], district: [%s], postal code: [%s]\n",
					addr->building_number, addr->street, addr->state, addr->country_code, addr->city, addr->district, addr->postal_code);
			}
			if(addr) location_address_free(addr);
			if(acc) location_accuracy_free(acc);
		}else if(0 == g_strcmp0("4a",strOpt) ){
			IdleData* data = g_new0(IdleData, 1);
			data->obj = map_obj;
			data->addr_cb = cb_address;
			data->user_data = map_obj;

			g_printf("[0].San jose [1].Suwon HQ [*].Custom\n");
			g_printf("Select Position: ");
			int opt = PromptInt();
			if(opt == 0)       data->pos = location_position_new(0, 37.335276, -121.890059, 0, LOCATION_STATUS_2D_FIX);
			else if (opt == 1) data->pos = location_position_new(0, 37.257809, 127.056383, 0, LOCATION_STATUS_2D_FIX);
			else {
				g_printf("Input latitude: ");
				gdouble lat = PromptDB();
				g_printf("Input longitude: ");
				gdouble lon = PromptDB();
				data->pos = location_position_new(0, lat, lon, 0, LOCATION_STATUS_2D_FIX);
			}
			g_idle_add((GSourceFunc)idle_address_from_position_async, data);
		}else if(0 == g_strcmp0("5", strOpt)) {
			g_printf("location_map_search_poi\n");

			LocationPOIFilter *filter = location_poi_filter_new();
			location_poi_filter_set(filter, "KEYWORD", "pizza");

			LocationPOIPreference *pref = location_poi_pref_new();
			location_poi_pref_set_max_result(pref, 2);
			location_poi_pref_set_sort_by(pref, "Distance");
			location_poi_pref_set_sort_order(pref, LOCATION_POI_PREF_SO_DESC);

			LocationPosition *position = location_position_new(0, 37.771008, -122.41175, 0, LOCATION_STATUS_2D_FIX);

			ret = location_map_search_poi(map_obj, filter, position, pref, cb_poi, NULL, &req_id);
			GetLocationError(str, ret);
			if(ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search POI. Error[%s]\n", str);
			} else {
					g_printf("Seach POI success, req_id %d\n", req_id);
			}

			location_poi_filter_free(filter);
			location_poi_pref_free(pref);
			location_position_free(position);
		}else if(0 == g_strcmp0("5a", strOpt)) {
			g_printf("location_map_search_poi_with_category\n");
			LocationPOIFilter *filter = location_poi_filter_new();
			location_poi_filter_set(filter, "CATEGORY", "restaurant");

			LocationPOIPreference *pref = location_poi_pref_new();
			location_poi_pref_set_max_result(pref, 5);
			location_poi_pref_set_sort_by(pref, "Distance");
			location_poi_pref_set_sort_order(pref, LOCATION_POI_PREF_SO_ASC);

			LocationPosition *position = location_position_new(0, 37.771008, -122.41175, 0, LOCATION_STATUS_2D_FIX);

			ret = location_map_search_poi(map_obj, filter, position, pref, cb_poi, NULL, &req_id);
			GetLocationError(str, ret);
			if(ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search POI. Error[%s]\n", str);
			} else {
					g_printf("Seach POI success, req_id %d\n", req_id);
			}

			location_poi_filter_free(filter);g_printf("location_search_poi_by_freeformed_address_with_poi_name\n");
			location_poi_pref_free(pref);
			location_position_free(position);

		}else if(0 == g_strcmp0("5b", strOpt)) {
			g_printf("location_map_search_poi_with_poi_name\n");

			LocationPOIFilter *filter = location_poi_filter_new();
			location_poi_filter_set(filter, "POIName", "cafe");

			LocationPOIPreference *pref = location_poi_pref_new();
			location_poi_pref_set_max_result(pref, 10);
			gchar *item = g_strdup("Distance");
			location_poi_pref_set_sort_by(pref, item);
			location_poi_pref_set_sort_order(pref, LOCATION_POI_PREF_SO_DESC);

			LocationPosition *position = location_position_new(0, 37.771008, -122.41175, 0, LOCATION_STATUS_2D_FIX);

			ret = location_map_search_poi(map_obj, filter, position, pref, cb_poi, NULL, &req_id);
			GetLocationError(str, ret);
			if(ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search POI. Error[%s]\n", str);
			} else {
					g_printf("Seach POI success, req_id %d\n", req_id);
			}

			location_poi_filter_free(filter);
			location_poi_pref_free(pref);
			location_position_free(position);

		}else if(0 == g_strcmp0("6", strOpt)) {
			g_printf("location_map_search_poi_by_area_with_keyword\n");

			// only circle supported by decarta
			LocationPosition *center = location_position_new(0, 37.336723, -121.889555, 0, LOCATION_STATUS_2D_FIX);
			gdouble radius = 400;
			LocationBoundary *bound = location_boundary_new_for_circle(center, radius);

			LocationPOIFilter *filter = location_poi_filter_new();
			gchar *key = g_strdup("KEYWORD");
			gchar *value = g_strdup("cafe");
			location_poi_filter_set(filter, key, value); // same with Type ?  CATEGORY, KEYWORD, POIName

			LocationPOIPreference *pref = location_poi_pref_new();
			location_poi_pref_set_max_result(pref, 10);
			gchar *item = g_strdup("Distance");
			location_poi_pref_set_sort_by(pref, item); // can't set const char* directly !!
			location_poi_pref_set_sort_order(pref, LOCATION_POI_PREF_SO_ASC);  //LOCATION_POI_PREF_SO_ASC


			ret = location_map_search_poi_by_area(map_obj, filter, bound, pref, cb_poi, NULL, &req_id);
			GetLocationError(str, ret);
			if (ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search POI by area. Error[%s]\n", str);
			} else {
					g_printf("Seach POI by area sucess, req_id %d\n", req_id);
			}

			g_free(key);
			g_free(value);
			g_free(item);
			location_position_free(center);
			location_boundary_free(bound);
			location_poi_filter_free(filter);
			location_poi_pref_free(pref);
		}else if(0 == g_strcmp0("6a", strOpt)) {
			g_printf("location_map_search_poi_by_area_with_category\n");

			// only circle supported by decarta
			LocationPosition *center = location_position_new(0, 37.336723, -121.889555, 0, LOCATION_STATUS_2D_FIX);
			gdouble radius = 400;
			LocationBoundary *bound = location_boundary_new_for_circle(center, radius);

			LocationPOIFilter *filter = location_poi_filter_new();
			gchar *key = g_strdup("CATEGORY");
			gchar *value = g_strdup("restaurant");
			location_poi_filter_set(filter, key, value);

			LocationPOIPreference *pref = location_poi_pref_new();
			location_poi_pref_set_max_result(pref, 5);
			gchar *item = g_strdup("Distance");
			location_poi_pref_set_sort_by(pref, item);
			location_poi_pref_set_sort_order(pref, LOCATION_POI_PREF_SO_ASC);

			ret = location_map_search_poi_by_area(map_obj, filter, bound, pref, cb_poi, NULL, &req_id);
			GetLocationError(str, ret);
			if (ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search POI by area. Error[%s]\n", str);
			} else {
					g_printf("Seach POI by area sucess, req_id %d\n", req_id);
			}

			g_free(key);
			g_free(value);
			g_free(item);
			location_position_free(center);
			location_boundary_free(bound);
			location_poi_filter_free(filter);
			location_poi_pref_free(pref);

		}else if(0 == g_strcmp0("6b", strOpt)) {
			g_printf("location_map_search_poi_by_area_with_poi_name\n");
			
			// only circle supported by decarta
			LocationPosition *center = location_position_new(0, 37.336723, -121.889555, 0, LOCATION_STATUS_2D_FIX);
			gdouble radius = 400;
			LocationBoundary *bound = location_boundary_new_for_circle(center, radius);

			LocationPOIFilter *filter = location_poi_filter_new();
			gchar *key = g_strdup("POIName");
			gchar *value = g_strdup("cafe");
			location_poi_filter_set(filter, key, value); // same with Type ?  CATEGORY, KEYWORD, POIName

			LocationPOIPreference *pref = location_poi_pref_new();
			location_poi_pref_set_max_result(pref, 10);
			gchar *item = g_strdup("Distance");
			location_poi_pref_set_sort_by(pref, item); // can't set const char* directly !!
			location_poi_pref_set_sort_order(pref, LOCATION_POI_PREF_SO_ASC);  //LOCATION_POI_PREF_SO_ASC


			ret = location_map_search_poi_by_area(map_obj, filter, bound, pref, cb_poi, NULL, &req_id);
			GetLocationError(str, ret);
			if (ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search POI by area. Error[%s]\n", str);
			} else {
					g_printf("Seach POI by area sucess, req_id %d\n", req_id);
			}

			g_free(key);
			g_free(value);
			g_free(item);
			location_position_free(center);
			location_boundary_free(bound);
			location_poi_filter_free(filter);
			location_poi_pref_free(pref);

		}else if(0 == g_strcmp0("7", strOpt)) {
			g_printf("location_map_search_poi_by_address_with_keyword\n");

			LocationAddress *addr = location_address_new("51", "N SAN PEDRO ST", NULL, "SAN JOSE", "SANTA CLARA", "CA", "95110");

			LocationPOIFilter *filter = location_poi_filter_new();
			gchar *key = g_strdup("KEYWORD");
			gchar *value = g_strdup("cafe");
			location_poi_filter_set(filter, key, value);

			LocationPOIPreference *pref = location_poi_pref_new();
			location_poi_pref_set_max_result(pref, 10);
			gchar *item = g_strdup("Distance");
			location_poi_pref_set_sort_by(pref, item);
			location_poi_pref_set_sort_order(pref, LOCATION_POI_PREF_SO_ASC);

			ret = location_map_search_poi_by_address(map_obj, filter, addr, pref, cb_poi, NULL, &req_id);
			GetLocationError(str, ret);
			if(ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search POI by address. Error[%s]\n", str);
			} else {
					g_printf("Seach POI by address sucess, req_id %d\n", req_id);
			}

			g_free(key);
			g_free(value);
			g_free(item);

			location_address_free(addr);
			location_poi_filter_free(filter);
			location_poi_pref_free(pref);
		}else if(0 == g_strcmp0("7a", strOpt)) {
			g_printf("location_search_poi_by_address_with_category\n");

			LocationAddress *addr = location_address_new("51", "N SAN PEDRO ST", NULL, "SAN JOSE", "SANTA CLARA", "CA", "95110");

			LocationPOIFilter *filter = location_poi_filter_new();
			location_poi_filter_set(filter, "CATEGORY", "restaurant");

			LocationPOIPreference *pref = location_poi_pref_new();
			location_poi_pref_set_max_result(pref, 10);
			location_poi_pref_set_sort_by(pref, "Distance");
			location_poi_pref_set_sort_order(pref, LOCATION_POI_PREF_SO_ASC);

			ret = location_map_search_poi_by_address(map_obj, filter, addr, pref, cb_poi, NULL, &req_id);
			GetLocationError(str, ret);
			if(ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search POI by address. Error[%s]\n", str);
			} else {
					g_printf("Seach POI by address sucess, req_id %d\n", req_id);
			}

			location_address_free(addr);
			location_poi_filter_free(filter);
			location_poi_pref_free(pref);
		}else if(0 == g_strcmp0("7b", strOpt)) {
			g_printf("location_map_search_poi_by_address_with_poi_name\n");

			LocationAddress *addr = location_address_new("51", "N SAN PEDRO ST", NULL, "SAN JOSE", "SANTA CLARA", "CA", "95110");

			LocationPOIFilter *filter = location_poi_filter_new();
			gchar *key = g_strdup("POIName");
			gchar *value = g_strdup("cafe");
			location_poi_filter_set(filter, key, value);

			LocationPOIPreference *pref = location_poi_pref_new();
			location_poi_pref_set_max_result(pref, 10);
			gchar *item = g_strdup("Distance");
			location_poi_pref_set_sort_by(pref, item);
			location_poi_pref_set_sort_order(pref, LOCATION_POI_PREF_SO_ASC);

			ret = location_map_search_poi_by_address(map_obj, filter, addr, pref, cb_poi, NULL, &req_id);
			GetLocationError(str, ret);
			if(ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search POI by address. Error[%s]\n", str);
			} else {
					g_printf("Seach POI by address sucess, req_id %d\n", req_id);
			}

			g_free(key);
			g_free(value);
			g_free(item);

			location_address_free(addr);
			location_poi_filter_free(filter);
			location_poi_pref_free(pref);

		}else if(0 == g_strcmp0("8", strOpt)) {
			g_printf("location_map_search_poi_by_freeformed_address_with_keyword\n");

			gchar *addr = g_strdup("North Second St.");

			LocationPOIFilter *filter = location_poi_filter_new();
			location_poi_filter_set(filter, "KEYWORD", "station");

			LocationPOIPreference *pref = location_poi_pref_new();
			location_poi_pref_set_max_result(pref, 10);
			gchar *item = g_strdup("Distance");
			location_poi_pref_set_sort_by(pref, item); // can't set const char* directly !!
			location_poi_pref_set_sort_order(pref, LOCATION_POI_PREF_SO_ASC);  //LOCATION_POI_PREF_SO_ASC

			ret = location_map_search_poi_by_freeformed_address(map_obj, filter, addr, pref, cb_poi, NULL, &req_id);
			GetLocationError(str, ret);
			if (ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search POI by address. Error[%s]\n", str);
			} else {
					g_printf("Seach POI by address success, req_id %d\n", req_id);
		}

			g_free(item);

			g_free(addr);
			location_poi_filter_free(filter);
			location_poi_pref_free(pref);
		}else if(0 == g_strcmp0("8a", strOpt)) {
			g_printf("location_map_search_poi_by_freeformed_address_with_category\n");
		
			gchar *addr = g_strdup("North Second St.");

			LocationPOIFilter *filter = location_poi_filter_new();
			gchar *key = g_strdup("CATEGORY");
			gchar *value = g_strdup("restaurant");
			location_poi_filter_set(filter, key, value);

			LocationPOIPreference *pref = location_poi_pref_new();
			location_poi_pref_set_max_result(pref, 10);
			gchar *item = g_strdup("Distance");
			location_poi_pref_set_sort_by(pref, item);
			location_poi_pref_set_sort_order(pref, LOCATION_POI_PREF_SO_ASC);
			
			ret = location_map_search_poi_by_freeformed_address(map_obj, filter, addr, pref, cb_poi, NULL, &req_id);
			GetLocationError(str, ret);
			if (ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search POI by address. Error[%s]\n", str);
			} else {
					g_printf("Seach POI by address success, req_id %d\n", req_id);
		}

			g_free(key);
			g_free(value);
			g_free(item);

			g_free(addr);
			location_poi_filter_free(filter);
			location_poi_pref_free(pref);
		}else if(0 == g_strcmp0("8b", strOpt)) {
			g_printf("location_map_search_poi_by_freeformed_address_with_poi_name\n");

			gchar *addr = g_strdup("North Second St.");

			LocationPOIFilter *filter = location_poi_filter_new();
			location_poi_filter_set(filter, "POIName", "pizza");

			LocationPOIPreference *pref = location_poi_pref_new();
			location_poi_pref_set_max_result(pref, 10);
			location_poi_pref_set_sort_by(pref, "Distance");
			location_poi_pref_set_sort_order(pref, LOCATION_POI_PREF_SO_ASC);

			ret = location_map_search_poi_by_freeformed_address(map_obj, filter, addr, pref, cb_poi, NULL, &req_id);
			GetLocationError(str, ret);
			if (ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search POI by address. Error[%s]\n", str);
			} else {
					g_printf("Seach POI by address success, req_id %d\n", req_id);
			}

			g_free(addr);
			location_poi_filter_free(filter);
			location_poi_pref_free(pref);

		}else if(0 == g_strcmp0("9", strOpt)) {
			int req_id;

			g_printf("Input ReqID : ");
			scanf("%d", &req_id);

			ret = location_map_cancel_poi_request(map_obj, req_id);
			GetLocationError(str, ret);
			if (ret != LOCATION_ERROR_NONE) {
				g_printf("Fail to cancel POI request. Error[%s]\n", str);
			}
			else
				g_printf("location_map_cancel_poi_request, req_id %d\n", req_id);
		}else if(0 == g_strcmp0("10", strOpt)) {
			g_printf("location_map_request_route\n\n");

			LocationPosition *origin = location_position_new(0, 37.564263, 126.974676, 0, LOCATION_STATUS_2D_FIX);	// Seoul city hall
			LocationPosition *destination = location_position_new(0, 37.557120, 126.992410, 0, LOCATION_STATUS_2D_FIX);	// NamSan

			GList *waypoint = NULL;
			LocationPosition *via_pos = location_position_new(0, 37.560950, 126.986240, 0, LOCATION_STATUS_2D_FIX); // Wangsimli
			waypoint = g_list_append (waypoint, (gpointer)via_pos);

			LocationRoutePreference *pref = location_route_pref_new();
			location_route_pref_set_route_type(pref, "FASTEST");

			ret = location_map_request_route(map_obj, origin, destination, waypoint, pref, cb_route, NULL, &req_id);
			GetLocationError(str, ret);
			if (ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search route by address. Error[%s]\n", str);
			} else {
					g_printf("Search Route successfully, req_id %d\n", req_id);
			}
		}else if(0 == g_strcmp0("10a", strOpt)) {
			g_printf("location_map_request_route for a long distance\n");

			LocationPosition *origin = location_position_new(0, 29.783449,-95.373688, 0, LOCATION_STATUS_2D_FIX);
			LocationPosition *destination = location_position_new(0, 39.749962,-104.984665, 0, LOCATION_STATUS_2D_FIX);

			GList *waypoint = NULL;

			LocationRoutePreference *pref = location_route_pref_new();
			location_route_pref_set_route_type(pref, "FASTEST");

			ret = location_map_request_route(map_obj, origin, destination, waypoint, pref, cb_route, NULL, &req_id);
			GetLocationError(str, ret);
			if (ret != LOCATION_ERROR_NONE) {
					g_printf("Fail to search route by address. Error[%s]\n", str);
			} else {
					g_printf("Search Route successfully, req_id %d\n", req_id);
			}


		}else if(0 == g_strcmp0("11", strOpt)) {
			g_printf("location_map_cancel_route_request\n");

			int req_id;
			g_printf("Input ReqID : ");
			scanf("%d", &req_id);

			ret = location_map_cancel_route_request(map_obj, req_id);
			GetLocationError(str, ret);
			if (ret != LOCATION_ERROR_NONE) {
				g_printf("Fail to cancel route request. Error[%s]\n", str);
			}
			else {
				g_printf("location_map_cancel_route_request, req_id %d\n", req_id);
			}

		}else if (0 == g_strcmp0 ("12", strOpt)) {
			int idx = 0;
			for (idx = 0; idx < MAP_SERVICE_TYPE_MAX; idx++) {
				g_printf("[%d:%d], ", idx, location_map_is_supported_provider_capability (map_obj, idx));
			}
		}else if (0 == g_strcmp0 ("13", strOpt)) {
			int idx = 0;
			for (idx = 0; idx < MAP_SERVICE_TYPE_MAX; idx++) {
				g_printf("[%d:", idx);
				print_map_service_keys(map_obj, idx);
				g_printf("]\n");
			}
		}else if (0 == g_strcmp0 ("14", strOpt)) {
			GList *list = location_map_get_supported_providers (map_obj);
			if (list) {
				g_list_foreach (list, _print_provider, NULL);
				g_list_free_full (list, g_free);
			}
			else {
				g_printf("Fail to get supported_providers\n");
			}
		}else if (0 == g_strcmp0 ("15", strOpt)) {
			gchar *default_provider = location_map_get_default_provider (map_obj);
			if (default_provider) {
				g_printf("Defaut : [%s]\n", default_provider);
				g_free (default_provider);
			}
			else {
				g_printf("Fail to get default provider\n");
			}
		}else if (0 == g_strcmp0 ("16", strOpt)) {
			g_printf("[0].default [1].decarta [2].osm\n");
			g_printf("Select provider: ");
			int opt = PromptInt();
			gboolean ret = FALSE;
			switch (opt)
			{
				case 0:
					ret = location_map_set_provider (map_obj, NULL);
					break;
				case 1:
					ret = location_map_set_provider (map_obj, "decarta");
					break;
				case 2:
					ret = location_map_set_provider (map_obj, "osm");
					break;
				default:
					break;
			}

			if (ret == TRUE) {
				g_printf ("Success to set provider [%d]\n", opt);
			}
			else {
				g_printf ("Fail to set provider [%d]\n", opt);
			}
		}else if (0 == g_strcmp0 ("99", strOpt)) {
			if (map_obj) {
				g_object_set (map_obj, "provider", NULL, NULL);
			}
		}else if (0 == g_strcmp0 ("99a", strOpt)) {
			if (map_obj) {
				g_object_set (map_obj, "provider", "decarta", NULL);
			}
		}else if (0 == g_strcmp0 ("99b", strOpt)) {
			if (map_obj) {
				g_object_set (map_obj, "provider", "osm",  NULL);
			}
		}else if(0 == g_strcmp0("q",strOpt) ){
			g_main_loop_quit(g_mainloop);
			break;
		}
	}
	g_thread_join(g_main);
	g_printf("\n--- Exit LBS Test App ---\n");
	return 1;
}
