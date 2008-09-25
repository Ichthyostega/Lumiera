/*
  config_wordlist.c  -  Lumiera wordlist access functions

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
#include "lib/error.h"
#include "lib/safeclib.h"


//TODO: Lumiera header includes//
#include "backend/config.h"

//TODO: internal/static forward declarations//
extern LumieraConfig lumiera_global_config;


//TODO: System includes//

/**
 * return nth word of a wordlist
 */
const char*
lumiera_config_wordlist_get_nth (const char* key, unsigned nth)
{
  const char* value;
  size_t len;

  if (!lumiera_config_wordlist_get (key, &value))
    return NULL;

  for (;;)
    {
      value += strspn (value, " \t,;");
      len = strcspn (value, " \t,;");
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
lumiera_config_wordlist_find (const char* key, const char* value)
{
  const char* itr;
  size_t vlen = strlen (value);
  size_t len;

  if (!lumiera_config_wordlist_get (key, &itr))
    return -1;

  for (int idx = 0; *itr; itr += len, ++idx)
    {
      itr += strspn (itr, " \t,;");
      len = strcspn (itr, " \t,;");

      if (len == vlen && !strncmp (itr, value, vlen))
        return idx;
    }

  return -1;
}


const char*
lumiera_config_wordlist_replace (const char* key, const char* value, const char* subst1, const char* subst2)
{
  const char* wordlist;
  const char* str = NULL;
  size_t vlen = strlen (value);
  size_t len;

  LUMIERA_MUTEX_SECTION (config_typed, &lumiera_global_config->lock)
    {
      if (lumiera_config_get (key, &wordlist))
        {
          const char* start = wordlist + strspn (wordlist, " \t,;");

          for (const char* itr = start; *itr; itr += len)
            {
              const char* left_end = itr;
              itr += strspn (itr, " \t,;");
              len = strcspn (itr, " \t,;");

              if (len == vlen && !strncmp (itr, value, vlen))
                {
                  TODO ("figure delimiter from original string out");
                  const char* delim = " ";

                  /* step over the word */
                  itr += len;
                  itr += strspn (itr, " \t,;");

                  /* getting the delimiters right for the corner cases looks ugly, want to refactor it? just do it */
                  str = lumiera_tmpbuf_snprintf (SIZE_MAX,
                                                 "%.*s%.*s%s%s%s%s%s%s",
                                                 start - wordlist, wordlist,
                                                 left_end - start, start,
                                                 (left_end - start && subst1 && *subst1) ? delim : "",
                                                 (subst1 && *subst1) ? subst1 : "",
                                                 ((left_end - start || (subst1 && *subst1)) && subst2 && *subst2) ? delim : "",
                                                 (subst2 && *subst2) ? subst2 : "",
                                                 ((left_end - start || (subst1 && *subst1) || (subst2 && *subst2)) && *itr) ? delim : "",
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



#if 0


LumieraConfigitem
lumiera_config_wordlist_remove_nth (const char* key, const char** value, unsigned nth)
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


LumieraConfigitem
lumiera_config_wordlist_add (const char* key, const char** value, unsigned nth)
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
