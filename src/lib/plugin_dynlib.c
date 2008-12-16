/*
  plugin_dynlib.c  -  Lumiera Plugin loader for dynamic libraries

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
#include "lib/plugin.h"

#include <dlfcn.h>
#include <nobug.h>

/**
 * @file
 * Plugin loader for dynamic libraries.
 */

LumieraPlugin
lumiera_plugin_load_DYNLIB (const char* name)
{
  TRACE (plugin);
  REQUIRE (name);
  LumieraPlugin self = lumiera_plugin_new (name);
  LumieraInterface plugin = NULL;

  void* handle = dlopen (name, RTLD_LAZY|RTLD_LOCAL);
  if (handle)
    {
      plugin = (LumieraInterface) dlsym (handle, LUMIERA_INTERFACE_DSTRING (lumieraorg__plugin, 0, lumieraorg_plugin));

      if (!plugin)
        LUMIERA_ERROR_SET (plugin, PLUGIN_WTF);
    }
  else
    LUMIERA_ERROR_SET (plugin, PLUGIN_OPEN);

#ifdef DEBUG
  if (lumiera_error_peek())
    {
      const char* problem = dlerror(); 
      WARN_IF (problem,  plugin, "Problem opening shared object %s : %s", name, problem);
    }
#endif

  return lumiera_plugin_init (self, handle, plugin);
}


void
lumiera_plugin_unload_DYNLIB (LumieraPlugin self)
{
  TRACE (plugin);
  void* handle = lumiera_plugin_handle (self);
  if (handle)
    dlclose (handle);
}
