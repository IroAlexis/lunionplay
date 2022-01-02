/*
 * main.c
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


#include <stdio.h>
#include <libgen.h>
#include <glib.h>

#include "session.h"
#include "config.h"
#include "debug.h"


#define TYPE "main"


int main(int argc, char* argv[])
{
	int error;
	int ret = EXIT_SUCCESS;
	LunionPlaySession* game = NULL;

	if (argv[1] != NULL)
	{
		game = lunionplay_init_session(argv[1], argv[2], &error);

		if (error == 0)
			lunionplay_setup_session(game);
		else
		{
			fprintf(stderr, "%s: There's a frog somewhere... *ribbit*\n", basename(argv[0]));
			ret = EXIT_FAILURE;
		}

		lunionplay_free_session(game);
		game = NULL;
	}
	else
		fprintf(stderr, "usage: %s <gameid> [ /path/to/game.exe ]\n", basename(argv[0]));

	return ret;
}
