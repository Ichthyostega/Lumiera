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
LUMIERA_ERROR_DEFINE(PLUGIN_DLOPEN, "Could not open plugin");

/*
  supported (planned) plugin types and their file extensions
*/

#define LUMIERA_PLUGIN_TYPES            \
  LUMIERA_PLUGIN_TYPE(DYNLIB, ".so")

/*
  only .so dynlibs for now, later we may support some more
  LUMIERA_PLUGIN_TYPE(LUA, ".lua")
  LUMIERA_PLUGIN_TYPE(CSOURCE, ".c")
*/

#define LUMIERA_PLUGIN_TYPE(type, ext) LUMIERA_PLUGIN_##type,
enum lumiera_plugin_type
  {
    LUMIERA_PLUGIN_TYPES
    LUMIERA_PLUGIN_NONE
  };
#undef LUMIERA_PLUGIN_TYPE

#define LUMIERA_PLUGIN_TYPE(type, ext) ext,
static const char* const lumiera_plugin_ext[] =
  {
    LUMIERA_PLUGIN_TYPES
    NULL
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

  /* kind of plugin */
  enum lumiera_plugin_type type;

  /* dlopen handle */
  void* handle;
};



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
    {
      size_t exts_sz = 4; /* / * { } \0 less one comma */
      const char*const* itr = lumiera_plugin_ext;
      while (*itr)
        {
          exts_sz += strlen (*itr) + 1;
          ++itr;
        }

      exts_globs = lumiera_malloc (exts_sz);
      *exts_globs = '\0';

      itr = lumiera_plugin_ext;
      strcat (exts_globs, "/*{");

      while (*itr)
        {
          strcat (exts_globs, *itr);
          strcat (exts_globs, ",");
          ++itr;
        }
      exts_globs[exts_sz-2] = '}';
      TRACE (plugin, "initialized extension glob to '%s'", exts_globs);
    }

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

  LUMIERA_RECMUTEX_SECTION (plugin, &lumiera_interface_mutex)
    {
      for (char** itr = globs.gl_pathv; *itr; ++itr)
        {

          if (!psplay_find (lumiera_pluginregistry, *itr, 100))
            {
              TRACE (plugin, "found new plugin '%s'", *itr);

              LumieraPlugin plugin = callback_load (*itr);
              if (plugin)
                callback_register (plugin);
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
  UNIMPLEMENTED();
  (void) plugin;

  return NULL;
}


int
lumiera_plugin_register (LumieraPlugin plugin)
{
  TRACE (plugin);
  UNIMPLEMENTED();

  LUMIERA_RECMUTEX_SECTION (plugin, &lumiera_interface_mutex)
    {
      (void) plugin;

    }
  return 0;
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

