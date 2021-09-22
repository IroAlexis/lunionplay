/*
 * core.h
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


#ifndef __CORE__
#define __CORE__


#include <glib.h>


typedef struct session
{
	char* winedir;
	char* winever;
	char* gamedir;
	GKeyFile* stream;
} Session;


int lunionplay_valid_gamedir(char** gamedir, const char* alias);


void lunionplay_free_session(Session** p_game);


Session* lunionplay_init_session(const char* alias, int* error);


int lunionplay_load_param(GKeyFile* stream, const char* name, char** value);


#endif
