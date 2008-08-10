/*
  config.h  -  Lumiera configuration system

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

#ifndef LUMIERA_CONFIG_H
#define LUMIERA_CONFIG_H

//TODO: Support library includes//
#include "lib/error.h"
#include "lib/rwlock.h"

//TODO: Forward declarations//
struct lumiera_config_struct;


/* master config subsystem debug flag */
NOBUG_DECLARE_FLAG (config_all);
/* config subsystem internals */
NOBUG_DECLARE_FLAG (config);
/* high level typed interface operations */
NOBUG_DECLARE_FLAG (config_typed);
/* file operations */
NOBUG_DECLARE_FLAG (config_file);


LUMIERA_ERROR_DECLARE (CONFIG_SYNTAX);
LUMIERA_ERROR_DECLARE (CONFIG_SYNTAX_KEY);
LUMIERA_ERROR_DECLARE (CONFIG_SYNTAX_VALUE);
LUMIERA_ERROR_DECLARE (CONFIG_NO_ENTRY);
LUMIERA_ERROR_DECLARE (CONFIG_DEFAULT);

//TODO: Lumiera header includes//


//TODO: System includes//
#include <nobug.h>


/**
 * @file
   * TODO documentation, http://www.pipapo.org/pipawiki/Lumiera/ConfigLoader
 */


struct lumiera_config_struct
{
  // cuckoo hash
  // configfile list
  char* path;
  /*
    all access is protected with rwlock's.
    We use rwlocks here since concurrent reads are likely common.

    So far this is a global config lock, if this is a problem we might granularize it by locking on a file level.
    config access is not planned to be transaction al yet, if this is a problem we need to expose the rwlock to a config_acquire/config_release function pair
   */
  lumiera_rwlock lock;
};

typedef struct lumiera_config_struct lumiera_config;
typedef lumiera_config* LumieraConfig;

/**
 * Supported high level types: TODO documenting
 */
/* TODO: add here as 'LUMIERA_CONFIG_TYPE(name, ctype)' the _get/_set prototypes are declared automatically below, you still have to implement them in config.c */
#define LUMIERA_CONFIG_TYPES                    \
  LUMIERA_CONFIG_TYPE(number, signed long long) \
  LUMIERA_CONFIG_TYPE(real, long double)        \
  LUMIERA_CONFIG_TYPE(string, char*)            \
  LUMIERA_CONFIG_TYPE(word, char*)              \
  LUMIERA_CONFIG_TYPE(bool, int)


//  * does only initialize the variables, so that they get valid values, but does not allocate them as they will be allocated before as they are singleton.
//  * lumiera_config_init (const char* searchpath) searchpath is a buildin-default, can be changed via configure and can be appended and overridden by using a flag, e.g. {{{ --config-path-append="" }}} or {{{ --config-path="" }}}

/**
 * Initialize the configuration subsystem.
 * @param path search path for config files.
 * Must be called only once
 */
int
lumiera_config_init (const char* path);


//  * frees all space allocated by the ConfigLoader.

/**
 * Destroys the configuration subsystem.
 * Subsequent calls are no-ops.
 */
void
lumiera_config_destroy ();


//  * reads '''one''' single configuration file that will include all settings from other files.
//  * does not read itself but give delegates reading. The actual reading and parsing will be done in configfile object. s.later.
/**
 *
 */
int
lumiera_config_load (const char* file);


//{{{ lumiera_config_save () { LLIST_FOREACH(config_singleton.files, f) { LumieraFile file = (LumieraFile) f; if(lumiera_configfile_isdirty (file)) lumiera_configfile_save(file); } } }}}
//  * saves all the changed settings to user's configuration files, but recognizes where settings came from and will write them to an appropriate named file. Example: '''changed''' values from  ''/usr/local/share/lumiera/plugins/blur.conf'' will be saved into ''~/.lumiera/plugins/blur.conf''
//  * finds out which files are dirty and which settings have to be written to user's config files.
//  * does initiate the actual saving procedure by delegating the save to the actual configfile objects, see below.
//  * empty user configuration files in RAM will be deleted from disk on write.
//  * checks whether the file has changed since last read, and will print out an error if necessary instead of overriding it without notification.
/**
 *
 */
int
lumiera_config_save ();


// * `lumiera_config_purge(const char* filename)` removes all configs loaded from filename
/**
 *
 */
int
lumiera_config_purge (const char* filename);


// * {{{ lumiera_config_get(...) }}}
//  * get a value by key
//  * handles internally everything as string:string key:value pair.
//  * lowlevel function
//  * lumiera_config_integer_get (const char* key, int *value) will return integers instead of strings and return 0 if succeeded and -1 if it failed.
/**
 *
 */
int
lumiera_config_get (const char* key, const char** value);


// * {{{ lumiera_config_set(...) }}}
//  * set a value by key
//  * handles internally everything as string:string key:value pair.
//  * lowlevel function
//  * tag file as dirty
//  * set will create a new user configuration file if it does not exist yet or will append a line to the existing one in RAM. These  files, tagged as 'dirty', will be only written if save() is called.
/**
 *
 */
int
lumiera_config_set (const char* key, const char* value);



//  * {{{int lumiera_config_TYPE_get(const char* key, TYPE* value, const char* default) }}}
//    High level config interface for different types.
//    if default is given (!NULL) then value is set to default in case key was not found or any other error occured.
//    error code is still set and -1 (fail) is returned in case of an error, but it might be cleared with no ill effects.
//    NOTE: errors are persistent in our error handler, they must still be cleared, even when ignored.
//          if default is given then 'KEY_NOT_FOUND' is not a error here, if default is NULL then it is
//    NOTE2: default values are given as strings, the config loader remembers a given default value and checks if it got changed
//           when it is _set(). Thus a default value can be supressed when set/written
/**
 *
 */
#define LUMIERA_CONFIG_TYPE(name, type) \
  int \
  lumiera_config_##name##_get (const char* key, type* value, const char* def);
LUMIERA_CONFIG_TYPES
#undef LUMIERA_CONFIG_TYPE



//  * {{{ lumiera_config_TYPE_set (const char* key, TYPE*value, const char* fmt) }}}
//    Highlevel interface for different types, fmt is a printf format specifier for the desired format, when NULL, defaults apply.
/**
 *
 */
#define LUMIERA_CONFIG_TYPE(name, type) \
  int \
  lumiera_config_##name##_set (const char* key, type* value, const char* fmt);
LUMIERA_CONFIG_TYPES
#undef LUMIERA_CONFIG_TYPE


// * {{{ lumiera_config_reset(...) }}}
//  * reset a value by key to the system default values, thus removes a user's configuration line.
/**
 *
 */
int
lumiera_config_reset(const char* key);


//  * Find exact place of a setting.
/**
 *
 */
int
lumiera_config_info (const char* key, const char** filename, unsigned* line);

#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
