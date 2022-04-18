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

#include <sys/stat.h>


void lunionplay_setup_dxvk_runtime(const GString* path)
{
	GString* dir = NULL;

	dir = g_string_new(path->str);
	g_string_append(dir, "/shadercache/dxvk_state_cache");
	g_mkdir_with_parents(dir->str, S_IRWXU);

	g_setenv("DXVK_STATE_CACHE_PATH", dir->str, TRUE);

	if (g_getenv("LUNIONPLAY_LOG") != NULL)
		g_setenv("DXVK_LOG_LEVEL", "info", FALSE);
	else
		g_setenv("DXVK_LOG_LEVEL", "none", FALSE);

	g_string_free(dir, TRUE);
}
