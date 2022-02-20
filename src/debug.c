/*
 * debug.c
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


#include "debug.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define SIZE 1024


int lunionplay_debug_mode(void)
{
	char* value = NULL;
	int ret = 0;

	value = getenv("LUNIONPLAY_DEBUG");
	if (value != NULL)
	{
		if (strncmp(value, "1", 1) == 0)
			ret = 1;

		if (strncmp(value, "2", 1) == 0)
			ret = 2;
	}

	return ret;
}


void lunionplay_printf(FILE* stream,
					   const char* level,
					   const char* type,
					   const char* format, ...)
{
	char buffer[SIZE];
	va_list args;

	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if (type == NULL)
		fprintf(stream, "> %s:: %s", level, buffer);
	else
		fprintf(stream, "> %s::%s: %s", level, type, buffer);
}


void lunionplay_printf_trace(const char* file,
							 const char* fct,
							 const char* format, ...)
{
	if (lunionplay_debug_mode() == 2)
	{
		char buffer[SIZE];
		va_list args;

		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		fprintf(stderr, " -> %s@%s: %s", file, fct, buffer);
	}
}
