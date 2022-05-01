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
#include <assert.h>
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


gchar* lunionplay_get_stdout(const gchar* cmd)
{
	g_assert(cmd != NULL);

	gint wait_status;
	gchar* buffer = NULL;
	gchar* str = NULL;
	g_autoptr (GError) error = NULL;

	g_spawn_command_line_sync(cmd, &buffer, NULL, &wait_status, &error);

	str = g_strndup(buffer, (gsize) strlen(buffer) - 1)
	g_free(buffer);

	return rslt;
}


GString* lunionplay_get_uname(void)
{
	struct utsname buffer;
	GString* kernel = NULL;

	if (uname(&buffer) != 0)
		return NULL;

	kernel = g_string_new(buffer.sysname);
	if (NULL == kernel)
		return NULL;

	g_string_append(kernel, " ");
	g_string_append(kernel, buffer.release);
	g_string_append(kernel, " ");
	g_string_append(kernel, buffer.version);
	g_string_append(kernel, " ");
	g_string_append(kernel, buffer.machine);

	return kernel;
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


gboolean lunionplay_run_proc(const gchar* workdir, gchar** argv, const gboolean s_out, const gboolean s_err)
{
	g_assert(argv != NULL);

	gint wait_status;
	gboolean ret;
	GSpawnFlags flags;
	g_autoptr (GError) err = NULL;

	/* TODO For later need rework log application
	for (char** tmp = argv; *tmp != NULL; tmp++)
		TRACE(__FILE__, __FUNCTION__, "\"%s\"\n", *tmp);
	*/

	flags = G_SPAWN_SEARCH_PATH_FROM_ENVP;

	if (! s_out)
		flags = flags | G_SPAWN_STDOUT_TO_DEV_NULL;
	if (! s_err)
	flags = flags | G_SPAWN_STDERR_TO_DEV_NULL;

	ret = g_spawn_sync(workdir, argv, NULL,
	                   flags, NULL, NULL,
	                   NULL, NULL,
	                   &wait_status, &err);

	return ret;
}

