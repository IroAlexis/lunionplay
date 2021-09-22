/*
 * config.c
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


#include "config.h"
#include "system.h"
#include "env.h"
#include "debug.h"

#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>


#define TYPE "config"
#define HOME_PATH_FILE "/.config/lunionplay/config.ini"


void lunionplay_close_config(GKeyFile** stream)
{
	assert (*stream != NULL);

	g_key_file_free(*stream);
}


int lunionplay_open_config(GKeyFile** stream, const char* pathname)
{
	struct stat statbuf;
	GString* cfg = NULL;
	GError* error = NULL;

	if (pathname != NULL)
		TRACE(__FILE__, __FUNCTION__, "pathname %s\n", pathname);

	if (NULL == pathname)
	{
		cfg = g_string_new(lunionplay_get_envar("HOME"));
		g_string_append(cfg, HOME_PATH_FILE);
	}
	else
		cfg = g_string_new(pathname);
	TRACE(__FILE__, __FUNCTION__, "cfg->str '%s'\n", cfg->str);

	/* Prevent memory leak with g_key_file_load_from_file */
	if (stat(cfg->str, &statbuf) == 0)
	{
		INFO(TYPE, "%s\n", cfg->str);
		if (!g_key_file_load_from_file(*stream, cfg->str, G_KEY_FILE_NONE, &error))
		{
			if (NULL != error)
				g_error_free(error);

			g_string_free(cfg, TRUE);
			return EXIT_FAILURE;
		}
	}
	else
	{
		g_string_free(cfg, TRUE);
		return EXIT_FAILURE;
	}

	g_string_free(cfg, TRUE);
	return EXIT_SUCCESS;
}


int lunionplay_parse_config(GKeyFile* stream, const char* group, const char* name, char** value)
{
	assert(stream != NULL);
	assert(group != NULL);
	assert(name != NULL);

	int ret = 0;
	GError* error = NULL;

	if (NULL == stream)
		return -1;

	*value = g_key_file_get_string(stream, group, name, &error);
	if (NULL == *value && error != NULL)
	{
		ret = 1;
		if (error->code ==  G_KEY_FILE_ERROR_GROUP_NOT_FOUND)
		{
			ret = 2;
			ERR(TYPE, "'%s' group not found\n", group);
		}
		else if (error->code == G_KEY_FILE_ERROR_KEY_NOT_FOUND)
			WARN(TYPE, "'%s' key not found\n", name);
		else
			ERR(TYPE, "%s\n", error->message);

		g_error_free(error);
	}

	return ret;
}
