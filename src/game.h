/*
 * game.h
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


#ifndef __GAME__
#define __GAME__


#include <glib.h>


int lunionplay_append_gameid(GString** gamedir, const char* gameid);


int lunionplay_init_game(GString** gamedir, GString** gamebin, const char* gameid, const char* exec);


GString* lunionplay_set_command(GString* gamedir, GString* exec);


#endif
