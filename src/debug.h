/*
 * debug.h
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


#ifndef __DEBUG__
#define __DEBUG__

#define ERR(n, ...)   lunionplay_printf(stderr, "err", n, __VA_ARGS__)
#define INFO(n, ...)  lunionplay_printf(stdout, "info", n, __VA_ARGS__)
#define WARN(n, ...)  lunionplay_printf(stdout, "warn", n, __VA_ARGS__)

#define TRACE(n, ...) lunionplay_printf_trace(n, __VA_ARGS__)


#include <glib.h>
#include <glib/gstdio.h>


int lunionplay_debug_mode(void);


void lunionplay_printf(FILE* stream,
					   const gchar* level,
					   const gchar* type,
					   const gchar* format, ...);


void lunionplay_printf_trace(const gchar* file,
							 const gchar* fct,
							 const gchar* format, ...);


#endif
