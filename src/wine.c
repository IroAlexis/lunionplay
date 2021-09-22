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
#include "env.h"
#include "system.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <assert.h>
#include <glib.h>


#define TYPE "wine"
#define BUFFER 2048


/* TODO Harmonize return code error */
int lunionplay_init_wine(char** winedir, char** winever)
{
	assert(winedir != NULL);

	int wow64;
	GString* winepath = NULL;
	GString* path = NULL;

	winepath = g_string_new(*winedir);

	if (winepath->str[winepath->len - 1] != '/')
		g_string_append(winepath, "/");
	else
	{
		free(*winedir);
		*winedir = strndup(winepath->str, winepath->len - 1);
	}
	g_string_append(winepath, "bin");

	if (lunionplay_exist_path(winepath->str) == 0)
	{
		path = g_string_new(lunionplay_get_envar("PATH"));
		g_string_append(winepath, ":");
		g_string_append(winepath, path->str);
		lunionplay_set_envar("PATH", winepath->str);
	}
	else
	{
		ERR(TYPE, "Invalid Wine directory\n");
		g_string_free(winepath, TRUE);
		g_string_free(path, TRUE);
		return EXIT_FAILURE;
	}

	wow64 = lunionplay_is_wine(*winedir, winever);

	g_string_free(winepath, TRUE);
	g_string_free(path, TRUE);

	return EXIT_SUCCESS;
}


/* https://github.com/doitsujin/dxvk/blob/552b29ca0c6ddcf595c8610095034249cf972643/setup_dxvk.sh#L63 */
/* wineboot is always present in all wine build */
int lunionplay_is_wine(const char* winedir, char** winever)
{
	assert(winedir != NULL);

	int wow64;
	char tmp[BUFFER];
	GString* version = NULL;
	GString* wine = NULL;
	FILE* fp = NULL;

	/* We verify the Wine build if it is 32 or 64 bit */
	/* Default consider that it is not pure 64bit Wine */
	wow64 = 0;

	wine = g_string_new(winedir);
	g_string_append(wine, "/bin/wine");
	TRACE(__FILE__, __FUNCTION__, "wine->str '%s' (%d)\n", wine->str, wine->len);

	if (lunionplay_exist_path(wine->str) == -1)
		wow64 = -1;

	if (wow64 == -1)
		fp = popen("wine64 --version | grep wine", "r");
	else
		fp = popen("wine --version | grep wine", "r");

	if (fp == NULL)
	{
		ERR(TYPE, "Impossible to execute the command\n");

		g_string_free(wine, TRUE);
		return 1;
	}
	fgets(tmp, BUFFER, fp);
	version = g_string_new(tmp);
	g_string_truncate(version, version->len - 1);

	TRACE(__FILE__, __FUNCTION__, "version->str '%s' (%ld)\n", version->str, version->len);

	if (strcmp(version->str, "") == 0)
	{
		ERR(TYPE, "Not a wine executable. Check your wine.\n");

		pclose(fp);
		g_string_free(wine, TRUE);
		return 2;
	}

	INFO(TYPE, "version: %s\n", version->str);
	*winever = strdup(version->str);

	pclose(fp);
	g_string_free(wine, TRUE);
	g_string_free(version, TRUE);
	return wow64;
}
