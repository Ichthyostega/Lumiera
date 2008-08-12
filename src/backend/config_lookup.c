/*
  config_lookup.c  -  Lookup functions for the config subsystem

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
#include "backend/config_lookup.h"
#include "backend/config.h"

//TODO: System includes//
//#include <stdlib.h>
//#include <stdint.h>

//TODO: internal/static forward declarations//
static size_t
h1 (const void* item, const uint32_t r);

static size_t
h2 (const void* item, const uint32_t r);

static size_t
h3 (const void* item, const uint32_t r);

static int
cmp (const void* keya, const void* keyb);

static int
cmp (const void* keya, const void* keyb);

/**
 * @file
 *
 */


//code goes here//



LumieraConfigLookup
lumiera_config_lookup_init (LumieraConfigLookup self)
{
  self->hash = cuckoo_new (h1, h2, h3, cmp, sizeof (lumiera_config_lookupentry), 3, NULL, NULL); // TODO copy func, dtor
  return self;
}


LumieraConfigLookup
lumiera_config_lookup_destroy (LumieraConfigLookup self)
{
  cuckoo_delete (self->hash);
  return self;
}






LumieraConfigLookupentry
lumiera_config_lookupentry_new (const char* prefix, const char* name, const char* suffix)
{
  char* tmpstr = lumiera_tmpbuf_snprintf (LUMIERA_CONFIG_KEY_MAX, "%s%s%s%s%s",
                                          prefix?prefix:"", prefix?".":"",
                                          name?name:"",
                                          suffix?".":"", suffix?suffix:"");

  TRACE (config_lookup, "new key %s", tmpstr);

  LumieraConfigLookupentry self = lumiera_malloc (sizeof (*self));

  self->full_key = lumiera_strndup (tmpstr, LUMIERA_CONFIG_KEY_MAX);
  llist_init (&self->configitems);

  return self;
}

void
lumiera_config_lookupentry_delete (LumieraConfigLookupentry self)
{
  TRACE (config_lookup);

  lumiera_free (self->full_key);
  ENSURE (llist_is_empty (&self->configitems));
  lumiera_free (self);
}




/*
  Support functions for the cuckoo hash
*/

static size_t
h1 (const void* item, const uint32_t r)
{
  (void) item;
  return r;
}

static size_t
h2 (const void* item, const uint32_t r)
{
  (void) item;
  return r;
}

static size_t
h3 (const void* item, const uint32_t r)
{
  (void) item;
  return r;
}

static int
cmp (const void* keya, const void* keyb)
{
  (void) keya;
  (void) keyb;

  return 0;
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
