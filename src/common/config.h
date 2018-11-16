/*
  CONFIG.h  -  Lumiera configuration system

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/


/** @file config.h
 ** Interface for a lumiera configuration system (draft).
 ** This configuration uses ini-style configuration files and supports some
 ** simple types. A mechanism for overlay / cascading was planned. After some
 ** debate it turned out that we had no clear vision regarding the scope of
 ** that effort: should this system also manage (layered) defaults? should
 ** it also be able to save user preferences? Anyway, development in that
 ** area stalled and never reached the level of just loading and parsing
 ** a simple file -- yet this was not considered a roadblock and we agreed
 ** to revisit the topic when we've gained a better understanding of
 ** session storage and management of default values and user preferences.
 ** 
 ** @todo as of 2016, this code is not in any meaningful use
 ** 
 ** @see lumiera::BasicSetup simple start-up configuration
 ** @see http://lumiera.org/documentation/technical/vault/ConfigLoader.html ConfigLoader draft from 2008
 */

#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#include "lib/error.h"
#include "lib/mutex.h"
#include "common/config-lookup.h"
#include "common/configitem.h"

#include <nobug.h>
#include <stdio.h>


LUMIERA_ERROR_DECLARE (CONFIG_SYNTAX);
LUMIERA_ERROR_DECLARE (CONFIG_SYNTAX_KEY);
LUMIERA_ERROR_DECLARE (CONFIG_SYNTAX_VALUE);
LUMIERA_ERROR_DECLARE (CONFIG_NO_ENTRY);

#define LUMIERA_CONFIG_KEY_CHARS "abcdefghijklmnopqrstuvwxyz0123456789_."
#define LUMIERA_CONFIG_ENV_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789__"



struct lumiera_config_struct
{
  lumiera_config_lookup keys;

  lumiera_configitem defaults;          /* registered default values */
  lumiera_configitem files;             /* all loaded files */
  lumiera_configitem TODO_unknown;      /* all values which are not part of a file and not default TODO: this will be removed when file support is finished */

  lumiera_mutex lock;
};

typedef struct lumiera_config_struct lumiera_config;
typedef lumiera_config* LumieraConfig;

/**
 * Supported high level types: TODO documenting
 */
/* TODO: add here as 'LUMIERA_CONFIG_TYPE(name, ctype)' the _get/_set prototypes are declared automatically below, you still have to implement them in config.c */
#define LUMIERA_CONFIG_TYPES                    \
  LUMIERA_CONFIG_TYPE(link, const char*)        \
  LUMIERA_CONFIG_TYPE(number, long long)        \
  LUMIERA_CONFIG_TYPE(real, long double)        \
  LUMIERA_CONFIG_TYPE(string, const char*)      \
  LUMIERA_CONFIG_TYPE(wordlist, const char*)    \
  LUMIERA_CONFIG_TYPE(word, const char*)        \
  LUMIERA_CONFIG_TYPE(bool, int)


/**
 * Initialise the configuration subsystem.
 * @param path search path for config files.
 * Must be called only once
 */
int
lumiera_config_init (const char* path);



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


/**
 * Does a diagnostic dump of the whole config database
 */
void
lumiera_config_dump (FILE* out);


// * {{{ lumiera_config_get(...) }}}
//  * get a value by key
//  * handles internally everything as string:string key:value pair.
//  * lowlevel function
//  * lumiera_config_integer_get (const char* key, int *value) will return integers instead of strings and return 0 if succeeded and -1 if it failed.
/**
 *
 */
const char*
lumiera_config_get (const char* key, const char** value);


const char*
lumiera_config_get_default (const char* key, const char** value);


// * {{{ lumiera_config_set(...) }}}
//  * set a value by key
//  * handles internally everything as string:string key:value pair.
//  * lowlevel function
//  * tag file as dirty
//  * set will create a new user configuration file if it does not exist yet or will append a line to the existing one in RAM. These  files, tagged as 'dirty', will be only written if save() is called.

/**
 *
 *
 * @param key
 * @param delim_value delimiter (= or <) followed by the value to be set
 *
 */
LumieraConfigitem
lumiera_config_set (const char* key, const char* delim_value);


/**
 * Installs a default value for a config key.
 * Any key might have an associated default value which is used when
 * no other configuration is available, this can be set once.
 * Any subsequent call will be a no-op. This function locks the config system.
 * @param line line with key, delimiter and value to store as default value
 * @return NULL in case of an error, else a pointer to the default configitem
 */
LumieraConfigitem
lumiera_config_setdefault (const char* line);



//  * {{{int lumiera_config_TYPE_get(const char* key, TYPE* value, const char* default) }}}
//    High level config interface for different types.
//    if default is given (!NULL) then value is set to default in case key was not found or any other error occurred.
//    error code is still set and -1 (fail) is returned in case of an error, but it might be cleared with no ill effects.
//    NOTE: errors are persistent in our error handler, they must still be cleared, even when ignored.
//          if default is given then 'KEY_NOT_FOUND' is not a error here, if default is NULL then it is
//    NOTE2: default values are given as strings, the config loader remembers a given default value and checks if it got changed
//           when it is _set(). Thus a default value can be suppressed when set/written
/**
 *
 */
#define LUMIERA_CONFIG_TYPE(name, type) \
  const char* \
  lumiera_config_##name##_get (const char* key, type* value);
LUMIERA_CONFIG_TYPES
#undef LUMIERA_CONFIG_TYPE

/**
 * Wordlists
 * Wordlists are lists of single words delimited by any of " \t,;".
 * They can be used to store groups of keys and other kinds of simple references into the config
 * system. Here are some functions to manipulate single word entries in a wordlist.
 */

/**
 * Get nth word of a wordlist.
 * @param key key under which this wordlist is stored
 * @param nth index of the word to get, starting with 0
 * @param delims a string literal listing all characters which are treated as delimiters
 * @return pointer to a tempbuf holding the nth word or NULL in case of error
 */
const char*
lumiera_config_wordlist_get_nth (const char* key, unsigned nth, const char* delims);


/**
 * Find the index of a word in a wordlist.
 * @param key key under which this wordlist is stored
 * @param value word to find
 * @param delims a string literal listing all characters which are treated as delimiters
 * @return index of the first occurrence of the word or -1 in case of failure
 */
int
lumiera_config_wordlist_find (const char* key, const char* value, const char* delims);


/**
 * Universal word replacement function.
 * Replaces a word with up to two new words. This can be used to delete a word (no replacements),
 * insert a new word before an existing word (giving the new word as subst1 and the old word as subst2)
 * insert a new word after an existing word (giving  the old word as subst1 and the new word as subst2)
 * or simply give 2 new words.
 * @param key key under which this wordlist is stored
 * @param value word to be replaced
 * @param subst1 first replacement word
 * @param subst2 second replacement word
 * @param delims a string literal listing all characters which are treated as delimiters
 * @return internal representation of the wordlist in a tmpbuf or NULL in case of an error
 */
const char*
lumiera_config_wordlist_replace (const char* key, const char* value, const char* subst1, const char* subst2, const char* delims);


/**
 * Add a word to the end of a wordlist if it doesn't exist already
 * @param key key under which this wordlist is stored
 * @param value new word to add
 * @param delims a string literal listing all characters which are treated as delimiters
 * @return internal representation of the wordlist in a tmpbuf or NULL in case of an error
 */
const char*
lumiera_config_wordlist_add (const char* key, const char* value, const char* delims);

//  * {{{ lumiera_config_TYPE_set (const char* key, TYPE*value, const char* fmt) }}}
//    Highlevel interface for different types, fmt is a printf format specifier for the desired format, when NULL, defaults apply.
/**
 *
 */
#define LUMIERA_CONFIG_TYPE(name, type)                         \
  LumieraConfigitem                                             \
  lumiera_config_##name##_set (const char* key, type* value);
LUMIERA_CONFIG_TYPES
#undef LUMIERA_CONFIG_TYPE



// * {{{ lumiera_config_reset(...) }}}
//  * reset a value by key to the system default values, thus removes a user's configuration line.
/**
 *
 */
int
lumiera_config_reset (const char* key);


//  * Find exact place of a setting.
/**
 *
 */
int
lumiera_config_info (const char* key, const char** filename, unsigned* line);

#endif /*COMMON_CONFIG_H*/
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
