/*
 * env.c
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


#include "env.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <assert.h>


#define TYPE "env"


char* lunionplay_get_envar(const char* name)
{
	assert(name != NULL);

	return getenv(name);
}


int lunionplay_set_envar(const char* name, const char* val)
{
	assert(name != NULL);

	int ret;
	errno = 0;

	ret = setenv(name, val, 1);
	if (ret == -1)
		ERR(TYPE, "%s\n", strerror(errno));

	return ret;
}


int lunionplay_unset_envar(const char* name)
{
	assert(name != NULL);

	int ret;
	errno = 0;

	ret = unsetenv(name);
	if (ret == -1)
		ERR(TYPE, "%s\n", strerror(errno));

	return ret;
}
