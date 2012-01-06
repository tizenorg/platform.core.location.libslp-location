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

#ifndef __LOCATION_IPS_H__
#define __LOCATION_IPS_H__

#include <glib-object.h>

/**
 * @file location-ips.h
 * @brief This file contains the internal definitions and structures related to IPS.
 * @addtogroup LocationInternal
 * @{
 * @}
 */

G_BEGIN_DECLS

#define LOCATION_TYPE_IPS                  (location_ips_get_type ())
#define LOCATION_IPS(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), LOCATION_TYPE_IPS, LocationIps))
#define LOCATION_IS_IPS(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LOCATION_TYPE_IPS))
#define LOCATION_IPS_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), LOCATION_TYPE_IPS, LocationIpsClass))
#define LOCATION_IS_IPS_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), LOCATION_TYPE_IPS))
#define LOCATION_IPS_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), LOCATION_TYPE_IPS, LocationIpsClass))

typedef struct _LocationIps        LocationIps;
typedef struct _LocationIpsClass   LocationIpsClass;

struct _LocationIps
{
	GObject parent_instance;
};

struct _LocationIpsClass
{
	GObjectClass parent_class;
};

GType location_ips_get_type (void);

G_END_DECLS

#endif
