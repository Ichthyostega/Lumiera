/*
  Config-wordlist  -  Lumiera wordlist access functions

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file config-wordlist.c
 ** Draft for a configuration system (2008).
 ** Part of this is the implementation of a word list with
 ** search and access functions
 ** @todo as of 2016 this code is unused and
 **       likely to be replaced by a different approach.
 */


#include "include/logging.h"
#include "lib/error.h"
#include "lib/tmpbuf.h"


#include "common/config.h"

extern LumieraConfig lumiera_global_config;


/**
 * return nth word of a wordlist
 */
const char*
lumiera_config_wordlist_get_nth (const char* key, unsigned nth, const char* delims)
{
  const char* value;
  size_t len;

  if (!lumiera_config_wordlist_get (key, &value))
    return NULL;

  for (;;)
    {
      value += strspn (value, delims);
      len = strcspn (value, delims);
      if (!nth && *value)
        break;

      --nth;
      value += len;

      if (!*value)
        return NULL;
    }

  return lumiera_tmpbuf_strndup (value, len);
}


int
lumiera_config_wordlist_find (const char* key, const char* value, const char* delims)
{
  const char* itr;
  size_t vlen = strlen (value);
  size_t len;

  if (!lumiera_config_wordlist_get (key, &itr))
    return -1;

  for (int idx = 0; *itr; itr += len, ++idx)
    {
      itr += strspn (itr, delims);
      len = strcspn (itr, delims);

      if (len == vlen && !strncmp (itr, value, vlen))
        return idx;
    }

  return -1;
}


const char*
lumiera_config_wordlist_replace (const char* key, const char* value, const char* subst1, const char* subst2, const char* delims)
{
  const char* wordlist;
  const char* str = NULL;
  size_t vlen = strlen (value);
  size_t len;

  if (!value)
    return NULL;

  LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_global_config->lock)
    {
      if (lumiera_config_get (key, &wordlist))
        {
          const char* start = wordlist + strspn (wordlist, " \t,;");

          for (const char* itr = start; *itr; itr += len)
            {
              const char* left_end = itr;
              itr += strspn (itr, delims);
              len = strcspn (itr, delims);

              if (len == vlen && !strncmp (itr, value, vlen))
                {
                  /* step over the word */
                  itr += len;
                  itr += strspn (itr, delims);

                  /* getting the delimiters right for the corner cases looks ugly, want to refactor it? just do it */
                  str = lumiera_tmpbuf_snprintf (SIZE_MAX,
                                                 "%.*s%.*s%.1s%s%.1s%s%.1s%s",
                                                 start - wordlist, wordlist,
                                                 left_end - start, start,
                                                 (left_end - start && subst1 && *subst1) ? delims : "",
                                                 (subst1 && *subst1) ? subst1 : "",
                                                 ((left_end - start || (subst1 && *subst1)) && subst2 && *subst2) ? delims : "",
                                                 (subst2 && *subst2) ? subst2 : "",
                                                 ((left_end - start || (subst1 && *subst1) || (subst2 && *subst2)) && *itr) ? delims : "",
                                                 itr
                                                 );

                  if (!lumiera_config_set (key, lumiera_tmpbuf_snprintf (SIZE_MAX, "=%s", str)))
                    str = NULL;

                  break;
                }
            }
        }
    }

  return str;
}


const char*
lumiera_config_wordlist_add (const char* key, const char* value, const char* delims)
{
  const char* wordlist = NULL;

  if (value && *value)
    {
      LUMIERA_MUTEX_SECTION (mutex_sync, &lumiera_global_config->lock)
        {
          if (lumiera_config_get (key, &wordlist))
            {
              size_t vlen = strlen (value);
              size_t len;

              for (const char* itr = wordlist; *itr; itr += len)
                {
                  itr += strspn (itr, delims);
                  len = strcspn (itr, delims);

                  if (len == vlen && !strncmp (itr, value, vlen))
                    goto end;
                }

              wordlist = lumiera_tmpbuf_snprintf (SIZE_MAX, "%s%.1s%s",
                                                  wordlist,
                                                  wordlist[strspn (wordlist, delims)] ? delims : "",
                                                  value);

              if (!lumiera_config_set (key, lumiera_tmpbuf_snprintf (SIZE_MAX, "=%s", wordlist)))
                wordlist = NULL;
            }
        end:;
        }
    }

  return wordlist;
}


#if 0

const char*
lumiera_config_wordlist_remove_nth (const char* key, unsigned nth)
{
}


LumieraConfigitem
lumiera_config_wordlist_append (const char* key, const char** value, unsigned nth)
{
}


LumieraConfigitem
lumiera_config_wordlist_preprend (const char* key, const char** value, unsigned nth)
{
}
#endif



/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
