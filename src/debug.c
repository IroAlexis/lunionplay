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

#define SIZE 1024


gint lunionplay_debug_mode(void)
{
	const gchar* value = NULL;
	gint ret = 0;

	value = g_getenv("LUNIONPLAY_DEBUG");
	if (value != NULL)
	{
		if (g_strcmp0(value, "1") == 0)
		{
			ret = 1;
		}

		if (g_strcmp0(value, "2") == 0)
		{
			ret = 2;
		}
	}

	return ret;
}


void lunionplay_printf(FILE* stream,
                       const gchar* level,
                       const gchar* type,
                       const gchar* format, ...)
{
	gchar buffer[SIZE];
	va_list args;

	va_start(args, format);
	g_vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if (type == NULL)
		g_fprintf(stream, "%s:: %s", level, buffer);
	else
		g_fprintf(stream, "%s::%s: %s", level, type, buffer);
}


void lunionplay_printf_trace(const gchar* file,
                             const gchar* fct,
                             const gchar* format, ...)
{
	if (lunionplay_debug_mode() == 2)
	{
		gchar buffer[SIZE];
		va_list args;

		va_start(args, format);
		g_vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		g_fprintf(stderr, " -> %s@%s: %s", file, fct, buffer);
	}
}
