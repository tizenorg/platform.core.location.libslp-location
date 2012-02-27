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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "location/location-log.h"
#include "location/location-ielement.h"

static void
location_ielement_base_init (gpointer g_class)
{
	static gboolean is_initialized = FALSE;

	if (is_initialized){
		/* add properties and signals to the interface here */

		is_initialized = TRUE;
	}
}

GType
location_ielement_get_type (void)
{
	static GType iface_type = 0;

	if (iface_type == 0) {
		static const GTypeInfo info = {
			sizeof (LocationIElementInterface),
			location_ielement_base_init, /* base_init */
			NULL /* base_finalize */
		};

		iface_type = g_type_register_static (G_TYPE_INTERFACE, "LocationIElement",
	                                     &info, 0);
	}

	return iface_type;
}

int
location_ielement_start (LocationIElement *self)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->start, LOCATION_ERROR_NOT_AVAILABLE);
	return LOCATION_IELEMENT_GET_INTERFACE (self)->start (self);
}

int
location_ielement_stop (LocationIElement *self)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->stop, LOCATION_ERROR_NOT_AVAILABLE);
	return LOCATION_IELEMENT_GET_INTERFACE (self)->stop (self);
}

int
location_ielement_get_position (LocationIElement *self,
	LocationPosition **position,
	LocationAccuracy **accuracy)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (position, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (accuracy, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_position, LOCATION_ERROR_NOT_AVAILABLE);

	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_position (self, position, accuracy);
}

int
location_ielement_get_velocity (LocationIElement *self,
	LocationVelocity **velocity,
	LocationAccuracy **accuracy)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (velocity, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_velocity, LOCATION_ERROR_NOT_AVAILABLE);
	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_velocity (self, velocity, accuracy);
}

int
location_ielement_get_geocode (LocationIElement *self,
	const LocationAddress *address,
	GList **position_list,
	GList **accuracy_list)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (address, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_geocode, LOCATION_ERROR_NOT_AVAILABLE);

	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_geocode (self, address, position_list, accuracy_list);
}

int
location_ielement_get_geocode_freeform (LocationIElement *self,
	const gchar *address,
	GList **position_list,
	GList **accuracy_list)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (address, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_geocode_freeform, LOCATION_ERROR_NOT_AVAILABLE);

	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_geocode_freeform (self, address, position_list, accuracy_list);
}

int
location_ielement_get_reversegeocode (LocationIElement *self,
	const LocationPosition *position,
	LocationAddress **address,
	LocationAccuracy **accuracy)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (position, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (address, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (accuracy, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_reversegeocode, LOCATION_ERROR_NOT_AVAILABLE);
	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_reversegeocode (self, position, address, accuracy);
}

int
location_ielement_get_geocode_async (LocationIElement *self,
	const LocationAddress *address,
	LocationPositionCB callback,
	gpointer userdata)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (address, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (callback, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_geocode_async, LOCATION_ERROR_NOT_AVAILABLE);
	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_geocode_async (self, address, callback, userdata);
}

int
location_ielement_get_geocode_freeform_async (LocationIElement *self,
	const gchar *address,
	LocationPositionCB callback,
	gpointer userdata)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (address, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (callback, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_geocode_freeform_async, LOCATION_ERROR_NOT_AVAILABLE);
	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_geocode_freeform_async (self, address, callback, userdata);
}

int
location_ielement_get_reversegeocode_async (LocationIElement *self,
	const LocationPosition *position,
	LocationAddressCB callback,
	gpointer userdata)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (position, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (callback, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_reversegeocode_async, LOCATION_ERROR_NOT_AVAILABLE);
	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_reversegeocode_async (self, position, callback, userdata);
}

int
location_ielement_get_poi (LocationIElement *self,
	gdouble radius,
	const gchar* keyword,
	LocationPOIInfo **poi_info)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (poi_info, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_poi, LOCATION_ERROR_NOT_AVAILABLE);
	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_poi(self, radius, keyword, poi_info);
}

int
location_ielement_get_poi_from_address (LocationIElement *self,
	const LocationAddress* address,
	gdouble radius,
	const gchar *keyword,
	LocationPOIInfo **poi_info)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (address, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (poi_info, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_poi_from_address, LOCATION_ERROR_NOT_AVAILABLE);
	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_poi_from_address(self, address, radius, keyword, poi_info);
}

int
location_ielement_get_poi_from_position (LocationIElement *self,
	const LocationPosition *position,
	gdouble radius,
	const gchar *keyword,
	LocationPOIInfo **poi_info)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (position, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (poi_info, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_poi_from_position, LOCATION_ERROR_NOT_AVAILABLE);
	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_poi_from_position(self, position, radius, keyword, poi_info);
}

int
location_ielement_get_poi_async (LocationIElement *self,
	gdouble radius,
	const gchar *keyword,
	LocationPOICB callback,
	gpointer userdata)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (callback, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_poi_async, LOCATION_ERROR_NOT_AVAILABLE);
	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_poi_async (self, radius, keyword, callback, userdata);
}

int
location_ielement_get_poi_from_address_async (LocationIElement *self,
	const LocationAddress *address,
	gdouble radius,
	const gchar* keyword,
	LocationPOICB callback,
	gpointer userdata)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (address, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (callback, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_poi_from_address_async, LOCATION_ERROR_NOT_AVAILABLE);
	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_poi_from_address_async (self, address, radius, keyword, callback, userdata);
}



int location_ielement_get_poi_from_position_async (LocationIElement *self,
	const LocationPosition* position,
	gdouble radius,
	const gchar* keyword,
	LocationPOICB callback,
	gpointer userdata)
{
	g_return_val_if_fail (LOCATION_IS_IELEMENT (self), LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (position, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (callback, LOCATION_ERROR_PARAMETER);
	g_return_val_if_fail (LOCATION_IELEMENT_GET_INTERFACE (self)->get_poi_from_position_async, LOCATION_ERROR_NOT_AVAILABLE);
	return LOCATION_IELEMENT_GET_INTERFACE (self)->get_poi_from_position_async (self, position, radius, keyword, callback, userdata);
}
