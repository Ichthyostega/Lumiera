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

//TODO: Support library includes//
#include "lib/cuckoo.h"
#include "lib/llist.h"


//TODO: Forward declarations//


//TODO: Lumiera header includes//


//TODO: System includes//
#include <nobug.h>


/**
 * @file
 *
 */

#define LUMIERA_CONFIG_KEY_MAX 1023



//TODO: declarations go here//

/*
  Lookup uses a cuckoo hash for now
*/

struct lumiera_config_lookup_struct
{
  Cuckoo hash;
};
typedef struct lumiera_config_lookup_struct lumiera_config_lookup;
typedef lumiera_config_lookup* LumieraConfigLookup;

LumieraConfigLookup
lumiera_config_lookup_init (LumieraConfigLookup self);

LumieraConfigLookup
lumiera_config_lookup_destroy (LumieraConfigLookup self);



/*
  Lookup hash entries for the cuckoo hash
*/

struct lumiera_config_lookupentry_struct
{
  /*
    we store a copy of the full key here
    configentry keys are complete as expected
    section keys are the prefix stored with a trailing dot
  */
  char* full_key;
  /* stack of all configitems stored under this key */
  llist configitems;
};
typedef struct lumiera_config_lookupentry_struct lumiera_config_lookupentry;
typedef lumiera_config_lookupentry* LumieraConfigLookupentry;

LumieraConfigLookupentry
lumiera_config_lookupentry_init (const char* prefix, const char* name, const char* suffix);

void
lumiera_config_lookupentry_destroy (LumieraConfigLookupentry self);


#endif
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
