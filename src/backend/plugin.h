/*
  plugin.h  -  Plugin loader

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
#ifndef LUMIERA_PLUGIN_H
#define LUMIERA_PLUGIN_H

#include "lib/psplay.h"
#include "lib/error.h"

#include "backend/interface.h"

#include <stdlib.h>
#include <nobug.h>

/**
 * @file
 * Lumiera plugins defines 'interfaces' as shown in interface.h, the plugin system handles the loading
 * of all kinds of plugins under the hood, invoked from the interface system. Anything defined here is
 * called internally and should not be used by other parts of the application.
 */


LUMIERA_ERROR_DECLARE(PLUGIN_INIT);
LUMIERA_ERROR_DECLARE(PLUGIN_DLOPEN);
LUMIERA_ERROR_DECLARE(PLUGIN_WTF);
LUMIERA_ERROR_DECLARE(PLUGIN_REGISTER);
LUMIERA_ERROR_DECLARE(PLUGIN_VERSION);


NOBUG_DECLARE_FLAG (plugin);

/* tool macros*/


struct lumiera_plugin_struct;
typedef struct lumiera_plugin_struct lumiera_plugin;
typedef lumiera_plugin* LumieraPlugin;
enum lumiera_plugin_type;


int
lumiera_plugin_cmp_fn (const void* keya, const void* keyb);

const void*
lumiera_plugin_key_fn (const PSplaynode node);

LumieraPlugin
lumiera_plugin_new (const char* name);

LumieraPlugin
lumiera_plugin_init (LumieraPlugin self, void* handle, LumieraInterface plugin);

LumieraPlugin
lumiera_plugin_load (const char* plugin);


int
lumiera_plugin_register (LumieraPlugin);




/**
 * discover new plugins
 * traverses the configured plugin paths and calls the callback_load function for any plugin
 * not actually loaded. If callback_load returns a plugin (and not NULL) then this is feed to
 * the callback_register function.
 */
int
lumiera_plugin_discover (LumieraPlugin (*callback_load)(const char* plugin),
                         int (*callback_register) (LumieraPlugin));





/**
 * Tries to unload a plugin.
 * When the Plugin is unused, then all resources associated with it are freed and it will be removed from memory
 * @param plugin name of the plugin to be unloaded.
 * @return 0 on success, else the number if users which keeping the plugin loaded
 */
int
lumiera_plugin_unload (const char* plugin);


#endif /* LUMIERA_PLUGIN_H */
