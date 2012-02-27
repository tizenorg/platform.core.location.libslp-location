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
#include <location/location.h>

int
main (int argc, char *argv[])
{
	LocationObject *loc = NULL;

	// If application is executed by AUL, this is not needed.
	g_setenv("PKG_NAME", "com.samsung.ips-test", 1);

	location_init ();

	loc  = location_new (LOCATION_METHOD_IPS);
	if (!loc) {
		g_debug("location_new failed");
		return -1;
	}

	LocationMethod method = LOCATION_METHOD_NONE;
	g_object_get(loc, "method", &method, NULL);
	g_debug("Get property>> method:%d", method);

	LocationAccuracy *acc = NULL;
	LocationPosition *pos = NULL;
	LocationAddress *addr = NULL;

	if (LOCATION_ERROR_NONE == location_get_position (loc, &pos, &acc)) {
		g_debug ("SYNC>> Current position> time: %d, lat: %f, long: %f, alt: %f, status: %d",
			pos->timestamp, pos->latitude, pos->longitude, pos->altitude, pos->status);
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)",
			acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_position_free(pos);
		location_accuracy_free(acc);
	} else g_warning ("SYNC>> Current position> failed");
	if (LOCATION_ERROR_NONE == location_get_address(loc, &addr, &acc)) {
		g_debug ("SYNC>> Current address> %s %s %s %s %s %s %s",
			addr->building_number, addr->street, addr->district, addr->city, addr->state, addr->postal_code, addr->country_code);
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_address_free(addr);
		location_accuracy_free(acc);
	} else g_warning ("SYNC>> Current address> failed");

	g_object_get(loc, "last-position", &pos, NULL);
	if (pos) {
		g_debug ("Get property>> last-position> time: %d, lat: %f, long: %f, alt: %f, status: %d",
			pos->timestamp, pos->latitude, pos->longitude, pos->altitude, pos->status);
		location_position_free(pos);
	} else 	g_warning("failed to get property> last-position");

	location_free (loc);

	return 0;
}
