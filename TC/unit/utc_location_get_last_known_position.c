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
#include <location.h>

static void startup(), cleanup();
void (*tet_startup) () = startup;
void (*tet_cleanup) () = cleanup;

static void utc_location_get_last_known_position_01();
static void utc_location_get_last_known_position_02();

struct tet_testlist tet_testlist[] = {
	{utc_location_get_last_known_position_01,1},
	{utc_location_get_last_known_position_02,2},
	{NULL,0},
};

int ret;
LocationObject* loc;

static void startup()
{
	ret = location_init();
	loc = location_new(LOCATION_METHOD_GPS);
	tet_printf("\n TC startup");
}

static void cleanup()
{
	if( loc )
		location_free(loc);
	tet_printf("\n TC End");
}

static void
utc_location_get_last_known_position_01()
{
	int ret = 0;
	LocationLastPosition last_known_position = {0, };

	ret = location_get_last_known_position(loc, LOCATION_METHOD_HYBRID, &last_known_position);
	if (ret == LOCATION_ERROR_NONE) tet_result(TET_PASS);
	else tet_result(TET_FAIL);
}

static void
utc_location_get_last_known_position_02()
{
	int ret = 0;
	LocationLastPosition last_known_position = {0, };
	ret = location_get_last_known_position(NULL, LOCATION_METHOD_HYBRID, &last_known_position);
	if (ret == LOCATION_ERROR_NONE) tet_result(TET_PASS);
	else tet_result(TET_FAIL);
}

