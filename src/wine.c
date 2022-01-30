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
#include <assert.h>

#include "system.h"
#include "debug.h"


#define TYPE "wine"
#define BUFFER 2048


static int lunionplay_exist_wine(const GString* path, const int arch)
{
	assert(path != NULL);
	assert(arch != 32 || arch != 64);

	int ret;
	GString* wine = NULL;

	wine = g_string_new(path->str);
	if (wine->str[wine->len - 1] != '/')
		g_string_append(wine, "/");

	switch(arch)
	{
		case 32:
			g_string_append(wine, "bin/wine");
			break;
		case 64:
			g_string_append(wine, "bin/wine64");
			break;
		default:
			break;
	}
	TRACE(__FILE__, __FUNCTION__, "GString [ \"%s\", %d ]\n", wine->str, wine->len);

	ret = lunionplay_exist_path(wine->str, TRUE);
	g_string_free(wine, TRUE);
	TRACE(__FILE__, __FUNCTION__, "int %d\n", ret);

	return ret;
}


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


int lunionplay_valid_wine_dir(const GString* winedir)
{
	assert(winedir != NULL);

	int wow64 = 0;
	GString* winepath = NULL;
	GString* path = NULL;

	/* WOW64 or not */
	if (lunionplay_exist_wineboot(winedir) != 0)
	{
		ERR(TYPE, "Invalid Wine directory.\n");
		return -1;
	}

	if (lunionplay_exist_wine(winedir, 32) != 0)
	{
		if (lunionplay_exist_wine(winedir, 64) == 0)
			wow64 = 1;
		else
		{
			ERR(TYPE, "No such file wine or wine64.\n");
			return -1;
		}
	}

	if (getenv("PATH") == NULL)
	{
		ERR(TYPE, "No $PATH detected.\n");
		return -1;
	}

	winepath = g_string_new(winedir->str);
	path = g_string_new(getenv("PATH"));

	g_string_append(winepath, "/bin:");
	g_string_append(winepath, path->str);
	setenv("PATH", winepath->str, 1);

	g_string_free(winepath, TRUE);
	g_string_free(path, TRUE);

	return wow64;
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

	if (getenv("LUNIONPLAY_LOG") == NULL && getenv("WINEDEBUG") == NULL)
		setenv("WINEDEBUG", "-all", 1);
}


/* https://github.com/doitsujin/dxvk/blob/552b29ca0c6ddcf595c8610095034249cf972643/setup_dxvk.sh#L63 */
/* wineboot is always present in all wine build */
GString* lunionplay_set_wine_version(const GString* winedir, const int wow64)
{
	assert(winedir != NULL);

	GString* version = NULL;
	GString* cmd = NULL;

	cmd = g_string_new("wine");

	if (wow64 == 1)
		g_string_append(cmd, "64");
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
	char* wboot[] = {"wineboot", NULL};
	char* wserv[] = {"wineserver", "-w", NULL};

	/* Save actual values */
	if (getenv("WINEDLLOVERRIDES") != NULL)
		dll = g_string_new(getenv("WINEDLLOVERRIDES"));
	if (getenv("WINEDEBUG") != NULL)
		dbg = g_string_new(getenv("WINEDEBUG"));

	setenv("WINEDLLOVERRIDES", "mscoree,mshtml,winemenubuilder.exe=", 1);
	setenv("WINEDEBUG", "-all", 1);

	lunionplay_run_process(wboot[0], wboot);
	lunionplay_run_process(wserv[0], wserv);

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
