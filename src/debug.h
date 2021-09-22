/*
 * debug.h
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


#ifndef __DEBUG__
#define __DEBUG__

#define ERR(n, ...)   lunionplay_debug_printf(stderr, "err", n, __VA_ARGS__)
#define INFO(n, ...)  lunionplay_debug_printf(stdout, "info", n, __VA_ARGS__)
#define WARN(n, ...)  lunionplay_debug_printf(stdout, "warn", n, __VA_ARGS__)
#define TRACE(n, ...) lunionplay_trace_printf(stderr, n, __VA_ARGS__)

/* FIXME Draft */
#define NOEXIST 1
#define NOPARSE 2


#include <stdio.h>


void lunionplay_debug_printf(FILE* stream,
							 const char* level,
							 const char* type,
							 const char* format, ...);


void lunionplay_trace_printf(FILE* stream,
							 const char* file,
							 const char* fct,
							 const char* format, ...);


#endif
