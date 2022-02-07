/*
 * wine.h
 * Copyright (C) 2021 Alexis Peypelut <peypeluta@live.fr>
 *
 * lunion-play is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lunion-play is distributed in the hope that it will be useful, but
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


void lunionplay_display_env_wine(FILE* stream);


void lunionplay_display_wine(const LunionPlayWine* wine);


void lunionplay_free_wine(LunionPlayWine* wine);


char* lunionplay_get_wine_bin(LunionPlayWine* wine);


LunionPlayWine* lunionplay_init_wine(const GString* winedir);


void lunionplay_set_wine_env(void);


int lunionplay_set_wine_prefix(GString* gamedir);


int lunionplay_update_wine_prefix(void);


void lunionplay_use_wineserver(const char* option);


#endif
