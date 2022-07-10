/*
 * game.h
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


#ifndef __GAME__
#define __GAME__


#include "config.h"
#include "debug.h"
#include "utils.h"

#include <glib.h>


typedef struct _lunion_play_game LunionPlayGame;


LunionPlayGame* lunionplay_game_create(GKeyFile* cfg, const gchar* id);


void lunionplay_game_free(LunionPlayGame* self);


const gchar* lunionplay_game_get_command(const LunionPlayGame* self);


const gchar* lunionplay_game_get_dir(const LunionPlayGame* self);


const gchar* lunionplay_game_get_title(const LunionPlayGame* self);


#endif
