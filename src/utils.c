/*
 * utils.c
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


#include "utils.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <fcntl.h>

#include "ini.h"
#include "debug.h"


static void lunionplay_insert_env(const char* name, const char* value, const char* separator, const int pos)
{
	g_assert(name != NULL);
	g_assert(value != NULL);

	const char* env = NULL;

	env = g_getenv(name);
	if (env != NULL)
	{
		GString* tmp = g_string_new(env);
		switch (pos)
		{
			case 1:
				if (separator != NULL)
					g_string_append(tmp, separator);
				g_string_append(tmp, value);
				break;
			case -1:
				if (separator != NULL)
					g_string_prepend(tmp, separator);
				g_string_prepend(tmp, value);
				break;
		}

		if (g_setenv(name, tmp->str, TRUE) == FALSE)
			ERR(NULL, "$%s Couldn't be set.\n", name);

		g_string_free(tmp, TRUE);
	}
}


void lunionplay_append_env(const char* name, const char* value, const char* separator)
{
	g_assert(name != NULL);
	g_assert(value != NULL);

	lunionplay_insert_env(name, value, separator, 1);
}


GString* lunionplay_concat_path(const GString* path, const gchar* val)
{
	g_assert(path != NULL);
	g_assert(val != NULL);

	gchar* tmp = NULL;
	GString* rslt = NULL;

	tmp = g_build_path("/", path->str, val, NULL);
	TRACE(__FILE__, __func__, "gchar \"%s\"\n", tmp);

	if (tmp != NULL)
	{
		if (g_file_test(tmp, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))
		{
			rslt = g_string_new(tmp);
			TRACE(__FILE_NAME__, __func__, "GString [ \"%s\" (%d) ]\n", rslt->str, rslt->len);
		}

		g_free(tmp);
	}

	return rslt;
}


gchar* lunionplay_get_output_cmd(const gchar* cmd)
{
	g_assert(cmd != NULL);

	gchar buffer[BUFFSIZE];
	gchar* str = NULL;
	FILE* fp = NULL;
	GString* contents = NULL;

	/* TODO Need use glib function */
	fp = popen(cmd, "r");
	if (fp == NULL)
	{
		ERR(NULL, "Error to open a pipe stream.\n");
		return NULL;
	}

	fgets(buffer, BUFFSIZE, fp);
	pclose(fp);

	/* TODO Don't use GString */
	contents = g_string_new(buffer);

	if (contents->str[contents->len - 1] == '\n')
		g_string_truncate(contents, contents->len - 1);

	if (contents->len <= 1)
	{
		g_string_free(contents, TRUE);
		contents = NULL;
	}
	else
	{
		str = g_string_free(contents, FALSE);
	}

	return str;
}


gchar* lunionplay_get_kernel(void)
{
	gchar* kernel = NULL;
	struct utsname buffer;

	if (uname(&buffer))
	{
		kernel = g_strjoin(" ", buffer.sysname, buffer.release,
		                   buffer.version, buffer.machine, NULL);
	}

	return kernel;
}


gchar* lunionplay_get_user_config_dir(void)
{
	gchar* path = NULL;
	const gchar* env = g_getenv("XDG_CONFIG_HOME");

	if (env != NULL)
	{
		path = strdup(env);
	}
	else
	{
		path = g_build_path("/", g_getenv("HOME"), ".config", NULL);
	}

	TRACE(__FILE__, __func__, "gchar* \"%s\"\n", path);

	return path;
}


static void lunionplay_log_file(const char* logfile)
{
	if (logfile != NULL)
	{
		TRACE(__FILE__, __func__, "\"%s\"\n", logfile);

		int fd = open(logfile, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);

		if (fd != -1)
		{
			dup2(fd, 1);
			dup2(fd, 2);
			close(fd);
		}
	}
}


void lunionplay_prepend_env(const char* name, const char* value, const char* separator)
{
	g_assert(name != NULL);
	g_assert(value != NULL);

	lunionplay_insert_env(name, value, separator, -1);
}


int lunionplay_run_process(const char* cmd, char* argv[])
{
	g_assert(cmd != NULL);
	g_assert(argv != NULL);

	int status;
	pid_t child;

	TRACE(__FILE__, __func__, "char* \"%s\"\n", cmd);
	for (char** tmp = argv; *tmp != NULL; tmp++)
	{
		TRACE(__FILE__, __func__, "char* \"%s\"\n", *tmp);
	}


	child = fork();
	switch(child)
	{
		case -1:
			perror("fork");
			return -1;
		case 0:
			lunionplay_log_file(getenv("LUNIONPLAY_LOG_FILE"));
			execvp(cmd, argv);
			exit(EXIT_FAILURE);
		default:
			break;
	}

	waitpid(child, &status, 0);
	return 0;
}
