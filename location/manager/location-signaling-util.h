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

#ifndef __LOCATION_SIGNALING_UTIL_H__
#define __LOCATION_SIGNALING_UTIL_H__

#include <location.h>
#include <location-ielement.h>

/**
 * @file location-signaling.h
 * @brief This file contains the signaling utils for LocationObject.
 */

G_BEGIN_DECLS

typedef enum {
	ZONE_STATUS_NONE = 0,
	ZONE_STATUS_IN,
	ZONE_STATUS_OUT,
} ZoneStatus;

void enable_signaling (LocationObject *obj,	guint32 signals[LAST_SIGNAL], gboolean *prev_enabled, gboolean enabled, LocationStatus status);

void position_signaling (LocationObject *obj, guint32 signals[LAST_SIGNAL],
		gboolean *prev_enabled, int interval, gboolean emit,
		guint *updated_interval, LocationPosition **prev_pos, LocationAccuracy **prev_acc,
		GList *prev_bound, ZoneStatus *zone_status,
		const LocationPosition *pos, const LocationAccuracy *acc);

void velocity_signaling (LocationObject* obj, guint32 signals[LAST_SIGNAL],
		gboolean *prev_enabled, int interval, gboolean emit,
		guint *updated_timestamp, LocationVelocity **prev_vel,
		const LocationVelocity *vel, const LocationAccuracy *acc);

void satellite_signaling(LocationObject *obj, guint32 signals[LAST_SIGNAL],
		gboolean *prev_enabled, int interval, gboolean emit,
		guint *updated_timestamp, LocationSatellite **pre_sat,
		const LocationSatellite *sat);

G_END_DECLS

#endif
