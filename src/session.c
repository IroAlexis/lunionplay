/*
 * session.c
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


#include "session.h"

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <glib/gstdio.h>
#include <assert.h>

#include "ini.h"
#include "system.h"
#include "wine.h"
#include "game.h"
#include "debug.h"


#define TYPE "session"


struct _lunion_play_session
{
	GKeyFile* stream;
	GString* waiting;

	GString* winedir;
	int wow64;
	GString* winever;

	GString* gamedir;
	GString* command;
};


static int lunionplay_wait_continue()
{
	char buffer;

	fprintf(stdout, "Press enter to continue. (q to quit) ");
	fscanf(stdin, "%c", &buffer);

	TRACE (__FILE__, __FUNCTION__, "\"%s\"\n", buffer);

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

	if (session->winedir != NULL)
		g_string_free(session->winedir, TRUE);

	if (session->winever != NULL)
		g_string_free(session->winever, TRUE);

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

		if (session->waiting != NULL)
			fprintf(stderr, " ->  * waiting: \"%s\" (%ld)\n", session->waiting->str, session->waiting->len);
		else
			fprintf(stderr, " ->  * waiting: %p\n", NULL);

		if (session->winedir != NULL)
			fprintf(stderr, " ->  * winedir: \"%s\" (%ld)\n", session->winedir->str, session->winedir->len);
		else
			fprintf(stderr, " ->  * winedir: %p\n", NULL);

		fprintf(stderr, " ->  * wow64: %d\n", session->wow64);

		if (session->winever != NULL)
			fprintf(stderr, " ->  * winever: \"%s\" (%ld)\n", session->winever->str, session->winever->len);
		else
			fprintf(stderr, " ->  * winever: %p\n", NULL);

		if (session->gamedir != NULL)
			fprintf(stderr, " ->  * gamedir: \"%s\" (%ld)\n", session->gamedir->str, session->gamedir->len);
		else
			fprintf(stderr, " ->  * gamedir: %p\n", NULL);

		if (session->command != NULL)
			fprintf(stderr, " ->  * command: \"%s\" (%ld)\n", session->command->str, session->command->len);
		else
			fprintf(stderr, " ->  * command: %p\n", NULL);

		fprintf(stderr, " ->\n");
	}
}


LunionPlaySession* lunionplay_init_session(const char* gameid, const char* exec, int* error)
{
	GString* dir = NULL;
	LunionPlaySession* session = NULL;

	session = (LunionPlaySession*) calloc(1, sizeof(LunionPlaySession));
	if (NULL == session)
	{
		*error = -1;
		return NULL;
	}

	*error = 0;
	session->stream = lunionplay_open_ini(getenv("LUNIONPLAY_CONFIGFILE"));

	/* Search Wine */
	session->winedir = lunionplay_get_app_setting(session->stream, "wine");
	if (session->winedir != NULL)
	{
		session->wow64 = lunionplay_valid_wine_dir(session->winedir);
		if (session->wow64 == 0 || session->wow64 == 1)
			session->winever = lunionplay_set_wine_version(session->winedir, session->wow64);

		if (session->winever == NULL)
			*error = 2;
	}
	else
	{
		ERR(TYPE, "No Wine detected.\n");
		*error = 1;
		return session;
	}


	/* Search directory */
	dir = lunionplay_get_app_setting(session->stream, "default_dir");
	if (dir != NULL)
	{
		session->gamedir = lunionplay_set_game_dir(dir->str, gameid);
		if (session->gamedir != NULL)
			session->command = lunionplay_set_command(session->gamedir, exec);

		g_string_free(dir, TRUE);

		if (session->gamedir == NULL || session->command == NULL)
			*error = 4;
	}
	else
	{
		ERR(TYPE, "No games directory detected.\n");
		*error = 3;
		return session;
	}

	/* Waiting confirmation */
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
		ERR(TYPE, "Unexpected error with runtime configuration option.");

	g_string_free(env, TRUE);

	if (value == NULL && stream != NULL)
	{
		tmp = lunionplay_parse_ini(stream, "lunionplay", name);
		value = g_string_new(tmp);

		free(tmp);
		tmp = NULL;
	}

	return value;
}


int lunionplay_run_game(const LunionPlaySession* session)
{
	assert(session != NULL);

	char* dir = NULL;
	char* exec = NULL;
	char** argv = NULL;
	char* wserv[] = {"wineserver", "-w", NULL};

	argv = (char**) calloc(3, sizeof(char*));
	if (NULL == argv)
		return -1;

	dir = g_path_get_dirname(session->command->str);
	exec = g_path_get_basename(session->command->str);

	argv[0] = strdup("wine");
	argv[1] = strdup(exec);
	argv[2] = NULL;
	g_chdir(dir);

	INFO(TYPE, "Starting...\n");
	lunionplay_run_process(argv[0], argv);

	INFO(TYPE, "Waiting all wine processes terminate...\n");
	lunionplay_run_process(wserv[0], wserv);

	free(argv[1]);
	free(argv[0]);
	free(argv);
	free(dir);
	free(exec);

	return 0;
}


int lunionplay_setup_session(LunionPlaySession* session)
{
	assert(session != NULL);

	lunionplay_set_wine_prefix(session->gamedir);

	if (g_strcmp0(session->waiting->str, "true") == 0 && getenv ("LUNIONPLAY_LOG_FILE") == NULL)
		if (lunionplay_wait_continue() == 1)
			return 2;

	INFO(TYPE, "Preparing to launch the game...\n");
	lunionplay_update_wine_prefix();

	lunionplay_set_wine_env();
	lunionplay_set_dxvk_env(session->gamedir);
	lunionplay_set_vkd3d_proton_env(session->gamedir);

	return 0;
}
