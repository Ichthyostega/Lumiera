/*
  PLUGIN.h  -  Plugin loader

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file plugin.h
 ** Lumiera plugins define 'interfaces' as shown in interface.h, the plugin system handles the loading
 ** of all kinds of plugins under the hood, invoked from the interface system. Most things defined here
 ** are called internally and should not be used by other parts of the application.
 **
 ** \par Plugin discovery
 ** The #lumiera_plugin_discover() function offers a automatic way to load and register new plugins. It
 ** traverses all configured plugin directories. It takes to function for loading and registering plugins
 ** as parameter, by now this only uses the here defined \c plugin_load() and \c plugin_register() functions
 ** which lets it load any newly found plugin unconditionally. Later these callbacks will be replaced by
 ** a smarter system (plugindb) which makes it possible to load plugins on-demand and select proper
 ** plugins based on their version and capabilities.
 **
 ** \par Plugin loading
 ** Plugins are loaded and initialised in a sequence of steps:
 ** plugin_load() dispatches to a specific loader function depending on the type (extension) of a plugin.
 ** This loader allocates a new plugin structure with plugin_new() and then does it work and eventually
 ** finalising the plugin structure initialisation with plugin_init() by providing a handle to a
 ** \c lumieraorg__plugin interface. plugin_init() also stores the current error state (which might be clean)
 ** into the plugin. After that the plugin can be registered which records it in the plugin registry and if
 ** its error state is clean, registering all interfaces it offers in the interface registry. With that
 ** the plugin is ready to be used. Plugins with the error state set should still be registered to prevent
 ** further discovery runs to try loading them again.
 **
 */

#ifndef COMMON_PLUGIN_H
#define COMMON_PLUGIN_H

#include "lib/psplay.h"
#include "lib/error.h"

#include "common/interface.h"

#include <stdlib.h>
#include <nobug.h>


LUMIERA_ERROR_DECLARE(PLUGIN_INIT);
LUMIERA_ERROR_DECLARE(PLUGIN_OPEN);
LUMIERA_ERROR_DECLARE(PLUGIN_WTF);
LUMIERA_ERROR_DECLARE(PLUGIN_REGISTER);
LUMIERA_ERROR_DECLARE(PLUGIN_VERSION);




struct lumiera_plugin_struct;
typedef struct lumiera_plugin_struct lumiera_plugin;
typedef lumiera_plugin* LumieraPlugin;


/**
 * Allocates an preinitialises a plugin structure
 * @internal
 * @param name path/filename of the plugin
 * @return new allocated/preinitialised plugin structure with its error state set to \c LUMIERA_ERROR_PLUGIN_INIT
 */
LumieraPlugin
lumiera_plugin_new (const char* name);

/**
 * @internal Complete plugin initialisation
 * Stores any pending error (from loading) in self which clears out the LUMIERA_ERROR_PLUGIN_INIT error state
 * which was initialised by lumiera_plugin_new(), stores the handle and plugin pointers in the plugin struct.
 * @param self pointer to the plugin struct
 * @param handle opaque handle referring to some plugin type specific data
 * @param plugin a lumieraorg__plugin interface which will be used to initialise this plugin
 */
LumieraPlugin
lumiera_plugin_init (LumieraPlugin self, void* handle, LumieraInterface plugin);


/**
 * Tries to load a plugin
 * Creates a new plugin structure and tries to load and initialise the plugin.
 * The plugins error state may be set on any problem which should be queried later.
 * @param plugin path/filename of the plugin to load
 * @return pointer to the new plugin structure (always, never NULL, check error state)
 */
LumieraPlugin
lumiera_plugin_load (const char* plugin);


/**
 * Register a plugin and its interfaces.
 * Registers the plugin (unconditionally) at the plugin registry.
 * When the error state of the plugin is NULL then use its lumieraorg__plugin interface
 * to register all interfaces offered by the plugin at the interface registry.
 * Registered plugins will be automatic unloaded at application end.
 * @param self the plugin to be registered (calling with NULL is a no-op)
 * @return 1 on success (including calling with NULL) and 0 when a error occurred
 */
int
lumiera_plugin_register (LumieraPlugin self);


/**
 * Query the error state of a plugin
 * @param self plugin to query
 * @return error identifier, NULL if no error was set
 */
lumiera_err
lumiera_plugin_error (LumieraPlugin self);

/**
 * Query the plugin handle
 * @param self plugin to query
 * @return opaque handle set by the loader functions
 */
void*
lumiera_plugin_handle (LumieraPlugin self);


/**
 * Query the plugin name
 * The name is the path and filename under which it was loaded
 * @param self plugin to query
 * @return pointer to the name string
 */
const char*
lumiera_plugin_name (LumieraPlugin self);


/**
 * @internal Increment refcount
 * @param self plugin which refcount to increment
 */
void
lumiera_plugin_refinc (LumieraPlugin self);


/**
 * @internal Decrement refcount
 * @param self plugin which refcount to decrement
 */
void
lumiera_plugin_refdec (LumieraPlugin self);


/**
 * Tries to unload a plugin.
 * When the Plugin is unused, then all resources associated with it are freed and it will be removed from memory
 * @param plugin the plugin to be unloaded.
 * @return 0 on success, else the refcount of users which keeping the plugin loaded
 */
unsigned
lumiera_plugin_unload (LumieraPlugin self);


/**
 * Lookup a plugin handle in the pluginregistry
 * @param name name of the plugin to be looked up
 * @return plugin handle on success, NULL if the plugin is not found in the registry
 */
LumieraPlugin
lumiera_plugin_lookup (const char* name);


/**
 * discover new plugins
 * traverses the configured plugin paths and calls the callback_load function for any plugin
 * not actually loaded. If callback_load returns a plugin (and not NULL) then this is feed to
 * the callback_register function.
 */
int
lumiera_plugin_discover (LumieraPlugin (*callback_load)(const char* plugin),
                         int (*callback_register) (LumieraPlugin));


/* === psplay support functions === */

int
lumiera_plugin_cmp_fn (const void* keya, const void* keyb);

const void*
lumiera_plugin_key_fn (const PSplaynode node);

void
lumiera_plugin_delete_fn (PSplaynode node);



#endif /*COMMON_PLUGIN_H*/
