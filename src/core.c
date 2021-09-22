/*
 * core.c
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


#include "core.h"
#include "config.h"
#include "debug.h"
#include "env.h"
#include "system.h"
#include "wine.h"

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>


#define TYPE "core"


/* TODO Harmonize return code error */
int lunionplay_valid_gamedir(char** gamedir, const char* alias)
{
	assert(alias != NULL);
	assert(*gamedir != NULL);

	GString* path = NULL;

	path = g_string_new(*gamedir);
	free(*gamedir);

	if (path->str[path->len - 1] != '/')
		g_string_append(path, "/");
	g_string_append(path, alias);
	TRACE(__FILE__, __FUNCTION__, "path->str '%s' (%d)\n", path->str, path->len);

	*gamedir = strdup(path->str);

	g_string_free(path, TRUE);

	return lunionplay_exist_path(*gamedir);
}


void lunionplay_free_session(Session** p_game)
{
	if (*p_game != NULL)
	{
		if ((*p_game)->stream != NULL)
			lunionplay_close_config(&(*p_game)->stream);

		if ((*p_game)->gamedir != NULL)
			free((*p_game)->gamedir);

		if ((*p_game)->winedir != NULL)
			free((*p_game)->winedir);

		if ((*p_game)->winever != NULL)
			free((*p_game)->winever);

		free(*p_game);
		*p_game = NULL;
	}
}


Session* lunionplay_init_session(const char* alias, int* error)
{
	int ret = EXIT_FAILURE;
	Session* tmp = NULL;

	tmp = (Session*) calloc(1, sizeof(Session));
	if (NULL == tmp)
	{
		*error = ret;
		return NULL;
	}
	tmp->gamedir = NULL;
	tmp->winedir = NULL;
	tmp->winever = NULL;
	tmp->stream = g_key_file_new();

	ret = lunionplay_open_config(&(tmp->stream), lunionplay_get_envar("LUNIONPLAY_CONFIGFILE"));
	if (ret == EXIT_FAILURE)
	{
		lunionplay_close_config(&(tmp->stream));
		tmp->stream = NULL;
	}

	ret = lunionplay_load_param(tmp->stream, "wine_custom", &(tmp->winedir));
	if (tmp->winedir == NULL)
	{
		ERR(TYPE, "No Wine detected\n");
		*error = ret;
		return tmp;
	}
	ret = lunionplay_init_wine(&(tmp->winedir), &(tmp->winever));
	if (ret != EXIT_SUCCESS)
	{
		*error = ret;
		return tmp;
	}

	ret = lunionplay_load_param(tmp->stream, "default_dir", &(tmp->gamedir));
	if (tmp->gamedir == NULL)
	{
		ERR(TYPE, "No games directory detected\n");
		*error = ret;
		return tmp;
	}
	ret = lunionplay_valid_gamedir(&(tmp->gamedir), alias);
	if (ret != 0)
	{
		*error = ret;
		return tmp;
	}

	return tmp;
}


int lunionplay_load_param(GKeyFile* stream, const char* name, char** value)
{
	char* envar = NULL;
	char* namup = NULL;

	namup = g_ascii_strup(name, strlen(name));
	envar = lunionplay_get_envar(namup);

	TRACE(__FILE__, __FUNCTION__, "namup %s | envar %p\n", namup, envar);
	free(namup);

	if (envar != NULL)
		*value = strdup(envar);
	if (*value == NULL && stream != NULL)
		lunionplay_parse_config(stream, "lunion-play", name, value);
	else
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
