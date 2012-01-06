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

#include "location/location-signaling-util.h"
#include "location/location-log.h"

#define POS_EXPIRATION_TIME		9	/* sec */

void
enable_signaling (LocationObject *obj,
	guint32 signals[LAST_SIGNAL],
	gboolean *prev_enabled,
	gboolean enabled,
	LocationStatus status)
{
	g_return_if_fail(obj);
	g_return_if_fail(signals);
	g_return_if_fail(prev_enabled);
	if (*prev_enabled == TRUE && enabled == FALSE) {
		*prev_enabled = FALSE;
		LOCATION_LOGD("Signal emit: SERVICE_DISABLED");
		g_signal_emit (obj, signals[SERVICE_DISABLED], 0, LOCATION_STATUS_NO_FIX);
	} else if (*prev_enabled == FALSE && enabled == TRUE){
		*prev_enabled = TRUE;
		LOCATION_LOGD("Signal emit: SERVICE_ENABLED");
		g_signal_emit (obj, signals[SERVICE_ENABLED], 0, status);
	}
}

void
position_signaling (LocationObject *obj,
	guint32 signals[LAST_SIGNAL],
	gboolean *prev_enabled,
	guint interval,
	LocationPosition **prev_pos,
	LocationAccuracy **prev_acc,
	GList *prev_bound,
	ZoneStatus *zone_status,
	gboolean enabled,
	const LocationPosition *pos,
	const LocationAccuracy *acc)
{
	g_return_if_fail(zone_status);
	g_return_if_fail(pos);
	g_return_if_fail(acc);
	g_return_if_fail(obj);
	g_return_if_fail(signals);

	int index = 0;
	gboolean is_inside = FALSE;
	guint updated_timestamp = 0;
	GList *boundary_list = prev_bound;
	LocationBoundary *boundary = NULL;

	if (prev_enabled == FALSE) enable_signaling(obj, signals, prev_enabled, enabled, pos->status);

	if(*prev_pos || !*prev_acc || location_accuracy_level_compare(*prev_acc, acc) != -1 ||
			(pos->timestamp - (*prev_pos)->timestamp) > POS_EXPIRATION_TIME) {

		if(*prev_pos) {
			updated_timestamp = (*prev_pos)->updated_timestamp;
			location_position_free(*prev_pos);
		}
		if(*prev_acc) location_accuracy_free(*prev_acc);

		if(!updated_timestamp || pos->timestamp - updated_timestamp >= interval) {
			LOCATION_LOGD("Signal emit: POSITION SERVICE_UPDATED");
			g_signal_emit(obj, signals[SERVICE_UPDATED], 0, POSITION_UPDATED, pos, acc);
			updated_timestamp = pos->timestamp;
		}

		if(boundary_list) {
			while((boundary = (LocationBoundary *)g_list_nth_data(boundary_list, index))!= NULL) {

				is_inside = location_boundary_if_inside(boundary, pos);
				if(is_inside) {
					break;
				}
				index++;
			}

			if(is_inside) {
				if(*zone_status != ZONE_STATUS_IN) {
					LOCATION_LOGD("Signal emit: ZONE IN");
					g_signal_emit(obj, signals[ZONE_IN], 0, NULL, pos, acc);
					*zone_status = ZONE_STATUS_IN;
				}
			}
			else {
				if (*zone_status != ZONE_STATUS_OUT) {
					LOCATION_LOGD("Signal emit : ZONE_OUT");
					g_signal_emit(obj, signals[ZONE_OUT], 0, NULL, pos, acc);
					*zone_status = ZONE_STATUS_OUT;
				}
			}

		}
	}

	*prev_pos = location_position_copy(pos);
	*prev_acc = location_accuracy_copy(acc);
	(*prev_pos)->updated_timestamp = updated_timestamp;
}

void
velocity_signaling (LocationObject *obj,
	guint32 signals[LAST_SIGNAL],
	gboolean *prev_enabled,
	guint interval,
	LocationVelocity **prev_vel,
	gboolean enabled,
	const LocationVelocity *vel,
	const LocationAccuracy *acc)
{
	g_return_if_fail(obj);
	g_return_if_fail(signals);
	g_return_if_fail(vel);

	guint updated_timestamp = 0;

	if (prev_enabled == FALSE) enable_signaling(obj, signals, prev_enabled, enabled, LOCATION_STATUS_NO_FIX);

	if(*prev_vel) {
		updated_timestamp = (*prev_vel)->updated_timestamp;
		if(!location_velocity_equal(*prev_vel, vel)) {
			location_velocity_free (*prev_vel);
		}
	}

	*prev_vel = location_velocity_copy (vel);

	if(!updated_timestamp || vel->timestamp - updated_timestamp >= interval) {

		LOCATION_LOGD ("Signal emit: VELOCITY SERVICE_UPDATED");
		LocationVelocity *temp_vel = location_velocity_copy (*prev_vel);
		LocationAccuracy *temp_acc = location_accuracy_copy(acc);

		g_signal_emit (obj, signals[SERVICE_UPDATED], 0, VELOCITY_UPDATED, temp_vel, temp_acc);

		(*prev_vel)->updated_timestamp = vel->timestamp;

		location_velocity_free(temp_vel);
		location_accuracy_free(temp_acc);
	}
	else {
		(*prev_vel)->updated_timestamp = updated_timestamp;
	}

}
