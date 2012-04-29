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

#ifndef __LOCATION_LOG_H__
#define __LOCATION_LOG_H__

/**
 * @file location-log.h
 * @brief This file contains macro functions for logging.
 */

#define TAG_LOCATION 		"location"

#ifdef LOCATION_DLOG_DEBUG        // if debug mode, show filename & line number
#include <dlog.h>
#define LOCATION_LOGD(fmt,args...)  SLOG(LOG_DEBUG, TAG_LOCATION, "[%-15s:%-4d:%-27s]"fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define LOCATION_LOGW(fmt,args...)  SLOG(LOG_WARN,  TAG_LOCATION, "[%-15s:%-4d:%-27s]"fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define LOCATION_LOGI(fmt,args...)  SLOG(LOG_INFO,  TAG_LOCATION, "[%-15s:%-4d:%-27s]"fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define LOCATION_LOGE(fmt,args...)  SLOG(LOG_ERROR, TAG_LOCATION, "[%-15s:%-4d:%-27s]"fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#elif LOCATION_DLOG_RELEASE      // if release mode, do not show filename & line number
#include <dlog.h>
#define LOCATION_LOGD(fmt,args...)  SLOG(LOG_DEBUG, TAG_LOCATION, fmt"\n", ##args)
#define LOCATION_LOGW(fmt,args...)  SLOG(LOG_WARN,  TAG_LOCATION, fmt"\n", ##args)
#define LOCATION_LOGI(fmt,args...)  SLOG(LOG_INFO,  TAG_LOCATION, fmt"\n", ##args)
#define LOCATION_LOGE(fmt,args...)  SLOG(LOG_ERROR, TAG_LOCATION, fmt"\n", ##args)
#else                       // if do not use dlog
#define LOCATION_LOGD(...)  g_debug(__VA_ARGS__);
#define LOCATION_LOGW(...)  g_warning(__VA_ARGS__);
#define LOCATION_LOGI(...)  g_message(__VA_ARGS__);
#define LOCATION_LOGE(...)  g_error(__VA_ARGS__);
#endif

#endif
