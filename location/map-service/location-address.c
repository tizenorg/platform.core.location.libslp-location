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

#include "location-address.h"
#include "location-log.h"

GType
location_address_get_type (void)
{
	static volatile gsize type_volatile = 0;
	if(g_once_init_enter(&type_volatile)) {
		GType type = g_boxed_type_register_static (
			g_intern_static_string ("LocationAddress"),
			(GBoxedCopyFunc) location_address_copy,
			(GBoxedFreeFunc) location_address_free);
		g_once_init_leave(&type_volatile, type);
	}
	return type_volatile;
}

EXPORT_API LocationAddress*
location_address_new (const gchar *building_number,
	const gchar *street,
	const gchar *district,
	const gchar *city,
	const gchar *state,
	const gchar *country_code,
	const gchar *postal_code)
{
	LocationAddress* address = g_slice_new0(LocationAddress);

	address->building_number = g_strdup(building_number);
	address->street = g_strdup(street);
	address->district = g_strdup(district);
	address->city = g_strdup(city);
	address->state = g_strdup(state);
	address->country_code = g_strdup(country_code);
	address->postal_code = g_strdup(postal_code);

	return address;
}

EXPORT_API void
location_address_free (LocationAddress* address)
{
	g_return_if_fail(address);
	g_free(address->building_number);
	g_free(address->street);
	g_free(address->district);
	g_free(address->city);
	g_free(address->state);
	g_free(address->country_code);
	g_free(address->postal_code);
	g_slice_free(LocationAddress, address);
}

EXPORT_API LocationAddress*
location_address_copy (const LocationAddress *address)
{
	g_return_val_if_fail(address, NULL);
	return location_address_new(address->building_number,
								address->street,
								address->district,
								address->city,
								address->state,
								address->country_code,
								address->postal_code);
}
