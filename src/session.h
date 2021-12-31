/*
 * session.h
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


#ifndef __SESSION__
#define __SESSION__


#include <glib.h>


typedef struct _lunion_play_session LunionPlaySession;


void lunionplay_free_session(LunionPlaySession** p_game);


void lunionplay_display_session(const LunionPlaySession* game);


LunionPlaySession* lunionplay_init_session(const char* gameid, const char* exec, int* error);


char* lunionplay_get_app_setting(GKeyFile* stream, const char* name);


int lunionplay_setup_session(LunionPlaySession* session);


#endif
