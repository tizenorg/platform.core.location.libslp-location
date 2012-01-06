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

#include <tet_api.h>
#include <glib.h>
#include <gconf/gconf-client.h>
#include <location.h>

static void startup(), cleanup();
void (*tet_startup) () = startup;
void (*tet_cleanup) () = cleanup;

static void utc_location_get_position_from_freeformed_address_01();
static void utc_location_get_position_from_freeformed_address_02();
static void utc_location_get_position_from_freeformed_address_03();
static void utc_location_get_position_from_freeformed_address_04();

struct tet_testlist tet_testlist[] = {
	{utc_location_get_position_from_freeformed_address_01,1},
	{utc_location_get_position_from_freeformed_address_02,2},
	{utc_location_get_position_from_freeformed_address_03,3},
	{utc_location_get_position_from_freeformed_address_04,4},
	{NULL,0},
};

static GMainLoop *loop = NULL;
LocationObject* loc;
int ret;
int isNetStarted = 0;

#define GCONF_PROXY_MODE            "/system/proxy/mode"
#define GCONF_HTTP_PROXY_HOST       "/system/http_proxy/host"
#define GCONF_HTTP_PROXY_PORT       "/system/http_proxy/port"
#define ENV_HTTP_PROXY              "http_proxy"

static gboolean
exit_loop (gpointer data)
{
	g_main_loop_quit (loop);
	return FALSE;
}

static void startup()
{	
	location_init();
	loc = location_new(LOCATION_METHOD_GPS);

	loop = g_main_loop_new(NULL,FALSE);		
	g_main_loop_run (loop);

	tet_printf("\n TC startup");	
}

static void cleanup()
{	
	location_free(loc);
	tet_printf("\n TC End");
}

static void
utc_location_get_position_from_freeformed_address_01()
{	
	LocationPosition *pos = NULL;
	LocationAccuracy *acc = NULL;
	char* addr_str = g_strdup("4 N 2nd Street 95113");
	ret = location_get_position_from_freeformed_address(loc, addr_str, &pos, &acc);
	g_free(addr_str);
	tet_printf("Returned value: %d", ret);
	if( (ret == LOCATION_ERROR_NONE && 
		37.325276 <= pos->latitude &&  pos->latitude <= 37.345276 &&
		-121.900059 <= pos->longitude && pos->longitude<= -121.880059) ||
		ret == LOCATION_ERROR_CONFIGURATION ) {
		location_position_free (pos);
		location_accuracy_free (acc);
		tet_result(TET_PASS);
	}
	else
		tet_result(TET_FAIL);
	
}

static void
utc_location_get_position_from_freeformed_address_02()
{
	LocationPosition *pos = NULL;
	LocationAccuracy *acc = NULL;
	char* addr_str = g_strdup("4 N 2nd Street 95113");
	ret = location_get_position_from_freeformed_address(NULL, addr_str, &pos, &acc);
	g_free(addr_str);
	tet_printf("Returned value: %d", ret);
	if (ret == LOCATION_ERROR_PARAMETER) tet_result(TET_PASS);
	else tet_result(TET_FAIL);
}

static void
utc_location_get_position_from_freeformed_address_03()
{
	LocationPosition *pos = NULL;
	LocationAccuracy *acc = NULL;
	ret = location_get_position_from_freeformed_address(loc, NULL, &pos, &acc);
	tet_printf("Returned value: %d", ret);
	if (ret == LOCATION_ERROR_PARAMETER) tet_result(TET_PASS);
	else tet_result(TET_FAIL);
}

static void
utc_location_get_position_from_freeformed_address_04()
{
	LocationAccuracy *acc = NULL;
	char* addr_str = g_strdup("4 N 2nd Street 95113");
	ret = location_get_position_from_freeformed_address(loc, addr_str, NULL, &acc);
	g_free (addr_str);
	tet_printf("Returned value: %d", ret);
	if (ret == LOCATION_ERROR_PARAMETER) tet_result(TET_PASS);
	else tet_result(TET_FAIL);
}

