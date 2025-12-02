/*
  Config-typed  -  Lumiera configuration high-level interface

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file config-typed.c
 ** Implementation draft for a configuration system (2008).
 ** This file implements high level typed configuration interfaces.
 ** @note unfinished draft from 2008
 ** @todo as of 2016 this code is unused and
 **       likely to be replaced by a different approach.
 */

#include "include/logging.h"
#include "lib/tmpbuf.h"
#include "common/config.h"

#include <stdint.h>



extern LumieraConfig lumiera_global_config;




const char*
lumiera_config_link_get (const char* key, const char** value)
{
  (void) key; (void) value;
  TRACE (configtyped_dbg);
  UNIMPLEMENTED();
  return 0;
}

LumieraConfigitem
lumiera_config_link_set (const char* key, const char** value)
{
  (void) key; (void) value;
  TRACE (configtyped_dbg);
  UNIMPLEMENTED();
  return 0;
}


/**
 * Number
 * signed integer numbers, in different formats (decimal, hex, oct, binary(for masks))
 */
const char*
lumiera_config_number_get (const char* key, long long* value)
{
  TRACE (configtyped_dbg);

  const char* raw_value = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_global_config->lock)
    {
      if (lumiera_config_get (key, &raw_value))
        {
          if (raw_value)
            {
              /* got it, scan it */
              if (sscanf (raw_value, "%Li", value) != 1)
                {
                  LUMIERA_ERROR_SET (config, CONFIG_SYNTAX_VALUE, lumiera_tmpbuf_snprintf (5000, "key '%s', value '%s'", key, raw_value));
                  raw_value = NULL;
                }
            }
          else
            LUMIERA_ERROR_SET_WARNING (config, CONFIG_NO_ENTRY, key);
        }
    }

  return raw_value;
}

LumieraConfigitem
lumiera_config_number_set (const char* key, long long* value)
{
  TRACE (configtyped_dbg);

  LumieraConfigitem item = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_global_config->lock)
    {
      const char* fmt = "= %lld"; TODO ("use the config system (config.format*...) to deduce the desired format for this key");
      item = lumiera_config_set (key, lumiera_tmpbuf_snprintf (SIZE_MAX, fmt, *value));
    }

  return item;
}


/**
 * Real
 * floating point number in standard formats (see printf/scanf)
 */
const char*
lumiera_config_real_get (const char* key, long double* value)
{
  (void) key; (void) value;
  TRACE (configtyped_dbg);
  UNIMPLEMENTED();
  return 0;
}

LumieraConfigitem
lumiera_config_real_set (const char* key, long double* value)
{
  (void) key; (void) value;
  TRACE (configtyped_dbg);
  UNIMPLEMENTED();
  return 0;
}



/**
 * String
 * unquoted string which covers the whole value area and gets chopped or
 * quoted string which preserves leading/trailing spaces
 * either single or double quotes are allowed, doubling the quote in a string escapes it
 */


/**
 * helper function, takes a raw input string and give a tmpbuf with the string parsed back.
 */
static char*
scan_string (const char* in)
{
  /* chop leading blanks */
  in += strspn(in, " \t");

  char quote = *in;
  char* end;
  char* ret = NULL;

  if (quote == '"' || quote == '\'')
    {
      /* quoted string */
      ++in;
      end = strchr (in, quote);
      while (end && end[1] == quote)
        end = strchr (end + 2, quote);

      if (end)
        {
          ret = lumiera_tmpbuf_strndup (in, end - in);

          /* replace double quote chars with single one */
          char* wpos;
          char* rpos;
          for (wpos = rpos = ret; *rpos; ++rpos, ++wpos)
            {
              if (*rpos == quote)
                ++rpos;
              *wpos = *rpos;
            }
          *wpos = '\0';
        }
      else
        /* quotes doesn't match */
        LUMIERA_ERROR_SET (config, CONFIG_SYNTAX_VALUE, "unmatched quotes");
    }
  else
    {
      /* unquoted string */
      ret = lumiera_tmpbuf_strndup (in, SIZE_MAX);

      /* chop trailing blanks */
      end = ret + strlen (ret) - 1;
      while (end > ret && (*end == ' ' || *end == '\t'))
        *end-- = '\0';
    }

  return ret;
}

const char*
lumiera_config_string_get (const char* key, const char** value)
{
  TRACE (configtyped_dbg);

  const char* raw_value = *value = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_global_config->lock)
    {
      if (lumiera_config_get (key, &raw_value))
        {
          if (raw_value)
            {
              *value = scan_string (raw_value);
            }
          else
            LUMIERA_ERROR_SET_WARNING (config, CONFIG_NO_ENTRY, key);
        }
    }

  return *value;
}

LumieraConfigitem
lumiera_config_string_set (const char* key, const char** value)
{
  TRACE (configtyped_dbg);

  LumieraConfigitem item = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_global_config->lock)
    {
      const char* fmt = "= %s"; TODO ("use the config system (config.format*...) to deduce the desired format for this key");
      item = lumiera_config_set (key, lumiera_tmpbuf_snprintf (SIZE_MAX, fmt, *value));
    }

  return item;
}


/**
 * Wordlist
 * words delimited by any of " \t,;"
 */

const char*
lumiera_config_wordlist_get (const char* key, const char** value)
{
  TRACE (configtyped_dbg, "KEY %s", key);

  const char* raw_value = *value = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_global_config->lock)
    {
      if (lumiera_config_get (key, &raw_value))
        {
          if (raw_value)
            {
              *value = raw_value;
            }
          else
            LUMIERA_ERROR_SET_WARNING (config, CONFIG_NO_ENTRY, key);

          /////////////////////////TODO "remove the ERROR_SET because config_get sets it already? also in all other getters in this file
        }
    }

  return *value;
}


LumieraConfigitem
lumiera_config_wordlist_set (const char* key, const char** value)
{
  TRACE (configtyped_dbg);

  LumieraConfigitem item = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_global_config->lock)
    {
      const char* fmt = "= %s"; TODO ("use the config system (config.format*...) to deduce the desired format for this key");
      item = lumiera_config_set (key, lumiera_tmpbuf_snprintf (SIZE_MAX, fmt, *value));
    }

  return item;
}



/**
 * helper function, takes a raw input string and give a tmpbuf with the word parsed back.
 * @remarks 'Word' is a single word, no quotes, chopped
 */
static char*
scan_word (const char* in)
{
  /* chop leading blanks */
  in += strspn(in, " \t");

  char* ret = lumiera_tmpbuf_strndup (in, SIZE_MAX);
  char* end = ret;

  /* chop trailing blanks */
  while (*end && *end != ' ' && *end != '\t')
    ++end;

  *end++ = '\0';

  return ret;
}

const char*
lumiera_config_word_get (const char* key, const char** value)
{
  TRACE (configtyped_dbg, "KEY %s", key);

  const char* raw_value = *value = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_global_config->lock)
    {
      if (lumiera_config_get (key, &raw_value))
        {
          if (raw_value)
            {
              *value = scan_word (raw_value);
            }
          else
            LUMIERA_ERROR_SET_WARNING (config, CONFIG_NO_ENTRY, key);
        }
    }

  return *value;
}

LumieraConfigitem
lumiera_config_word_set (const char* key, const char** value)
{
  TRACE (configtyped_dbg);

  LumieraConfigitem item = NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_global_config->lock)
    {
      const char* fmt = "= %s"; ///////////////////////TODO use the config system (config.format*...) to deduce the desired format for this key
      item = lumiera_config_set (key, lumiera_tmpbuf_snprintf (SIZE_MAX, fmt, scan_word (*value)));
    }

  return item;
}


/**
 * Bool
 * Bool in various formats, (0,1(!1), yes/no, true/false, on/off, set/clear)
 */
const char*
lumiera_config_bool_get (const char* key, int* value)
{
  (void) key; (void) value;
  TRACE (configtyped_dbg);
  UNIMPLEMENTED("get bool from config system");
  return 0;
}


LumieraConfigitem
lumiera_config_bool_set (const char* key, int* value)
{
  (void) key; (void) value;
  TRACE (configtyped_dbg);
  UNIMPLEMENTED("set bool in config system");
  return 0;
}


/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
