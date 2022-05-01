/*
 * ini.c
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


#include "ini.h"

#include "utils.h"
#include "debug.h"


#define TYPE "ini"


GKeyFile* lunionplay_open_ini(const gchar* pathname)
{
	GKeyFile* stream = NULL;
	GString* cfg = NULL;
	GError* error = NULL;

	if (NULL == pathname)
	{
		cfg = g_string_new(lunionplay_get_user_config_dir());
		g_string_append(cfg, "/lunionplay/config.ini");
	}
	else
		cfg = g_string_new(pathname);

	TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %d ]\n", cfg->str, cfg->len);

	/* Prevent memory leak with g_key_file_load_from_file */
	if (g_file_test(cfg->str, G_FILE_TEST_EXISTS))
	{
		INFO(TYPE, "%s\n", cfg->str);

		stream = g_key_file_new();
		if (! g_key_file_load_from_file(stream, cfg->str, G_KEY_FILE_NONE, &error))
		{
			g_clear_error(&error);
			g_clear_pointer(&stream, g_key_file_free);
		}
	}

	if (stream == NULL)
		INFO(TYPE, "No such user configuration file.\n");

	g_string_free(cfg, TRUE);
	return stream;
}


gchar* lunionplay_parse_ini(GKeyFile* stream, const gchar* group, const gchar* name)
{
	g_assert(stream != NULL);
	g_assert(group != NULL);
	g_assert(name != NULL);

	gchar* value = NULL;
	GError* error = NULL;

	if (g_key_file_has_group(stream, group))
	{
		if (g_key_file_has_key(stream, group, name, &error))
			value = g_key_file_get_string(stream, group, name, &error);
		else
			ERR(TYPE, "Key \"%s\" not found.\n", name);
	}
	else
		ERR(TYPE, "Group \"%s\" not found.\n", group);

	if (error != NULL)
		g_clear_error(&error);

	return value;
}
