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

#ifndef __LOCATION_MODULE_INTERNAL_H__
#define __LOCATION_MODULE_INTERNAL_H__

#include <gmodule.h>
#include "location/location-module.h"

/**
 * @file location-module-internal.h
 * @brief This file contains the internal definitions and structures related to module.
 * @addtogroup LocationInternal
 * @{
 * @}
 */

typedef struct{
	GModule* module;
	char* name;
	char* path;	
} GMod;

typedef struct{
	GMod* gmod;
	gpointer handler;
	gpointer (*init)(LocModGeoOps* ops);
	void (*shutdown)(gpointer handle);
	LocModGeoOps ops;
} LocationGeoMod;

typedef struct{
	GMod* gmod;
	gpointer handler;
	gpointer (*init)(LocModPoiOps* ops);
	void (*shutdown)(gpointer handle);
	LocModPoiOps ops;
} LocationPoiMod;

typedef struct{
	GMod* gmod;	
	gpointer handler;
	gpointer (*init)(LocModGpsOps* ops);
	void (*shutdown)(gpointer handle);
	LocModGpsOps ops;
} LocationGpsMod;

typedef struct{
	GMod* gmod;	
	gpointer handler;
	gpointer (*init)(LocModWpsOps* ops);
	void (*shutdown)(gpointer handle);
	LocModWpsOps ops;
} LocationWpsMod;

typedef struct{
	GMod* gmod;	
	gpointer handler;
	gpointer (*init)(LocModCpsOps* ops);
	void (*shutdown)(gpointer handle);
	LocModCpsOps ops;
} LocationCpsMod;

typedef struct{
	GMod* gmod;
	gpointer handler;
	gpointer (*init)(LocModIpsOps* ops);
	void (*shutdown)(gpointer handle);
	LocModIpsOps ops;
} LocationIpsMod;

typedef struct{
	GMod* gmod;
	gpointer handler;
	gpointer (*init)(LocModSpsOps* ops);
	void (*shutdown)(gpointer handle);
	LocModSpsOps ops;
} LocationSpsMod;

G_BEGIN_DECLS

gboolean module_init(void);
gpointer module_new(const char* module_name);
void module_free(gpointer mod, const char* module_name);
	
G_END_DECLS

#endif
