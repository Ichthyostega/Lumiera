/*
  config.c  -  Lumiera configuration system

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

//TODO: Support library includes//
#include "lib/safeclib.h"


//TODO: Lumiera header includes//
#include "backend/config.h"

//TODO: internal/static forward declarations//


//TODO: System includes//
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * @file
 *
 */

NOBUG_DEFINE_FLAG_PARENT (config_all, backend);
NOBUG_DEFINE_FLAG_PARENT (config, config_all);
NOBUG_DEFINE_FLAG_PARENT (config_typed, config_all);
NOBUG_DEFINE_FLAG_PARENT (config_file, config_all);
NOBUG_DEFINE_FLAG_PARENT (config_item, config_all);
NOBUG_DEFINE_FLAG_PARENT (config_lookup, config_all);

LUMIERA_ERROR_DEFINE (CONFIG_SYNTAX, "syntax error in configfile");
LUMIERA_ERROR_DEFINE (CONFIG_SYNTAX_KEY, "syntax error in key");
LUMIERA_ERROR_DEFINE (CONFIG_SYNTAX_VALUE, "syntax error in value");
LUMIERA_ERROR_DEFINE (CONFIG_NO_ENTRY, "no configuration entry");
LUMIERA_ERROR_DEFINE (CONFIG_DEFAULT, "illegal default value");


/* singleton config */
LumieraConfig lumiera_global_config = NULL;


int
lumiera_config_init (const char* path)
{
  TRACE (config);
  REQUIRE (!lumiera_global_config, "Configuration subsystem already initialized");
  REQUIRE (path);

  NOBUG_INIT_FLAG (config_all);
  NOBUG_INIT_FLAG (config);
  NOBUG_INIT_FLAG (config_typed);
  NOBUG_INIT_FLAG (config_file);
  NOBUG_INIT_FLAG (config_item);
  NOBUG_INIT_FLAG (config_lookup);

  lumiera_global_config = lumiera_malloc (sizeof (*lumiera_global_config));
  lumiera_global_config->path = lumiera_strndup (path, SIZE_MAX);
  lumiera_config_lookup_init (&lumiera_global_config->keys);

  lumiera_configitem_init (&lumiera_global_config->defaults);
  lumiera_configitem_init (&lumiera_global_config->files);
  lumiera_configitem_init (&lumiera_global_config->TODO_unknown);

  lumiera_rwlock_init (&lumiera_global_config->lock, "config rwlock", &NOBUG_FLAG (config));

  TODO ("register path as config.path itself");

  return 0;
}


void
lumiera_config_destroy ()
{
  TRACE (config);
  if (lumiera_global_config)
    {
      lumiera_rwlock_destroy (&lumiera_global_config->lock, &NOBUG_FLAG (config));
      lumiera_configitem_destroy (&lumiera_global_config->defaults, &lumiera_global_config->keys);
      lumiera_configitem_destroy (&lumiera_global_config->files, &lumiera_global_config->keys);
      lumiera_configitem_destroy (&lumiera_global_config->TODO_unknown, &lumiera_global_config->keys);
      lumiera_config_lookup_destroy (&lumiera_global_config->keys);
      lumiera_free (lumiera_global_config->path);
      lumiera_free (lumiera_global_config);
      lumiera_global_config = NULL;
    }
  else
    WARN (config, "Tried to destroy non initialized config subsystem");
}


int
lumiera_config_load (const char* file)
{
  TRACE (config);
  UNIMPLEMENTED();
  return -1;
}


int
lumiera_config_save ()
{
  TRACE (config);
  UNIMPLEMENTED();
  return -1;
}


int
lumiera_config_purge (const char* filename)
{
  TRACE (config);

  UNIMPLEMENTED();
  return -1;
}


int
lumiera_config_get (const char* key, const char** value)
{
  TRACE (config);
  REQUIRE (key);
  REQUIRE (value);

  int ret = -1;

  /* we translate the key for the env var override by making it uppercase and replace . with _,
   as side effect, this also checks the key syntax */
  char* tr_key = lumiera_tmpbuf_tr (key,
                                    LUMIERA_CONFIG_KEY_CHARS,
                                    LUMIERA_CONFIG_ENV_CHARS,
                                    NULL);

  if (tr_key)
    {
      char* env = lumiera_tmpbuf_snprintf (2048, "LUMIERA_%s", tr_key);

      *value = getenv(env);
      if (*value)
        {
          ret = 0;
          NOTICE (config, "envvar override for config %s = %s", env, *value);
        }
      else
        {
          TODO ("follow '<' delegates?");
          LumieraConfigitem item = lumiera_config_lookup_item_find (&lumiera_global_config->keys, key);

          if (item)
            {
              *value = item->delim+1;
              ret = 0;
            }
          else
            LUMIERA_ERROR_SET (config, CONFIG_NO_ENTRY);
        }
    }
  else
    {
      LUMIERA_ERROR_SET (config, CONFIG_SYNTAX_KEY);
    }

  return ret;
}


int
lumiera_config_get_default (const char* key, const char** value)
{
  TRACE (config);
  REQUIRE (key);
  REQUIRE (value);

  int ret = -1;

  TODO ("follow '<' delegates?");
  TODO ("refactor _get and get_default to iterator access");
  LumieraConfigitem item = lumiera_config_lookup_item_tail_find (&lumiera_global_config->keys, key);

  if (item && item->parent == &lumiera_global_config->defaults)
    {
      *value = item->delim+1;
      ret = 0;
    }

  return ret;
}


int
lumiera_config_set (const char* key, const char* delim_value)
{
  TRACE (config);

  TODO ("if does this item already exist in a user writeable file?");
  TODO ("       replace delim_value");

  TODO ("else");
  TODO ("       find matching prefix");
  TODO ("       find matching suffix");
  TODO ("       find proper prefix indentation, else use config.indent");
  TODO ("       create configitem with prefix/suffix removed");



//  * set a value by key
//  * handles internally everything as string:string key:value pair.
//  * lowlevel function
//  * tag file as dirty
//  * set will create a new user configuration file if it does not exist yet or will append a line to the existing one in RAM. These  files, tagged as 'dirty', will be only written if save() is called.



  UNIMPLEMENTED();
  return -1;
}


LumieraConfigitem
lumiera_config_setdefault (const char* line)
{
  TRACE (config);
  REQUIRE (line);

  LumieraConfigitem item = NULL;

  LUMIERA_WRLOCK_SECTION (config, &lumiera_global_config->lock)
    {
      const char* key = line;
      while (*key && isspace (*key))
        key++;

      key = lumiera_tmpbuf_strndup (line, strspn (line, LUMIERA_CONFIG_KEY_CHARS));

      if (!(item = lumiera_config_lookup_item_find (&lumiera_global_config->keys, key)) || item->parent != &lumiera_global_config->defaults)
        {
          item = lumiera_configitem_new (line);

          if (item)
            {
              ENSURE (item->delim, "default must be a configentry with key=value or key<delegate syntax")
              ENSURE (*item->delim == '=' || *item->delim == '<', "default must be a configentry with key=value or key<delegate syntax");
              TRACE (config, "registering default: '%s'", item->line);

              llist_insert_head (&lumiera_global_config->defaults.childs, &item->link);
              item->parent = &lumiera_global_config->defaults;

              lumiera_config_lookup_insert_default (&lumiera_global_config->keys, item);
            }
        }
    }

  return item;
}


int
lumiera_config_reset (const char* key)
{
  TRACE (config);
  UNIMPLEMENTED();
  return -1;
}


int
lumiera_config_info (const char* key, const char** filename, unsigned* line)
{
  TRACE (config);
  UNIMPLEMENTED();
  return -1;
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
