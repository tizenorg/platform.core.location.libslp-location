/*
 * libslp-location
 *
 * Copyright (c) 2010-2013 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Youngae Kang <youngae.kang@samsung.com>, Minjune Kim <sena06.kim@samsung.com>
 *          Genie Kim <daejins.kim@samsung.com>
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

#include "location-log.h"
#include "location.h"
#include "location-types.h"
#include "map-service.h"

struct _LocationLandmark {

	guint id;			///< Unique identifier of the landmark
	guint priority;			///< Degree of importance of the landmark (Between HIGHEST_LANDMARK_PRIORITY and LOWEST_LANDMARK_PRIORITY)
	gchar *name;			///< Name of landmark
	LocationPosition *position;		///< Positon of the landmark, may be null if not known
	LocationAddress *addr;		///< Textual address information of the landmark, may be null if not
	LocationBoundary *bbox;	///< Coverage area of the landmark, may be null if not available
	guint timestamp;		///< Time when the landmark data was last updated (updated by system and used to facilitate tracking)

	gchar *author;			///< Provider of the landmark data, may be null
	gchar *store;			///< Name of landmark store that the landmark belongs to
	gchar *phone_number;		///< Nhone number of the landmark, may be null
	GList *category;		///< Categories to which the landmark belongs to
	GList  *url;			///< Url to additional content (Optional)
	gchar *desc;			///< Description of the landmark, may be null if not available

	GHashTable *properties;		///< Extra info of the landmark

};

struct _LocationLandmarkUrl {
	gchar *path;			///< Url of landmark url info
	gchar *desc;			///< Description of landmark url info
};

EXPORT_API guint
location_landmark_get_id (const LocationLandmark *landmark)
{
	g_return_val_if_fail(landmark, 0);

	return landmark->id;
}


EXPORT_API gchar *
location_landmark_get_name (const LocationLandmark *landmark)
{
	g_return_val_if_fail(landmark, NULL);

	return landmark->name;
}

EXPORT_API LocationPosition *
location_landmark_get_position (const LocationLandmark *landmark)
{
	g_return_val_if_fail(landmark, NULL);

	return landmark->position;
}

EXPORT_API LocationAddress *
location_landmark_get_address (const LocationLandmark *landmark)
{
	g_return_val_if_fail(landmark, NULL);

	return landmark->addr;
}

EXPORT_API gchar *
location_landmark_get_description (const LocationLandmark *landmark)
{
	g_return_val_if_fail(landmark, NULL);

	return landmark->desc;
}

EXPORT_API guint
location_landmark_get_timestamp (const LocationLandmark *landmark)
{
	g_return_val_if_fail(landmark, 0);

	return landmark->timestamp;
}

EXPORT_API guint
location_landmark_get_priority (const LocationLandmark *landmark)
{
	g_return_val_if_fail(landmark, 0);

	return landmark->priority;
}

EXPORT_API LocationBoundary *
location_landmark_get_bounding_box (const LocationLandmark *landmark)
{
	g_return_val_if_fail(landmark, NULL);

	return landmark->bbox;
}

EXPORT_API gchar *
location_landmark_get_author (const LocationLandmark *landmark)
{
	g_return_val_if_fail(landmark, NULL);

	return landmark->author;
}

EXPORT_API GList *
location_landmark_get_url (const LocationLandmark *landmark)
{
	g_return_val_if_fail(landmark, NULL);

	return landmark->url;
}

EXPORT_API GList *
location_landmark_get_category (const LocationLandmark *landmark)
{
	g_return_val_if_fail(landmark, NULL);

	return landmark->category;
}

EXPORT_API gchar *
location_landmark_get_phone_number (const LocationLandmark *landmark)
{
	g_return_val_if_fail(landmark, NULL);

	return landmark->phone_number;
}

EXPORT_API GList *
location_landmark_get_property_key (const LocationLandmark *landmark)
{
	g_return_val_if_fail(landmark, NULL);

	return g_hash_table_get_keys (landmark->properties);
}

EXPORT_API gconstpointer
location_landmark_get_property (const LocationLandmark *landmark, gconstpointer key)
{
	g_return_val_if_fail(landmark, NULL);
	g_return_val_if_fail(key, NULL);
	if (!landmark->properties) return NULL;

	return g_hash_table_lookup (landmark->properties, key);
}

EXPORT_API gboolean
location_landmark_set_id (LocationLandmark *landmark, guint id)
{
	g_return_val_if_fail(landmark, FALSE);

	landmark->id = id;

	return TRUE;
}

EXPORT_API gboolean
location_landmark_set_name (LocationLandmark *landmark, const gchar *name)
{
	g_return_val_if_fail(landmark, FALSE);

	if (landmark->name) {
		g_free(landmark->name);
		landmark->name = NULL;
	}
	if (name) landmark->name = g_strdup (name);

	return TRUE;
}

EXPORT_API gboolean
location_landmark_set_position (LocationLandmark *landmark, const LocationPosition *position)
{
	g_return_val_if_fail(landmark, FALSE);

	if (landmark->position) {
		location_position_free(landmark->position);
		landmark->position = NULL;
	}

	if (position) landmark->position = location_position_copy (position);

	return TRUE;

}

EXPORT_API gboolean
location_landmark_set_address (LocationLandmark *landmark, const LocationAddress *addr)
{
	g_return_val_if_fail(landmark, FALSE);

	if (landmark->addr) {
		location_address_free (landmark->addr);
		landmark->addr = NULL;
	}

	if (addr) landmark->addr = location_address_copy (addr);

	return TRUE;

}

EXPORT_API gboolean
location_landmark_set_description (LocationLandmark *landmark, const gchar *desc)
{
	g_return_val_if_fail(landmark, FALSE);

	if (landmark->desc) {
		g_free(landmark->desc);
		landmark->desc = NULL;
	}

	if (desc) landmark->desc = g_strdup (desc);

	return TRUE;
}

EXPORT_API gboolean
location_landmark_set_timestamp (LocationLandmark *landmark, guint timestamp)
{
	g_return_val_if_fail(landmark, FALSE);

	landmark->timestamp = timestamp;

	return TRUE;

}

EXPORT_API gboolean
location_landmark_set_priority (LocationLandmark *landmark, guint priority)
{
	g_return_val_if_fail(landmark, FALSE);

	landmark->priority = priority;

	return TRUE;
}

EXPORT_API gboolean
location_landmark_set_bounding_box (LocationLandmark *landmark, const LocationBoundary *bbox)
{
	g_return_val_if_fail(landmark, FALSE);

	if (landmark->bbox) {
		location_boundary_free(landmark->bbox);
		landmark->bbox = NULL;
	}

	if (bbox) landmark->bbox = location_boundary_copy (bbox);

	return TRUE;
}

EXPORT_API gboolean
location_landmark_set_author (LocationLandmark *landmark, const gchar *author)
{
	g_return_val_if_fail(landmark, FALSE);

	if (landmark->author) {
		g_free(landmark->author);
		landmark->author = NULL;
	}
	if (author) landmark->author = g_strdup (author);

	return TRUE;
}

static void landmark_url_foreach_free (gpointer data)
{
	g_return_if_fail(data);

	LocationLandmarkUrl *url = (LocationLandmarkUrl *)data;

	location_landmark_url_free (url);
}

static void landmark_foreach_url_copy (gpointer data, gpointer user_data)
{
	g_return_if_fail(data);
	g_return_if_fail(user_data);

	LocationLandmarkUrl *url = (LocationLandmarkUrl *)data;
	LocationLandmark *landmark = (LocationLandmark *) user_data;

	if(!url || !landmark) return;

	landmark->url = g_list_append(landmark->url, url);
}

EXPORT_API gboolean
location_landmark_set_url (LocationLandmark *landmark, GList *url)
{
	g_return_val_if_fail(landmark, FALSE);

	if (landmark->url) {
		g_list_free_full(landmark->url, landmark_url_foreach_free);
		landmark->url = NULL;
	}

	if (url) g_list_foreach (url, landmark_foreach_url_copy, landmark);

	return TRUE;
}

static void landmark_foreach_copy_category (gpointer data, gpointer user_data)
{
	g_return_if_fail (data);
	g_return_if_fail (user_data);

	gchar *category_name = (gchar *)data;
	LocationLandmark *landmark = (LocationLandmark *) user_data;

	landmark->category = g_list_append (landmark->category, g_strdup(category_name));
}

static void landmark_free_category (gpointer data)
{
	g_return_if_fail (data);

	gchar * category_name = (gchar *)data;

	g_free (category_name);
}

EXPORT_API gboolean
location_landmark_set_category (LocationLandmark *landmark, GList *category)
{
	g_return_val_if_fail(landmark, FALSE);

	if (landmark->category) {
		g_list_free_full(landmark->category, landmark_free_category);
		landmark->category = NULL;
	}

	if (category) g_list_foreach ((GList*) category, landmark_foreach_copy_category, landmark);

	return TRUE;
}

EXPORT_API gboolean
location_landmark_set_phone_number (LocationLandmark *landmark, const gchar *number)
{
	g_return_val_if_fail(landmark, FALSE);
	g_return_val_if_fail(number, FALSE);

	if (landmark->phone_number) {
		g_free(landmark->phone_number);
		landmark->phone_number = NULL;
	}
	if (number) landmark->phone_number = g_strdup (number);

	return TRUE;
}

EXPORT_API gboolean
location_landmark_set_property(LocationLandmark *landmark, gconstpointer key, gconstpointer value)
{
	g_return_val_if_fail(landmark, FALSE);
	g_return_val_if_fail(key, FALSE);
	if (!landmark->properties) return FALSE;

	if (value) {
		gchar *re_key = g_strdup (key);
		gchar *re_val = g_strdup (value);
		g_hash_table_insert(landmark->properties, re_key, re_val);
	}
	else g_hash_table_remove (landmark->properties, key);

	return TRUE;
}

EXPORT_API gchar *
location_landmark_url_get_url_path (const LocationLandmarkUrl *url)
{
	g_return_val_if_fail (url, NULL);

	return url->path;
}

EXPORT_API gchar *
location_landmark_url_get_description (const LocationLandmarkUrl *url)
{
	g_return_val_if_fail (url, NULL);

	return url->desc;
}

EXPORT_API gboolean
location_landmark_url_set_url_path (LocationLandmarkUrl *url, const gchar *path)
{
	g_return_val_if_fail (url, FALSE);

	if (url->path) {
		g_free(url->path);
		url->path = NULL;
	}

	if (path) url->path = g_strdup (path);

	return TRUE;
}

EXPORT_API gboolean
location_landmark_url_set_description (LocationLandmarkUrl *url, const gchar *desc)
{
	g_return_val_if_fail (url, FALSE);

	if (url->desc) {
		g_free(url->desc);
		url->desc = NULL;
	}

	if (desc) url->desc = g_strdup (desc);

	return TRUE;
}

EXPORT_API LocationLandmarkUrl *
location_landmark_url_new (void)
{
	LocationLandmarkUrl *url = g_slice_new0 (LocationLandmarkUrl);
	if(!url) return NULL;


	return url;
}

EXPORT_API void
location_landmark_url_free (LocationLandmarkUrl * url)
{
	g_return_if_fail (url);


	location_landmark_url_set_url_path(url, NULL);
	location_landmark_url_set_description(url, NULL);

	g_slice_free(LocationLandmarkUrl, url);
}

EXPORT_API LocationLandmarkUrl *
location_landmark_url_copy (const LocationLandmarkUrl *url)
{
	g_return_val_if_fail(url, NULL);

	LocationLandmarkUrl *new_url = location_landmark_url_new();

	location_landmark_url_set_url_path(new_url, location_landmark_url_get_url_path(url));
	location_landmark_url_set_description(new_url, location_landmark_url_get_description(url));

	return new_url;
}

EXPORT_API LocationLandmark *
location_landmark_new (void)
{
	LocationLandmark *landmark = g_slice_new0 (LocationLandmark);
	if (!landmark) return NULL;

	landmark->properties = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	return landmark;
}

EXPORT_API LocationLandmark *
location_landmark_copy (const LocationLandmark *landmark)
{
	g_return_val_if_fail (landmark, NULL);

	LocationLandmark *new_landmark = location_landmark_new();
	if (!new_landmark) return NULL;

	location_landmark_set_id(new_landmark, location_landmark_get_id(landmark));
	location_landmark_set_name (new_landmark, location_landmark_get_name (landmark));
	location_landmark_set_position (new_landmark, location_landmark_get_position(landmark));
	location_landmark_set_address (new_landmark, location_landmark_get_address (landmark));
	location_landmark_set_description (new_landmark, location_landmark_get_description(landmark));
	location_landmark_set_timestamp (new_landmark, location_landmark_get_timestamp(landmark));
	location_landmark_set_priority (new_landmark, location_landmark_get_priority(landmark));
	location_landmark_set_bounding_box (new_landmark, location_landmark_get_bounding_box(landmark));
	location_landmark_set_author (new_landmark, location_landmark_get_author(landmark));
	location_landmark_set_url(new_landmark, location_landmark_get_url(landmark));

	return new_landmark;
}

EXPORT_API void
location_landmark_free (LocationLandmark *landmark)
{
	g_return_if_fail (landmark);

	location_landmark_set_id(landmark, 0);
	location_landmark_set_name (landmark, NULL);
	location_landmark_set_position (landmark, NULL);
	location_landmark_set_address (landmark, NULL);
	location_landmark_set_description (landmark, NULL);
	location_landmark_set_timestamp (landmark, 0);
	location_landmark_set_priority (landmark, 0);
	location_landmark_set_bounding_box (landmark, NULL);
	location_landmark_set_author (landmark, NULL);
	location_landmark_set_url(landmark, NULL);

	g_slice_free (LocationLandmark, landmark);
}
