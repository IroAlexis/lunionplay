/*
 * ini.h
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


#ifndef __INI__
#define __INI__


#include <glib.h>


/* DEPRECATED (for now)
void lunionplay_close_config(GKeyFile** stream);
*/


GKeyFile* lunionplay_open_ini(const char* pathname);


char* lunionplay_parse_ini(GKeyFile* stream, const char* group, const char* name);


#endif
