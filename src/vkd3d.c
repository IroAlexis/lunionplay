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

#include "utils.h"



static gboolean lunionplay_check_dll_file(const gchar* path, const char* file)
{
	g_assert(path != NULL);

	gchar* dll = NULL;
	gchar* old = NULL;
	gchar* none = NULL;
	gboolean rslt = FALSE;

	dll = g_build_path("/", path, file, NULL);
	old = g_strconcat(dll, ".old", NULL);
	none = g_strconcat(old, "_none", NULL);

	rslt = g_file_test(dll, G_FILE_TEST_IS_REGULAR) &&
	       (g_file_test(old, G_FILE_TEST_IS_REGULAR) ||
	       g_file_test(none, G_FILE_TEST_IS_REGULAR));

	g_free(dll);
	g_free(old);
	g_free(none);

	return rslt;
}


void lunionplay_setup_vkd3d_proton_runtime(const gchar* path)
{
	gchar* dir = NULL;

	if (! lunionplay_vkd3d_proton_installed())
	{
		return;
	}

	dir = g_build_path("/", path, "shadercache", "vkd3d_proton_shader_cache", NULL);
	g_mkdir_with_parents(dir, S_IRWXU);

	g_setenv("VKD3D_SHADER_CACHE_PATH", dir, TRUE);

	if (g_getenv("LUNIONPLAY_LOG") != NULL)
		g_setenv("VKD3D_DEBUG", "warn", FALSE);
	else
		g_setenv("VKD3D_DEBUG", "none", FALSE);

	g_free(dir);
}


/*
 * TODO Merge with dxvk.c (maybe runtime.c)
 */
gboolean lunionplay_vkd3d_proton_installed(void)
{
	gboolean dll64;
	gboolean dll32;
	GString* path = NULL;

	path = g_string_new(g_getenv("WINEPREFIX"));
	g_string_append(path, "/drive_c/windows/system32");
	dll64 = lunionplay_check_dll_file(path->str, "d3d12.dll");

	// "system32" = 9 characters
	g_string_truncate(path, path->len - 8);
	g_string_append(path, "syswow64");
	dll32 = lunionplay_check_dll_file(path->str, "d3d12.dll");

	g_string_free(path, TRUE);

	return dll64 || dll32;
}
