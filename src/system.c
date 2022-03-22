/*
 * system.c
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


#include "system.h"

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


#define TYPE "system"


int lunionplay_exist_path(const char* path, const int boolean)
{
	assert (path != NULL);

	int ret;
	struct stat statbuf;

	TRACE(__FILE__, __FUNCTION__, "\"%s\"\n", path);

	ret = stat(path, &statbuf);
	if (ret != 0 && boolean == TRUE)
		ERR(TYPE, "%s: No such file or directory.\n", path);

	return ret;
}


GString* lunionplay_get_output_cmd(const char* cmd)
{
	assert(cmd != NULL);

	char buffer[BUFFSIZE];
	FILE* fp = NULL;
	GString* output = NULL;

	fp = popen(cmd, "r");
	if (fp == NULL)
	{
		ERR(TYPE, "Error to open a pipe stream.\n");
		return NULL;
	}

	fgets(buffer, BUFFSIZE, fp);
	pclose(fp);

	output = g_string_new(buffer);

	if (output->str[output->len - 1] == '\n')
		g_string_truncate(output, output->len - 1);

	return output;
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


int lunionplay_run_process(const char* cmd, char* argv[])
{
	assert(cmd != NULL);
	assert(argv != NULL);

	int status;
	pid_t child;

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


GString* lunionplay_valid_path(const char* path, const char* str)
{
	assert(path != NULL);

	GString* new = NULL;

	new = g_string_new(path);
	if (NULL == new)
	{
		ERR(TYPE, "Allocation problem.\n");
		return NULL;
	}

	if (str != NULL)
		g_string_append(new, str);

	if (lunionplay_exist_path(new->str, FALSE) != 0)
	{
		g_string_free(new, TRUE);
		return NULL;
	}

	return new;
}
