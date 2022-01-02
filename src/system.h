/*
 * system.h
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


#ifndef __SYSTEM__
#define __SYSTEM__


#include <glib.h>


char* lunionplay_default_place();


int lunionplay_exist_path(const char* path, const int msg);


GString* lunionplay_get_absolut_path(const char* path);


GString* lunionplay_get_output_cmd(const char* cmd);


GString* lunionplay_get_uname(void);


int lunionplay_run_process(const char* cmd, char* const argv[]);


#endif
