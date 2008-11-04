/*
  plugin.c  -  Lumiera Plugin loader

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#include "lib/safeclib.h"
#include "lib/psplay.h"
#include "lib/mutex.h"
#include "lib/error.h"

#include "backend/interfaceregistry.h"
#include "backend/config.h"
#include "backend/plugin.h"

#include <glob.h>

#include <nobug.h>

/**
 * @file
 * Plugin loader.
 */

extern PSplay lumiera_pluginregistry;

/* TODO should be set by the build system to the actual plugin path */

/* errors */
LUMIERA_ERROR_DEFINE(PLUGIN_INIT, "Initialization error");
LUMIERA_ERROR_DEFINE(PLUGIN_DLOPEN, "Could not open plugin");
LUMIERA_ERROR_DEFINE(PLUGIN_WTF, "Not a Lumiera plugin");
LUMIERA_ERROR_DEFINE(PLUGIN_REGISTER, "Could not register plugin");
LUMIERA_ERROR_DEFINE(PLUGIN_VERSION, "Plugin Version unsupported");

#define LUMIERA_PLUGIN_TYPE_PLANNED(name, ext)

/**
 * Supported (and planned) plugin types and their file extensions
 * This maps filename extensions to implementations (of the respective _load_NAME and _unload_NAME functions)
 * So far we only support platform dynamic libraries, later we may add plugins implemented in lua
 * and c source modules which get compiled on the fly.
 */
#define LUMIERA_PLUGIN_TYPES                    \
  LUMIERA_PLUGIN_TYPE(DYNLIB, ".so")            \
  LUMIERA_PLUGIN_TYPE(DYNLIB, ".lum")           \
  LUMIERA_PLUGIN_TYPE_PLANNED(LUA, ".lua")      \
  LUMIERA_PLUGIN_TYPE_PLANNED(CSOURCE, ".c")


/**
 * record the extension and a callback function for loading the associated plugin for each plugin type
 */
struct lumiera_plugintype_struct
{
  LumieraPlugin (*lumiera_plugin_load_fn)(const char*);
  void (*lumiera_plugin_unload_fn)(LumieraPlugin);
  const char* ext;
};
typedef struct lumiera_plugintype_struct lumiera_plugintype;
typedef lumiera_plugintype* LumieraPlugintype;

/* forward declare loader functions for all types */
#define LUMIERA_PLUGIN_TYPE(type, ext)                          \
  LumieraPlugin lumiera_plugin_load_##type (const char*);       \
  void lumiera_plugin_unload_##type (LumieraPlugin);
LUMIERA_PLUGIN_TYPES
#undef LUMIERA_PLUGIN_TYPE

/* and now setup a table which will be used for dispatching loaders depending on the type of the plugin */
#define LUMIERA_PLUGIN_TYPE(type, ext) {lumiera_plugin_load_##type, lumiera_plugin_unload_##type, ext},
static lumiera_plugintype lumiera_plugin_types[] =
  {
    LUMIERA_PLUGIN_TYPES
    {NULL, NULL, NULL}
  };
#undef LUMIERA_PLUGIN_TYPE


struct lumiera_plugin_struct
{
  psplaynode node;

  /* long names as looked up ("/usr/local/lib/lumiera/plugins/effects/audio/normalize.so") */
  const char* name;

  /* use count for all interfaces of this plugin */
  unsigned refcnt;

  /* time when the last open or close action happened */
  time_t last;

  /* when loading plugins en masse we do not want to fail completely if one doesnt cooperate, instead we record local errors here */
  lumiera_err error;

  /* the 'plugin' interface itself */
  LumieraInterface plugin;

  /* generic handle for the plugin, dlopen handle, etc */
  void* handle;
};


LumieraPlugin
lumiera_plugin_new (const char* name)
{
  LumieraPlugin self = lumiera_malloc (sizeof (*self));

  psplaynode_init (&self->node);
  self->name = lumiera_strndup (name, SIZE_MAX);
  self->refcnt = 0;
  time (&self->last);
  self->error = LUMIERA_ERROR_PLUGIN_INIT;
  self->plugin = NULL;
  self->handle = NULL;
  return self;
}


LumieraPlugin
lumiera_plugin_init (LumieraPlugin self, void* handle, LumieraInterface plugin)
{
  self->error = lumiera_error ();
  self->plugin = plugin;
  self->handle = handle;
  return self;
}




static char* init_exts_globs ();

int
lumiera_plugin_discover (LumieraPlugin (*callback_load)(const char* plugin),
                         int (*callback_register) (LumieraPlugin))
{
  TRACE (plugin);
  REQUIRE (callback_load);
  REQUIRE (callback_register);

  /* construct glob trail {.so,.c,.foo} ... */
  static char* exts_globs = NULL;
  if (!exts_globs)
    exts_globs = init_exts_globs (&exts_globs);

  const char* path;
  unsigned i = 0;
  int flags = GLOB_PERIOD|GLOB_BRACE|GLOB_TILDE_CHECK;
  glob_t globs;

  while ((path = lumiera_config_wordlist_get_nth ("plugin.path", i, ":")))
    {
      path = lumiera_tmpbuf_snprintf (SIZE_MAX,"%s%s", path, exts_globs);
      TRACE (plugin, "globbing path '%s'", path);
      int ret = glob (path, flags, NULL, &globs);
      if (ret == GLOB_NOSPACE)
        LUMIERA_DIE (NO_MEMORY);

      flags |= GLOB_APPEND;
      ++i;
    }

  if (globs.gl_pathc)
    LUMIERA_RECMUTEX_SECTION (plugin, &lumiera_interface_mutex)
      {
        for (char** itr = globs.gl_pathv; *itr; ++itr)
          {
            if (!psplay_find (lumiera_pluginregistry, *itr, 100))
              {
                TRACE (plugin, "found new plugin '%s'", *itr);
                callback_register (callback_load (*itr));
              }
          }
      }

  globfree (&globs);

  return !lumiera_error_peek ();
}


LumieraPlugin
lumiera_plugin_load (const char* plugin)
{
  TRACE (plugin);

  /* dispatch on ext, call the registered function */
  const char* ext = strrchr (plugin, '.');

  LumieraPlugintype itr = lumiera_plugin_types;
  while (itr->ext)
    {
      if (!strcmp (itr->ext, ext))
        return itr->lumiera_plugin_load_fn (plugin);
      ++itr;
    }
  return NULL;
}

int
lumiera_plugin_register (LumieraPlugin plugin)
{
  TRACE (plugin);
  if (!plugin)
    return 1;

  LUMIERA_RECMUTEX_SECTION (plugin, &lumiera_interface_mutex)
    {
      if (psplay_insert (lumiera_pluginregistry, &plugin->node, 100))
        {
          if (!plugin->error)
            {
              switch (lumiera_interface_version (plugin->plugin, "lumieraorg__plugin"))
                {
                case 0:
                  {
                    TRACE (plugin, "registering %s", plugin->name);
                    LUMIERA_INTERFACE_HANDLE(lumieraorg__plugin, 0) handle =
                      LUMIERA_INTERFACE_CAST(lumieraorg__plugin, 0) plugin->plugin;
                    lumiera_interfaceregistry_bulkregister_interfaces (handle->plugin_interfaces (), plugin);
                  }
                  break;
                default:
                  LUMIERA_ERROR_SET (plugin, PLUGIN_VERSION);
                }
            }
        }
      else
        {
          LUMIERA_ERROR_SET (plugin, PLUGIN_REGISTER);
        }
    }
  return !!lumiera_error_peek();
}




static char* init_exts_globs ()
{
  char* exts_globs;
  size_t exts_sz = 4; /* / * { } \0 less one comma */
  LumieraPlugintype itr = lumiera_plugin_types;
  while (itr->ext)
    {
      exts_sz += strlen (itr->ext) + 1;
      ++itr;
    }

  exts_globs = lumiera_malloc (exts_sz);
  *exts_globs = '\0';

  itr = lumiera_plugin_types;
  strcat (exts_globs, "/*{");

  while (itr->ext)
    {
      strcat (exts_globs, itr->ext);
      strcat (exts_globs, ",");
      ++itr;
    }
  exts_globs[exts_sz-2] = '}';
  TRACE (plugin, "initialized extension glob to '%s'", exts_globs);
  return exts_globs;
}

int
lumiera_plugin_cmp_fn (const void* keya, const void* keyb)
{
  return strcmp ((const char*)keya, (const char*)keyb);
}


const void*
lumiera_plugin_key_fn (const PSplaynode node)
{
  return ((LumieraPlugin)node)->name;
}

