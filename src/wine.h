/*
 * wine.h
 * Copyright (C) 2021-2022 IroAlexis <iroalexis@outlook.fr>
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


#ifndef __WINE__
#define __WINE__

#include <stdio.h>
#include <glib.h>


typedef struct _lunion_play_wine LunionPlayWine;


void lunionplay_wine_print_runtime(FILE* stream);


void lunionplay_wine_print(const LunionPlayWine* wine);


void lunionplay_wine_free(LunionPlayWine* wine);


const gchar* lunionplay_wine_get_bin(const LunionPlayWine* self);


const gchar* lunionplay_wine_get_bin64(const LunionPlayWine* self);


const gchar* lunionplay_wine_get_server(const LunionPlayWine* self);


const gchar* lunionplay_wine_get_version(const LunionPlayWine* self);


LunionPlayWine* lunionplay_wine_create(const gchar* winedir);


LunionPlayWine* lunionplay_wine_system_create(void);


void lunionplay_wine_setup_runtime(const LunionPlayWine* self);


void lunionplay_wine_setup_prefix(GString* dir);


void lunionplay_wine_update_prefix(const LunionPlayWine* self);


void lunionplay_wine_use_server(const LunionPlayWine* self, const gchar* option);


#endif
