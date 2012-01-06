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

#include "location/location-geocode.h"
#include "location/location-marshal.h"
#include "location/location-ielement.h"

typedef struct _LocationGeocodePrivate {
	LocationGeoMod* mod;
} LocationGeocodePrivate;

enum {
	PROP_0,
	PROP_SERVICE_NAME
};

#define DEFAULT_SERVICE "decarta"

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), LOCATION_TYPE_GEOCODE, LocationGeocodePrivate))

static void location_ielement_interface_init (LocationIElementInterface *iface);

G_DEFINE_TYPE_WITH_CODE (LocationGeocode, location_geocode, G_TYPE_OBJECT,
	G_IMPLEMENT_INTERFACE (LOCATION_TYPE_IELEMENT, location_ielement_interface_init));


static void
location_geocode_dispose (GObject *gobject)
{
	LOCATION_LOGD("location_geocode_dispose");

	G_OBJECT_CLASS (location_geocode_parent_class)->dispose (gobject);
}

static void
location_geocode_finalize (GObject *gobject)
{
	LOCATION_LOGD("location_geocode_finalize");
	LocationGeocodePrivate* priv = GET_PRIVATE(gobject);
	module_free(priv->mod, "geocode");
	priv->mod = NULL;
	G_OBJECT_CLASS (location_geocode_parent_class)->finalize (gobject);
}

static void
location_geocode_get_property (GObject *object,
	guint property_id,
	GValue *value,
	GParamSpec *pspec)
{
	LocationGeocodePrivate *priv = GET_PRIVATE (object);

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
location_geocode_get_geocode (LocationGeocode *self,
	const LocationAddress *address,
	LocationPosition **position,
	LocationAccuracy **accuracy)
{
	LOCATION_LOGD("location_geocode_get_geocode");
	LocationGeocodePrivate* priv = GET_PRIVATE(self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.get_geocode, LOCATION_ERROR_NOT_AVAILABLE);
	return (priv->mod->ops.get_geocode)(priv->mod->handler, address, position, accuracy);
}

static int
location_geocode_get_geocode_freeform (LocationGeocode *self,
	const gchar  *address,
	LocationPosition **position,
	LocationAccuracy **accuracy)
{
	LOCATION_LOGD("location_geocode_get_geocode_freeform");
	LocationGeocodePrivate* priv = GET_PRIVATE(self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.get_geocode_freetext, LOCATION_ERROR_NOT_AVAILABLE);
	return (priv->mod->ops.get_geocode_freetext)(priv->mod->handler, address, position, accuracy);
}

static int
location_geocode_get_reversegeocode (LocationGeocode *self,
	const LocationPosition *position,
	LocationAddress **address,
	LocationAccuracy **accuracy)
{
	LOCATION_LOGD("location_geocode_get_reversegeocode");
	LocationGeocodePrivate* priv = GET_PRIVATE(self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.get_reverse_geocode, LOCATION_ERROR_NOT_AVAILABLE);
	return (priv->mod->ops.get_reverse_geocode)(priv->mod->handler, position, address, accuracy);
}

static int
location_geocode_get_geocode_async (LocationGeocode *self,
	LocationAddress *address,
	LocationPositionCB callback,
	gpointer userdata)
{
	LOCATION_LOGD("location_geocode_get_geocode_async");
	LocationGeocodePrivate* priv = GET_PRIVATE(self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.get_geocode_async, LOCATION_ERROR_NOT_AVAILABLE);
	return (priv->mod->ops.get_geocode_async)(priv->mod->handler, address, callback, userdata);
}

static int
location_geocode_get_geocode_freeform_async (LocationGeocode *self,
	const gchar *address,
	LocationPositionCB callback,
	gpointer userdata)
{
	LOCATION_LOGD("location_geocode_get_geocode_freeform_async");
	LocationGeocodePrivate* priv = GET_PRIVATE(self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.get_geocode_freetext_async, LOCATION_ERROR_NOT_AVAILABLE);
	return (priv->mod->ops.get_geocode_freetext_async)(priv->mod->handler, address, callback, userdata);
}

static int
location_geocode_get_reversegeocode_async (LocationGeocode *self,
	const LocationPosition *position,
	LocationAddressCB callback,
	gpointer userdata)
{
	LOCATION_LOGD("location_geocode_get_reversegeocode_async");
	LocationGeocodePrivate* priv = GET_PRIVATE(self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (priv->mod->ops.get_reverse_geocode_async, LOCATION_ERROR_NOT_AVAILABLE);
	return (priv->mod->ops.get_reverse_geocode_async)(priv->mod->handler, position, callback, userdata);
}

static void
location_ielement_interface_init (LocationIElementInterface *iface)
{
	iface->get_geocode = (TYPE_GET_GEOCODE)location_geocode_get_geocode;
	iface->get_geocode_freeform = (TYPE_GET_GEOCODE_FREEFORM)location_geocode_get_geocode_freeform;
	iface->get_reversegeocode = (TYPE_GET_REVERSEGEOCODE)location_geocode_get_reversegeocode;
	iface->get_geocode_async = (TYPE_GET_GEOCODE_ASYNC)location_geocode_get_geocode_async;
	iface->get_geocode_freeform_async = (TYPE_GET_GEOCODE_FREEFORM_ASYNC)location_geocode_get_geocode_freeform_async;
	iface->get_reversegeocode_async = (TYPE_GET_REVERSEGEOCODE_ASYNC)location_geocode_get_reversegeocode_async;
}

static void
location_geocode_init (LocationGeocode *self)
{
	LOCATION_LOGD("location_geocode_init");
	LocationGeocodePrivate* priv = GET_PRIVATE(self);

	priv->mod = (LocationGeoMod*)module_new("geocode");
	if(!priv->mod) LOCATION_LOGW("module loading failed");
}

static void
location_geocode_class_init (LocationGeocodeClass *klass)
{
	LOCATION_LOGD("location_geocode_class_init");
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GParamSpec *pspec;

	gobject_class->get_property = location_geocode_get_property;

	gobject_class->dispose = location_geocode_dispose;
	gobject_class->finalize = location_geocode_finalize;

	g_type_class_add_private (klass, sizeof (LocationGeocodePrivate));

	pspec = g_param_spec_string ("service provider",
                                "geocode service provider name prop",
                                "geocode service provider name",
                                DEFAULT_SERVICE,
                                G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
                                   PROP_SERVICE_NAME,
                                   pspec);
}
