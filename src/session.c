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
#include <assert.h>

#include "config.h"
#include "system.h"
#include "wine.h"
#include "game.h"
#include "debug.h"


#define TYPE "session"


struct _lunion_play_session
{
	GKeyFile* stream;
	GString* winedir;
	GString* winever;
	int wow64;
	GString* gamedir;
	GString* command;
};


void lunionplay_free_session(LunionPlaySession* session)
{
	assert(session != NULL);

	g_clear_pointer(&(session->stream), g_key_file_free);

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


void lunionplay_display_session(const LunionPlaySession* game)
{
	if (lunionplay_debug_mode())
	{
		fprintf(stderr, " ->\n");

		if (game != NULL)
		{
			fprintf(stderr, " -> LunionPlaySession:\n");

			if (game->winedir != NULL)
				fprintf(stderr, " ->  * winedir: \"%s\" (%ld)\n", game->winedir->str, game->winedir->len);
			else
				fprintf(stderr, " ->  * winedir: %p\n", NULL);

			if (game->winever != NULL)
				fprintf(stderr, " ->  * winever: \"%s\" (%ld)\n", game->winever->str, game->winever->len);
			else
				fprintf(stderr, " ->  * winever: %p\n", NULL);

			if (game->wow64 == TRUE)
				fprintf(stderr, " ->  * wow64: TRUE\n");
			else
				fprintf(stderr, " ->  * wow64: FALSE\n");

			if (game->gamedir != NULL)
				fprintf(stderr, " ->  * gamedir: \"%s\" (%ld)\n", game->gamedir->str, game->gamedir->len);
			else
				fprintf(stderr, " ->  * gamedir: %p\n", NULL);

			if (game->command != NULL)
				fprintf(stderr, " ->  * command: \"%s\" (%ld)\n", game->command->str, game->command->len);
			else
				fprintf(stderr, " ->  * command: %p\n", NULL);
		}

		fprintf(stderr, " ->\n");
	}
}


LunionPlaySession* lunionplay_init_session(const char* gameid, const char* exec, int* error)
{
	int ret = EXIT_FAILURE;
	char* value = NULL;
	LunionPlaySession* session = NULL;

	session = (LunionPlaySession*) calloc(1, sizeof(LunionPlaySession));
	if (NULL == session)
	{
		*error = ret;
		return NULL;
	}

	session->stream = g_key_file_new();
	ret = lunionplay_open_config(&(session->stream), getenv("LUNIONPLAY_CONFIGFILE"));
	if (ret == EXIT_FAILURE)
	{
		lunionplay_close_config(&(session->stream));
		session->stream = NULL;
	}

	/* Search Wine */
	value = lunionplay_get_app_setting(session->stream, "wine");
	if (value == NULL)
	{
		ERR(TYPE, "No Wine detected.\n");
		*error = EXIT_FAILURE;
		return session;
	}

	session->winedir = g_string_new(value);
	free(value);
	value = NULL;

	if (session->winedir->str[session->winedir->len - 1] != '/')
		g_string_append(session->winedir, "/");

	ret = lunionplay_init_wine(session->winedir, &(session->winever), &(session->wow64));
	if (ret != EXIT_SUCCESS)
	{
		lunionplay_display_session(session);
		*error = EXIT_FAILURE;
		return session;
	}


	/* Search directory */
	value = lunionplay_get_app_setting(session->stream, "default_dir");
	if (value == NULL)
	{
		lunionplay_display_session(session);
		*error = EXIT_FAILURE;
		return session;
	}

	session->gamedir = g_string_new(value);
	free(value);
	value = NULL;

	ret = lunionplay_init_game(&(session->gamedir), &(session->command), gameid, exec);
	if (ret != EXIT_SUCCESS)
	{
		lunionplay_display_session(session);
		*error = EXIT_FAILURE;
		return session;
	}

	lunionplay_display_session(session);
	*error = EXIT_SUCCESS;
	return session;
}


char* lunionplay_get_app_setting(GKeyFile* stream, const char* name)
{
	char* value = NULL;
	GString* env = NULL;

	env = g_string_new("LUNIONPLAY_");
	if (env != NULL)
	{
		char* tmp = g_ascii_strup(name, strnlen(name, 1024));

		env = g_string_append(env, tmp);
		TRACE(__FILE__, __FUNCTION__, "%s=%s\n", env->str, getenv(env->str));
		if (getenv(env->str) != NULL)
			value = strndup(getenv(env->str), strnlen(getenv(env->str), 2048));

		free(tmp);
	}
	else
		ERR(TYPE, "Unexpected error with runtime configuration option.");

	g_string_free(env, TRUE);

	if (value == NULL && stream != NULL)
		value = lunionplay_parse_config(stream, "lunionplay", name);

	return value;
}


int lunionplay_setup_session(LunionPlaySession* session)
{
	assert(session != NULL);

	char* value = NULL;

	lunionplay_set_wine_prefix(session->gamedir);

	/* Disable by default */
	value = lunionplay_get_app_setting(session->stream, "confirm");
	TRACE(__FILE__, __FUNCTION__, "%p\n", value);
	/*
	if (lunionplay_wait_continue(value))
		return EXIT_FAILURE;
	*/

	INFO(TYPE, "Preparing to launch the game...\n");
	lunionplay_update_wine_prefix();
	/*lunionplay_set_wine_env();*/

	if (value != NULL)
		free(value);

	return EXIT_SUCCESS;
}
