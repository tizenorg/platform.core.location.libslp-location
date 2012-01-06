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

#include "location/location-module-internal.h"

#include "location/location-poi.h"
#include "location/location-marshal.h"
#include "location/location-ielement.h"

typedef struct _LocationPoiPrivate {
	LocationPoiMod* mod;
} LocationPoiPrivate;

enum {
	PROP_0,
	PROP_SERVICE_NAME
};

#define DEFAULT_SERVICE "decarta"

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), LOCATION_TYPE_POI, LocationPoiPrivate))

static void location_ielement_interface_init (LocationIElementInterface *iface);

G_DEFINE_TYPE_WITH_CODE (LocationPoi, location_poi, G_TYPE_OBJECT,
	G_IMPLEMENT_INTERFACE (LOCATION_TYPE_IELEMENT, location_ielement_interface_init));


static void
location_poi_dispose (GObject *gobject)
{
	LOCATION_LOGD("location_poi_dispose");

	G_OBJECT_CLASS (location_poi_parent_class)->dispose (gobject);
}

static void
location_poi_finalize (GObject *gobject)
{
	LOCATION_LOGD("location_poi_finalize");
	LocationPoiPrivate* priv = GET_PRIVATE(gobject);
	module_free(priv->mod, "poi");
	priv->mod = NULL;
	G_OBJECT_CLASS (location_poi_parent_class)->finalize (gobject);
}

static void
location_poi_get_property (GObject *object,
	guint property_id,
	GValue *value,
	GParamSpec *pspec)
{
	LocationPoiPrivate *priv = GET_PRIVATE (object);

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

static int
location_poi_get_poi (LocationPoi *poi,
	gdouble radius,
	const gchar *keyword,
	LocationPOIInfo **poi_info)
{
	LOCATION_LOGD("location_poi_get_poi");
	LocationPoiPrivate* priv = GET_PRIVATE(poi);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.get_poi, LOCATION_ERROR_NOT_AVAILABLE);
	return (priv->mod->ops.get_poi)(priv->mod->handler, radius, keyword, poi_info);
}

static int
location_poi_get_poi_from_address (LocationPoi *poi,
	const LocationAddress *address,
	gdouble radius,
	const gchar *keyword,
	LocationPOIInfo **poi_info)
{
	LOCATION_LOGD("location_poi_get_poi_from_address");
	LocationPoiPrivate* priv = GET_PRIVATE(poi);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.get_poi_from_address, LOCATION_ERROR_NOT_AVAILABLE);
	return (priv->mod->ops.get_poi_from_address)(priv->mod->handler, address, radius, keyword, poi_info);
}

static int
location_poi_get_poi_from_position (LocationPoi *poi,
	const LocationPosition *position,
	gdouble radius,
	const gchar *keyword,
	LocationPOIInfo **poi_info)
{
	LOCATION_LOGD("location_poi_get_poi_from_position");
	LocationPoiPrivate* priv = GET_PRIVATE(poi);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.get_poi_from_position, LOCATION_ERROR_NOT_AVAILABLE);
	return (priv->mod->ops.get_poi_from_position)(priv->mod->handler, position, radius, keyword, poi_info);
}

static int
location_poi_get_poi_async (LocationPoi *poi,
	gdouble radius,
	const gchar *keyword,
	LocationPOICB callback,
	gpointer userdata)
{
	LOCATION_LOGD("location_poi_get_poi_async");
	LocationPoiPrivate* priv = GET_PRIVATE(poi);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.get_poi_async, LOCATION_ERROR_NOT_AVAILABLE);
	return (priv->mod->ops.get_poi_async)(priv->mod->handler, radius, keyword, callback, userdata);
}

static int
location_poi_get_poi_from_address_async (LocationPoi *poi,
	const LocationAddress *address,
	gdouble radius,
	const gchar *keyword,
	LocationPOICB callback,
	gpointer userdata)
{
	LOCATION_LOGD("location_poi_get_poi_from_address_async");
	LocationPoiPrivate* priv = GET_PRIVATE(poi);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.get_poi_from_address_async, LOCATION_ERROR_NOT_AVAILABLE);
	return (priv->mod->ops.get_poi_from_address_async)(priv->mod->handler, address, radius, keyword, callback, userdata);
}

static int
location_poi_get_poi_from_position_async (LocationPoi *poi,
	const LocationPosition *position,
	gdouble radius,
	const gchar* keyword,
	LocationPOICB callback,
	gpointer userdata)
{
	LOCATION_LOGD("location_poi_get_poi_from_position_async");
	LocationPoiPrivate* priv = GET_PRIVATE(poi);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.get_poi_from_position_async, LOCATION_ERROR_NOT_AVAILABLE);
	return (priv->mod->ops.get_poi_from_position_async)(priv->mod->handler, position, radius, keyword, callback, userdata);
}

static void
location_ielement_interface_init (LocationIElementInterface *iface)
{
	iface->get_poi = (TYPE_GET_POI)location_poi_get_poi;
	iface->get_poi_from_address = (TYPE_GET_POI_FROM_ADDRESS)location_poi_get_poi_from_address;
	iface->get_poi_from_position = (TYPE_GET_POI_FROM_POSITION)location_poi_get_poi_from_position;
	iface->get_poi_async = (TYPE_GET_POI_ASYNC)location_poi_get_poi_async;
	iface->get_poi_from_address_async = (TYPE_GET_POI_FROM_ADDRESS_ASYNC)location_poi_get_poi_from_address_async;
	iface->get_poi_from_position_async = (TYPE_GET_POI_FROM_POSITION_ASYNC)location_poi_get_poi_from_position_async;
}

static void
location_poi_init (LocationPoi *poi)
{
	LOCATION_LOGD("location_poi_init");
	LocationPoiPrivate* priv = GET_PRIVATE(poi);

	priv->mod = (LocationPoiMod*)module_new("poi");
	if(!priv->mod) LOCATION_LOGW("module loading failed");
}

static void
location_poi_class_init (LocationPoiClass *klass)
{
	LOCATION_LOGD("location_poi_class_init");
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GParamSpec *pspec;

	gobject_class->get_property = location_poi_get_property;

	gobject_class->dispose = location_poi_dispose;
	gobject_class->finalize = location_poi_finalize;

	g_type_class_add_private (klass, sizeof (LocationPoiPrivate));

	pspec = g_param_spec_string ("service provider",
                                "poi service provider name prop",
                                "poi service provider name",
                                DEFAULT_SERVICE,
                                G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
                                   PROP_SERVICE_NAME,
                                   pspec);
}
