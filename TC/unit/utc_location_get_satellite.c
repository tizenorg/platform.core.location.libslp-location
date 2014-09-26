/*
 * libslp-location
 *
 * Copyright (c) 2010-2013 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Youngae Kang <youngae.kang@samsung.com>, Minjune Kim <sena06.kim@samsung.com>
 *          Genie Kim <daejins.kim@samsung.com>
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

#include <tet_api.h>
#include <glib.h>
#include <location.h>

static void startup(), cleanup();
void (*tet_startup) () = startup;
void (*tet_cleanup) () = cleanup;

static void utc_location_get_satellite_01();
static void utc_location_get_satellite_02();
static void utc_location_get_satellite_03();

struct tet_testlist tet_testlist[] = {
	{utc_location_get_satellite_01,1},
	{utc_location_get_satellite_02,2},
	{utc_location_get_satellite_03,3},
	{NULL,0},
};

static GMainLoop *loop = NULL;
int ret;
LocationObject* loc;

static gboolean
exit_loop (gpointer data)
{
	g_main_loop_quit (loop);
	tet_result(TET_FAIL);
	return FALSE;
}

static void startup()
{
	location_init();
	loc = location_new(LOCATION_METHOD_GPS);
	location_start(loc);
	loop = g_main_loop_new(NULL,FALSE);
	tet_printf("\n TC startup");
}

static void cleanup()
{
	location_stop(loc);
	location_free(loc);
	tet_printf("\n TC End");
}

static void
_get_satellite (GObject *self,
              			guint _status,
                    gpointer userdata)
{
	LocationSatellite *sat = NULL;
	LocationObject *loc = (LocationObject*)userdata;

	ret = location_get_satellite (loc, &sat);
	tet_printf("Returned value: %d", ret);
	if (ret == LOCATION_ERROR_NONE) {
		location_satellite_free(sat);
		tet_result(TET_PASS);
	} else tet_result(TET_FAIL);
	g_main_loop_quit (loop);
}

static void
utc_location_get_satellite_01()
{
	g_signal_connect (loc, "service-enabled", G_CALLBACK(_get_satellite), loc);
	g_timeout_add_seconds(60, exit_loop, loop);
	g_main_loop_run (loop);
}

static void
utc_location_get_satellite_02()
{
	LocationSatellite *sat = NULL;
	ret = location_get_satellite (NULL, &sat);
	tet_printf("Returned value: %d", ret);
	if (ret == LOCATION_ERROR_PARAMETER) tet_result(TET_PASS);
	else tet_result(TET_FAIL);
}

static void
utc_location_get_satellite_03()
{
	LocationSatellite *sat = NULL;
	ret = location_get_satellite (loc, NULL);
	tet_printf("Returned value: %d", ret);
	if (ret == LOCATION_ERROR_PARAMETER) tet_result(TET_PASS);
	else tet_result(TET_FAIL);
}