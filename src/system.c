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
#include "env.h"
#include "debug.h"

#include <glib.h>
#include <sys/stat.h>
#include <assert.h>


#define TYPE "system"


char* lunionplay_default_place()
{
	char* tmp = NULL;
	GString* path = NULL;

	path = g_string_new(lunionplay_get_envar("HOME"));
	g_string_append(path, "/Games");

	tmp = strdup(path->str);
	g_string_free(path, TRUE);

	return tmp;
}


int lunionplay_exist_path(const char* pathname)
{
	assert (pathname != NULL);

	int ret;
	struct stat statbuf;

	TRACE(__FILE__, __FUNCTION__, "pathname '%s'\n", pathname);

	ret = stat(pathname, &statbuf);
	if (ret != 0)
		ERR(TYPE, "%s: No such file or directory\n", pathname);

	return ret;
}
