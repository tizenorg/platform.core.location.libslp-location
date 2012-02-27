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

#ifndef __LOCATION_MODULE_H__
#define __LOCATION_MODULE_H__

#include <gmodule.h>
#include <location/location-types.h>
#include <location/location-position.h>
#include <location/location-velocity.h>
#include <location/location-accuracy.h>
#include <location/location-address.h>
#include <location/location-satellite.h>
#include <location/location-poi-info.h>

G_BEGIN_DECLS

/**
 * @file location-module.h
 * @brief This file contains the structure and enumeration for location plug-in development.
 */

/**
 * @addtogroup LocationFW
 * @{
 * @defgroup LocationModules Location Moduels
 * @brief  This sub module provides the definitions and structrues for 3rd party plugin modules.
 * @addtogroup LocationModules
 * @{
 */

/**
 * @brief This represents APIs declared in a geocode plug-in for location geocode modules.
 */
typedef struct{
	int (*get_service_name)(gpointer handle, gchar **servicename);                                                                         ///< This is used for getting a service name from a plug-in.
	int (*get_geocode)(gpointer handle, const LocationAddress *address, GList **position_list, GList **accuracy_list);         ///< This is used for getting a geocode from a plug-in.
	int (*get_geocode_freetext)(gpointer handle, const gchar *address, GList **position_list, GList **accuracy_list);          ///< This is used for getting a geocode by using a free-fromed address from a plug-in.
	int (*get_reverse_geocode)(gpointer handle, const LocationPosition *position, LocationAddress **address, LocationAccuracy **accuracy); ///< This is used for getting a reverse geocode from a plug-in.
	int (*get_geocode_async)(gpointer handle, const LocationAddress *address, LocationPositionCB callback, gpointer userdata);             ///< This is used for getting a geocode from a plug-in asynchronously.
	int (*get_geocode_freetext_async)(gpointer handle, const gchar *address, LocationPositionCB callback, gpointer userdata);              ///< This is used for getting a geocode by using a free-fromed address from a plug-in asynchronously.
	int (*get_reverse_geocode_async)(gpointer handle, const LocationPosition *position, LocationAddressCB callback, gpointer userdata);    ///< This is used for getting a reverse geocode from a plug-in asynchronously.	
} LocModGeoOps;

/**
 * @brief This represents APIs declared in a POI plug-in for location POI modules.
 */
typedef struct{
	int (*get_service_name)(gpointer handle, gchar** servicename);                                                                                                            ///< This is used for getting a service name from a plug-in.
	int (*get_poi)(gpointer handle, gdouble radius, const gchar* keyword, LocationPOIInfo **poi);                                                                             ///< This is used for getting a POI from a plug-in.
	int (*get_poi_from_address)(gpointer handle, const LocationAddress *address, gdouble radius, const gchar* keyword, LocationPOIInfo **poi);                                ///< This is used for getting a POI by using a address from a plug-in.
	int (*get_poi_from_position)(gpointer handle, const LocationPosition *position, gdouble radius, const gchar* keyword, LocationPOIInfo **poi);                             ///< This is used for getting a POI by using a position from a plug-in.
	int (*get_poi_async)(gpointer handle, gdouble radius, const gchar* keyword, LocationPOICB callback, gpointer userdata);                                                   ///< This is used for getting a POI from a plug-in asynchronously.
	int (*get_poi_from_address_async)(gpointer handle, const LocationAddress *address, gdouble radius, const gchar* keyword, LocationPOICB callback, gpointer userdata);      ///< This is used for getting a POI by using a address from a plug-in asynchronously.
	int (*get_poi_from_position_async)(gpointer handle, const LocationPosition *position, gdouble radius, const gchar* keyword,  LocationPOICB callback, gpointer userdata);  ///< This is used for getting a POI by using a position from a plug-in asynchronously.
} LocModPoiOps;

/**
* @brief This represents a enabled/disabled callback function for a plug-in.
*/
typedef void (*LocModStatusCB)(gboolean enabled, LocationStatus status, gpointer userdata);

/**
 * @brief This represents a position callback function for a plug-in.
 */
typedef void (*LocModPositionCB) (gboolean enabled, LocationPosition *position, LocationAccuracy *accuracy, gpointer userdata);

/**
 * @brief This represents a velocity callback function for a plug-in.
 */
typedef void (*LocModVelocityCB) (gboolean enabled, LocationVelocity *velocity, LocationAccuracy *accuracy, gpointer userdata);

/**
 * @brief This represents APIs declared in a GPS plug-in for location GPS modules.
 */
typedef struct{
	int (*start)(gpointer handle, LocModStatusCB status_cb, LocModPositionCB pos_cb, LocModVelocityCB vel_cb, gpointer userdata);  ///< This is used for starting a GPS device from a plug-in. #LocModStatusCB, #LocModPositionCB, and #LocModVelocityCB are given from a location framework to a plug-in for asynchronous signaling.
	int (*stop)(gpointer handle);                                                                                                  ///< This is used for stopping a GPS device name from a plug-in.
	int (*get_position)(gpointer handle, LocationPosition **position, LocationAccuracy **accuracy);                                ///< This is used for getting a position from a plug-in.
	int (*get_velocity)(gpointer handle, LocationVelocity **velocity, LocationAccuracy **accuracy);                                ///< This is used for getting a velocity from a plug-in.
	int (*get_nmea)(gpointer handle, gchar** nmea_data);                                                                           ///< This is used for getting a nmea string from a plug-in.
	int (*get_satellite)(gpointer handle, LocationSatellite **satellite);                                                          ///< This is used for getting a satellite information from a plug-in.
	int (*set_devname)(gpointer handle, const gchar *devname);                                                                     ///< This is used for setting a device name from a plug-in.
	int (*get_devname)(gpointer handle, gchar **devname);                                                                          ///< This is used for getting a device name from a plug-in.
} LocModGpsOps;

/**
 * @brief This represents APIs declared in a WPS plug-in for location WPS modules.
 */
typedef struct{
	int (*start)(gpointer handle, LocModStatusCB status_cb, LocModPositionCB pos_cb, LocModVelocityCB vel_cb, gpointer userdata);   ///< This is used for starting a WPS service from a plug-in. #LocModStatusCB, #LocModPositionCB, and #LocModVelocityCB are given from a location framework to a plug-in for asynchronous signaling.
	int (*stop)(gpointer handle);                                                                                                   ///< This is used for stopping a WPS service from a plug-in.
	int (*get_position)(gpointer handle, LocationPosition **position, LocationAccuracy **accuracy);                                 ///< This is used for getting a position from a plug-in.
	int (*get_velocity)(gpointer handle, LocationVelocity **velocity, LocationAccuracy **accuracy);                                 ///< This is used for getting a velocity from a plug-in.
} LocModWpsOps;

/**
 * @brief This represents APIs declared in a SPS plug-in for location SPS modules.
 */
typedef struct{
	int (*start)(gpointer handle, LocModStatusCB status_cb, LocModPositionCB pos_cb, LocModVelocityCB vel_cb, gpointer userdata);                                                   ///< This is used for starting a SPS service from a plug-in. #LocModStatusCB, #LocModPositionCB and #LocModVelocityCB are given from a location framework to a plug-in for asynchronous signaling.
	int (*stop)(gpointer handle);                                                                                                                                                   ///< This is used for stopping a SPS service from a plug-in.
	int (*get_position)(gpointer handle, LocationPosition **position, LocationAccuracy **accuracy);                                                                                 ///< This is used for getting a position from a plug-in.
	int (*get_velocity)(gpointer handle, LocationVelocity **velocity, LocationAccuracy **accuracy);                                                                                 ///< This is used for getting a velocity from a plug-in.
	int (*update_data)(gpointer handle, const LocationPosition *position, const LocationVelocity *velocity, const LocationAccuracy *accuracy, const LocationSatellite *satellite);  ///< This is used for updating compensated position/velocity data for a plug-in.
} LocModSpsOps;

/**
 * @brief This represents APIs declared in a IPS plug-in for location IPS modules.
 */
typedef struct{
	int (*get_position)(gpointer handle, LocationPosition **position, LocationAccuracy **accuracy);          ///< This is used for getting a position from a plug-in.
} LocModIpsOps;

/**
 * @brief This represents APIs declared in a CPS plug-in for location CPS modules.
 */
typedef struct{
	int (*get_position)(gpointer handle, LocationPosition **position, LocationAccuracy **accuracy);          ///< This is used for getting a position from a plug-in.
} LocModCpsOps;

/**
 * @brief This is used for exported APIs in a plug-in for a location framework.
 */
#define LOCATION_MODULE_API __attribute__((visibility("default"))) G_MODULE_EXPORT 

/**
 * @} @}
 */
G_END_DECLS

#endif
