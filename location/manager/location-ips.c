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

#include "location-setting.h"
#include "location-log.h"

#include "module-internal.h"

#include "location-ips.h"
#include "location-marshal.h"
#include "location-ielement.h"

typedef struct _LocationIpsPrivate {
	LocationIpsMod *mod;
	LocationPosition *pos;
	LocationAccuracy *acc;
} LocationIpsPrivate;

enum {
	PROP_0,
	PROP_METHOD_TYPE,
	PROP_LAST_POSITION
};

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), LOCATION_TYPE_IPS, LocationIpsPrivate))

static void location_ielement_interface_init (LocationIElementInterface *iface);

G_DEFINE_TYPE_WITH_CODE (LocationIps, location_ips, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (LOCATION_TYPE_IELEMENT,
                         location_ielement_interface_init));

static void
location_ips_dispose (GObject *gobject)
{
	LOCATION_LOGD("location_ips_dispose");
	G_OBJECT_CLASS (location_ips_parent_class)->dispose (gobject);
}

static void
location_ips_finalize (GObject *gobject)
{
	LOCATION_LOGD("location_ips_finalize");
	LocationIpsPrivate* priv = GET_PRIVATE(gobject);
	module_free(priv->mod, "ips");
	priv->mod = NULL;

	G_OBJECT_CLASS (location_ips_parent_class)->finalize (gobject);
}

static void
location_ips_get_property (GObject *object,
	guint property_id,
	GValue *value,
	GParamSpec *pspec)
{
	LocationIpsPrivate *priv = GET_PRIVATE (object);

	switch (property_id){
		case PROP_METHOD_TYPE:
			g_value_set_int(value, LOCATION_METHOD_IPS);
			break;
		case PROP_LAST_POSITION:{
			g_value_set_boxed (value, priv->pos);
			break;
		}
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static int
location_ips_get_position (LocationIps *self,
	LocationPosition **position,
	LocationAccuracy **accuracy)
{
	LOCATION_LOGD("location_ips_get_position");

	LocationIpsPrivate *priv = GET_PRIVATE (self);
	g_return_val_if_fail (priv->mod, LOCATION_ERROR_NOT_AVAILABLE);
	setting_retval_if_fail(GPS_ENABLED);
	setting_retval_if_fail(NETWORK_ENABLED);

	LocModIpsOps ops = priv->mod->ops;
	g_return_val_if_fail (priv->mod->handler, LOCATION_ERROR_NOT_AVAILABLE);
	g_return_val_if_fail (ops.get_position, LOCATION_ERROR_NOT_AVAILABLE);
	int ret = ops.get_position(priv->mod->handler, position, accuracy);
	if (priv->pos) location_position_free(priv->pos);
	if (priv->acc) location_accuracy_free(priv->acc);
	priv->pos = location_position_copy(*position);
	priv->acc = location_accuracy_copy(*accuracy);
	return ret;
}

static void
location_ielement_interface_init (LocationIElementInterface *iface)
{
	iface->get_position = (TYPE_GET_POSITION)location_ips_get_position;
}

static void
location_ips_init (LocationIps *self)
{
	LOCATION_LOGD("location_ips_init");
	LocationIpsPrivate* priv = GET_PRIVATE(self);

	priv->mod = (LocationIpsMod*)module_new("ips");
	if(!priv->mod) LOCATION_LOGW("module loading failed");

	priv->pos = NULL;
	priv->acc = NULL;
}

static void
location_ips_class_init (LocationIpsClass *klass)
{
	LOCATION_LOGD("location_ips_class_init");
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GParamSpec *pspec;

	gobject_class->get_property = location_ips_get_property;

	gobject_class->dispose = location_ips_dispose;
	gobject_class->finalize = location_ips_finalize;

	g_type_class_add_private (klass, sizeof (LocationIpsPrivate));

	pspec = g_param_spec_int ("method",
                                "method type",
                                "location method type name",
                                LOCATION_METHOD_IPS,
                                LOCATION_METHOD_IPS,
                                LOCATION_METHOD_IPS,
                                G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
                                   PROP_METHOD_TYPE,
                                   pspec);

	pspec = g_param_spec_boxed ("last-position",
                                "ips last position prop",
                                "ips last position data",
                                LOCATION_TYPE_POSITION,
                                G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
                                   PROP_LAST_POSITION,
                                   pspec);
}

