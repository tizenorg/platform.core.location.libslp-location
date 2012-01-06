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

static GMainLoop *loop = NULL;

static void
update_gps(LocationObject* loc)
{
	g_debug("update gps");
	double ran = g_random_double()/200;
	LocationPosition* pos = location_position_new (time(NULL), 37.257809 + ran, 127.056383 + ran, 150.199997 + ran, LOCATION_STATUS_3D_FIX);
	g_object_set(loc, "position-base", pos, NULL);
	location_position_free (pos);

	LocationVelocity* vel = location_velocity_new (pos->timestamp, 0, 0, 0);
	g_object_set(loc, "velocity-base", vel, NULL);
	location_velocity_free (vel);

	LocationAccuracy* acc = location_accuracy_new (LOCATION_ACCURACY_LEVEL_DETAILED, 0, 0);
	g_object_set(loc, "accuracy-info", acc, NULL);
	location_accuracy_free (acc);

	LocationSatellite* sat = location_satellite_new (9);
	location_satellite_set_satellite_details(sat, 0, 5, 1, 0, 0, 41);
	location_satellite_set_satellite_details(sat, 1, 9, 1, 0, 0, 46);
	location_satellite_set_satellite_details(sat, 2, 15, 1, 0, 0, 45);
	location_satellite_set_satellite_details(sat, 3, 18, 1, 0, 0, 41);
	location_satellite_set_satellite_details(sat, 4, 21, 1, 0, 0, 34);
	location_satellite_set_satellite_details(sat, 5, 22, 0, 0, 0, 41);
	location_satellite_set_satellite_details(sat, 6, 26, 0, 0, 0, 46);
	location_satellite_set_satellite_details(sat, 7, 27, 0, 0, 0, 31);
	location_satellite_set_satellite_details(sat, 8, 28, 0, 0, 0, 33);
	g_object_set(loc, "satellite-info", sat, NULL);
	location_satellite_free (sat);
}

static gboolean
update_timer(gpointer data)
{
	update_gps(data);
	return TRUE;
}

static void
cb_service_updated (GObject *self,
	guint type,
	gpointer data,
	gpointer accuracy,
	gpointer userdata)
{
	g_debug("cb_service_updated: type(%d) userdata(0x%x)", type, (unsigned int)userdata);

	LocationAccuracy *acc = (LocationAccuracy*) accuracy;
	switch (type) {
	case POSITION_UPDATED: {
			LocationPosition *pos = (LocationPosition*) data;
			g_debug ("ASYNC>> Current position> time: %d, lat: %f, long: %f, alt: %f, status: %d",
				pos->timestamp, pos->latitude, pos->longitude, pos->altitude, pos->status);
			g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)",
				acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		}
		break;
	case VELOCITY_UPDATED: {
			LocationVelocity *vel = (LocationVelocity*) data;
			g_debug ("ASYNC>> Current velocity> time: %d, speed: %f, direction:%f, climb:%f",
				vel->timestamp, vel->speed, vel->direction, vel->climb);
			g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)",
				acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		}
		break;
	default:
		g_warning ("ASYNC>> Undefined update type");
		break;
	}
}

static void
cb_service_enabled (GObject *self,
	guint status,
	gpointer userdata)
{
	g_debug("cb_service_enabled: status(%d) userdata(0x%x)", status, (unsigned int)userdata);

	LocationObject *loc = (LocationObject*)userdata;
	LocationAccuracy *acc = NULL;
	LocationPosition *pos = NULL;
	LocationVelocity *vel = NULL;
	LocationAddress *addr = NULL;

	if (LOCATION_ERROR_NONE == location_get_position (loc, &pos, &acc)) {
		g_debug ("SYNC>> Current position> time: %d, lat: %f, long: %f, alt: %f, status: %d",
			pos->timestamp, pos->latitude, pos->longitude, pos->altitude, pos->status);
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)",
			acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_position_free(pos);
		location_accuracy_free(acc);
	} else g_warning ("SYNC>> Current position> failed");
	if (LOCATION_ERROR_NONE == location_get_velocity (loc, &vel, &acc)) {
		g_debug ("SYNC>> Current velocity> time: %d, speed: %f, direction:%f, climb:%f",
			vel->timestamp, vel->speed, vel->direction, vel->climb);
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)",
			acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_velocity_free(vel);
		location_accuracy_free(acc);
	} else g_warning ("SYNC>> Current velocity> failed");
	if (LOCATION_ERROR_NONE == location_get_address(loc, &addr, &acc)) {
		g_debug ("SYNC>> Current address> %s %s %s %s %s %s %s",
			addr->building_number, addr->street, addr->district, addr->city, addr->state, addr->postal_code, addr->country_code);
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_address_free(addr);
		location_accuracy_free(acc);
	} else g_warning ("SYNC>> Current address> failed");
}

static void
cb_service_disabled (GObject *self,
	guint status,
	gpointer userdata)
{
	g_debug("cb_service_disabled: status(%d) userdata(0x%x)", status, (unsigned int)userdata);
}

static void
cb_zone_in (GObject *self,
	guint type,
	gpointer position,
	gpointer accuracy)
{
	g_debug("cb_zone_in: type(%d)", type);
	LocationPosition *pos = (LocationPosition*) position;
	LocationAccuracy *acc = (LocationAccuracy*) accuracy;

	g_debug ("ASYNC>> ZoneIn> Current position: time: %d, lat: %f, long: %f, alt: %f",
		pos->timestamp, pos->latitude, pos->longitude, pos->altitude);
	g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)",
		acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
}

static void
cb_zone_out (GObject *self,
	guint type,
	gpointer position,
	gpointer accuracy)
{
	g_debug("cb_zone_out: type(%d)", type);
	LocationPosition *pos = (LocationPosition*) position;
	LocationAccuracy *acc = (LocationAccuracy*) accuracy;

	g_debug ("ASYNC>> ZoneOut> Current position: time: %d, lat: %f, long: %f, alt: %f",
		pos->timestamp, pos->latitude, pos->longitude, pos->altitude);
	g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)",
		acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
}


int
main (int argc, char *argv[])
{
	LocationObject *loc = NULL;

	// If application is executed by AUL, this is not needed.
	g_setenv("PKG_NAME", "org.tizen.sps-test", 1);

	location_init ();

	loop = g_main_loop_new (NULL, TRUE);

	loc  = location_new (LOCATION_METHOD_SPS);
	if (!loc) {
		g_debug("location_new failed");
		return -1;
	}

	LocationMethod method = LOCATION_METHOD_NONE;
	g_object_get(loc, "method", &method, NULL);
	g_debug("Get property>> method:%d", method);

	LocationBoundary *bound = NULL;
	g_object_get(loc, "boundary", &bound, NULL);
	if (bound) {
		g_debug("Get property>> boundary> type: %d", bound->type);
		location_boundary_free (bound);
	} else g_warning("failed to get property> boundary");

	LocationPosition *rb = location_position_new(0, 37.255, 127.060, 0, LOCATION_STATUS_2D_FIX);
	LocationPosition *lt = location_position_new(0, 37.260, 127.055, 0, LOCATION_STATUS_2D_FIX);
	bound = location_boundary_new_for_rect(lt, rb);
	location_position_free (rb);
	location_position_free (lt);
	if (bound) {
		g_object_set(loc, "boundary", bound, NULL);
		location_boundary_free(bound);
	} else g_warning("failed to location_boundary_new_for_rect()");
	g_object_get(loc, "boundary", &bound, NULL);
	if (bound) {
		g_debug("Set property>> boundary> type: %d, (%f,%f),(%f,%f)",
			bound->type,
			bound->rect.right_bottom->latitude, bound->rect.right_bottom->longitude,
			bound->rect.left_top->latitude, bound->rect.left_top->longitude);
		location_boundary_free (bound);
	} else 	g_warning("failed to set property> boundary");

	update_gps(loc);
	g_signal_connect (loc, "service-enabled", G_CALLBACK(cb_service_enabled), loc);
	g_signal_connect (loc, "service-disabled", G_CALLBACK(cb_service_disabled), loc);
	g_signal_connect (loc, "service-updated", G_CALLBACK(cb_service_updated), loc);
	g_signal_connect (loc, "zone-in", G_CALLBACK(cb_zone_in), loc);
	g_signal_connect (loc, "zone-out", G_CALLBACK(cb_zone_out), loc);

	if( LOCATION_ERROR_NONE != location_start (loc) ){
		g_debug("location_start failed");
		return -1;
	}

	g_timeout_add_seconds(1, update_timer, loc);
	g_main_loop_run (loop);

	location_stop (loc);

	LocationPosition *pos = NULL;
	g_object_get(loc, "last-position", &pos, NULL);
	if (pos) {
		g_debug ("Get property>> last-position> time: %d, lat: %f, long: %f, alt: %f, status: %d",
			pos->timestamp, pos->latitude, pos->longitude, pos->altitude, pos->status);
		location_position_free(pos);
	} else 	g_warning("failed to get property> last-position");

	location_free (loc);

	return 0;
}
