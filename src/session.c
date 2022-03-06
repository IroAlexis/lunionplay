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
#include "system.h"
#include "wine.h"
#include "dxvk.h"
#include "vkd3d.h"
#include "debug.h"


#define TYPE "session"


struct _lunion_play_session
{
	GKeyFile* stream;
	GString* waiting;

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
	if (gamedir->str[gamedir->len - 1] != '/')
		g_string_append(gamedir, "/");
	g_string_append(gamedir, gameid);

	TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %d ]\n", gamedir->str, gamedir->len);

	if (lunionplay_exist_path(gamedir->str, FALSE) != 0)
	{
		ERR(TYPE, "%s: No such game id.\n", gameid);
		g_string_free(gamedir, TRUE);
		gamedir = NULL;
	}

	return gamedir;
}


static GString* lunionplay_set_command(const GString* gamedir, const char* exec)
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
		if (lunionplay_exist_path(gamestart->str, FALSE) == 0)
		{
			char buffer[BUFFSIZE];
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
	else
		ERR(TYPE, "No such executable game.\n");

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
		g_string_free(session->waiting, TRUE);

	if (session->wine != NULL)
		lunionplay_free_wine(session->wine);

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

		lunionplay_display_session_struct("waiting", session->waiting);
		lunionplay_display_wine(session->wine);
		lunionplay_display_session_struct("gametitle", session->gametitle);
		lunionplay_display_session_struct("gameid", session->gameid);
		lunionplay_display_session_struct("gamedir", session->gamedir);
		lunionplay_display_session_struct("command", session->command);

		fprintf(stderr, " ->\n");
	}
}


LunionPlaySession* lunionplay_init_session(const char* gameid, const char* exec)
{
	GString* dir = NULL;
	LunionPlaySession* session = NULL;

	session = (LunionPlaySession*) calloc(1, sizeof(LunionPlaySession));
	if (NULL == session)
		return NULL;

	session->stream = lunionplay_open_ini(getenv("LUNIONPLAY_CONFIGFILE"));

	/* Search Wine */
	dir = lunionplay_get_app_setting(session->stream, "wine");
	if (dir == NULL)
	{
		char* pwd = g_get_current_dir();

		dir = g_string_new(pwd);
		free(pwd);

		if (dir == NULL)
		{
			lunionplay_free_session(session);
			return NULL;
		}

		g_string_append(dir, "/dist");
		if (lunionplay_exist_path(dir->str, FALSE) != 0)
		{
			ERR(TYPE, "No Wine directory detected.\n");
			g_string_free(dir, TRUE);
			lunionplay_free_session(session);
			return NULL;
		}
	}
	if (dir->str[dir->len - 1] == '/')
		g_string_truncate(dir, dir->len - 1);

	session->wine = lunionplay_init_wine(dir);
	g_string_free(dir, TRUE);

	if (session->wine == NULL)
	{
		lunionplay_free_session(session);
		return NULL;
	}


	/* Search directory */
	dir = lunionplay_get_app_setting(session->stream, "default_dir");
	if (dir != NULL)
	{
		if (dir->str[dir->len - 1] == '/')
			g_string_truncate(dir, dir->len - 1);

		session->gamedir = lunionplay_set_gamedir(dir->str, gameid);
		if (session->gamedir != NULL)
			session->command = lunionplay_set_command(session->gamedir, exec);

		g_string_free(dir, TRUE);

		if (session->gamedir == NULL || session->command == NULL)
		{
			lunionplay_free_session(session);
			return NULL;
		}
	}
	else
	{
		ERR(TYPE, "No games directory detected.\n");
		lunionplay_free_session(session);
		return NULL;
	}

	/* Copy gameid */
	session->gameid = g_string_new(gameid);

	/* For waiting confirmation */
	session->waiting = lunionplay_get_app_setting(session->stream, "waiting");

	lunionplay_display_session(session);
	return session;
}


GString* lunionplay_get_app_setting(GKeyFile* stream, const char* name)
{
	assert(name != NULL);

	char* tmp = NULL;
	GString* value = NULL;
	GString* env = NULL;

	env = g_string_new("LUNIONPLAY_");
	if (env != NULL)
	{
		tmp = g_ascii_strup(name, strnlen(name, 1024));

		env = g_string_append(env, tmp);
		TRACE(__FILE__, __FUNCTION__, "%s=%s\n", env->str, getenv(env->str));

		if (getenv(env->str) != NULL)
			value = g_string_new(getenv(env->str));

		free(tmp);
		tmp = NULL;
	}
	else
		ERR(TYPE, "Allocation problem.");

	g_string_free(env, TRUE);

	if (value == NULL && stream != NULL)
	{
		tmp = lunionplay_parse_ini(stream, "lunionplay", name);
		if (tmp != NULL)
		{
			value = g_string_new(tmp);
			free(tmp);
			tmp = NULL;
		}
	}

	return value;
}


int lunionplay_prepare_session(const LunionPlaySession* session)
{
	assert(session != NULL);

	lunionplay_setup_wineprefix(session->gamedir);

	if (session->waiting != NULL)
		if (g_strcmp0(session->waiting->str, "true") == 0 && getenv("LUNIONPLAY_LOG_FILE") == NULL)
			if (lunionplay_wait_continue() == 1)
				return -1;

	if (session->gametitle != NULL)
		INFO(NULL, "Preparing to launch %s...\n", session->gametitle->str);
	else
		INFO(NULL, "Preparing to launch the game...\n");
	lunionplay_update_wineprefix();

	lunionplay_setup_wine_runtime();
	lunionplay_setup_dxvk_runtime(session->gamedir);
	lunionplay_setup_vkd3d_proton_runtime(session->gamedir);

	return 0;
}


int lunionplay_run_session(const LunionPlaySession* session)
{
	assert(session != NULL);

	char* dir = NULL;
	char* wine = NULL;
	char* exec = NULL;
	char** argv = NULL;

	argv = (char**) calloc(3, sizeof(char*));
	if (NULL == argv)
		return -1;

	dir = g_path_get_dirname(session->command->str);
	exec = g_path_get_basename(session->command->str);

	TRACE(__FILE__, __FUNCTION__, "\"%s\"\n", dir);
	TRACE(__FILE__, __FUNCTION__, "\"%s\"\n", exec);

	wine = lunionplay_get_wine_bin(session->wine);
	if (wine != NULL)
		argv[0] = strndup(wine, strnlen(wine, 1024));
	else
		return -1;

	argv[1] = strndup(exec, strnlen(exec, 128));
	argv[2] = NULL;

	INFO(NULL, "Starting...\n");

	g_chdir(dir);
	lunionplay_run_process(argv[0], argv);
	lunionplay_use_wineserver("-w");

	for (char** tmp = argv; *tmp != NULL; tmp++)
		free(*tmp);
	free(argv);

	free(dir);
	free(exec);

	return 0;
}
