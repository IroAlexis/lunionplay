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


#define TYPE "wine"


struct _lunion_play_wine
{
	gchar* base_dir;
	gchar* lib32_dir;
	gchar* lib64_dir;
	gchar* bin_dir;
	gchar* bin;
	gchar* bin64;
	gchar* server;
	gchar* version;
	gboolean wow64;
};


/*
 * Pure 64-bit Wine (non Wow64) requries skipping 32-bit steps.
 * In such case, wine64 and wineboot will be present,
 * but wine binary will be missing, however it can be present in other PATHs,
 * so it shouldn't be used, to avoid versions mixing.static
 */
static gboolean lunionplay_wine_is_wow64(gchar** bin64, gchar** bin)
{
	g_assert(*bin64 != NULL);

	gboolean wow64 = TRUE;

	if (NULL == *bin)
	{
		*bin = *bin64;
		wow64 = FALSE;
	}

	return wow64;
}


/* Only in case where we want to find wine system */
static gchar* lunionplay_wine_find_in_path()
{
	gchar* tmp = NULL;
	gchar* dir = NULL;

	tmp = g_find_program_in_path("wine");
	if (NULL == tmp)
		tmp = g_find_program_in_path("wine64");

	dir = g_path_get_dirname(tmp);
	g_free(tmp);

	return dir;
}


static void lunionplay_wine_free_param(gchar* param)
{
	if (param != NULL)
	{
		g_free(param);
		param = NULL;
	}
}


/*
 * In such case, there are only lib/lib64 or lib/lib32
 */
static void lunionplay_wine_lib_dir(const gchar* path, gchar** lib32, gchar** lib64)
{
	g_assert(path != NULL);

	if (NULL == *lib64 && *lib32 != NULL)
		*lib64 = g_build_path("/", path, "lib", NULL);
	else if (*lib64 != NULL && NULL == *lib32)
		*lib32 = g_build_path("/", path, "lib", NULL);
}


static void lunionplay_wine_print_property(const gchar* name, const gchar* value)
{
	g_assert(name != NULL);

	fprintf(stderr, " ->  * %s: \"%s\"\n", name, value);
}


static LunionPlayWine* lunionplay_wine_new(gchar* base_dir,
                                           gchar* lib32_dir,
                                           gchar* lib64_dir,
                                           gchar* bin_dir,
                                           gchar* bin,
                                           gchar* bin64,
                                           gchar* server,
                                           gchar* version,
                                           gboolean wow64)
{
	g_assert(bin != NULL);
	g_assert(bin64 != NULL);
	g_assert(server != NULL);
	g_assert(version != NULL);

	LunionPlayWine* self = NULL;

	self = (LunionPlayWine*) g_malloc0(sizeof(LunionPlayWine));
	if (self != NULL)
	{
		self->base_dir = base_dir;
		self->lib32_dir = lib32_dir;
		self->lib64_dir = lib64_dir;
		self->bin_dir = bin_dir;
		self->bin = bin;
		self->bin64 = bin64;
		self->server = server;
		self->version = version;
		self->wow64 = wow64;
	}

	return self;
}


static gchar* lunionplay_wine_set_version(const gchar* bin)
{
	g_assert(bin != NULL);

	gchar* version = NULL;
	gchar* cmd = NULL;

	cmd = g_strconcat(bin, " --version", NULL);

	version = lunionplay_get_stdout(cmd);
	if (NULL == version)
		ERR(TYPE, "Not a wine executable. Check your wine.\n");

	g_free(cmd);

	return version;
}


static void lunionplay_wine_setup_library_path(const LunionPlayWine* self)
{
	g_assert (self != NULL);

	if (g_getenv("LD_LIBRARY_PATH") == NULL)
		g_setenv("LD_LIBRARY_PATH", self->lib64_dir, TRUE);
	else
		lunionplay_append_env("LD_LIBRARY_PATH", self->lib64_dir, ":");
	lunionplay_append_env("LD_LIBRARY_PATH", self->lib32_dir, ":");
}


void lunionplay_wine_debug_runtime(FILE* stream)
{
	char* env[] = {"LD_LIBRARY_PATH",
	               "WINEPREFIX",
	               "WINEFSYNC",
	               "WINEESYNC",
	               "WINEDLLOVERRIDES",
	               "WINEDEBUG",
	               NULL};

	for (gint id = 0; env[id] != NULL; id++)
		fprintf(stream, " -> %s=%s\n", env[id], getenv(env[id]));
}


/*
 * factory method
 */
LunionPlayWine* lunionplay_wine_create(GKeyFile* config)
{
	gchar key[] = "wine";
	gchar* base_dir = NULL;
	gchar* lib32_dir = NULL;
	gchar* lib64_dir = NULL;
	gchar* bin_dir = NULL;
	gchar* bin = NULL;
	gchar* bin64 = NULL;
	gchar* server = NULL;
	gchar* version = NULL;
	gboolean wow64;
	LunionPlayWine* self = NULL;

	base_dir = lunionplay_config_get(config, key);
	if (NULL == base_dir)
	{
		return lunionplay_wine_system_create();
	}

	lib32_dir = g_build_path("/", base_dir, "lib32", NULL);
	lib64_dir = g_build_path("/", base_dir, "lib64", NULL);
	lunionplay_wine_lib_dir(base_dir, &lib32_dir, &lib64_dir);

	bin_dir = g_build_path("/", base_dir, "bin", NULL);
	bin = g_build_path("/", bin_dir, "wine", NULL);
	bin64 = g_build_path("/", bin_dir, "wine64", NULL);
	server = g_build_path("/", bin_dir, "wineserver", NULL);
	wow64 = lunionplay_wine_is_wow64(&bin64, &bin);

	version = lunionplay_wine_set_version(bin);

	self = lunionplay_wine_new(base_dir, lib32_dir, lib64_dir, bin_dir,
	                           bin, bin64, server, version, wow64);
	if (NULL == self)
	{
		ERR(TYPE, "Not a valid Wine directory.\n");
	}

	return self;
}


LunionPlayWine* lunionplay_wine_system_create(void)
{
	gchar* bin_dir = NULL;
	gchar* bin = NULL;
	gchar* bin64 = NULL;
	gchar* server = NULL;
	gchar* version = NULL;
	gboolean wow64 = TRUE;
	LunionPlayWine* self = NULL;

	bin_dir = lunionplay_wine_find_in_path();
	bin = g_build_path("/", bin_dir, "wine", NULL);
	bin64 = g_build_path("/", bin_dir, "wine64", NULL);
	server = g_build_path("/", bin_dir, "wineserver", NULL);
	wow64 = lunionplay_wine_is_wow64(&bin64, &bin);

	version = lunionplay_wine_set_version(bin);

	self = lunionplay_wine_new(NULL, NULL, NULL, bin_dir,
	                           bin, bin64, server, version, wow64);
	if (NULL == self)
	{
		ERR(TYPE, "No Wine directory found or installed.\n");
	}

	return self;
}


void lunionplay_wine_print(const LunionPlayWine* wine)
{
	g_assert(wine != NULL);

	if (lunionplay_debug_mode())
	{
		fprintf(stderr, " ->\n");
		fprintf(stderr, " -> LunionPlayWine:\n");

		lunionplay_wine_print_property("base_dir", wine->base_dir);
		lunionplay_wine_print_property("lib32_dir", wine->lib32_dir);
		lunionplay_wine_print_property("lib64_dir", wine->lib64_dir);
		lunionplay_wine_print_property("bin_dir", wine->bin_dir);
		lunionplay_wine_print_property("bin", wine->bin);
		lunionplay_wine_print_property("bin64", wine->bin64);
		lunionplay_wine_print_property("server", wine->server);
		lunionplay_wine_print_property("version", wine->version);

		if (wine->wow64 == FALSE)
			fprintf(stderr, " ->  * wow64: FALSE\n");
		else
			fprintf(stderr, " ->  * wow64: TRUE\n");

		fprintf(stderr, " ->\n");
	}
}



void lunionplay_wine_free(LunionPlayWine* self)
{
	g_assert(self != NULL);

	lunionplay_wine_free_param(self->base_dir);
	lunionplay_wine_free_param(self->lib32_dir);
	lunionplay_wine_free_param(self->lib64_dir);
	lunionplay_wine_free_param(self->bin_dir);
	lunionplay_wine_free_param(self->bin);
	lunionplay_wine_free_param(self->bin64);
	lunionplay_wine_free_param(self->server);
	lunionplay_wine_free_param(self->version);

	g_free(self);
	self = NULL;
}


const gchar* lunionplay_wine_get_bin(const LunionPlayWine* self)
{
	g_assert(self != NULL);

	return self->bin;
}


const gchar* lunionplay_wine_get_bin64(const LunionPlayWine* self)
{
	g_assert(self != NULL);

	return self->bin64;
}


const gchar* lunionplay_wine_get_server(const LunionPlayWine* self)
{
	g_assert(self != NULL);

	return self->server;
}


const gchar* lunionplay_wine_get_version(const LunionPlayWine* self)
{
	g_assert(self != NULL);

	return self->version;
}


void lunionplay_wine_setup_runtime(const LunionPlayWine* self)
{
	const char* env =NULL;

	if (self->lib32_dir != NULL && self->lib64_dir != NULL)
		lunionplay_wine_setup_library_path(self);

	g_setenv("WINEFSYNC", "1", FALSE);
	/* fallback when fsync don't support */
	g_setenv("WINEESYNC", "1", FALSE);

	if (g_getenv("WINEDLLOVERRIDES") != NULL)
	{
		lunionplay_append_env("WINEDLLOVERRIDES", "winemenubuilder.exe=", ";");
	}
	else
	{
		g_setenv("WINEDLLOVERRIDES", "winemenubuilder.exe=", TRUE);
	}

	env = g_getenv("LUNIONPLAY_LOG");

	if (env == NULL)
	{
		g_setenv("WINEDEBUG", "-all", TRUE);
	}
	else
	{
		if (g_strcmp0(env, "1") == 0)
		{
			g_setenv("WINEDEBUG", "fixme-all", FALSE);
		}
		else if (g_strcmp0(env, "3") == 0)
		{
			g_setenv("WINEDEBUG", "warn+seh", FALSE);
		}
		else if (g_strcmp0(env, "all") == 0)
		{
			g_setenv("WINEDEBUG", "+timestamp,+pid,+tid,+seh,+debugstr,+loaddll,+mscoree", TRUE);
		}
	}
}


void lunionplay_wine_setup_prefix(const gchar* path)
{
	g_assert(path != NULL);

	const char* env = NULL;

	TRACE(__FILE__, __func__, "%s\n", path);

	env = getenv("WINEPREFIX");
	if (NULL == env)
	{
		gchar* pfx = g_build_path("/", path, "pfx", NULL);

		g_setenv("WINEPREFIX", pfx, FALSE);
		g_free(pfx);

		env = g_getenv("WINEPREFIX");
	}

	if (! g_file_test(env, G_FILE_TEST_IS_DIR))
	{
		WARN(TYPE, "Creating a new wine prefix: %s\n", env);
		g_setenv("LUNIONPLAY_WAITING", "true", TRUE);
	}
}


void lunionplay_wine_update_prefix(const LunionPlayWine* self)
{
	g_autofree gchar* dll = NULL;
	g_autofree gchar* dbg = NULL;
	gchar** cmdline = NULL;

	cmdline = (gchar**) calloc(3, sizeof(gchar*));
	if (NULL == cmdline)
	{
		return;
	}

	cmdline[0] = g_strdup(self->bin);
	cmdline[1] = g_strdup("wineboot");
	cmdline[2] = NULL;

	/* Save actual values */
	dll = g_strdup(g_getenv("WINEDLLOVERRIDES"));
	dbg = g_strdup(g_getenv("WINEDEBUG"));

	g_setenv("WINEDLLOVERRIDES", "mscoree,mshtml,winemenubuilder.exe=", TRUE);
	g_setenv("WINEDEBUG", "-all", TRUE);

	lunionplay_run_proc(NULL, cmdline, FALSE, FALSE);
	lunionplay_wine_use_server(self, "-w");

	if (NULL == dll)
	{
		g_unsetenv("WINEDLLOVERRIDES");
	}

	if (NULL == dbg)
	{
		g_unsetenv("WINEDEBUG");
	}

	g_free(cmdline[1]);
	g_free(cmdline[0]);
	g_free(cmdline);
}


void lunionplay_wine_use_server(const LunionPlayWine* self, const gchar* option)
{
	g_assert(self != NULL);
	g_assert(option != NULL);

	gchar** cmd = NULL;

	cmd = (gchar**) calloc(3, sizeof(gchar*));
	if (NULL == cmd)
		return;

	cmd[0] = g_strdup(self->server);
	cmd[1] = g_strdup(option);
	cmd[2] = NULL;

	lunionplay_run_proc(NULL, cmd, FALSE, FALSE);

	g_free(cmd[1]);
	g_free(cmd[0]);
	g_free(cmd);
}
