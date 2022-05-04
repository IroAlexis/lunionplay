/*
 * session.h
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


#ifndef __SESSION__
#define __SESSION__


#include <glib.h>


typedef struct _lunion_play_session LunionPlaySession;


void lunionplay_free_session(LunionPlaySession* session);


void lunionplay_display_session(const LunionPlaySession* session);


LunionPlaySession* lunionplay_init_session(const char* gameid, const char* exec);


int lunionplay_prepare_session(const LunionPlaySession* session);


int lunionplay_run_session(const LunionPlaySession* session);


#endif
