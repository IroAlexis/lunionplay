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

#include <glib/gstdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unistd.h>


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


static gboolean lunionplay_redirect_log()
{
	gboolean ret = FALSE;
	const gchar* filename = g_getenv("LUNIONPLAY_LOG_FILE");

	if (filename != NULL)
	{
		g_autoptr (GError) err = NULL;
		gint fd = g_open(filename, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);

		if (fd != -1)
		{
			dup2(fd, 1);
			dup2(fd, 2);

			ret = g_close(fd, &err);
		}
	}

	return ret;
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
	g_autofree gchar* std_out = NULL;
	g_autofree gchar* std_err = NULL;
	g_autoptr (GError) error = NULL;

	g_spawn_command_line_sync(cmd, &std_out, &std_err, &wait_status, &error);
	if (error != NULL)
	{
		g_error_free(error);
	}

	g_spawn_check_wait_status(wait_status, &error);

	return g_strndup(std_out, (gsize) strlen(std_out) - 1);
}


gchar* lunionplay_get_uname(void)
{
	struct utsname buffer;

	if (uname(&buffer) != 0)
		return NULL;

	return g_strconcat(buffer.sysname,
	                   buffer.release,
	                   buffer.version,
	                   buffer.machine,
	                   NULL);
}


void lunionplay_prepend_env(const char* name, const char* value, const char* separator)
{
	g_assert(name != NULL);
	g_assert(value != NULL);

	lunionplay_insert_env(name, value, separator, -1);
}


gboolean lunionplay_run_proc(const gchar* workdir,
                             gchar** argv,
                             const gboolean _stdout,
                             const gboolean _stderr)
{
	g_assert(argv != NULL);

	gint wait_status;
	GSpawnFlags flags;
	g_autoptr (GError) error = NULL;

	for (char** tmp = argv; *tmp != NULL; tmp++)
		TRACE(__FILE__, __func__, "\"%s\"\n", *tmp);

	flags = G_SPAWN_CHILD_INHERITS_STDIN;
	if (! _stdout) flags = flags | G_SPAWN_STDOUT_TO_DEV_NULL;
	if (! _stderr) flags = flags | G_SPAWN_STDERR_TO_DEV_NULL;


	g_spawn_sync(workdir, argv, NULL,
	             flags,
	             NULL, NULL,
	             NULL, NULL,
	             &wait_status,
	             &error);

	if (error != NULL)
		g_error_free(error);

	return g_spawn_check_wait_status(wait_status, &error);
}

