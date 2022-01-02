/*
 * system.c
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


#include "system.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/utsname.h>

#include "debug.h"


#define LEN_MAX 4096
#define TYPE "system"


int lunionplay_exist_path(const char* path, const int msg)
{
	assert (path != NULL);

	int ret;
	struct stat statbuf;

	TRACE(__FILE__, __FUNCTION__, "\"%s\"\n", path);

	ret = stat(path, &statbuf);
	if (ret != 0 && msg == TRUE)
		ERR(TYPE, "%s: No such file or directory.\n", path);

	return ret;
}


GString* lunionplay_get_absolut_path(const char* path)
{
	char buff[PATH_MAX];
	char* tmp = NULL;

	/* TODO More feedback about error */
	tmp = realpath(path, buff);
	if (tmp == NULL)
		return NULL;

	return g_string_new(tmp);

}


GString* lunionplay_get_output_cmd(const char* cmd)
{
	assert(cmd != NULL);

	char tmp[LEN_MAX];
	FILE* fp = NULL;
	GString* output = NULL;

	fp = popen(cmd, "r");
	if (fp == NULL)
	{
		ERR(TYPE, "Error to open a pipe stream.\n");
		return NULL;
	}

	fgets(tmp, LEN_MAX, fp);
	fclose(fp);

	output = g_string_new(tmp);

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


int lunionplay_run_process(const char* cmd, char* const argv[])
{
	assert(cmd != NULL);
	assert(argv != NULL);

	int status;
	pid_t child;
	pid_t pid;

	child = fork();
	switch(child)
	{
		case -1:
			perror("fork");
			return -1;
		case 0:
			execvp(cmd, argv);
			exit(EXIT_FAILURE);
		default:
			break;
	}

	pid = waitpid(child, &status, 0);

	return 0;
}
