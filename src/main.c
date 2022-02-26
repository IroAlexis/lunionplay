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


#include "config.h"
#include "debug.h"
#include "driver.h"
#include "game.h"
#include "session.h"
#include "wine.h"

#include <stdio.h>
#include <glib.h>


int main(int argc, char* argv[])
{
	gint status = 0;
	g_autofree gchar* bin = NULL;
	g_autofree gchar* bin_dir = NULL;
	g_autoptr (GKeyFile) cfg = NULL;
	LunionPlayWine* wine = NULL;
	LunionPlayGame* game = NULL;

	lunionplay_driver_get();

	status = -1;
	bin = g_path_get_basename(argv[0]);
	bin_dir = g_path_get_dirname(argv[0]);

	//lunionplay_check_opts(argc, argv);
	cfg = lunionplay_config_open();

	wine = lunionplay_wine_create(cfg);
	game = lunionplay_game_create(cfg, argv[1]);

	if (game != NULL && wine != NULL)
	{
		if (lunionplay_prepare_session(wine, game))
		{
			status = lunionplay_run_session(wine, game);
		}
	}

	if (status != 0)
	{
		ERR(bin, "There's a frog somewhere... *ribbit*\n");
	}

	if (game != NULL)
	{
		lunionplay_game_free(game);
	}
	if (wine != NULL)
	{
		lunionplay_wine_free(wine);
	}

	return status;
}

