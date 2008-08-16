/*
  config_lookup.h  -  Lookup functions for the config subsystem

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

#ifndef LUMIERA_CONFIG_LOOKUP_H
#define LUMIERA_CONFIG_LOOKUP_H

#include "lib/cuckoo.h"
#include "lib/llist.h"


typedef struct lumiera_config_lookup_struct lumiera_config_lookup;
typedef lumiera_config_lookup* LumieraConfigLookup;

typedef struct lumiera_config_lookupentry_struct lumiera_config_lookupentry;
typedef lumiera_config_lookupentry* LumieraConfigLookupentry;


#include "backend/configitem.h"


#include <nobug.h>


/**
 * @file
 *
 */


/*
  Lookup uses a cuckoo hash for now
*/

struct lumiera_config_lookup_struct
{
  Cuckoo hash;
};

LumieraConfigLookup
lumiera_config_lookup_init (LumieraConfigLookup self);

LumieraConfigLookup
lumiera_config_lookup_destroy (LumieraConfigLookup self);

LumieraConfigLookupentry
lumiera_config_lookup_insert (LumieraConfigLookup self, LumieraConfigitem item);

LumieraConfigitem
lumiera_config_lookup_remove (LumieraConfigLookup self, LumieraConfigitem item);

LumieraConfigLookupentry
lumiera_config_lookup_find (LumieraConfigLookup self, const char* key);

LumieraConfigitem
lumiera_config_lookup_item_find (LumieraConfigLookup self, const char* key);



/*
  Lookup hash entries for the cuckoo hash
*/

struct lumiera_config_lookupentry_struct
{
  /* stack of all configitems stored under this key MUST BE FIRST IN THIS STRUCT */
  llist configitems;
  /*
    we store a copy of the full key here
    configentry keys are complete as expected
    section keys are the prefix stored with a trailing dot, suffixes will be found by iteration
  */
  char* full_key;
};


LumieraConfigLookupentry
lumiera_config_lookupentry_init (LumieraConfigLookupentry self, const char* key);

LumieraConfigLookupentry
lumiera_config_lookupentry_destroy (LumieraConfigLookupentry self);


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
