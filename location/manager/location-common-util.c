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

#include "location.h"
#include "location-common-util.h"
#include "location-setting.h"
#include "location-log.h"

static gint compare_position (gconstpointer a, gconstpointer b)
{
	g_return_val_if_fail(a, 1);
	g_return_val_if_fail(b, -1);

	if(location_position_equal((LocationPosition*) a, (LocationPosition *)b) == TRUE) {
		return 0;
	}

	return -1;
}

static int
boundary_compare (gconstpointer comp1, gconstpointer comp2)
{
	g_return_val_if_fail(comp1, 1);
	g_return_val_if_fail(comp2, -1);

	int ret = -1;

	LocationBoundary *boundary1 = (LocationBoundary *)comp1;
	LocationBoundary *boundary2 = (LocationBoundary *)comp2;

	if (boundary1->type == boundary2->type) {
		switch (boundary1->type) {
			case LOCATION_BOUNDARY_CIRCLE: {
				if (location_position_equal(boundary1->circle.center, boundary2->circle.center)
					&& boundary1->circle.radius == boundary2->circle.radius) {
					ret = 0;
				}
				break;
			}
			case LOCATION_BOUNDARY_RECT: {
				if (location_position_equal(boundary1->rect.left_top, boundary2->rect.left_top)
					&& location_position_equal(boundary1->rect.right_bottom, boundary2->rect.right_bottom)) {
					ret = 0;
				}
				break;
			}
			case LOCATION_BOUNDARY_POLYGON: {

				GList *boundary1_next = NULL;
				GList *boundary2_start = NULL, *boundary2_prev = NULL, *boundary2_next = NULL;
				if (g_list_length(boundary1->polygon.position_list) != g_list_length(boundary2->polygon.position_list)) {
					return -1;
				}

				// Find a matching index of Boundary2 with Boundary1's 1st postion.
				boundary2_start = g_list_find_custom(boundary2->polygon.position_list, g_list_nth_data(boundary1->polygon.position_list, 0), (GCompareFunc) compare_position);
				if (boundary2_start == NULL) return -1;

				boundary2_prev = g_list_previous(boundary2_start);
				boundary2_next = g_list_next(boundary2_start);
				if (boundary2_prev == NULL) boundary2_prev = g_list_last(boundary2->polygon.position_list);
				if (boundary2_next == NULL) boundary2_next = g_list_first(boundary2->polygon.position_list);

				boundary1_next = g_list_next(boundary1->polygon.position_list);
				if (location_position_equal((LocationPosition*)boundary1_next->data, (LocationPosition*)boundary2_prev->data) == TRUE){
					boundary1_next = g_list_next(boundary1_next);
					while (boundary1_next) {
						boundary2_prev = g_list_previous(boundary2_prev);
						if (boundary2_prev == NULL) boundary2_prev = g_list_last(boundary2->polygon.position_list);
						if (location_position_equal((LocationPosition*)boundary1_next->data, (LocationPosition*) boundary2_prev->data) == FALSE){
							return -1;
						}
						boundary1_next = g_list_next(boundary1_next);
					}
					ret = 0;
				}
				else if (location_position_equal((LocationPosition*)boundary1_next->data, (LocationPosition*)boundary2_next->data) == TRUE) {
					boundary1_next = g_list_next(boundary1_next);
					while(boundary1_next) {
						boundary2_next = g_list_next(boundary2_next);
						if (boundary2_next == NULL) boundary2_next = g_list_first(boundary2->polygon.position_list);
						if (location_position_equal((LocationPosition*)boundary1_next->data, (LocationPosition*) boundary2_next->data) == FALSE){
							return -1;
						}
						boundary1_next = g_list_next(boundary1_next);
					}
					ret = 0;
				}
				else {
					return -1;
				}
				break;
			}
			default:{
				 ret = -1;
				 break;
			}

		}
	}

	return ret;
}

int set_prop_boundary(GList **prev_boundary_list, GList *new_boundary_list)
{
	g_return_val_if_fail(new_boundary_list, LOCATION_ERROR_PARAMETER);

	int index;
	GList *check_list = NULL;

	LocationBoundary *new_boundary = NULL;
	LocationBoundary *copy_boundary = NULL;

	index = 0;
	while((new_boundary = (LocationBoundary*) g_list_nth_data(new_boundary_list, index)) != NULL) {

		check_list = g_list_find_custom(*prev_boundary_list, new_boundary, (GCompareFunc)boundary_compare);
		if (check_list == NULL) {
			LOCATION_LOGD("Set Prop >> boundary type: [%d]", new_boundary->type);
			copy_boundary = location_boundary_copy(new_boundary);
			*prev_boundary_list = g_list_append(*prev_boundary_list, copy_boundary);
		}
		index++;
	}
	*prev_boundary_list = g_list_first(*prev_boundary_list);

	return LOCATION_ERROR_NONE;
}



int set_prop_removal_boundary(GList **prev_boundary_list, LocationBoundary* boundary)
{
	g_return_val_if_fail(*prev_boundary_list, LOCATION_ERROR_PARAMETER);

	GList *check_list = NULL;

	check_list = g_list_find_custom (*prev_boundary_list, boundary, (GCompareFunc) boundary_compare);
	if (check_list) {
		LOCATION_LOGD("Found");
		*prev_boundary_list = g_list_delete_link(*prev_boundary_list, check_list);
	}

	if (g_list_length(*prev_boundary_list) == 0 ) {
		LOCATION_LOGD("Boundary List is empty");
		g_list_free(*prev_boundary_list);
		*prev_boundary_list = NULL;
	}

	return LOCATION_ERROR_NONE;
}
