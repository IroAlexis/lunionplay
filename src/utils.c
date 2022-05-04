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

#include "debug.h"


static void lunionplay_insert_env(const char* name, const char* value, const char* separator, const int pos)
{
	g_assert(name != NULL);
	g_assert(value != NULL);

	const char* pt = NULL;

	pt = g_getenv(name);
	if (pt != NULL)
	{
		GString* tmp = g_string_new(pt);
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


GString* lunionplay_clean_path(GString* path)
{
	assert(path != NULL);

	if (path != NULL && path->str[path->len - 1] == '/')
		g_string_truncate(path, path->len - 1);

	return path;
}


GString* lunionplay_concat_path(const GString* path, const char* val)
{
	g_assert(path != NULL);
	g_assert(val != NULL);

	GString* tmp = NULL;

	tmp = g_string_new(path->str);
	if (tmp->str[tmp->len - 1] != '/')
		g_string_append (tmp, "/");
	g_string_append(tmp, val);

	if (g_file_test(tmp->str, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))
		TRACE(__FILE__, __FUNCTION__, "[ \"%s\", %d ]\n", tmp->str, tmp->len);
	else
	{
		g_string_free(tmp, TRUE);
		tmp = NULL;
	}

	return tmp;
}


GString* lunionplay_get_output_cmd(const char* cmd)
{
	assert(cmd != NULL);

	char buffer[BUFFSIZE];
	FILE* fp = NULL;
	GString* out = NULL;

	fp = popen(cmd, "r");
	if (fp == NULL)
	{
		ERR(NULL, "Error to open a pipe stream.\n");
		return NULL;
	}

	fgets(buffer, BUFFSIZE, fp);
	pclose(fp);

	out = g_string_new(buffer);

	if (out->str[out->len - 1] == '\n')
		g_string_truncate(out, out->len - 1);

	if (out->len <= 1)
	{
		g_string_free(out, TRUE);
		out = NULL;
	}

	return out;
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
		TRACE(__FILE__, __FUNCTION__, "\"%s\"\n", logfile);

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
	assert(cmd != NULL);
	assert(argv != NULL);

	int status;
	pid_t child;

	TRACE(__FILE__, __FUNCTION__, "\"%s\"\n", cmd);
	for (char** tmp = argv; *tmp != NULL; tmp++)
		TRACE(__FILE__, __FUNCTION__, "\"%s\"\n", *tmp);


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
