/*
  Config  -  Lumiera configuration system

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file config.c
 ** Implementation of a draft for application configuration from 2008.
 ** @warning since 2012 it is unclear if we retain this approach
 ** @todo as of 2016 this code is not used and never was
 */


#include "include/logging.h"
#include "lib/safeclib.h"
#include "lib/tmpbuf.h"
#include "common/config.h"

#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>


LUMIERA_ERROR_DEFINE (CONFIG_SYNTAX, "syntax error in configfile");
LUMIERA_ERROR_DEFINE (CONFIG_SYNTAX_KEY, "syntax error in key");
LUMIERA_ERROR_DEFINE (CONFIG_SYNTAX_VALUE, "syntax error in value");
LUMIERA_ERROR_DEFINE (CONFIG_NO_ENTRY, "no configuration entry");


/**
 *  defaults for the configuration system itself
 */
const char* lumiera_config_defaults[] =
  {
    /* Low level formating, don't change these */
    "config.formatstr.link = '< %s'",
    "config.formatstr.number.dec = '= %lld'",
    "config.formatstr.number.hex = '= 0x%llX'",
    "config.formatstr.number.oct = '= 0%llo'",
    "config.formatstr.real = '= %Lg'",
    "config.formatstr.real.dec = '= %Lf'",
    "config.formatstr.real.sci = '= %Le'",
    "config.formatstr.string = '= %s'",
    "config.formatstr.string.dquoted = '= \"%s\"'",
    "config.formatstr.string.quoted = '= ''%s'''",
    "config.formatstr.word = '= %s'",
    "config.formatstr.bool = '= %d'",

    /* default representations per type */
    "config.formatdef.link < config.formatstr.link",
    "config.formatdef.number < config.formatstr.number.dec",
    "config.formatdef.real < config.formatstr.real",
    "config.formatdef.string < config.formatstr.string",
    "config.formatdef.word < config.formatstr.word",
    "config.formatdef.bool < config.formatstr.bool",

    /* per key formatting override stored under */
    "config.formatkey ='config.format.%s'",

    NULL
  };


/* singleton config */
LumieraConfig lumiera_global_config = NULL;


int
lumiera_config_init (const char* path)
{
  TRACE (config_dbg);
  REQUIRE (!lumiera_global_config, "Configuration subsystem already initialised");
  REQUIRE (path);


  lumiera_global_config = lumiera_malloc (sizeof (*lumiera_global_config));
  lumiera_config_lookup_init (&lumiera_global_config->keys);

  lumiera_configitem_init (&lumiera_global_config->defaults);
  lumiera_configitem_init (&lumiera_global_config->files);
  lumiera_configitem_init (&lumiera_global_config->TODO_unknown);

  lumiera_mutex_init (&lumiera_global_config->lock, "config mutex", &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);

  lumiera_config_setdefault (lumiera_tmpbuf_snprintf (SIZE_MAX, "config.path = %s", path));

  for (const char** itr = lumiera_config_defaults; *itr; ++itr)
    {
      lumiera_config_setdefault (*itr);
    }

  return 0;
}


void
lumiera_config_destroy ()
{
  TRACE (config_dbg);
  if (lumiera_global_config)
    {
      lumiera_mutex_destroy (&lumiera_global_config->lock, &NOBUG_FLAG (mutex_dbg), NOBUG_CONTEXT);
      lumiera_configitem_destroy (&lumiera_global_config->defaults, &lumiera_global_config->keys);
      lumiera_configitem_destroy (&lumiera_global_config->files, &lumiera_global_config->keys);
      lumiera_configitem_destroy (&lumiera_global_config->TODO_unknown, &lumiera_global_config->keys);
      lumiera_config_lookup_destroy (&lumiera_global_config->keys);
      lumiera_free (lumiera_global_config);
      lumiera_global_config = NULL;
    }
  else
    WARN (config_dbg, "Config system not initialised or already destroyed");
}


int
lumiera_config_load (const char* file)
{
  (void) file;
  TRACE (config_dbg);
  UNIMPLEMENTED();
  return -1;
}


int
lumiera_config_save ()
{
  TRACE (config_dbg);
  UNIMPLEMENTED();
  return -1;
}


int
lumiera_config_purge (const char* filename)
{
  (void) filename;
  TRACE (config_dbg);

  UNIMPLEMENTED();
  return -1;
}


const char*
lumiera_config_get (const char* key, const char** value)
{
  TRACE (config_dbg);
  REQUIRE (key);
  REQUIRE (value);

  *value = NULL;

  /* we translate the key for the env var override by making it uppercase and replace . with _,
   as side effect, this also checks the key syntax */
  char* tr_key = lumiera_tmpbuf_tr (key,
                                    LUMIERA_CONFIG_KEY_CHARS,
                                    LUMIERA_CONFIG_ENV_CHARS,
                                    NULL);

  if (tr_key)
    {
      char* env = lumiera_tmpbuf_snprintf (2048, "LUMIERA_%s", tr_key);

      *value = getenv (env);
      if (*value)
        {
          NOTICE (config, "envvar override for config %s = %s", env, *value);
        }
      else
        {
          /////////////////////////TODO "follow '<' delegates?";
          LumieraConfigitem item = lumiera_config_lookup_item_find (&lumiera_global_config->keys, key);

          if (item)
            {
              *value = item->delim+1;
            }
          else
            LUMIERA_ERROR_SET_WARNING (config, CONFIG_NO_ENTRY, key);
        }
    }
  else
    {
      LUMIERA_ERROR_SET (config, CONFIG_SYNTAX_KEY, key);
    }

  return *value;
}


const char*
lumiera_config_get_default (const char* key, const char** value)
{
  TRACE (config_dbg);
  REQUIRE (key);
  REQUIRE (value);

  *value = NULL;

          /////////////////////////TODO follow '<' delegates?
          /////////////////////////TODO refactor _get and get_default to iterator access (return LList or Lookupentry)
  LumieraConfigitem item = lumiera_config_lookup_item_tail_find (&lumiera_global_config->keys, key);

  if (item && item->parent == &lumiera_global_config->defaults)
    {
      *value = item->delim+1;
    }

  return *value;
}


LumieraConfigitem
lumiera_config_set (const char* key, const char* delim_value)
{
  TRACE (config_dbg);

  LumieraConfigitem item = lumiera_config_lookup_item_find (&lumiera_global_config->keys, key);
  if (item && item->parent != &lumiera_global_config->defaults)
    {
      TODO ("is a user writable file?");
      TODO ("       replace delim_value");
      lumiera_configitem_set_value (item, delim_value);
    }
  else
    {
      TODO ("create item");
      TODO ("       find matching prefix");
      TODO ("       find matching suffix");
      TODO ("       find proper prefix indentation, else use config.indent");
      TODO ("       create configitem with prefix/suffix removed");

      char* line = lumiera_tmpbuf_snprintf (SIZE_MAX, "%s %s", key, delim_value);
      item = lumiera_configitem_new (line);

      if (item)
        {
          TODO ("next 2 ensure must generate runtime errors");
          ENSURE (item->delim, "syntax error");
          ENSURE (*item->delim == '=' || *item->delim == '<', "syntax error,");

          TODO ("insert in proper parent (file)");
          llist_insert_tail (&lumiera_global_config->TODO_unknown.children, &item->link);
          item->parent = &lumiera_global_config->TODO_unknown;
          lumiera_config_lookup_insert (&lumiera_global_config->keys, item);

          TODO ("tag file as dirty");
        }
    }

  return item;
}


LumieraConfigitem
lumiera_config_setdefault (const char* line)
{
  TRACE (config_dbg);
  REQUIRE (line);

  LumieraConfigitem item = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_global_config->lock)
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
              ENSURE (item->delim, "default must be a configentry with key=value or key<delegate syntax");
              ENSURE (*item->delim == '=' || *item->delim == '<', "default must be a configentry with key=value or key<delegate syntax");
              TRACE (config_dbg, "registering default: '%s'", item->line);

              llist_insert_head (&lumiera_global_config->defaults.children, &item->link);
              item->parent = &lumiera_global_config->defaults;

              lumiera_config_lookup_insert (&lumiera_global_config->keys, item);
            }
        }
    }

  return item;
}


void
lumiera_config_dump (FILE* out)
{
  fprintf (out, "# registered defaults:\n");

  LLIST_FOREACH (&lumiera_global_config->defaults.children, node)
    fprintf (out, "%s\n", ((LumieraConfigitem) node)->line);

  fprintf (out, "# end of defaults\n\n");

#if 0 /*TODO UNIMPLEMENTED */
  fprintf (out, "# files:\n");
  lumiera_configitem files;
  fprintf (out, "# volatiles:")
  lumiera_configitem TODO_unknown;
#endif
}


int
lumiera_config_reset (const char* key)
{
  (void) key;
  TRACE (config_dbg);
  UNIMPLEMENTED();
  return -1;
}


int
lumiera_config_info (const char* key, const char** filename, unsigned* line)
{
  (void) key;
  (void) filename;
  (void) line;
  TRACE (config_dbg);
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
