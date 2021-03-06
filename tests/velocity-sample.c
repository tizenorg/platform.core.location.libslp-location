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
#include <location.h>

static GMainLoop *loop = NULL;

static gboolean
exit_program (gpointer data)
{
	g_main_loop_quit (loop);
	g_debug ("Quit g_main_loop");
	return FALSE;
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
	case VELOCITY_UPDATED: {
			LocationVelocity *vel = (LocationVelocity*) data;
			g_debug ("ASYNC>> Current velocity> time: %d, speed: %f, direction:%f, climb:%f",
				vel->timestamp, vel->speed, vel->direction, vel->climb);
			g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)",
				acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		}
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
	LocationVelocity *vel = NULL;

	if (LOCATION_ERROR_NONE == location_get_velocity (loc, &vel, &acc)) {
		g_debug ("SYNC>> Current velocity> time: %d, speed: %f, direction:%f, climb:%f",
			vel->timestamp, vel->speed, vel->direction, vel->climb);
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)",
			acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_velocity_free(vel);
		location_accuracy_free(acc);
	} else g_warning ("SYNC>> Current velocity> failed");
}

static void
cb_service_disabled (GObject *self,
	guint status,
	gpointer userdata)
{
	g_debug("cb_service_disabled: status(%d) userdata(0x%x)", status, (unsigned int)userdata);
}

int
main (int argc, char *argv[])
{
	LocationObject *loc = NULL;

	location_init ();

	loop = g_main_loop_new (NULL, TRUE);

	loc  = location_new (LOCATION_METHOD_GPS);
	if (!loc) {
		g_debug("location_new failed");
		return -1;
	}

	g_signal_connect (loc, "service-enabled", G_CALLBACK(cb_service_enabled), loc);
	g_signal_connect (loc, "service-disabled", G_CALLBACK(cb_service_disabled), loc);
	g_signal_connect (loc, "service-updated", G_CALLBACK(cb_service_updated), loc);

	if( LOCATION_ERROR_NONE != location_start (loc) ){
		g_debug("location_start failed");
		return -1;
	}

	g_timeout_add_seconds(60, exit_program, NULL);
	g_main_loop_run (loop);

	location_stop (loc);
	location_free (loc);

	return 0;
}

