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
	GString* title;
	GString* dir;
	GString* command;
};


/*
 * Free member per member
 */
static void lunionplay_game_free_param(GString* param)
{
	if (param != NULL)
	{
		g_string_free(param, TRUE);
		param = NULL;
	}
}


static LunionPlayGame* lunionplay_game_new(GString* title,
                                           GString* dir,
                                           GString* command)
{
	g_assert(dir != NULL);
	g_assert(command != NULL);

	LunionPlayGame* self = NULL;

	self = (LunionPlayGame*) calloc(1, sizeof(LunionPlayGame));
	if (self != NULL)
	{
		self->title = title;
		self->dir = dir;
		self->command = command;
	}

	return self;
}


static GString* lunionplay_game_set_command(const gchar* path)
{
	g_assert(path != NULL);

	GString* exec = NULL;
	gchar* file = NULL;

	/* TODO Need read the second argument */
	file = g_build_path("/", path, "gamestart", NULL);

	if (file != NULL && g_file_test(file, G_FILE_TEST_EXISTS))
	{
		gsize size;
		g_autofree gchar* contents;
		g_autoptr (GError) error = NULL;

		TRACE(__FILE__, __func__, "gchar* [ \"%s\" ]\n", file);

		g_file_get_contents(file, &contents, &size, &error);

		exec = g_string_new(contents);
		if (exec != NULL)
		{
			g_string_truncate(exec, exec->len - 1);
		}
	}

	TRACE(__FILE__, __func__, "GString [ \"%s\" (%d) ]\n", exec->str, exec->len);
	g_free(file);

	return exec;
}


static GString* lunionplay_game_set_title(const gchar* path)
{
	g_assert(path != NULL);

	return NULL;
}


LunionPlayGame* lunionplay_game_create(GKeyFile* cfg, const gchar* id)
{
	gchar key[] = "install_dir";
	g_autofree gchar* path =NULL;
	GString* title = NULL;
	GString* dir = NULL;
	GString* command = NULL;
	LunionPlayGame* self = NULL;

	if (cfg != NULL)
	{
		path = lunionplay_config_get(cfg, key);
	}
	else
	{
		path = lunionplay_config_get_env(key);
	}

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
	TRACE(__FILE__, __func__, "gchar* [ \"%s\" ]\n", path);

	dir = g_string_new(path);
	if (NULL == dir)
	{
		return NULL;
	}

	command = lunionplay_game_set_command(dir->str);
	if (command == NULL)
	{
		g_string_free(dir, TRUE);
		return NULL;
	}
	title = lunionplay_game_set_title(dir->str);

	self = lunionplay_game_new(title, dir, command);
	if (NULL == self)
	{
		ERR(TYPE, "No game found: %s\n", path);
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

	return self->command->str;
}


const gchar* lunionplay_game_get_dir(const LunionPlayGame* self)
{
	g_assert(self != NULL);

	return self->dir->str;
}


const gchar* lunionplay_game_get_title(const LunionPlayGame* self)
{
	g_assert(self != NULL);

	const gchar* pt = NULL;

	if (self->title != NULL && self->title->str != NULL)
	{
		pt = self->title->str;
	}

	return pt;
}

