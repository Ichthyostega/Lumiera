/*
  CONFIG-LOOKUP.h  -  Lookup functions for the config subsystem

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file config-lookup.h
 ** Lookup of configuration keys in a low-level configuration system.
 ** Configuration keys are dynamically stored in a splay tree.
 ** This happens for defaults, loaded config files and entries which are set explicitly.
 ** The system maintains no central registry of all possible keys.
 ** We store here the full keys of config entries as well as the keys of section prefixes.
 ** Section prefixes are stored with a trailing dot to disambiguate them from entry keys.
 ** 
 ** @warning since 2012 it is not clear if we retain this kind of configuration system.
 ** @todo as of 2016, the code is still there but remains mostly unused
 */


#ifndef COMMON_CONFIG_LOOKUP_H
#define COMMON_CONFIG_LOOKUP_H

#include "lib/psplay.h"
#include "lib/llist.h"
#include "lib/error.h"

typedef struct lumiera_config_lookup_struct lumiera_config_lookup;
typedef lumiera_config_lookup* LumieraConfigLookup;

typedef struct lumiera_config_lookupentry_struct lumiera_config_lookupentry;
typedef lumiera_config_lookupentry* LumieraConfigLookupentry;


#include "common/configitem.h"

#include <nobug.h>


LUMIERA_ERROR_DECLARE (CONFIG_LOOKUP);


/**
 * Just contains a hashtable to give sufficient abstraction.
 */
struct lumiera_config_lookup_struct
{
  psplay tree;
};

/**
 * Initialise a lookup structure.
 * @param self lookup structure to be initialised
 * @return self on success else NULL
 */
LumieraConfigLookup
lumiera_config_lookup_init (LumieraConfigLookup self);

/**
 * Destroy a lookup structure.
 * @param self lookup structure to be destroyed
 * @return self
 */
LumieraConfigLookup
lumiera_config_lookup_destroy (LumieraConfigLookup self);

/**
 * Add a config item to a lookup structure.
 * Config items are stored under their key and stacked in insertion order.
 * @param self lookup structure where the item shall be added
 * @param item config item to add to the lookup structure
 * @return opaque pointer to a hashtable entry
 */
LumieraConfigLookupentry
lumiera_config_lookup_insert (LumieraConfigLookup self, LumieraConfigitem item);


/**
 * @internal Add a default config item to a lookup structure.
 * The item must contain a full key and not part of any 'section'
 * and is inserted as tail of the lookup list.
 * @param self lookup structure where the item shall be added
 * @param item config item to add to the lookup structure
 * @return opaque pointer to a hashtable entry
 */
LumieraConfigLookupentry
lumiera_config_lookup_insert_default (LumieraConfigLookup self, LumieraConfigitem item);


/**
 * Remove a config item from a lookup structure.
 * Config must be removed from the lookup when they are not used anymore.
 * Removing a config item unlinks it from the stack of all config items with the same key.
 * When this was the last config item under that key, the lookup entry is cleaned up.
 * @param self lookup structure where the item shall be removed
 * @param item config item to be removed from the lookup
 * @return item
 */
LumieraConfigitem
lumiera_config_lookup_remove (LumieraConfigLookup self, LumieraConfigitem item);

/**
 * Find a hashtable entry in the lookup structure.
 * Internal function, can be used to check if at least one item is available for a given key.
 * @param self lookup structure where the key shall be searched
 * @param key string to be looked up
 * @return NULL if nothing is found, otherwise a opaque pointer to a hash table entry
 */
LumieraConfigLookupentry
lumiera_config_lookup_find (LumieraConfigLookup self, const char* key);

/**
 * Find a the topmost config item stored to a given key.
 * @param self lookup structure where the key shall be searched
 * @param key string to be looked up
 * @return the config item which was last stored under the given key or NULL when nothing was found
 */
LumieraConfigitem
lumiera_config_lookup_item_find (LumieraConfigLookup self, const char* key);

/**
 * Find a the bottom most config item stored to a given key.
 * defaults sits at the bottom if exists
 * @param self lookup structure where the key shall be searched
 * @param key string to be looked up
 */
LumieraConfigitem
lumiera_config_lookup_item_tail_find (LumieraConfigLookup self, const char* key);



/* == lookup hash entries using the PSplay tree */


/** @internal Structure defining single hash table entries.*/
struct lumiera_config_lookupentry_struct
{
  psplaynode node;

  /** stack of all configitems stored under this key */
  llist configitems;

  /**
   * we store a copy of the full key here
   * configentry keys are complete as expected
   * section keys are the prefix stored with a trailing dot,
   * suffixes will be found by iterative search
   */
  char* full_key;
};


/* internal */
LumieraConfigLookupentry
lumiera_config_lookupentry_init (LumieraConfigLookupentry self, const char* key);

LumieraConfigLookupentry
lumiera_config_lookupentry_new (const char* key);

/* internal */
LumieraConfigLookupentry
lumiera_config_lookupentry_destroy (LumieraConfigLookupentry self);


void
lumiera_config_lookupentry_delete (LumieraConfigLookupentry self);


#endif /*COMMON_CONFIG_LOOKUP_H*/
/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
