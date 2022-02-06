/*
 * wine.c
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
	GString* version;
	GString* wine_bin;
	GString* wine64_bin;
};


static int lunionplay_exist_wineboot(const GString* path)
{
	assert (path != NULL);

	int ret;
	GString* wineboot = NULL;

	wineboot = g_string_new(path->str);
	if (wineboot->str[wineboot->len - 1] != '/')
		g_string_append(wineboot, "/");
	g_string_append(wineboot, "bin/wineboot");

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


static void lunionplay_set_wine_path(const GString* bindir)
{
	assert (bindir != NULL);

	if (getenv("PATH") != NULL)
	{
		GString* path = NULL;
		GString* tmp = NULL;

		path = g_string_new(getenv("PATH"));
		tmp = g_string_new(bindir->str);

		g_string_append(tmp, ":");
		g_string_append(tmp, path->str);
		setenv("PATH", tmp->str, 1);

		g_string_free(tmp, TRUE);
		g_string_free(path, TRUE);
	}
	else
		ERR(TYPE, "No $PATH detected.\n");
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

		if (wine->base_dir != NULL)
			fprintf(stderr, " ->  * base_dir: \"%s\" (%ld)\n", wine->base_dir->str, wine->base_dir->len);
		else
			fprintf(stderr, " ->  * base_dir: %p\n", NULL);

		if (wine->bin_dir != NULL)
			fprintf(stderr, " ->  * bin_dir: \"%s\" (%ld)\n", wine->bin_dir->str, wine->bin_dir->len);
		else
			fprintf(stderr, " ->  * bin_dir: %p\n", NULL);

		if (wine->version != NULL)
			fprintf(stderr, " ->  * version: \"%s\" (%ld)\n", wine->version->str, wine->version->len);
		else
			fprintf(stderr, " ->  * version: %p\n", NULL);

		if (wine->wine_bin != NULL)
			fprintf(stderr, " ->  * wine_bin: \"%s\" (%ld)\n", wine->wine_bin->str, wine->wine_bin->len);
		else
			fprintf(stderr, " ->  * wine_bin: %p\n", NULL);

		if (wine->wine64_bin != NULL)
			fprintf(stderr, " ->  * wine64_bin: \"%s\" (%ld)\n", wine->wine64_bin->str, wine->wine64_bin->len);
		else
			fprintf(stderr, " ->  * wine64_bin: %p\n", NULL);

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

	if (wine->version != NULL)
		g_string_free(wine->version, TRUE);

	if (wine->wine_bin != NULL)
		g_string_free(wine->wine_bin, TRUE);

	if (wine->wine64_bin != NULL)
		g_string_free(wine->wine64_bin, TRUE);

	free(wine);
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

	/* base directory */
	wine->base_dir = g_string_new(winedir->str);
	if (NULL == wine->base_dir)
	{
		ERR(TYPE, "Allocation problem.\n");
		lunionplay_free_wine(wine);
		return NULL;
	}

	/* binaries directory */
	wine->bin_dir = g_string_new(wine->base_dir->str);
	if (NULL == wine->bin_dir)
	{
		ERR(TYPE, "Allocation problem.\n");
		lunionplay_free_wine(wine);
		return NULL;
	}
	g_string_append(wine->bin_dir, "/bin");
	if (lunionplay_exist_path(wine->bin_dir->str, FALSE) != 0)
	{
		ERR(TYPE, "Invalid Wine directory.\n");
		lunionplay_free_wine(wine);
		return NULL;
	}

	/*
	 * In such case, winebooot and wine64 will be present,
	 * but wine binary will be missing
	 */
	if (lunionplay_exist_wineboot(winedir) != 0)
	{
		ERR(TYPE, "Invalid Wine directory.\n");
		lunionplay_free_wine(wine);
		return NULL;
	}

	/* wine binary */
	wine->wine_bin = g_string_new(wine->bin_dir->str);
	if (NULL == wine->wine_bin)
	{
		ERR(TYPE, "Allocation problem.\n");
		lunionplay_free_wine(wine);
		return NULL;
	}
	g_string_append(wine->wine_bin, "/wine");
	if (lunionplay_exist_path(wine->wine_bin->str, FALSE) != 0)
	{
		g_string_free(wine->wine_bin, TRUE);
		wine->wine_bin = NULL;
	}

	/* wine64 binary */
	wine->wine64_bin = g_string_new(wine->bin_dir->str);
	if (NULL == wine->wine64_bin)
	{
		ERR(TYPE, "Allocation problem.\n");
		lunionplay_free_wine(wine);
		return NULL;
	}
	g_string_append(wine->wine64_bin, "/wine64");
	if (lunionplay_exist_path(wine->wine64_bin->str, FALSE) != 0)
	{
		ERR(TYPE, "Invalid Wine directory.\n");
		lunionplay_free_wine(wine);
		return NULL;
	}

	/* wine version */
	if (wine->wine_bin != NULL)
		wine->version = lunionplay_set_wine_version(wine->wine_bin);
	else if (wine->wine64_bin != NULL)
		wine->version = lunionplay_set_wine_version(wine->wine64_bin);
	else
	{
		ERR(TYPE, "Problem with your wine.");
		lunionplay_free_wine(wine);
		return NULL;
	}

	lunionplay_set_wine_path(wine->bin_dir);

	return wine;
}


void lunionplay_set_wine_env(void)
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
