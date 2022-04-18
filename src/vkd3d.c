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

#include "system.h"



static gboolean lunionplay_check_dll_file(const GString* path, const char* file)
{
	g_assert(path != NULL);

	gboolean rslt;
	GString* dll = NULL;
	GString* old = NULL;
	GString* none = NULL;

	dll = g_string_new(path->str);
	g_string_append(dll, "/");
	g_string_append(dll, file);

	old = g_string_new(dll->str);
	g_string_append(old, ".old");
	none = g_string_new(old->str);
	g_string_append(none, "_none");

	rslt = g_file_test(dll->str, G_FILE_TEST_IS_REGULAR)
	       && (g_file_test(old->str, G_FILE_TEST_IS_REGULAR)
	       || g_file_test(none->str, G_FILE_TEST_IS_REGULAR));

	g_string_free(dll, TRUE);
	g_string_free(old, TRUE);
	g_string_free(none, TRUE);

	return rslt;
}


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


gboolean lunionplay_vkd3d_proton_installed(void)
{
	gboolean dll64;
	gboolean dll32;
	GString* path = NULL;

	path = g_string_new(g_getenv("WINEPREFIX"));
	g_string_append(path, "/drive_c/windows/system32");
	dll64 = lunionplay_check_dll_file(path, "d3d12.dll");

	g_string_truncate(path, path->len - strlen("/system32"));
	g_string_append(path, "/syswow64");
	dll32 = lunionplay_check_dll_file(path, "d3d12.dll");

	g_string_free(path, TRUE);

	return dll64 || dll32;
}
