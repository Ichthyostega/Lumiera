/*
  plugin.h  -  Plugin loader

  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>

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
#ifndef CINELERRA_PLUGIN_H
#define CINELERRA_PLUGIN_H

#ifdef __cplusplus
extern "C" {
#elif 0
} /*eek, fixes emacs indenting for now*/
#endif

#include <stdlib.h>
#include <nobug.h>

NOBUG_DECLARE_FLAG (cinelerra_plugin);

/* tool macros*/
#define CINELERRA_INTERFACE_TYPE(name, version) struct cinelerra_interface_##name##_##version
#define CINELERRA_INTERFACE_CAST(name, version) (CINELERRA_INTERFACE_TYPE(name, version)*)

/* Interface definition */
#define CINELERRA_INTERFACE(name, version, ...) \
CINELERRA_INTERFACE_TYPE(name, version)         \
{                                               \
  struct cinelerra_interface interface_header_; \
  __VA_ARGS__                                   \
}

#define CINELERRA_INTERFACE_PROTO(ret, name, params)  ret (*name) params;

/* Interface instantiation */
#define CINELERRA_INTERFACE_IMPLEMENT(iname, version, name, open, close, ...)   \
CINELERRA_INTERFACE_TYPE(iname, version) name##_##version =                     \
{                                                                               \
  {                                                                             \
    version,                                                                    \
    sizeof(CINELERRA_INTERFACE_TYPE(iname, version)),                           \
    NULL,                                                                       \
    0,                                                                          \
    open,                                                                       \
    close                                                                       \
  },                                                                            \
  __VA_ARGS__                                                                   \
}

#define CINELERRA_INTERFACE_FUNC(protoname, funcname) funcname,



/* internally used */
struct cinelerra_plugin;

/**
 * This is the header for any interface exported by plugins.
 * Real interfaces append their functions at the end. There are few standard functions on each Interface
 * Every function is required to be implemnted.
 */
struct cinelerra_interface
{
  /// interface version number
  unsigned version;
  /// size of the full structure is used to determine the revision (adding a new function increments the size)
  size_t size;
  /// back reference to plugin
  struct cinelerra_plugin* plugin;
  /// incremented for each user of this interface and decremented when closed
  unsigned use_count;

  /// called for each open of this interface
  int (*open)(void);
  /// called for each close of this interface
  int (*close)(void);
};

/**
 * Make an interface available.
 * To use an interface provided by a plugin it must be opened first. It is allowed to open an interface more than once.
 * Each open must be paired with a close.
 * @param plugin name of the plugin to use.
 * @param name name of the interface to open.
 * @param min_revision the size of the interface structure is used as measure of a minimal required revision (new functions are appended at the end)
 * @return handle to the interface or NULL in case of a error. The application shall cast this handle to the actual interface type.
 */
struct cinelerra_interface*
cinelerra_interface_open (const char* plugin, const char* name, size_t min_revision);


/**
 * Close an interface. Does not free associated resources
 * Calling this function with self==NULL is legal. Every interface handle must be closed only once.
 * @param self interface to be closed
 */
void
cinelerra_interface_close (struct cinelerra_interface* self);

/**
 * Tries to unload a plugin.
 * When the Plugin is unused, then all resources associated with it are freed and it will be removed from memory
 * @param plugin name of the plugin to be unloaded.
 * @return 0 on success, else the number if users which keeping the plugin loaded
 */
int
cinelerra_plugin_unload (const char* plugin);

/**
 * Tries to unload plugins which are not in use.
 * Calls cinelerra_plugin_unload() for each Plugin which is not used for more than age seconds.
 * This function might be infrequently called by the scheduler to remove things which are not needed.
 * @param age timeout in seconds when to unload plugins
 */
void
cinelerra_plugin_expire (time_t age);

/**
 * Query and reset the error state.
 * report last error, reset error state. Errors are thread local.
 * Error strings are guaranteed to point to a C string with a unique comparable address.
 * Note that the error state gets cleared by calling this function. The application may store it temporary for further handling.
 * @return pointer to one of the error C-strings, NULL when no error happened.
 */
const char*
cinelerra_plugin_error ();

/// success, NULL
extern const char* CINELERRA_PLUGIN_SUCCESS;
/// memory allocation error
extern const char* CINELERRA_PLUGIN_EALLOC;
/// Plugin not found
extern const char* CINELERRA_PLUGIN_ENFOUND;
/// no such interface
extern const char* CINELERRA_PLUGIN_ENIFACE;
/// revision not sufficient
extern const char* CINELERRA_PLUGIN_EREVISION;

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* CINELERRA_PLUGIN_H */
