/*
 * vkd3d.c
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


#include "vkd3d.h"

#include <sys/stat.h>


void lunionplay_setup_vkd3d_proton_runtime(const GString* path)
{
	GString* dir = NULL;

	dir = g_string_new(path->str);
	g_string_append(dir, "/shadercache/vkd3d_proton_shader_cache");
	g_mkdir_with_parents(dir->str, S_IRWXU);

	g_setenv("VKD3D_SHADER_CACHE_PATH", dir->str, TRUE);

	if (g_getenv("LUNIONPLAY_LOG") != NULL)
		g_setenv("VKD3D_DEBUG", "warn", FALSE);
	else
		g_setenv("VKD3D_DEBUG", "none", FALSE);

	g_string_free(dir, TRUE);
}
