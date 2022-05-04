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

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <glib/gstdio.h>
#include <assert.h>

#include "ini.h"
#include "utils.h"
#include "wine.h"
#include "dxvk.h"
#include "vkd3d.h"
#include "debug.h"


#define TYPE "session"


struct _lunion_play_session
{
	GKeyFile* stream;
	gchar* waiting;

	LunionPlayWine* wine;

	GString* gametitle;
	GString* gameid;
	GString* gamedir;
	GString* command;
};


static void lunionplay_display_session_struct(const char* name, const GString* value)
{
	assert(name != NULL);

	if (value != NULL)
		fprintf(stderr, " ->  * %s: \"%s\" (%ld)\n", name, value->str, value->len);
	else
		fprintf(stderr, " ->  * %s: %p\n", name, NULL);
}


static GString* lunionplay_set_gamedir(const char* path, const char* gameid)
{
	assert(path != NULL);
	assert(gameid != NULL);

	GString* gamedir = NULL;

	gamedir = g_string_new(path);
	if (NULL == gamedir)
		return NULL;

	g_string_append(gamedir, "/");
	if (NULL == gamedir)
		return NULL;

	g_string_append(gamedir, gameid);
	if (gamedir != NULL)
	{
		TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %d ]\n", gamedir->str, gamedir->len);

		if (! g_file_test(gamedir->str, G_FILE_TEST_IS_DIR))
		{
			ERR(TYPE, "%s: No such game id.\n", gameid);
			g_string_free(gamedir, TRUE);
			gamedir = NULL;
		}
	}

	return gamedir;
}


static GString* lunionplay_set_command(const GString* gamedir, const char* exec)
{
	assert(gamedir != NULL);

	GString* bin = NULL;

	if (exec == NULL)
	{
		GString* gamestart = g_string_new(gamedir->str);
		if (gamestart != NULL)
		{
			g_string_append(gamestart, "/");
			if (gamestart != NULL)
				g_string_append(gamestart, "gamestart");
		}

		TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %d ]\n", gamestart->str, gamestart->len);
		if (gamestart != NULL && g_file_test(gamestart->str, G_FILE_TEST_EXISTS))
		{
			char buffer[BUFFSIZE];
			FILE* stream = fopen(gamestart->str, "r");
			if (NULL == stream)
				return NULL;

			fgets(buffer, BUFFSIZE, stream);
			fclose(stream);

			bin = g_string_new(buffer);
			if (bin != NULL)
				if (bin->str[bin->len - 1] == '\n')
					g_string_truncate(bin, bin->len - 1);
		}

		g_string_free(gamestart, TRUE);
	}
	else
		bin = g_string_new(exec);

	return bin;
}


static int lunionplay_wait_continue()
{
	char buffer;

	fprintf(stdout, "Press enter to continue. (q to quit) ");
	fscanf(stdin, "%c", &buffer);

	if ('q' == buffer)
		return 1;

	return 0;
}


void lunionplay_free_session(LunionPlaySession* session)
{
	assert(session != NULL);

	g_clear_pointer(&(session->stream), g_key_file_free);

	if (session->waiting != NULL)
		g_free(session->waiting);

	if (session->wine != NULL)
		lunionplay_wine_free(session->wine);

	if (session->gametitle != NULL)
		g_string_free(session->gametitle, TRUE);

	if (session->gameid != NULL)
		g_string_free(session->gameid, TRUE);

	if (session->gamedir != NULL)
		g_string_free(session->gamedir, TRUE);

	if (session->command != NULL)
		g_string_free(session->command, TRUE);

	free(session);
}


void lunionplay_display_session(const LunionPlaySession* session)
{
	assert(session != NULL);

	if (lunionplay_debug_mode())
	{
		fprintf(stderr, " ->\n");

		fprintf(stderr, " -> LunionPlaySession:\n");

		if (session->stream != NULL)
			fprintf(stderr, " ->  * stream: (loaded)\n");
		else
			fprintf(stderr, " ->  * stream: %p\n", NULL);

		fprintf(stderr, " ->  * waiting: %s\n", session->waiting);
		lunionplay_wine_print(session->wine);
		lunionplay_display_session_struct("gametitle", session->gametitle);
		lunionplay_display_session_struct("gameid", session->gameid);
		lunionplay_display_session_struct("gamedir", session->gamedir);
		lunionplay_display_session_struct("command", session->command);

		fprintf(stderr, " ->\n");
	}
}


LunionPlaySession* lunionplay_init_session(const char* gameid, const char* exec)
{
	gchar* dir = NULL;
	LunionPlaySession* session = NULL;

	session = (LunionPlaySession*) calloc(1, sizeof(LunionPlaySession));
	if (NULL == session)
		return NULL;

	session->stream = lunionplay_open_ini(getenv("LUNIONPLAY_CONFIGFILE"));

	/* Search Wine */
	dir = lunionplay_get_param(session->stream, "wine");
	if (dir == NULL)
	{
		dir = g_get_current_dir();
		if (dir == NULL)
		{
			lunionplay_free_session(session);
			return NULL;
		}

		g_strconcat(dir, "/dist", NULL);
		if (! g_file_test(dir, G_FILE_TEST_IS_DIR))
		{
			g_free(dir);
			dir = NULL;
		}
	}

	if (dir != NULL)
	{
		session->wine = lunionplay_wine_create(dir);
		g_free(dir);
		dir = NULL;
	}
	else
		session->wine = lunionplay_wine_system_create();

	if (session->wine == NULL)
	{
		lunionplay_free_session(session);
		return NULL;
	}


	/* Search directory */
	dir = lunionplay_get_param(session->stream, "default_dir");
	if (dir != NULL)
	{
		session->gamedir = lunionplay_set_gamedir(dir, gameid);
		if (session->gamedir != NULL)
			session->command = lunionplay_set_command(session->gamedir, exec);

		g_free(dir);

		if (session->gamedir == NULL || session->command == NULL)
		{
			lunionplay_free_session(session);
			return NULL;
		}
	}
	else
	{
		ERR(TYPE, "No games location detected.\n");
		lunionplay_free_session(session);
		return NULL;
	}

	/* Copy gameid */
	session->gameid = g_string_new(gameid);

	/* For waiting confirmation */
	session->waiting = lunionplay_get_param(session->stream, "waiting");

	lunionplay_display_session(session);
	return session;
}


int lunionplay_prepare_session(const LunionPlaySession* session)
{
	assert(session != NULL);

	lunionplay_wine_setup_prefix(session->gamedir);

	if (session->waiting != NULL)
		if (g_strcmp0(session->waiting, "true") == 0 && getenv("LUNIONPLAY_LOG_FILE") == NULL)
			if (lunionplay_wait_continue() == 1)
				return -1;

	if (session->gametitle != NULL)
		INFO(NULL, "Preparing to launch %s...\n", session->gametitle->str);
	else
		INFO(NULL, "Preparing to launch...\n");
	lunionplay_wine_update_prefix(session->wine);

	lunionplay_wine_setup_runtime(session->wine);

	if (lunionplay_dxvk_installed())
		lunionplay_setup_dxvk_runtime(session->gamedir);

	if (lunionplay_vkd3d_proton_installed())
		lunionplay_setup_vkd3d_proton_runtime(session->gamedir);

	return 0;
}


int lunionplay_run_session(const LunionPlaySession* session)
{
	assert(session != NULL);

	char* dir = NULL;
	char* exec = NULL;
	char** argv = NULL;

	argv = (char**) calloc(3, sizeof(char*));
	if (NULL == argv)
		return -1;

	dir = g_path_get_dirname(session->command->str);
	exec = g_path_get_basename(session->command->str);

	TRACE(__FILE__, __FUNCTION__, "\"%s\"\n", dir);
	TRACE(__FILE__, __FUNCTION__, "\"%s\"\n", exec);

	argv[0] = g_path_get_basename(lunionplay_wine_get_bin(session->wine));
	argv[1] = g_strdup(exec);
	argv[2] = NULL;

	INFO(NULL, "Starting...\n");

	g_chdir(dir);
	lunionplay_run_process(lunionplay_wine_get_bin(session->wine), argv);
	lunionplay_wine_use_server(session->wine, "-w");

	for (char** tmp = argv; *tmp != NULL; tmp++)
		free(*tmp);
	free(argv);

	free(dir);
	free(exec);

	return 0;
}
