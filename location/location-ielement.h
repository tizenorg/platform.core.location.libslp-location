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

#ifndef __LOCATION_IELEMENT_H__
#define __LOCATION_IELEMENT_H__

#include <glib-object.h>
#include <location/location-types.h>
#include <location/location-position.h>
#include <location/location-velocity.h>
#include <location/location-accuracy.h>
#include <location/location-address.h>
#include <location/location-boundary.h>
#include <location/location-satellite.h>
#include <location/location-poi-info.h>

/**
 * @file location-ielement.h
 * @brief This file contains the internal definitions and structures related to location interface.
 * @addtogroup LocationInternal
 * @{
 *@}
 */

G_BEGIN_DECLS

enum {
	SERVICE_ENABLED,
	SERVICE_DISABLED,
	SERVICE_UPDATED,
	ZONE_IN,
	ZONE_OUT,
	LAST_SIGNAL
};

#define LOCATION_TYPE_IELEMENT                  (location_ielement_get_type ())
#define LOCATION_IELEMENT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), LOCATION_TYPE_IELEMENT, LocationIElement))
#define LOCATION_IS_IELEMENT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LOCATION_TYPE_IELEMENT))
#define LOCATION_IELEMENT_GET_INTERFACE(obj)    (G_TYPE_INSTANCE_GET_INTERFACE ((obj), LOCATION_TYPE_IELEMENT, LocationIElementInterface))

typedef struct _LocationIElement          LocationIElement;
typedef struct _LocationIElementInterface LocationIElementInterface;

typedef int (*TYPE_START_FUNC)(LocationIElement *self);
typedef int (*TYPE_STOP_FUNC) (LocationIElement *self);
typedef int (*TYPE_GET_POSITION)(LocationIElement *self, LocationPosition **position, LocationAccuracy **accuracy);
typedef int (*TYPE_GET_VELOCITY)(LocationIElement *self, LocationVelocity **velocity, LocationAccuracy **accuracy);
typedef int (*TYPE_GET_GEOCODE)(LocationIElement *self, const LocationAddress *address, LocationPosition **position, LocationAccuracy **accuracy);
typedef int (*TYPE_GET_GEOCODE_FREEFORM)(LocationIElement *self, const gchar *address, LocationPosition **position, LocationAccuracy **accuracy);
typedef int (*TYPE_GET_REVERSEGEOCODE)(LocationIElement *self, const LocationPosition *position, LocationAddress **address, LocationAccuracy **accuracy);
typedef int (*TYPE_GET_GEOCODE_ASYNC)(LocationIElement *self, const LocationAddress *address, LocationPositionCB callback, gpointer userdata);
typedef int (*TYPE_GET_GEOCODE_FREEFORM_ASYNC)(LocationIElement *self, const gchar *address,  LocationPositionCB callback, gpointer userdata);
typedef int (*TYPE_GET_REVERSEGEOCODE_ASYNC)(LocationIElement *self, const LocationPosition *position, LocationAddressCB callback, gpointer userdata);
typedef int (*TYPE_GET_POI)(LocationIElement *self, gdouble radius, const gchar* keyword, LocationPOIInfo** poi);
typedef int (*TYPE_GET_POI_ASYNC)(LocationIElement *self, gdouble radius, const gchar* keyword, LocationPOICB callback, gpointer userdata);
typedef int (*TYPE_GET_POI_FROM_ADDRESS)(LocationIElement *self, const LocationAddress* address, gdouble radius, const gchar* keyword, LocationPOIInfo** poi);
typedef int (*TYPE_GET_POI_FROM_POSITION)(LocationIElement *self, const LocationPosition* position, gdouble radius, const gchar* keyword, LocationPOIInfo** poi);
typedef int (*TYPE_GET_POI_FROM_ADDRESS_ASYNC)(LocationIElement *self, const LocationAddress* address, gdouble radius, const gchar* keyword, LocationPOICB callback, gpointer userdata);
typedef int (*TYPE_GET_POI_FROM_POSITION_ASYNC)(LocationIElement *self, const LocationPosition* position, gdouble radius, const gchar* keyword, LocationPOICB callback, gpointer userdata);

struct _LocationIElementInterface
{
	GTypeInterface parent_iface;

	TYPE_START_FUNC start;
	TYPE_STOP_FUNC stop;
	TYPE_GET_POSITION get_position;
	TYPE_GET_VELOCITY get_velocity;
	TYPE_GET_GEOCODE get_geocode;
	TYPE_GET_GEOCODE_FREEFORM get_geocode_freeform;
	TYPE_GET_REVERSEGEOCODE get_reversegeocode;
	TYPE_GET_GEOCODE_ASYNC get_geocode_async;
	TYPE_GET_GEOCODE_FREEFORM_ASYNC get_geocode_freeform_async;
	TYPE_GET_REVERSEGEOCODE_ASYNC get_reversegeocode_async;
	TYPE_GET_POI get_poi;
	TYPE_GET_POI_FROM_ADDRESS get_poi_from_address;
	TYPE_GET_POI_FROM_POSITION get_poi_from_position;
	TYPE_GET_POI_ASYNC get_poi_async;
	TYPE_GET_POI_FROM_ADDRESS_ASYNC get_poi_from_address_async;
	TYPE_GET_POI_FROM_POSITION_ASYNC get_poi_from_position_async;
};

GType location_ielement_get_type (void);

int location_ielement_start (LocationIElement *self);
int location_ielement_stop(LocationIElement *self);
int location_ielement_get_position (LocationIElement *self, LocationPosition **position, LocationAccuracy **accuracy);
int location_ielement_get_velocity (LocationIElement *self, LocationVelocity **velocity, LocationAccuracy **accuracy);
int location_ielement_get_geocode (LocationIElement *self, const LocationAddress *address, LocationPosition **position, LocationAccuracy **accuracy);
int location_ielement_get_geocode_freeform (LocationIElement *self, const gchar *address, LocationPosition **position, LocationAccuracy **accuracy);
int location_ielement_get_reversegeocode (LocationIElement *self, const LocationPosition *position, LocationAddress **address, LocationAccuracy **accuracy);
int location_ielement_get_geocode_async (LocationIElement *self, const LocationAddress *address, LocationPositionCB callback, gpointer userdata);
int location_ielement_get_geocode_freeform_async (LocationIElement *self, const gchar *address, LocationPositionCB callback, gpointer userdata);
int location_ielement_get_reversegeocode_async (LocationIElement *self, const LocationPosition *position, LocationAddressCB callback, gpointer userdata);
int location_ielement_get_poi(LocationIElement *self, gdouble radius, const gchar *keyword, LocationPOIInfo **poi);
int location_ielement_get_poi_from_address(LocationIElement *self, const LocationAddress *address, gdouble radius, const gchar *keyword, LocationPOIInfo **poi);
int location_ielement_get_poi_from_position (LocationIElement *self, const LocationPosition *position, gdouble radius, const gchar *keyword, LocationPOIInfo **poi);
int location_ielement_get_poi_async(LocationIElement *self, gdouble radius, const gchar *keyword, LocationPOICB callback, gpointer userdata);
int location_ielement_get_poi_from_address_async(LocationIElement *self, const LocationAddress *address, gdouble radius, const gchar *keyword, LocationPOICB callback, gpointer userdata);
int location_ielement_get_poi_from_position_async (LocationIElement *self, const LocationPosition *position, gdouble radius, const gchar *keyword, LocationPOICB callback, gpointer userdata);

G_END_DECLS

#endif
