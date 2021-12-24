/*
 * wine.c
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


#include "wine.h"

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <assert.h>

#include "system.h"
#include "debug.h"


#define TYPE "wine"
#define BUFFER 2048


/* TODO Harmonize return code error */
int lunionplay_init_wine(GString* winedir, GString** winever, int* wow64)
{
	assert(winedir != NULL);

	GString* winepath = NULL;
	GString* path = NULL;

	winepath = g_string_new(winedir->str);

	g_string_append(winepath, "bin");

	if (lunionplay_exist_path(winepath->str) == 0)
	{
		/* Get the old PATH */
		path = g_string_new(getenv("PATH"));

		g_string_append(winepath, ":");
		g_string_append(winepath, path->str);
		setenv("PATH", winepath->str, 1);
	}
	else
	{
		ERR(TYPE, "Invalid Wine directory\n");
		g_string_free(winepath, TRUE);
		g_string_free(path, TRUE);
		return EXIT_FAILURE;
	}

	*wow64 = lunionplay_is_wine(winedir, winever);

	g_string_free(winepath, TRUE);
	g_string_free(path, TRUE);

	return EXIT_SUCCESS;
}


/* https://github.com/doitsujin/dxvk/blob/552b29ca0c6ddcf595c8610095034249cf972643/setup_dxvk.sh#L63 */
/* wineboot is always present in all wine build */
int lunionplay_is_wine(const GString* winedir, GString** winever)
{
	assert(winedir != NULL);

	int wow64;
	GString* version = NULL;
	GString* winecmd = NULL;

	/* We verify the Wine build if it is 32 or 64 bit */
	/* Default consider that it is not pure 64bit Wine */
	wow64 = TRUE;

	winecmd = g_string_new(winedir->str);
	if (winecmd->str[winecmd->len - 1] != '/')
		g_string_append(winecmd, "/");

	g_string_append(winecmd, "bin/wine");
	TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %d ]\n", winecmd->str, winecmd->len);

	if (lunionplay_exist_path(winecmd->str) != 0)
		wow64 = FALSE;

	if (wow64 == FALSE)
		g_string_append(winecmd, "64");
	g_string_append(winecmd, " --version");
	TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %ld ]\n", winecmd->str, winecmd->len);

	version = lunionplay_get_output_cmd(winecmd->str);
	TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %ld ]\n", version->str, version->len);
	if (version->len == 0)
	{
		ERR(TYPE, "Not a wine executable. Check your wine.\n");

		g_string_free(winecmd, TRUE);
		return -1;
	}

	INFO(TYPE, "version: %s\n", version->str);
	*winever = g_string_new(version->str);

	g_string_free(winecmd, TRUE);
	g_string_free(version, TRUE);
	return wow64;
}
