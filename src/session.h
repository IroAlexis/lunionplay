/*
 * session.h
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


#ifndef __SESSION__
#define __SESSION__


#include "config.h"
#include "debug.h"
#include "dxvk.h"
#include "game.h"
#include "utils.h"
#include "vkd3d.h"

#include "wine.h"

#include <glib.h>


typedef struct _lunion_play_session LunionPlaySession;


gint lunionplay_prepare_session(const LunionPlayWine* wine,
                                const LunionPlayGame* game);


gint lunionplay_run_session(const LunionPlayWine* wine,
                            const LunionPlayGame* game);


#endif
