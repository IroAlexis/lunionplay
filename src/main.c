/*
 * main.c
 * Copyright (C) 2021-2022 IroAlexis <iroalexis@outlook.fr>
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


#include <stdio.h>
#include <libgen.h>
#include <glib.h>

#include "session.h"
#include "debug.h"


#define TYPE "main"


int main(int argc, char* argv[])
{
	int ret;
	char* bin = NULL;
	LunionPlaySession* game = NULL;

	bin = g_path_get_basename(argv[0]);

	if (argc == 1)
	{
		fprintf(stderr, "usage: %s <gameid> [ /path/to/game.exe ]\n", bin);
		free(bin);
		return EXIT_FAILURE;
	}

	ret = 1;

	game = lunionplay_init_session(argv[1], argv[2]);
	if (game != NULL)
	{
		ret = lunionplay_prepare_session(game);
		if (ret == 0)
			lunionplay_run_session(game);
		else if (ret != -1)
			fprintf(stderr, "%s: There's a frog somewhere... *ribbit*\n", bin);

		lunionplay_free_session(game);
	}

	free(bin);
	if (ret != 0)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
