/*
 * config.c
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


#include "config.h"

#include "debug.h"
#include "utils.h"


#define TYPE "config"


static gchar* lunionplay_config_parse(GKeyFile* stream,
                                      const gchar* group,
                                      const gchar* name,
                                      const gboolean errmsg)
{
	g_assert(stream != NULL);
	g_assert(group != NULL);
	g_assert(name != NULL);

	gchar* value = NULL;
	g_autoptr (GError) error = NULL;

	if (g_key_file_has_group(stream, group))
	{
		if (g_key_file_has_key(stream, group, name, &error))
		{
			value = g_key_file_get_string(stream, group, name, &error);
		}
		else if (errmsg)
		{
			ERR(TYPE, "Key \"%s\" not found.\n", name);
		}
	}
	else
	{
		ERR(TYPE, "Group \"%s\" not found.\n", group);
	}

	TRACE(__FILE__, __func__, "%s\n", value);

	return value;
}


gchar* lunionplay_config_get(GKeyFile* stream,
                             const gchar* group,
                             const gchar* key,
                             const gboolean errmsg)
{
	g_assert(key != NULL);

	gchar* value = NULL;

	value = lunionplay_config_get_env(key);
	if (NULL == value && stream != NULL)
	{
		value = lunionplay_config_parse(stream, group, key, errmsg);
	}

	return value;
}


gchar* lunionplay_config_get_env(const gchar* param)
{
	g_assert(param != NULL);

	gchar* strup = NULL;
	gchar* name = NULL;
	gchar* value = NULL;

	strup = g_ascii_strup(param, strlen(param));
	name = g_strconcat("LUNIONPLAY_", strup, NULL);
	if (name != NULL)
	{
		value = g_strdup(g_getenv(name));
		TRACE(__FILE__, __func__, "%s=%s\n", name, value);

		g_free(name);
	}

	g_free(strup);

	return value;
}


GKeyFile* lunionplay_config_open(void)
{
	gchar* cfg = NULL;
	const gchar* env = NULL;
	GKeyFile* stream = NULL;
	g_autoptr (GError) error = NULL;

	env = g_getenv("LUNIONPLAY_CONFIGFILE");
	if (NULL == env)
	{
		g_autofree gchar* path = lunionplay_get_user_config_dir();

		cfg = g_build_path("/", path, "lunionplay", "config.ini", NULL);
	}
	else
	{
		cfg = g_strdup(env);
	}

	/* Prevent memory leak with g_key_file_load_from_file */
	if (g_file_test(cfg, G_FILE_TEST_EXISTS))
	{
		INFO(TYPE, "%s\n", cfg);

		stream = g_key_file_new();
		if (! g_key_file_load_from_file(stream, cfg, G_KEY_FILE_NONE, &error))
		{
			g_clear_pointer(&stream, g_key_file_free);
		}
	}

	if (stream == NULL)
	{
		INFO(TYPE, "No such user configuration file.\n");
	}

	g_free(cfg);

	return stream;
}

