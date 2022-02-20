/*
 * wine.c
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


#include "wine.h"

#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

#include "system.h"
#include "debug.h"


#define TYPE "wine"


struct _lunion_play_wine
{
	GString* base_dir;
	GString* bin_dir;
	GString* lib32_dir;
	GString* lib64_dir;
	GString* bin;
	GString* bin64;
	GString* version;
	gboolean wow64;
};


static GString* lunionplay_compose_wine_path(const char* path, const char* str)
{
	assert(path != NULL);

	GString* new = NULL;

	new = g_string_new(path);
	if (NULL == new)
	{
		ERR(TYPE, "Allocation problem.\n");
		return NULL;
	}
	g_string_append(new, str);
	if (lunionplay_exist_path(new->str, FALSE) != 0)
	{
		g_string_free(new, TRUE);
		return NULL;
	}

	return new;
}


static void lunionplay_display_wine_struct(const char* name, const GString* value)
{
	assert(name != NULL);

		if (value != NULL)
			fprintf(stderr, " ->  * %s: \"%s\" (%ld)\n", name, value->str, value->len);
		else
			fprintf(stderr, " ->  * %s: %p\n", name, NULL);
}


static int lunionplay_exist_wineboot(const GString* path)
{
	assert (path != NULL);

	int ret;
	GString* wineboot = NULL;

	wineboot = g_string_new(path->str);
	g_string_append(wineboot, "/wineboot");

	ret = lunionplay_exist_path(wineboot->str, TRUE);
	g_string_free(wineboot, TRUE);

	return ret;
}


static GString* lunionplay_set_wine_version(const GString* winebin)
{
	assert(winebin != NULL);

	GString* version = NULL;
	GString* cmd = NULL;

	cmd = g_string_new(winebin->str);
	g_string_append(cmd, " --version");
	TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %ld ]\n", cmd->str, cmd->len);

	version = lunionplay_get_output_cmd(cmd->str);
	if (version->len == 0)
		ERR(TYPE, "Not a wine executable. Check your wine.\n");

	TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %ld ]\n", version->str, version->len);
	if (version != NULL)
		INFO(TYPE, "version: %s\n", version->str);

	g_string_free(cmd, TRUE);

	return version;
}


static void lunionplay_set_wine(const LunionPlayWine* wine)
{
	assert (wine != NULL);

	char* p_env = NULL;
	GString* path = NULL;
	GString* tmp = NULL;

	p_env = getenv("PATH");

	if (p_env != NULL)
	{
		path = g_string_new(p_env);
		tmp = g_string_new(wine->bin_dir->str);

		g_string_append(tmp, ":");
		g_string_append(tmp, path->str);
		setenv("PATH", tmp->str, 1);

		g_string_free(tmp, TRUE);
		g_string_free(path, TRUE);
	}
	else
		ERR(TYPE, "No $PATH detected.\n");

	p_env = getenv("LD_LIBRARY_PATH");

	if (p_env != NULL)
	{
		path = g_string_new(p_env);
		tmp = g_string_new(wine->lib64_dir->str);

		g_string_append(tmp, ":");
		g_string_append(tmp, wine->lib32_dir->str);
		g_string_append(tmp, ":");
		g_string_append(tmp, path->str);

		setenv("LD_LIBRARY_PATH", tmp->str, 1);

		g_string_free(tmp, TRUE);
		g_string_free(path, TRUE);
	}
	else
	{
		tmp = g_string_new(wine->lib64_dir->str);

		g_string_append(tmp, ":");
		g_string_append(tmp, wine->lib32_dir->str);

		setenv("LD_LIBRARY_PATH", tmp->str, 1);

		g_string_free(tmp, TRUE);
	}
}


static void lunionplay_set_wine_lib_dir(LunionPlayWine* wine)
{
	wine->lib32_dir = lunionplay_compose_wine_path(wine->base_dir->str, "/lib32");
	wine->lib64_dir = lunionplay_compose_wine_path(wine->base_dir->str, "/lib64");

	if (wine->lib64_dir == NULL && wine->lib32_dir != NULL)
		wine->lib64_dir = lunionplay_compose_wine_path(wine->base_dir->str, "/lib");
	else if (wine->lib64_dir != NULL && wine->lib32_dir == NULL)
		wine->lib32_dir = lunionplay_compose_wine_path(wine->base_dir->str, "/lib");
}


static int lunionplay_valid_wine_prefix(GString* winepfx)
{
	assert(winepfx != NULL);

	int ret;
	GString* s_reg = NULL;

	ret = lunionplay_exist_path(winepfx->str, FALSE);
	if (ret == 0)
	{
		s_reg = g_string_new(winepfx->str);

		if (s_reg->str[s_reg->len - 1] != '/')
			g_string_append(s_reg, "/");
		g_string_append(s_reg, "system.reg");

		TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %d ]\n", s_reg->str, s_reg->len);

		ret = lunionplay_exist_path(s_reg->str, FALSE);
		g_string_free(s_reg, TRUE);
	}

	return ret;
}


void lunionplay_display_env_wine(FILE* stream)
{
	fprintf(stream, "===========================================\n");
	fprintf(stream, " -> PATH=%s\n", getenv("PATH"));
	fprintf(stream, " -> LD_LIBRARY_PATH=%s\n", getenv("LD_LIBRARY_PATH"));
	fprintf(stream, " -> WINEPREFIX=%s\n", getenv("WINEPREFIX"));
	fprintf(stream, " -> WINEFSYNC=%s\n", getenv("WINEFSYNC"));
	fprintf(stream, " -> WINEESYNC=%s\n", getenv("WINEESYNC"));
	fprintf(stream, " -> WINEDLLOVERRIDES=%s\n", getenv("WINEDLLOVERRIDES"));
	fprintf(stream, " -> WINEDEBUG=%s\n", getenv("WINEDEBUG"));
	fprintf(stream, "===========================================\n");
}


void lunionplay_display_wine(const LunionPlayWine* wine)
{
	assert(wine != NULL);

	if (lunionplay_debug_mode())
	{
		fprintf(stderr, " ->\n");

		fprintf(stderr, " -> LunionPlayWine:\n");

		lunionplay_display_wine_struct("base_dir", wine->base_dir);
		lunionplay_display_wine_struct("bin_dir", wine->bin_dir);
		lunionplay_display_wine_struct("lib32_dir", wine->lib32_dir);
		lunionplay_display_wine_struct("lib64_dir", wine->lib64_dir);
		lunionplay_display_wine_struct("bin_dir", wine->bin_dir);
		lunionplay_display_wine_struct("bin", wine->bin);
		lunionplay_display_wine_struct("bin64", wine->bin64);
		lunionplay_display_wine_struct("version", wine->version);

		if (wine->wow64 == 0)
			fprintf(stderr, " ->  * wow64: FALSE\n");
		else
			fprintf(stderr, " ->  * wow64: TRUE\n");

		fprintf(stderr, " ->\n");
	}
}



void lunionplay_free_wine(LunionPlayWine* wine)
{
	assert(wine != NULL);

	if (wine->base_dir != NULL)
		g_string_free(wine->base_dir, TRUE);

	if (wine->bin_dir != NULL)
		g_string_free(wine->bin_dir, TRUE);

	if (wine->lib32_dir != NULL)
		g_string_free(wine->lib32_dir, TRUE);

	if (wine->lib64_dir != NULL)
		g_string_free(wine->lib64_dir, TRUE);

	if (wine->bin != NULL)
		g_string_free(wine->bin, TRUE);

	if (wine->bin64 != NULL)
		g_string_free(wine->bin64, TRUE);

	if (wine->version != NULL)
		g_string_free(wine->version, TRUE);

	free(wine);
}


char* lunionplay_get_wine_bin(LunionPlayWine* wine)
{
	assert(wine != NULL);

	if (wine->bin != NULL)
		return wine->bin->str;
	else
		return NULL;
}


LunionPlayWine* lunionplay_init_wine(const GString* winedir)
{
	assert(winedir != NULL);

	LunionPlayWine* wine = NULL;

	if (lunionplay_exist_path(winedir->str, TRUE) != 0)
		return NULL;

	wine = (LunionPlayWine*) calloc(1, sizeof(LunionPlayWine));
	if (NULL == wine)
	{
		ERR(TYPE, "Allocation problem.\n");
		return NULL;
	}

	wine->wow64 = TRUE;

	/* base directory */
	wine->base_dir = g_string_new(winedir->str);
	if (NULL == wine->base_dir)
	{
		ERR(TYPE, "Allocation problem.\n");
		lunionplay_free_wine(wine);
		return NULL;
	}

	/* binaries directory */
	wine->bin_dir = lunionplay_compose_wine_path(wine->base_dir->str, "/bin");
	if (NULL == wine->bin_dir)
	{
		ERR(TYPE, "Invalid Wine directory.\n");
		lunionplay_free_wine(wine);
		return NULL;
	}

	/* libraries directory */
	lunionplay_set_wine_lib_dir(wine);
	if (wine->lib32_dir == NULL && wine->lib64_dir == NULL)
	{
		ERR(TYPE, "Invalid Wine directory.\n");
		lunionplay_free_wine(wine);
		return NULL;
	}

	/*
	 * In such case, winebooot and wine64 will be present,
	 * but wine binary will be missing
	 */
	if (lunionplay_exist_wineboot(wine->bin_dir) != 0)
	{
		ERR(TYPE, "Invalid Wine directory.\n");
		lunionplay_free_wine(wine);
		return NULL;
	}

	/* wine binary */
	wine->bin = lunionplay_compose_wine_path(wine->bin_dir->str, "/wine");

	/* wine64 binary */
	wine->bin64 = lunionplay_compose_wine_path(wine->bin_dir->str, "/wine64");
	if (NULL == wine->bin64)
	{
		ERR(TYPE, "Invalid Wine directory.\n");
		lunionplay_free_wine(wine);
		return NULL;
	}
	if (wine->bin == NULL)
	{
		wine->bin = wine->bin64;
		wine->wow64 = FALSE;
	}

	/* wine version */
	wine->version = lunionplay_set_wine_version(wine->bin);
	if (NULL == wine->version)
	{
		ERR(TYPE, "Not a wine executable. Check your %s\n", wine->base_dir->str);
		lunionplay_free_wine(wine);
		return NULL;
	}

	lunionplay_set_wine(wine);

	return wine;
}


void lunionplay_set_wine_envar(void)
{
	GString* buffer = NULL;

	setenv("WINEFSYNC", "1", 0);
	/* fallback when fsync don't support */
	setenv("WINEESYNC", "1", 0);

	if (getenv("WINEDLLOVERRIDES") != NULL)
	{
		buffer = g_string_new(getenv("WINEDLLOVERRIDES"));
		g_string_append(buffer, ";winemenubuilder.exe=");

		setenv("WINEDLLOVERRIDES", buffer->str, 1);
		g_string_free(buffer, TRUE);
	}
	else
		setenv("WINEDLLOVERRIDES", "winemenubuilder.exe=", 0);

	if (getenv("LUNIONPLAY_LOG") == NULL)
		setenv("WINEDEBUG", "-all", 0);
	else
	{
		if (strcmp(getenv("LUNIONPLAY_LOG"), "1") == 0)
			setenv("WINEDEBUG", "fixme-all", 1);
		else if (strcmp(getenv("LUNIONPLAY_LOG"), "2") == 0)
			setenv("WINEDEBUG", "warn+seh", 0);
		else if (strcmp(getenv("LUNIONPLAY_LOG"), "3") == 0)
			setenv("WINEDEBUG", "+timestamp,+pid,+tid,+seh,+debugstr,+loaddll,+mscoree", 0);
	}
}


int lunionplay_set_wine_prefix(GString* gamedir)
{
	assert(gamedir != NULL);

	char* env = NULL;
	GString* winepfx = NULL;

	env = getenv("WINEPREFIX");
	if (env == NULL)
	{
		winepfx = g_string_new(gamedir->str);
		if (winepfx == NULL)
			return 1;

		if (winepfx->str[winepfx->len - 1] != '/')
			g_string_append(winepfx, "/");

		g_string_append(winepfx, "pfx");
		TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %d ]\n", winepfx->str, winepfx->len);
		setenv("WINEPREFIX", winepfx->str, 1);
	}
	else
		winepfx = g_string_new(env);

	if (lunionplay_valid_wine_prefix(winepfx) == 0)
		INFO(TYPE, "prefix: %s\n", winepfx->str);
	else
		ERR(TYPE, "%s: Not a valid wine prefix.\n", winepfx->str);

	g_string_free(winepfx, TRUE);
	return 0;
}


int lunionplay_update_wine_prefix(void)
{
	GString* dll = NULL;
	GString* dbg = NULL;
	char* wboot[] = {"wine", "wineboot", NULL};

	/* Save actual values */
	if (getenv("WINEDLLOVERRIDES") != NULL)
		dll = g_string_new(getenv("WINEDLLOVERRIDES"));
	if (getenv("WINEDEBUG") != NULL)
		dbg = g_string_new(getenv("WINEDEBUG"));

	setenv("WINEDLLOVERRIDES", "mscoree,mshtml,winemenubuilder.exe=", 1);
	setenv("WINEDEBUG", "-all", 1);

	lunionplay_run_process(wboot[0], wboot);
	lunionplay_use_wineserver("-w");

	unsetenv("WINEDLLOVERRIDES");
	unsetenv("WINEDEBUG");

	if (dll != NULL)
	{
		if (dll->len != 0)
			setenv("WINEDLLOVERRIDES", dll->str, 1);
		g_string_free(dll, TRUE);
	}
	if (dbg != NULL)
	{
		if (dbg->len != 0)
			setenv("WINEDEBUG", dbg->str, 1);
		g_string_free(dbg, TRUE);
	}

	return 0;
}


void lunionplay_use_wineserver(const char* option)
{
	assert(option != NULL);

	char** cmd = NULL;

	cmd = (char**) calloc(3, sizeof(char*));
	if (cmd != NULL)
	{
		cmd[0] = strndup("wineserver", strnlen("wineserver", 12));
		cmd[1] = strdup(option);
		cmd[2] = NULL;

		lunionplay_run_process(cmd[0], cmd);

		free(cmd[1]);
		free(cmd[0]);
	}

	free(cmd);
}
