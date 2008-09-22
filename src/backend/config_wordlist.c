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

#if 0
int
lumiera_config_wordlist_find (const char* key, const char* value)
{
}


LumieraConfigitem
lumiera_config_wordlist_set_nth (const char* key, const char** value, unsigned nth)
{
}

LumieraConfigitem
lumiera_config_wordlist_append (const char* key, const char** value, unsigned nth)
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
