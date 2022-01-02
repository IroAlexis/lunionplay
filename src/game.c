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


GString* lunionplay_set_game_dir(const char* path, const char* gameid)
{
	assert(path != NULL);
	assert(gameid != NULL);

	GString* gamedir = NULL;

	gamedir = g_string_new(path);
	if (gamedir->str[gamedir->len - 1] != '/')
		g_string_append(gamedir, "/");
	g_string_append(gamedir, gameid);

	TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %d ]\n", gamedir->str, gamedir->len);

	if (lunionplay_exist_path(gamedir->str, TRUE) != 0)
	{
		g_string_free(gamedir, TRUE);
		gamedir = NULL;
	}

	return gamedir;
}


GString* lunionplay_set_command(const GString* gamedir, const char* exec)
{
	assert(gamedir != NULL);

	char* file = "gamestart";
	GString* gamestart = NULL;
	GString* bin = NULL;

	if (exec == NULL)
	{
		gamestart = g_string_new(gamedir->str);

		if (gamestart->str[gamestart->len - 1] != '/')
			g_string_append(gamestart, "/");
		g_string_append(gamestart, file);

		TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %d ]\n", gamestart->str, gamestart->len);
		if (lunionplay_exist_path(gamestart->str, TRUE) == 0)
		{
			char buffer[4096];
			FILE* stream = fopen(gamestart->str, "r");

			fgets(buffer, 4096, stream);
			fclose(stream);

			bin = g_string_new(buffer);
			if (bin->str[bin->len - 1] == '\n')
				g_string_truncate(bin, bin->len - 1);
		}

		g_string_free(gamestart, TRUE);
	}
	else
		bin = g_string_new(exec);

	if (bin != NULL)
		INFO(TYPE, "command: %s\n", bin->str);

	return bin;
}
