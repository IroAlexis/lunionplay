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


static gboolean lunionplay_check_dll_file(const GString* path, const char* file)
{
	g_assert(path != NULL);

	GString* dll = NULL;
	GString* old = NULL;
	gboolean rslt;

	dll = g_string_new(path->str);
	g_string_append(dll, "/");
	g_string_append(dll, file);

	old = g_string_new(dll->str);
	g_string_append(old, ".old");

	rslt = g_file_test(dll->str, G_FILE_TEST_IS_REGULAR)
	       && g_file_test(old->str, G_FILE_TEST_IS_REGULAR);

	g_string_free(dll, TRUE);
	g_string_free(old, TRUE);

	return rslt;
}


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


gboolean lunionplay_dxvk_installed(void)
{
	gboolean dll64;
	gboolean dll32;
	GString* path = NULL;
	gchar* dxvk_file[] = {"d3d9.dll", "d3d10_core", "d3d11.dll", NULL};

	path = g_string_new(g_getenv("WINEPREFIX"));
	g_string_append(path, "/drive_c/windows/system32");

	for (gchar** tmp = dxvk_file; *tmp != NULL; tmp++)
		dll64 = dll64 && lunionplay_check_dll_file(path, *dxvk_file);

	g_string_truncate(path, path->len - strlen("/system32"));
	g_string_append(path, "/syswow64");
	for (gchar** tmp = dxvk_file; *tmp != NULL; tmp++)
		dll32 = dll32 && lunionplay_check_dll_file(path, *dxvk_file);

	g_string_free(path, TRUE);

	return dll64 || dll32;
}
