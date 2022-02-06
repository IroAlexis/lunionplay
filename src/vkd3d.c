/*
 * vkd3d.c
 * Copyright (C) 2022 IroAlexis <iroalexis@outlook.fr>
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


#include "vkd3d.h"

#include <stdlib.h>


void lunionplay_set_vkd3d_proton_env(void)
{
	if (getenv("LUNIONPLAY_LOG") != NULL)
		setenv("VKD3D_DEBUG", "warn", 0);
	else
		setenv("VKD3D_DEBUG", "none", 0);
}
