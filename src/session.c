/*
 * session.c
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


#include "session.h"


#define TYPE "session"


static gint lunionplay_wait_continue()
{
	char buffer;

	fprintf(stdout, "Press enter to continue. (q to quit) ");
	fscanf(stdin, "%c", &buffer);

	if ('q' == buffer)
		return 1;

	return 0;
}


gboolean lunionplay_prepare_session(const LunionPlayWine* wine,
                                    const LunionPlayGame* game)
{
	g_assert(wine != NULL);
	g_assert(game != NULL);

	gboolean status = TRUE;
	const gchar* game_dir = NULL;
	const gchar* game_title = NULL;

	INFO("wine", "version: %s\n", lunionplay_wine_get_version(wine));
	INFO("game", "command: %s\n", lunionplay_game_get_command(game));

	game_dir = lunionplay_game_get_dir(game);
	game_title = lunionplay_game_get_title(game);

	lunionplay_wine_setup_prefix(game_dir);

	/* TODO Rework waiting confirmation
	if (waiting != NULL)
		if (g_strcmp0(waiting, "true") == 0 && getenv("LUNIONPLAY_LOG_FILE") == NULL)
			if (lunionplay_wait_continue() == 1)
				return -1;
	*/

	if (game_title != NULL)
		INFO(NULL, "Preparing to launch %s...\n", game_title);
	else
		INFO(NULL, "Preparing to launch...\n");

	lunionplay_wine_update_prefix(wine);

	lunionplay_wine_setup_runtime(wine);
	lunionplay_setup_dxvk_runtime(game_dir);
	lunionplay_setup_vkd3d_proton_runtime(game_dir);

	return status;
}


gint lunionplay_run_session(const LunionPlayWine* wine,
                            const LunionPlayGame* game)
{
	g_assert(wine != NULL);
	g_assert(game != NULL);

	gchar* dir = NULL;
	gchar* exec = NULL;
	gchar** argv = NULL;

	argv = (gchar**) g_malloc0(sizeof(gchar*) * 3);
	if (NULL == argv)
		return -1;

	dir = g_path_get_dirname(lunionplay_game_get_command(game));
	exec = g_path_get_basename(lunionplay_game_get_command(game));

	TRACE(__FILE__, __func__, "\"%s\"\n", dir);
	TRACE(__FILE__, __func__, "\"%s\"\n", exec);

	argv[0] = g_strdup(lunionplay_wine_get_bin(wine));
	argv[1] = g_strdup(exec);
	argv[2] = NULL;

	INFO(NULL, "Starting...\n");

	lunionplay_run_proc(dir, argv, TRUE, TRUE);
	lunionplay_wine_use_server(wine, "-w");

	for (char** tmp = argv; *tmp != NULL; tmp++)
		g_free(*tmp);
	g_free(argv);

	g_free(dir);
	g_free(exec);

	return 0;
}
