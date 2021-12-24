/*
 * game.c
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


#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "system.h"
#include "debug.h"


#define TYPE "game"


/* TODO Harmonize return code error */
int lunionplay_append_gameid(GString** gamedir, const char* gameid)
{
	assert(gameid != NULL);
	assert(*gamedir != NULL);

	GString* path = NULL;

	path = g_string_new((*gamedir)->str);
	g_string_free(*gamedir, TRUE);

	if (path->str[path->len - 1] != '/')
		g_string_append(path, "/");
	g_string_append(path, gameid);
	TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %d ]\n", path->str, path->len);

	*gamedir = g_string_new(path->str);

	g_string_free(path, TRUE);

	return lunionplay_exist_path((*gamedir)->str);
}


int lunionplay_init_game(GString** gamedir, GString** command, const char* gameid, const char* exec)
{
	assert(gameid != NULL);

	GString* exepath = NULL;

	if (lunionplay_append_gameid(gamedir, gameid) == EXIT_FAILURE)
		return EXIT_FAILURE;

	exepath = lunionplay_get_absolut_path(exec);
	*command = lunionplay_set_command(*gamedir, exepath);

	if (exepath != NULL)
		g_string_free(exepath, TRUE);

	return EXIT_SUCCESS;
}


GString* lunionplay_set_command(GString* gamedir, GString* exec)
{
	assert(gamedir != NULL);

	GString* gamestart = NULL;
	GString* bin = NULL;

	if (exec == NULL)
	{
		gamestart = g_string_new(gamedir->str);

		if (gamestart->str[gamestart->len - 1] != '/')
			g_string_append(gamestart, "/");
		g_string_append(gamestart, "gamestart");

		TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %d ]\n", gamestart->str, gamestart->len);
		if (lunionplay_exist_path(gamestart->str) == 0)
		{
			FILE* stream = NULL;
			char buffer[4096];

			stream = fopen(gamestart->str, "r");
			fgets(buffer, 4096, stream);
			fclose(stream);

			bin = g_string_new(buffer);
			if (bin->str[bin->len - 1] == '\n')
				g_string_truncate(bin, bin->len - 1);
		}

		g_string_free(gamestart, TRUE);
	}
	else
		bin = g_string_new(exec->str);

	return bin;
}
