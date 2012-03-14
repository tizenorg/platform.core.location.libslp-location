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

#include <gconf/gconf-client.h>
#include <location/location.h>

static GMainLoop *loop = NULL;

#define GCONF_PROXY_MODE            "/system/proxy/mode"
#define GCONF_HTTP_PROXY_HOST       "/system/http_proxy/host"
#define GCONF_HTTP_PROXY_PORT       "/system/http_proxy/port"
#define ENV_HTTP_PROXY              "http_proxy"

static gboolean
exit_program (gpointer data)
{
	g_main_loop_quit (loop);
	g_debug ("Quit g_main_loop");
	return FALSE;
}

static void
print_pos (gpointer data, gpointer user_data)
{
	LocationPosition *pos = (LocationPosition *)data;

	if (pos) {
		g_debug ("time: %d, lat: %f, long: %f, alt: %f, status: %d",
			pos->timestamp, pos->latitude, pos->longitude, pos->altitude, pos->status);
		location_position_free (pos);
	}
}

static void
print_acc (gpointer data, gpointer user_data)
{
	LocationAccuracy *acc = (LocationAccuracy *)data;

	if (acc) {
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_accuracy_free (acc);
	}
}
static void
cb_address (LocationError error,
	LocationAddress *addr,
	LocationAccuracy *acc,
	gpointer userdata)
{
	if (error != LOCATION_ERROR_NONE) {
		g_debug("cb_address failed: error=%d\n", error);
		return;
	}
	g_debug ("ASYNC>> location_get_address_async> %s %s %s %s %s %s %s",
			addr->building_number, addr->street, addr->district, addr->city, addr->state, addr->postal_code, addr->country_code);
	g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
}

static void
cb_service_disabled (GObject *self,
	guint status,
	gpointer userdata)
{
	g_debug("cb_service_disabled: status(%d) userdata(0x%x)", status, (unsigned int)userdata);
}

static void
cb_position_from_address (LocationError error, GList *pos_list, GList *acc_list, gpointer userdata)
{
	if (error != LOCATION_ERROR_NONE) {
		g_debug("cb_position_from_address failed: error=%d\n", error);
		return;
	}

	g_list_foreach (pos_list, print_pos, NULL);
	g_list_foreach (acc_list, print_acc, NULL);
}

static void
cb_position_from_freeformed_address (LocationError error, GList *pos_list, GList *acc_list, gpointer userdata)
{
	if (error != LOCATION_ERROR_NONE) {
		g_debug("cb_position_from_freeformed_address failed: error=%d\n", error);
		return;
	}

	g_list_foreach (pos_list, print_pos, NULL);
	g_list_foreach (acc_list, print_acc, NULL);
}

static void
cb_address_from_position (LocationError error, LocationAddress *addr, LocationAccuracy *acc, gpointer userdata)
{
	if (error != LOCATION_ERROR_NONE) {
		g_debug("cb_address_from_position failed: error=%d\n", error);
		return;
	}
	g_debug ("ASYNC>> location_get_address_from_position_async> %s %s %s %s %s %s %s",
			addr->building_number, addr->street, addr->district, addr->city, addr->state, addr->postal_code, addr->country_code);
	g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
}

static void
cb_service_enabled (GObject *self,
	guint status,
	gpointer userdata)
{
	g_debug("cb_service_enabled: status(%d) userdata(0x%x)", status, (unsigned int)userdata);

	LocationAddress *addr = NULL;
	LocationAccuracy *acc = NULL;
	LocationObject *loc = (LocationObject*)userdata;

	LocationError err = location_get_address(loc, &addr, &acc);
	if (LOCATION_ERROR_NONE == err) {
		g_debug ("SYNC>> location_get_address() success> %s %s %s %s %s %s %s",
			addr->building_number, addr->street, addr->district, addr->city, addr->state, addr->postal_code, addr->country_code);
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_address_free(addr);
		location_accuracy_free(acc);
	} else g_warning ("SYNC>> location_get_address() failed> error code:%d", err);

	err = location_get_address_async(loc, cb_address, loc);
	if (LOCATION_ERROR_NONE == err) g_debug("location_get_address_async() success");
	else g_warning ("location_get_address_async() failed> error code:%d", err);
}

static gboolean
async_request (gpointer loc)
{
	LocationAddress *addr = location_address_new ("1", "Post Street", NULL, "san jose", "ca", NULL, "95113");
	LocationError err = location_get_position_from_address_async(loc, addr, cb_position_from_address, loc);
	if (LOCATION_ERROR_NONE == err)
		g_debug("location_get_position_from_address_async() success");
	else g_warning ("location_get_position_from_address_async() failed> error code:%d", err);
	location_address_free (addr);

	gchar *addr_str = g_strdup("4 N 2nd Street 95113");
	err = location_get_position_from_freeformed_address_async(loc, addr_str, cb_position_from_freeformed_address, loc);
	if (LOCATION_ERROR_NONE == err)
		g_debug("location_get_position_from_freeformed_address_async() success");
	else g_warning ("location_get_position_from_freeformed_address_async() failed> error code:%d", err);
	g_free(addr_str);

	LocationPosition *pos = location_position_new (0, 37.3322, -121.8720, 0, LOCATION_STATUS_2D_FIX);
	err = location_get_address_from_position_async(loc, pos, cb_address_from_position, loc);
	if (LOCATION_ERROR_NONE == err)
		g_debug("location_get_address_from_position_async() success");
	else g_warning ("location_get_address_from_position_async() failed> error code:%d", err);
	location_position_free (pos);
	return FALSE;
}

int
main (int argc, char *argv[])
{
	LocationObject *loc = NULL;

	// If application is executed by AUL, this is not needed.
	g_setenv("PKG_NAME", "com.samsung.address-sample", 1);

	g_type_init();
	location_init ();
	loop = g_main_loop_new (NULL, TRUE);

	loc  = location_new (LOCATION_METHOD_GPS);
	if (!loc) {
		g_warning("location_new failed");
		return -1;
	}

	LocationPosition *pos = NULL;
	LocationAccuracy *acc = NULL;
	GList *pos_list = NULL;
	GList *acc_list = NULL;
	LocationAddress *addr = NULL;

	addr = location_address_new ("1", "Post Street", NULL, "san jose", "ca", NULL, "95113");
	LocationError err = location_get_position_from_address(loc, addr, &pos_list, &acc_list);
	if (LOCATION_ERROR_NONE == err) {
		g_list_foreach (pos_list, print_pos, NULL);
		g_list_foreach (acc_list, print_acc, NULL);
	} else g_warning ("SYNC>>>location_get_position_from_address() failed> error code:%d", err);
	location_address_free (addr);

	char* addr_str = g_strdup("4 N 2nd Street 95113");
	err = location_get_position_from_freeformed_address(loc, addr_str, &pos_list, &acc_list);
	if (LOCATION_ERROR_NONE == err) {
		g_list_foreach (pos_list, print_pos, NULL);
		g_list_foreach (acc_list, print_acc, NULL);
	} else g_warning ("SYNC>> location_get_position_from_freeformed_address() failed> error code:%d", err);
	g_free(addr_str);

	pos = location_position_new (0, 37.3322, -121.8720, 0, LOCATION_STATUS_2D_FIX);
	err = location_get_address_from_position(loc, pos, &addr, &acc);
	if (LOCATION_ERROR_NONE == err) {
		g_debug ("SYNC>> location_get_address_from_position() success> %s %s %s %s %s %s %s",
			addr->building_number, addr->street, addr->district, addr->city, addr->state, addr->postal_code, addr->country_code);
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_address_free(addr);
		location_accuracy_free(acc);
	} else g_warning ("SYNC>> location_get_address_from_position() failed> error code:%d", err);
	location_position_free (pos);

	g_signal_connect (loc, "service-enabled", G_CALLBACK(cb_service_enabled), loc);
	g_signal_connect (loc, "service-disabled", G_CALLBACK(cb_service_disabled), loc);

	g_timeout_add_seconds (3, async_request, loc);
	int ret = location_start (loc);
	if( LOCATION_ERROR_NONE != ret ){
		g_debug("location_start failed[%d]",ret);
		return -1;
	}

	g_timeout_add_seconds (60, exit_program, NULL);
	g_main_loop_run (loop);

	location_stop (loc);
	location_free (loc);

	return 0;
}