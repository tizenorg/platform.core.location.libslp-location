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


#ifndef __LOCATION_H__
#define __LOCATION_H__


#include <glib.h>
#include <location/location-types.h>
#include <location/location-position.h>
#include <location/location-velocity.h>
#include <location/location-accuracy.h>
#include <location/location-address.h>
#include <location/location-boundary.h>
#include <location/location-satellite.h>
#include <location/location-poi-info.h>

G_BEGIN_DECLS

/**
 * @file location.h
 * @brief This file contains the Location API and related structure and enumeration.
 */
/**
 * @defgroup LocationFW LocationFW
 * @brief This is a Locatoin Framework for providing location based services.
 * @addtogroup LocationFW
 * @{
 * @defgroup LocationAPI Location API
 * @brief This sub module provides the Location API.
 * @addtogroup LocationAPI
 * @{
 */

/**
 * @brief
 * Initialize location sub module.
 * @remarks None.
 * This API should be called before any other Location APIs.
 * @pre None.
 * @post None.
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 * @see None.
 * @par Example
 * @code
#include <location.h>
int main (int argc, char *argv[])
{
    location_init();
    // Now you can use any other Location APIs.
    return 0;
}
 * @endcode
 */
int location_init (void);

/**
 * @brief
 * Create a new #LocationObject by using given #LocationMethod.
 * @remarks
 * Returned object is necessary for other APIs.
 * @pre
 * #location_init should be called before.
 * @post None.
 * @param [in]
 * method - Location method to be used.
 * @return a new #LocationObject
 * @retval NULL              if error occured
 * @see location_free
 * @par Example
 * @code
#include <location.h>
int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	location_init ();

	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc)
		return -1;
	// You can use new location object now.
	return 0;
}
 * @endcode
 */
LocationObject *location_new (LocationMethod method);

/**
 * @brief
 * Free memory of given #LocationObject.
 * @remarks None.
 * @pre
 * #location_init should be called before.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new.
 * @return int
 * @retval 0                              Success.
 *
 * Please refer #LocationError for more information.
 * @see location_new
 * @par Example
 * @code
#include <location.h>
int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	location_init ();
	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc)
		return -1;

	location_free (loc);
	// You can not use location object anymore.
	return 0;
}
 * @endcode
 */
int location_free (LocationObject *obj);

/**
 * @brief
 * Start the location service by using given #LocationObject.
 * @remarks
 * If you want to recieve signals, you should use this API.
 * @pre
 * #location_init should be called before.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @return int
 * @retval 0                              Success.
 *
 * Please refer #LocationError for more information.
 * @see location_stop
 * @par Example
 * @code
#include <location.h>
static GMainLoop *loop = NULL;

static void cb_service_enabled (GObject *self, guint status, gpointer userdata)
{
	LocationObject *loc = (LocationObject*)userdata;
	LocationAccuracy *acc = NULL;
	LocationPosition *pos = NULL;
	LocationVelocity *vel = NULL;
	LocationAddress *addr = NULL;
	LocationMethod method;

	g_object_get(loc, "method", &method, NULL);
	g_debug("Get property>> method:%d", method);

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
cb_service_updated (GObject *self,
       guint type,
       gpointer data,
       gpointer accuracy,
       gpointer userdata)
{
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


int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	int interval = 6; 	//seconds
	location_init ();

	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc)
		return -1;

	g_object_set(loc, "update-interval", interval, NULL);

	g_signal_connect (loc, "service-enabled", G_CALLBACK(cb_service_enabled), loc);
	g_signal_connect (loc, "service-updated", G_CALLBACK(cb_service_updated), loc);

	location_start(loc);
	loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (loop);  // GMainLoop is needed for receiving signals.

	// ...
	return 0;
}
* @endcode
*/
int location_start (LocationObject *obj);

/**
 * @brief
 * Stop the location service by using given #LocationObject.
 * @remarks
 * After call this API, you can not recieve signals.
 * @pre
 * #location_init should be called before.\n
 * #location_start should be called before.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 * @see location_start
 * @par Example
 * @code
#include <location.h>
static GMainLoop *loop = NULL;

int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	location_init ();
	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc)
		return -1;

	location_start(loc);
	loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (loop);

	// ....

	location_stop (loc);
	// you can not receive signals anymore.
	return 0;
}
 * @endcode
 */
int location_stop (LocationObject *obj);

/**
 * @brief
 * Check wheither a method is available.
 * @remarks
 * @pre
 * #location_init should be called before.\n
 * @post None.
 * @param [in] method - a #LocationMethod
 * @return int
 * @retval True		Supported
 *	   False 	Not supported
 * @par Example
 #include <location.h>
static GMainLoop *loop = NULL;

int main (int argc, char *argv[])
{
	gboolean is_supported = FALSE;

	// ....

	is_supported = location_is_supported_method(LOCATION_METHOD_HYBRID);
	if(is_supported == TRUE)
		g_printf("Hybrid Method is supported.\n");
	else
		g_printf("Hybrid Method is not supported.\n");

	return 0;
}* @code
 * @endcode
 */
gboolean location_is_supported_method(LocationMethod method);

/**
 * @brief
 * Check wheither GPS is turned on or off.
 * @remarks
 * @pre
 * #location_init should be called before.\n
 * @post None.
 * @param [in] method - a #LocationMethod
 * @return int
 * @retval True		Turned on
 *	   False 	Turned off
 * @par Example
 #include <location.h>
static GMainLoop *loop = NULL;

int main (int argc, char *argv[])
{
	gboolean is_enabled = FALSE;

	// ....

	is_enabled = location_is_enabled_gps(loc);
	if(is_enable == TRUE)
		g_printf("GPS is turned on.\n");
	else
		g_printf("GPS is turned off.\n");

	return 0;
}* @code
 * @endcode
 */
gboolean location_is_enabled_gps(LocationObject *obj);

/**
 * @brief
 * Get current position information with estimate of the accuracy.
 * @remarks Out parameters are should be freed.
 * @pre
 * #location_init should be called before.\n
 * #location_start should be called before.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [out]
 * position - a new #LocationPosition
 * @param [out]
 * accuracy - a new #LocationAccuracy
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 * @see location_get_velocity
 * @par Example
 * @code
#include <location.h>
static GMainLoop *loop = NULL;

static void cb_service_enabled (GObject *self, guint status, gpointer userdata)
{
	g_debug("cb_service_enabled: status(%d) userdata(0x%x)", status, (unsigned int)userdata);

	LocationObject *loc = (LocationObject*)userdata;
	LocationAccuracy *acc = NULL;
	LocationPosition *pos = NULL;

	// This function works properly after service is enabled.
	if (LOCATION_ERROR_NONE == location_get_position (loc, &pos, &acc)) {
		g_debug ("SYNC>> Current position> time: %d, lat: %f, long: %f, alt: %f, status: %d",
			pos->timestamp, pos->latitude, pos->longitude, pos->altitude, pos->status);
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)",
			acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_position_free(pos);
		location_accuracy_free(acc);
	} else g_warning ("SYNC>> Current position> failed");
}

int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	gulong handler_id = 0;

	location_init ();
	loop = g_main_loop_new (NULL, TRUE);
	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc){
		g_debug("location_new failed");
		return -1;
	}

	handler_id = g_signal_connect (loc, "service-enabled", G_CALLBACK(cb_service_enabled), loc);
	location_start (loc);
	g_main_loop_run (loop);

	g_signal_handler_disconnect(loc, handler_id);
	location_stop (loc);
	location_free (loc);

	return 0;
}
 * @endcode
 */
int location_get_position (LocationObject *obj, LocationPosition **position, LocationAccuracy **accuracy);

/**
 * @brief
 * Get last position information with estimate of the accuracy.
 * @remarks This API is not implemented now. \n
 *   Out parameters are should be freed.
 * @pre
 * #location_init should be called before.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [out]
 * position - a new #LocationPosition
 * @param [out]
 * accuracy - a new #LocationAccuracy
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 * @see location_get_position
 * @par Example
 * @code
#include <location.h>

int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	int ret = 0;
	LocationPosition *last_pos = NULL;
	LocationAccuracy *last_acc = NULL;

	location_init ();
	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc){
		g_debug("location_new failed");
		return -1;
	}

	if (LOCATION_ERROR_NONE == location_get_last_position (loc, LOCATION_METHOD_GPS, &last_pos, &last_acc)) {
		g_debug ("SYNC>> Last position> time: %d, lat: %f, long: %f, alt: %f, status: %d",
			last_pos->timestamp, last_pos->latitude, last_pos->longitude, last_pos->altitude, last_pos->status);
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)",
			last_acc->level, last_acc->horizontal_accuracy, last_acc->vertical_accuracy);
		location_position_free(last_pos);
		location_accuracy_free(last_acc);
	} else g_warning ("SYNC>> Last position> failed");

	location_free (loc);

	return 0;
}
 * @endcode
 */
int location_get_last_position (LocationObject *obj, LocationMethod method, LocationPosition **position, LocationAccuracy **accuracy);

/**
 * @brief
 * Get last satellite information.
 * @remarks This API is not implemented now. \n
 *   Out parameters are should be freed.
 * @pre
 * #location_init should be called before.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [out] satellite - a new #LocationSatellite
 * @return int
 * @retval 0                              Success
 * Please refer #LocationError for more information.
 * @see location_get_last_satellite
 * @par Example
 * @code
#include <location.h>

int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	int ret = 0, idx = 0;
	LocationSatellite *sat = NULL;
	guint prn;
	gboolean used;
	guint elevation;
	guint azimuth;
	gint snr;

	location_init ();
	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc){
		g_debug("location_new failed");
		return -1;
	}

	if (LOCATION_ERROR_NONE == location_get_satellite (loc, &sat)) {
		g_debug ("SYNC>> Current Sattelite> satellite in view = %d, satellite in used = %d", sat->num_of_sat_inview, sat->num_of_sat_used);
		g_debug ("\tinview satellite information = ");
		for (idx=0; idx<sat->num_of_sat_inview; idx++) {
			location_satellite_get_satellite_details(sat, idx, &prn, &used, &elevation, &azimuth, &snr);
			g_debug ("\t\t[%02d] used: %d, prn: %d, elevation: %d, azimuth: %d, snr: %d", idx, used, prn, elevation, azimuth, snr);
		}
		location_satellite_free (sat);
	} else g_warning ("SYNC>> Current satellite> failed");

	location_free (loc);

	return 0;
}
 * @endcode
 */
int location_get_satellite (LocationObject *obj, LocationSatellite **satellite);

/**
 * @brief
 * Get last satellite information.
 * @remarks This API is not implemented now. \n
 *   Out parameters are should be freed.
 * @pre
 * #location_init should be called before.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [out]
 * satellite - a new #LocationSatellite
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 * @par Example
 * @code
#include <location.h>

int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	int ret = 0, idx = 0;
	LocationSatellite *last_sat = NULL;
	guint prn;
	gboolean used;
	guint elevation;
	guint azimuth;
	gint snr;

	location_init ();
	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc){
		g_debug("location_new failed");
		return -1;
	}

	if (LOCATION_ERROR_NONE == location_get_last_satellite (loc, &last_sat)) {
		g_debug ("SYNC>> Last Sattelite> satellite in view = %d, satellite in used = %d", last_sat->num_of_sat_inview, last_sat->num_of_sat_used);
		g_debug ("\tinview satellite information = ");
		for (idx=0; idx<last_sat->num_of_sat_inview; idx++) {
			location_satellite_get_satellite_details(last_sat, idx, &prn, &used, &elevation, &azimuth, &snr);
			g_debug ("\t\t[%02d] used: %d, prn: %d, elevation: %d, azimuth: %d, snr: %d", idx, used, prn, elevation, azimuth, snr);
		}
		location_satellite_free (last_sat);
	} else g_warning ("SYNC>> Last satellite> failed");

	location_free (loc);

	return 0;
}
 * @endcode
 */
int location_get_last_satellite (LocationObject *obj, LocationSatellite **satellite);

/**
 * @brief
 * Get last known position information with estimate of the accuracy.
 * @remarks This API would be DEPRECATED. \n
 * @see location_get_last_position
 */
int location_get_last_known_position (LocationObject *obj, LocationMethod method, LocationLastPosition *last_position) LOCATION_DEPRECATED_API;

/**
 * @brief
 * Get current position information with estimate of the accuracy by using given address information.
 * @remarks Out parameters are should be freed.
 * @pre
 * #location_init should be called before.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [in]
 * address - a #LocationAddress
 * @param [out]
 * position_list - a list of #LocationPosition
 * @param [out]
 * accuracy_list - a list of #LocationAccuracy
 * @return int
 * @retval 0                              Success.
 *
 * Please refer #LocationError for more information.
 * @see
 * location_get_position_from_address_async\n
 * @par Example
 * @code
#include <location.h>

static void PrintPos (gpointer data, gpointer user_data)
{
	LocationPosition *pos = (LocationPosition *)data;

	if (pos) {
		g_debug("time: [%d], latitude: [%f], longitude: [%f], altitude: [%f]\n", pos->timestamp, pos->latitude, pos->longitude, pos->altitude);
		location_position_free (pos);
	}
}

static void PrintAcc (gpointer data, gpointer user_data)
{
	LocationAccuracy *acc = (LocationAccuracy *)data;

	if (acc) {
		g_debug("level: [%d], horizontal_accuracy: [%f], vertical_accuracy: [%f]\n", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_accuracy_free (acc);
	}
}

int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	int ret = LOCATION_ERROR_NONE;

	location_init ();
	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc){
		g_debug("location_new failed");
		return -1;
	}

	GList *pos_list = NULL;
	GList *acc_list = NULL;
	LocationAddress *addr = NULL;

	addr = location_address_new ("1", "Post Street", NULL, "san jose", "ca", NULL, "95113");
	if (LOCATION_ERROR_NONE == location_get_position_from_address(loc, addr, &pos_list, &acc_list)) {
		g_list_foreach (pos_list, PrintPos, NULL);
		g_list_foreach (acc_list, PrintAcc, NULL);
	} else g_warning ("SYNC>> position from address> failed");

	if (pos_list) {
		g_list_free (pos_list);
	}
	if (acc_list) {
		g_list_free (acc_list);
	}

	location_address_free (addr);
	location_free (loc);
	return 0;
}
 * @endcode
 */
int location_get_position_from_address (LocationObject *obj, const LocationAddress *address, GList **position_list, GList **accuracy_list);

/**
 * @brief
 * Get current position information asynchronously with estimate of the accuracy by using given address information.
 * @remarks None.
 * @pre
 * #location_init should be called before.\n
 * Calling application must have glib or ecore main loop.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [in]
 * address - a #LocationAddress
 * @param [in]
 * callback - A pointer of function which will be called after position is gained or when an error occurs.
 * @param [in]
 * userdata - data to pass to function
 * @return int
 * @retval 0                              Success.
 *
 * Please refer #LocationError for more information.
 * @see
 * location_get_position_from_address\n
 * @par Example
 * @code
#include <location.h>

static void PrintPos (gpointer data, gpointer user_data)
{
	LocationPosition *pos = (LocationPosition *)data;

	if (pos) {
		g_debug("time: [%d], latitude: [%f], longitude: [%f], altitude: [%f]\n", pos->timestamp, pos->latitude, pos->longitude, pos->altitude);
	}
}

static void PrintAcc (gpointer data, gpointer user_data)
{
	LocationAccuracy *acc = (LocationAccuracy *)data;

	if (acc) {
		g_debug("level: [%d], horizontal_accuracy: [%f], vertical_accuracy: [%f]\n", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
	}
}
static void
cb_position_from_address (LocationError error, GList *position_list, GList *accuracy_list, gpointer userdata)
{
	if (position_list && accuracy_list) {
		g_list_foreach (position_list, PrintPos);
		g_list_foreach (accuracy_list, PrintAcc);
	}
}

void get_position_from_address(LocationObject* loc)
{
	LocationAddress *addr = location_address_new ("1", "Post Street", NULL, "san jose", "ca", NULL, "95113");
	//Calling application must have an active data connection before using this function.
	if (LOCATION_ERROR_NONE == location_get_position_from_address_async(loc, addr, cb_position_from_address, loc))
		g_debug("location_get_position_from_address_async() success");
	else g_warning ("location_get_position_from_address_async() failed");
	location_address_free (addr);
}
 * @endcode
 */
int location_get_position_from_address_async (LocationObject *obj, const LocationAddress *address, LocationPositionCB callback, gpointer userdata);

/**
 * @brief
 * Get current position information with estimate of the accuracy by using given free-formed address string.
 * @remarks Out parameters are should be freed.
 * @pre
 * #location_init should be called before.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [in]
 * address - Free-formed address string to be used
 * @param [out]
 * position_list - a list of #LocationPosition
 * @param [out]
 * accuracy_list - a list of #LocationAccuracy
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 * @see
 * location_get_position_from_freeformed_address_async\n
 * @par Example
 * @code
#include <location.h>

static void PrintPos (gpointer data, gpointer user_data)
{
	LocationPosition *pos = (LocationPosition *)data;

	if (pos) {
		g_debug("time: [%d], latitude: [%f], longitude: [%f], altitude: [%f]\n", pos->timestamp, pos->latitude, pos->longitude, pos->altitude);
		location_position_free (pos);
	}
}

static void PrintAcc (gpointer data, gpointer user_data)
{
	LocationAccuracy *acc = (LocationAccuracy *)data;

	if (acc) {
		g_debug("level: [%d], horizontal_accuracy: [%f], vertical_accuracy: [%f]\n", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_accuracy_free (acc);
	}
}

int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	int ret = LOCATION_ERROR_NONE;

	location_init ();
	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc){
		g_debug("location_new failed");
		return -1;
	}

	GList *pos_list = NULL;
	GList *acc_list = NULL;
	char* addr_str = g_strdup("4 N 2nd Street 95113");
	//Calling application must have an active data connection before using this function.
	if (LOCATION_ERROR_NONE == location_get_position_from_freeformed_address(loc, addr_str, &pos_list, &acc_list)) {
		g_list_foreach (pos_list, PrintPos, NULL);
		g_list_foreach (acc_list, PrintAcc, NULL);
	} else g_warning ("SYNC>> position from freeformed address> failed");
	g_free(addr_str);

	location_free (loc);
	return 0;
}
 * @endcode
 */
int location_get_position_from_freeformed_address (LocationObject *obj, const gchar *address, GList **position_list, GList **accuracy_list);

/**
 * @brief
 * Get current position information asynchronously with estimate of the accuracy by using given free-formed address string.
 * @remarks None.
 * @pre
 * #location_init should be called before.\n
 * Calling application must have glib or ecore main loop.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [in]
 * address - Free-formed address string to be used
 * @param [in]
 * callback - A pointer of function which will be called after position is gained or when an error occurs.
 * @param [in]
 * userdata - data to pass to function
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 * @see
 * location_get_position_from_freeformed_address\n
 * @par Example
 * @code
#include <location.h>

static void PrintPos (gpointer data, gpointer user_data)
{
	LocationPosition *pos = (LocationPosition *)data;

	if (pos) {
		g_debug("time: [%d], latitude: [%f], longitude: [%f], altitude: [%f]\n", pos->timestamp, pos->latitude, pos->longitude, pos->altitude);
	}
}

static void PrintAcc (gpointer data, gpointer user_data)
{
	LocationAccuracy *acc = (LocationAccuracy *)data;

	if (acc) {
		g_debug("level: [%d], horizontal_accuracy: [%f], vertical_accuracy: [%f]\n", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
	}
}

static void
cb_position_from_freeformed_address (LocationError error, GList *position_list, GList *accuracy_list, gpointer userdata)
{
	if (position_list && accuracy_list) {
		g_list_foreach (position_list, PrintPos);
		g_list_foreach (accuracy_list, PrintAcc);
	}
}

void get_position_from_address(LocationObject* loc)
{
	gchar *addr_str = g_strdup("4 N 2nd Street 95113");
	//Calling application must have an active data connection before using this function.
	if (LOCATION_ERROR_NONE == location_get_position_from_freeformed_address_async(loc, addr_str, cb_position_from_freeformed_address, loc))
		g_debug("location_get_position_from_freeformed_address_async() success");
	else g_warning ("location_get_position_from_freeformed_address_async() failed");
	g_free(addr_str);

}
 * @endcode
 */
int location_get_position_from_freeformed_address_async (LocationObject *obj, const gchar *address, LocationPositionCB callback,	gpointer userdata);

/**
 * @brief
 * Get current velocity information with estimate of the accuracy.
 * @remarks Out parameters are should be freed.
 * @pre
 * #location_init should be called before.\n
 * #location_start should be called before.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [out]
 * velocity - a new #LocationVelocity
 * @param [out]
 * accuracy - a new #LocationAccuracy
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 * @see location_get_position
 * @par Example
 * @code
#include <location.h>
static GMainLoop *loop = NULL;

static void cb_service_enabled (GObject *self, guint status, gpointer userdata)
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

int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	gulong hander_id = 0;
	location_init ();

	loop = g_main_loop_new (NULL, TRUE);

	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc){
		g_debug("location_new failed");
		return -1;
	}

	handler_id = g_signal_connect (loc, "service-enabled", G_CALLBACK(cb_service_enabled), loc);
	location_start (loc);
	g_main_loop_run (loop);

	g_signal_handler_disconnect(loc, handler_id);
	location_stop (loc);
	location_free (loc);

	return 0;
}

 * @endcode
 */
int location_get_velocity (LocationObject *obj, LocationVelocity **velocity, LocationAccuracy **accuracy);

/**
 * @brief
 * Get last velocity information with estimate of the accuracy.
 * @remarks This API is not implemented now. \n
 * Out parameters are should be freed.
 * @pre
 * #location_init should be called before.\n
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [out]
 * velocity - a new #LocationVelocity
 * @param [out]
 * accuracy - a new #LocationAccuracy
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 * @see location_get_velocity
 * @par Example
 * @code
#include <location.h>

int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	LocationVelocity *last_vel = NULL;
	LocationAccuracy *last_acc = NULL;
	location_init ();

	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc){
		g_debug("location_new failed");
		return -1;
	}

	if (LOCATION_ERROR_NONE == location_get_last_velocity (loc, &last_vel, &last_acc)) {
		g_debug ("SYNC>> Last velocity> time: %d, speed: %f, direction:%f, climb:%f",
			last_vel->timestamp, last_vel->speed, last_vel->direction, last_vel->climb);
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)",
			last_acc->level, last_acc->horizontal_accuracy, last_acc->vertical_accuracy);
		location_velocity_free(last_vel);
		location_accuracy_free(last_acc);
	} else g_warning ("SYNC>> Last velocity> failed.");

	location_free (loc);

	return 0;
}
 * @endcode
 */
int location_get_last_velocity (LocationObject *obj, LocationVelocity **velocity, LocationAccuracy **accuracy);


/**
 * @brief
 * Get current address information with estimate of the accuracy by using current position.
 * @remarks Out parameters are should be freed.
 * @pre
 * #location_init should be called before.\n
 * #location_start should be called before.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [out]
 * address - a new #LocationAddress
 * @param [out]
 * accuracy - a new #LocationAccuracy
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 * @see
 * location_get_address_async\n
 * @par Example
 * @code
#include <location.h>
static GMainLoop *loop = NULL;

static void cb_service_enabled (GObject *self, guint status, gpointer userdata)
{
	g_debug("cb_service_enabled: status(%d) userdata(0x%x)", status, (unsigned int)userdata);

	LocationAddress *addr = NULL;
	LocationAccuracy *acc = NULL;
	LocationObject *loc = (LocationObject*)userdata;

	// This function works properly after service is enabled.
	//Calling application must have an active data connection before using this function.
	if (LOCATION_ERROR_NONE == location_get_address(loc, &addr, &acc)) {
		g_debug ("SYNC>> Current address> %s %s %s %s %s %s %s",
			addr->building_number, addr->street, addr->district, addr->city, addr->state, addr->postal_code, addr->country_code);
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_address_free(addr);
		location_accuracy_free(acc);
	} else g_warning ("SYNC>> Current address> failed");
}

int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	gulong handler_id = 0;
	int ret = LOCATION_ERROR_NONE;

	location_init ();

	loop = g_main_loop_new (NULL, TRUE);

	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc){
		g_debug("location_new failed");
		return -1;
	}

	handler_id = g_signal_connect (loc, "service-enabled", G_CALLBACK(cb_service_enabled), loc);
	location_start (loc);
	g_main_loop_run (loop);

	g_signal_handler_disconnect(loc, handler_id);
	location_stop (loc);
	location_free (loc);

	return 0;
}
 * @endcode
 */
int location_get_address (LocationObject *obj, LocationAddress **address, LocationAccuracy **accuracy);

/**
 * @brief
 * Get current address information asynchronously with estimate of the accuracy by using current position.
 * @remarks None.
 * @pre
 * #location_init should be called before.\n
 * #location_start should be called before.\n
 * Calling application must have glib or ecore main loop.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [in]
 * callback - A pointer of function which will be called after address is gained or when an error occurs.
 * @param [in]
 * userdata - data to pass to function
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 * @see
 * location_get_address\n
 * @par Example
 * @code
#include <location.h>
static GMainLoop *loop = NULL;

static void
cb_address (LocationError error, LocationAddress *addr, LocationAccuracy *acc, gpointer userdata)
{
	g_debug ("ASYNC>> location_get_address_async> %s %s %s %s %s %s %s",
			addr->building_number, addr->street, addr->district, addr->city, addr->state, addr->postal_code, addr->country_code);
	g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
}

static void cb_service_enabled (GObject *self, guint status, gpointer userdata)
{
	g_debug("cb_service_enabled: status(%d) userdata(0x%x)", status, (unsigned int)userdata);

	LocationObject *loc = (LocationObject*)userdata;
	// This function works properly after service is enabled.
	//Calling application must have an active data connection before using this function.
	if (LOCATION_ERROR_NONE == location_get_address_async(loc, cb_address, loc))
		g_debug("location_get_address_async() success");
	else g_warning ("location_get_address_async() failed");
}


int main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	gulong handler_id = 0;
	int ret = LOCATION_ERROR_NONE;

	location_init ();

	loop = g_main_loop_new (NULL, TRUE);

	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc){
		g_debug("location_new failed");
		return -1;
	}

	handler_id = g_signal_connect (loc, "service-enabled", G_CALLBACK(cb_service_enabled), loc);
	location_start (loc);
	g_main_loop_run (loop);

	g_signal_handler_disconnect(loc, handler_id);
	location_stop (loc);
	location_free (loc);

	return 0;
}
 * @endcode
 */
int location_get_address_async (LocationObject *obj, LocationAddressCB callback, gpointer userdata);

/**
 * @brief
 * Get current address information with estimate of the accuracy by using given position information.
 * @remarks Out parameters are should be freed.
 * @pre
 * #location_init should be called before.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [in]
 * position - a #LocationPosition
 * @param [out]
 * address - a new #LocationAddress
 * @param [out]
 * accuracy - a new #LocationAccuracy
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 * @see
 * location_get_address_from_position_async\n
 * @par Example
 * @code
#include <location.h>
static GMainLoop *loop = NULL;

int
main (int argc, char *argv[])
{
	LocationObject *loc = NULL;
	int ret = LOCATION_ERROR_NONE;

	location_init ();

	loop = g_main_loop_new (NULL, TRUE);

	loc  = location_new (LOCATION_METHOD_GPS);
	if(!loc){
		g_debug("location_new failed");
		return -1;
	}

	LocationPosition *pos = NULL;
	LocationAccuracy *acc = NULL;
	LocationAddress *addr = NULL;

	//Calling application must have an active data connection before using this function.
	pos = location_position_new (0, 37.257809, 127.056383, 0, LOCATION_STATUS_2D_FIX);
	if (LOCATION_ERROR_NONE == location_get_address_from_position(loc, pos, &addr, &acc)) {
		g_debug ("SYNC>> address from position> %s %s %s %s %s %s %s",
			addr->building_number, addr->street, addr->district, addr->city, addr->state, addr->postal_code, addr->country_code);
		g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
		location_address_free(addr);
		location_accuracy_free(acc);
	} else g_warning ("SYNC>> address from position> failed");
	location_position_free (pos);
}
 * @endcode
 */
int location_get_address_from_position (LocationObject *obj, const LocationPosition *position, LocationAddress **address, LocationAccuracy **accuracy);

/**
 * @brief
 * Get current address information asynchronously with estimate of the accuracy by using given position information.
 * @remarks None.
 * @pre
 * #location_init should be called before.\n
 * Calling application must have glib or ecore main loop.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]
 * obj - a #LocationObject created by #location_new
 * @param [in]
 * position - a #LocationPosition
 * @param [in]
 * callback - A pointer of function which will be called after address is gained or when an error occurs.
 * @param [in]
 * userdata - data to pass to function
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 * @see
 * location_get_address_from_position\n
 * @par Example
 * @code
#include <location.h>
static GMainLoop *loop = NULL;

static void
cb_address_from_position (LocationError error, LocationAddress *addr, LocationAccuracy *acc, gpointer userdata)
{
	g_debug ("ASYNC>> location_get_address_from_position_async> %s %s %s %s %s %s %s",
			addr->building_number, addr->street, addr->district, addr->city, addr->state, addr->postal_code, addr->country_code);
	g_debug ("\tAccuracy level %d (%.0f meters %.0f meters)", acc->level, acc->horizontal_accuracy, acc->vertical_accuracy);
}

void get_address_from_position(LocationObject* loc)
{
	LocationPosition *pos = location_position_new (0, 37.257809, 127.056383, 0, LOCATION_STATUS_2D_FIX);
	//Calling application must have an active data connection before using this function.
	if (LOCATION_ERROR_NONE == location_get_address_from_position_async(loc, pos, cb_address_from_position, loc))
		g_debug("location_get_address_from_position_async() success");
	else g_warning ("location_get_address_from_position_async() failed");
	location_position_free (pos);
}
 * @endcode
 */
int location_get_address_from_position_async (LocationObject *obj, const LocationPosition *position,	LocationAddressCB callback, gpointer userdata);

/**
 * @brief
 * Get information of point of interests by using current position within a given radius.
 * @remarks Out parameters are should be freed.
 * \n This functions is not implemneted yet.
 * @pre
 * #location_init should be called before.\n
 * #location_start should be called before.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]  obj - a #LocationObject created by #location_new
 * @param [in]  radius - radius of a circle
 * @param [in]  keyword - keyword for POI
 * @param [out] poi_info - a new #LocationPOIInfo
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 */
int location_get_poi (LocationObject *obj, gdouble radius, const gchar *keyword, LocationPOIInfo **poi_info);

/**
 * @brief
 * Get information of point of interests asynchronously by using current position within a given radius.
 * @remarks This functions is not implemneted yet.
 * @pre
 * #location_init should be called before.\n
 * #location_start should be called before.\n
 * Calling application must have glib or ecore main loop.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]  obj - a #LocationObject created by #location_new
 * @param [in]  radius - radius of a circle
 * @param [in]  callback - function to call
 * @param [in]  userdata - data to pass to function
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 */
int location_get_poi_async (LocationObject *obj, gdouble radius, const gchar *keyword, LocationPOICB callback, gpointer userdata);

/**
 * @brief
 * Get information of point of interests by using given address within a given radius.
 * @remarks Out parameters are should be freed.
 * \n This functions is not implemneted yet.
 * @pre
 * #location_init should be called before.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]  obj - a #LocationObject created by #location_new
 * @param [in]  address - a #LocationAddress
 * @param [in]  radius - radius of a circle
 * @param [in]  keyword - keyword for POI
 * @param [out] poi_info - a new #LocationPOIInfo
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 */
int location_get_poi_from_address(LocationObject *obj, const LocationAddress *address, gdouble radius, const gchar *keyword,	LocationPOIInfo **poi_info);

/**
 * @brief
 * Get information of point of interests asynchronously by using given address within a given radius.
 * @remarks This functions is not implemneted yet.
 * @pre
 * #location_init should be called before.\n
 * Calling application must have glib or ecore main loop.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]  obj - a #LocationObject created by #location_new
 * @param [in]  address - a #LocationAddress
 * @param [in]  radius - radius of a circle
 * @param [in]  callback - function to call
 * @param [in]  userdata - data to pass to function
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 */
int location_get_poi_from_address_async (LocationObject *obj, const LocationAddress *address, gdouble radius, const gchar *keyword, LocationPOICB callback, gpointer userdata);

/**
 * @brief
 * Get information of point of interests by using given position within a given radius.
 * @remarks Out parameters are should be freed.
 * \n This functions is not implemneted yet.
 * @pre
 * #location_init should be called before.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]  obj - a #LocationObject created by #location_new
 * @param [in]  position - a #LocationPosition
 * @param [in]  radius - radius of a circle
 * @param [in]  keyword - keyword for POI
 * @param [out] poi_info - a new #LocationPOIInfo
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 */
int location_get_poi_from_position(LocationObject *obj, const LocationPosition *position, gdouble radius,	const gchar *keyword, LocationPOIInfo **poi_info);

/**
 * @brief
 * Get information of point of interests asynchronously by using given position within a given radius.
 * @remarks This functions is not implemneted yet.
 * @pre
 * #location_init should be called before.\n
 * Calling application must have glib or ecore main loop.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]  obj - a #LocationObject created by #location_new
 * @param [in]  position - a #LocationPosition
 * @param [in]  radius - radius of a circle
 * @param [in]  callback - function to call
 * @param [in]  userdata - data to pass to function
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 */
int location_get_poi_from_position_async (LocationObject *obj, const LocationPosition *position, gdouble radius,	const gchar*keyword, LocationPOICB callback, gpointer userdata);

/**
 * @brief
 * Send command to the server.
 * @remarks This functions is not implemneted yet.
 * @pre
 * #location_init should be called before.\n
 * Calling application must have glib or ecore main loop.\n
 * Calling application must have an active data connection.
 * @post None.
 * @param [in]  cmd - a #char
 * @return int
 * @retval 0                              Success
 *
 * Please refer #LocationError for more information.
 */
int location_send_command(const char *cmd);

/**
 * @} @}
 */

G_END_DECLS

#endif /* __LOCATION_H__ */
