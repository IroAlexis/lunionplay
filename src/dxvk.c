/*
 * dxvk.c
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


#include "dxvk.h"

#include <stdlib.h>
#include <sys/stat.h>


void lunionplay_setup_dxvk_runtime(const GString* path)
{
	GString* dir = NULL;

	dir = g_string_new(path->str);
	g_string_append(dir, "/shadercache/dxvk_state_cache");
	g_mkdir_with_parents(dir->str, S_IRWXU);

	setenv("DXVK_STATE_CACHE_PATH", dir->str, 0);

	if (getenv("LUNIONPLAY_LOG") != NULL)
		setenv("DXVK_LOG_LEVEL", "info", 0);
	else
		setenv("DXVK_LOG_LEVEL", "none", 0);

	g_string_free(dir, TRUE);
}
