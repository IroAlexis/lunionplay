/*
 * game.c
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


#include "game.h"


#define TYPE "game"


struct _lunion_play_game
{
	gchar* title;
	gchar* dir;
	gchar* command;
};


/*
 * Free member per member
 */
static void lunionplay_game_free_param(gchar* param)
{
	if (param != NULL)
	{
		g_free(param);
		param = NULL;
	}
}


static LunionPlayGame* lunionplay_game_new(gchar* title,
                                           gchar* dir,
                                           gchar* command)
{
	g_assert(dir != NULL);
	g_assert(command != NULL);

	LunionPlayGame* self = NULL;

	self = (LunionPlayGame*) g_malloc0(sizeof(LunionPlayGame));
	if (self != NULL)
	{
		self->title = title;
		self->dir = dir;
		self->command = command;
	}

	return self;
}


static gchar* lunionplay_game_set_command(const gchar* path)
{
	g_assert(path != NULL);

	gchar* exec = NULL;
	g_autofree gchar* file = NULL;

	/* TODO Need read the second argument */
	file = g_build_path("/", path, "gamestart", NULL);

	if (file != NULL && g_file_test(file, G_FILE_TEST_EXISTS))
	{
		gsize size;
		gchar** split;
		g_autofree gchar* contents;
		g_autoptr (GError) error = NULL;

		TRACE(__FILE__, __func__, "gchar* [ \"%s\" ]\n", file);

		g_file_get_contents(file, &contents, &size, &error);

		split = g_strsplit(contents, "\n", -1);
		for (int id = 0; NULL == exec && split[id] != NULL; id++)
		{
			if (g_str_has_suffix(split[id], ".exe") &&
			    g_file_test(split[id], G_FILE_TEST_EXISTS))
			{
				exec = strdup(split[id]);
			}
		}
		TRACE(__FILE__, __func__, "gchar* [ \"%s\" ]\n", exec);

		g_strfreev(split);
	}

	return exec;
}


static gchar* lunionplay_game_set_title(const gchar* path)
{
	g_assert(path != NULL);

	return NULL;
}


LunionPlayGame* lunionplay_game_create(GKeyFile* cfg, const gchar* id)
{
	gchar key[] = "install_dir";
	gchar* title = NULL;
	gchar* path = NULL;
	gchar* command = NULL;
	LunionPlayGame* self = NULL;

	path = lunionplay_config_get(cfg, key, TRUE);
	if (path != NULL)
	{
		gchar* new = NULL;

		new = g_build_path("/", path, id, NULL);
		if (new != NULL)
		{
			g_free(path);
			path = new;
			new = NULL;
		}
	}
	else
	{
		ERR(TYPE, "No library directory found.\n");
		return NULL;
	}

	command = lunionplay_game_set_command(path);
	if (command != NULL)
	{
		title = lunionplay_game_set_title(path);

		self = lunionplay_game_new(title, path, command);
		if (NULL == self)
		{
			ERR(TYPE, "No game found: %s\n", path);
		}
	}
	else
	{
		ERR(TYPE, "No such file or directory: %s\n", command);
		g_free(command);
		g_free(path);
	}

	return self;
}


void lunionplay_game_free(LunionPlayGame* self)
{
	g_assert(self != NULL);

	lunionplay_game_free_param(self->title);
	lunionplay_game_free_param(self->dir);
	lunionplay_game_free_param(self->command);

	g_free(self);
	self = NULL;
}


const gchar* lunionplay_game_get_command(const LunionPlayGame* self)
{
	g_assert(self != NULL);

	return self->command;
}


const gchar* lunionplay_game_get_dir(const LunionPlayGame* self)
{
	g_assert(self != NULL);

	return self->dir;
}


const gchar* lunionplay_game_get_title(const LunionPlayGame* self)
{
	g_assert(self != NULL);

	const gchar* pt = NULL;

	if (self->title != NULL)
	{
		pt = self->title;
	}

	return pt;
}

