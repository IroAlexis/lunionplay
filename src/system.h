/*
 * system.h
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


#ifndef __SYSTEM__
#define __SYSTEM__


#include <glib.h>


#define BUFFSIZE 4096


int lunionplay_exist_path(const char* path, const int boolean);


GString* lunionplay_get_output_cmd(const char* cmd);


GString* lunionplay_get_uname(void);


int lunionplay_run_process(const char* cmd, char* argv[]);


GString* lunionplay_valid_path(const char* path, const char* str);


#endif
