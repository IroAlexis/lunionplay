/*
 * driver.h
 * Copyright (C) 2022 IroAlexis <iroalexis@outlook.fr>
 *
 * lunionplay is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lunionplay is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef __DRIVER__
#define __DRIVER__


#include <glib.h>


typedef struct _lunion_play_driver
{
	guint32 versionID;
	gchar* version;
	guint32 deviceID;
	gchar* deviceName;
	gchar* vendorName;
} LunionPlayDriver;


LunionPlayDriver* lunionplay_driver_create();


void lunionplay_driver_print_version(void);


void lunionplay_driver_free(LunionPlayDriver* self);


#endif

