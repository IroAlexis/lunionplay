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
#include <stdlib.h>
#include <glib.h>

#include "core.h"
#include "env.h"
#include "config.h"
#include "debug.h"


#define TYPE "main"


int main(int argc, char* argv[])
{
	int ret = EXIT_FAILURE;
	Session* game = NULL;

	if (argc == 2)
		game = lunionplay_init_session(argv[1], &ret);

	if (game != NULL)
	{
		TRACE(__FILE__, __FUNCTION__, "gamedir '%s'\n", game->gamedir);
		TRACE(__FILE__, __FUNCTION__, "winedir '%s'\n", game->winedir);
		TRACE(__FILE__, __FUNCTION__, "winever '%s'\n", game->winever);
	}

	lunionplay_free_session(&game);
	return ret;
}
