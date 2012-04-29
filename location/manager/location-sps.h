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

#ifndef __LOCATION_SPS_H__
#define __LOCATION_SPS_H__

#include <glib-object.h>

/**
 * @file location-sps.h
 * @brief This file contains the internal definitions and structures related to SPS.
 */

G_BEGIN_DECLS

#define LOCATION_TYPE_SPS                  (location_sps_get_type ())
#define LOCATION_SPS(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), LOCATION_TYPE_SPS, LocationSps))
#define LOCATION_IS_SPS(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LOCATION_TYPE_SPS))
#define LOCATION_SPS_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), LOCATION_TYPE_SPS, LocationSpsClass))
#define LOCATION_IS_SPS_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), LOCATION_TYPE_SPS))
#define LOCATION_SPS_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), LOCATION_TYPE_SPS, LocationSpsClass))

typedef struct _LocationSps        LocationSps;
typedef struct _LocationSpsClass   LocationSpsClass;

struct _LocationSps
{
	GObject parent_instance;
};

struct _LocationSpsClass
{
	GObjectClass parent_class;

	void (* enabled) (guint type);
	void (* disabled) (guint type);
	void (* updated) (guint type, gpointer data);
	void (* zone_in) (guint type, gpointer position, gpointer boundary);
	void (* zone_out) (guint type, gpointer position, gpointer boundary);
};

GType location_sps_get_type (void);

G_END_DECLS

#endif
