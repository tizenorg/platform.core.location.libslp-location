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

#include <glib-object.h>
#include <vconf.h>
#include "location-log.h"
#include "location-pref.h"

#include "module-internal.h"

#include "map-internal.h"
#include "map-service.h"
#include "location-ielement.h"

enum {
	PROP_0,
	PROP_SERVICE_NAME
};

static void map_service_ielement_interface_init (LocationIElementInterface *iface);

G_DEFINE_TYPE_WITH_CODE (MapService, map_service, G_TYPE_OBJECT,
	G_IMPLEMENT_INTERFACE (LOCATION_TYPE_IELEMENT, map_service_ielement_interface_init));

static void
_get_lang (gchar country_code[3], gchar lang_code[3])
{
       if (!country_code || !lang_code) return;
       gchar* langset = vconf_get_str(VCONFKEY_LANGSET);
       LOCATION_LOGD("getenv: %s", langset);

       if(langset == NULL){
              lang_code[0] = 'E';
              lang_code[1] = 'N';
              lang_code[2] = '\0';
              country_code[0] = 'U';
              country_code[1] = 'S';
              country_code[2] = '\0';
       }else{
              gchar* langset_upper = g_ascii_strup(langset, -1);
              lang_code[0] = langset_upper[0];
              lang_code[1] = langset_upper[1];
              lang_code[2] = '\0';
              country_code[0] = langset_upper[3];
              country_code[1] = langset_upper[4];
              country_code[2] = '\0';
              g_free(langset_upper);
       }
       LOCATION_LOGD("Language: %s, Country: %s", lang_code, country_code);
}


static void
map_service_dispose (GObject *gobject)
{
	LOCATION_LOGD("map_service_dispose");

	G_OBJECT_CLASS (map_service_parent_class)->dispose (gobject);
}

static void
map_service_finalize (GObject *gobject)
{
	LOCATION_LOGD("map_service_finalize");
	MapServicePrivate* priv = GET_PRIVATE(gobject);
	module_free(priv->mod, "map-service");
	priv->mod = NULL;
	G_OBJECT_CLASS (map_service_parent_class)->finalize (gobject);
}

static void
map_service_get_property (GObject *object,
	guint property_id,
	GValue *value,
	GParamSpec *pspec)
{
	MapServicePrivate *priv = GET_PRIVATE (object);

	g_return_if_fail(priv->mod);
	g_return_if_fail(priv->mod->handler);
 	switch (property_id){
	case PROP_SERVICE_NAME:{
		char* service_name = NULL;
		if(priv->mod->ops.get_service_name){
			if( LOCATION_ERROR_NONE != priv->mod->ops.get_service_name(priv->mod->handler, &service_name) ){
				service_name = NULL;
			}
		}
		LOCATION_LOGD("Get prop>> Service name: %s", service_name);
		g_value_set_string(value, service_name);
		g_free(service_name);
		break;
	}
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

static void
map_service_ielement_interface_init (LocationIElementInterface *iface)
{
	iface->get_geocode = (TYPE_GET_GEOCODE)map_service_get_geocode;
	iface->get_geocode_freeform = (TYPE_GET_GEOCODE_FREEFORM)map_service_get_geocode_freeform;
	iface->get_reversegeocode = (TYPE_GET_REVERSEGEOCODE)map_service_get_reversegeocode;
	iface->get_geocode_async = (TYPE_GET_GEOCODE_ASYNC)map_service_get_geocode_async;
	iface->get_geocode_freeform_async = (TYPE_GET_GEOCODE_FREEFORM_ASYNC)map_service_get_geocode_freeform_async;
	iface->get_reversegeocode_async = (TYPE_GET_REVERSEGEOCODE_ASYNC)map_service_get_reversegeocode_async;
	iface->search_poi = (TYPE_SEARCH_POI) map_service_search_poi;
	iface->search_poi_by_area = (TYPE_SEARCH_POI_BY_AREA) map_service_search_poi_by_area;
	iface->search_poi_by_address = (TYPE_SEARCH_POI_BY_ADDR) map_service_search_poi_by_address;
	iface->search_poi_by_freeform = (TYPE_SEARCH_POI_BY_FREEFORM) map_service_search_poi_by_freeform;
	iface->cancel_poi_request = (TYPE_CANCEL_POI_REQUEST) map_service_cancel_poi_request;
	iface->request_route = (TYPE_REQUEST_ROUTE) map_service_request_route;
	iface->cancel_route_request = (TYPE_CANCEL_ROUTE_REQUEST) map_service_cancel_route_request;
	iface->is_supported_map_provider_capability = (TYPE_IS_SUPPORTED_MAP_PROVIDER_CAPABILITY) map_service_is_supported_map_provider_capability;
	iface->get_map_provider_capability_key = (TYPE_GET_MAP_PROVIDER_CAPABILITY_KEY) map_service_get_map_provider_capability_key;

}

static void
map_service_init (MapService *self)
{
	LOCATION_LOGD("map_service_init");
	MapServicePrivate* priv = GET_PRIVATE(self);

	priv->mod = (LocationServiceMod*)module_new("map-service");
	if(!priv->mod) LOCATION_LOGW("module loading failed");
	priv->pref = location_pref_new ();
	if(!priv->pref) LOCATION_LOGW("Service preference failed");

	gchar country[3], lang[3];
	_get_lang (country, lang);
	location_pref_set_language (priv->pref, lang);
	location_pref_set_distance_unit (priv->pref, "MI");
	location_pref_set_provider_name (priv->pref, "decarta");
}

static void
map_service_class_init (MapServiceClass *klass)
{
	LOCATION_LOGD("map_service_class_init");
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GParamSpec *pspec;

	gobject_class->get_property = map_service_get_property;

	gobject_class->dispose = map_service_dispose;
	gobject_class->finalize = map_service_finalize;

	g_type_class_add_private (klass, sizeof (MapServicePrivate));

	pspec = g_param_spec_string ("service provider",
                                "map service provider name prop",
                                "map service provider name",
                                DEFAULT_SERVICE,
                                G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
                                   PROP_SERVICE_NAME,
                                   pspec);
}

LocationPreference *
map_service_get_pref(GObject *obj)
{
	LOCATION_LOGD("map_service_get_pref");
	MapServicePrivate* priv = GET_PRIVATE(obj);
	if (!priv) return NULL;
	if (!priv->pref) return NULL;

	LocationPreference *pref = location_pref_copy(priv->pref);

	return pref;
}

gboolean
map_service_set_pref (GObject *obj, LocationPreference *pref)
{
	LOCATION_LOGD("map_service_set_pref");
	MapServicePrivate* priv = GET_PRIVATE(obj);
	if (!priv) return FALSE;
	if (!priv->pref) location_pref_free(priv->pref);

	priv->pref = location_pref_copy (pref);

	return TRUE;
}
