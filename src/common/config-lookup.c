/*
  Config-lookup  -  Lookup functions for the config subsystem

  Copyright (C)         Lumiera.org
    2008,               Christian Thaeter <ct@pipapo.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/


/** @file config-lookup.c
 ** Implementation of the lookup of configuration keys
 */


#include "include/logging.h"
#include "lib/safeclib.h"
#include "lib/tmpbuf.h"

#include "common/config-lookup.h"
#include "common/config.h"

/* we only use one fatal error for now, when allocation in the config system fail, something else is pretty wrong */
LUMIERA_ERROR_DEFINE (CONFIG_LOOKUP, "config lookup failure");



/* === support functions for the splay tree === */

static int
cmp_fn (const void* a, const void* b);

static void
delete_fn (PSplaynode node);

static const void*
key_fn (const PSplaynode node);




LumieraConfigLookup
lumiera_config_lookup_init (LumieraConfigLookup self)
{
  TRACE (configlookup_dbg);
  psplay_init (&self->tree, cmp_fn, key_fn, delete_fn);
  return self;
}


LumieraConfigLookup
lumiera_config_lookup_destroy (LumieraConfigLookup self)
{
  TRACE (configlookup_dbg);
  if (self)
    psplay_destroy (&self->tree);
  return self;
}


LumieraConfigLookupentry
lumiera_config_lookup_insert (LumieraConfigLookup self, LumieraConfigitem item)
{
  TRACE (configlookup_dbg, "%s", item->line);
  REQUIRE (self);
  REQUIRE (item);
  REQUIRE (item->key);
  REQUIRE (item->key_size);

  ////////////////////////////////////////TICKET #839 implement section prefix/suffix for the key"
  
  const char* key = lumiera_tmpbuf_strcat3 (NULL, 0, item->key, item->key_size, NULL, 0);

  LumieraConfigLookupentry entry = (LumieraConfigLookupentry)psplay_find (&self->tree, key, 100);
  if (!entry)
    entry = (LumieraConfigLookupentry)psplay_insert (&self->tree, &lumiera_config_lookupentry_new (key)->node, 100);

  llist_insert_head (&entry->configitems, &item->lookup);
  return entry;
}


LumieraConfigLookupentry
lumiera_config_lookup_insert_default (LumieraConfigLookup self, LumieraConfigitem item)
{
  TRACE (configlookup_dbg, "%s", item->line);
  REQUIRE (self);
  REQUIRE (item);
  REQUIRE (item->key);
  REQUIRE (item->key_size);

  const char* key = lumiera_tmpbuf_snprintf (SIZE_MAX, "%.*s", item->key_size, item->key);
  LumieraConfigLookupentry entry = (LumieraConfigLookupentry)psplay_find (&self->tree, key, 100);
  if (!entry)
    entry = (LumieraConfigLookupentry)psplay_insert (&self->tree, &lumiera_config_lookupentry_new (key)->node, 100);
  
   ////////////////////////////////////////TICKET #839 check that no 'default' item already exists when inserting a default
   ////////////////////////////////////////TICKET #839 ...that is, the tail element's parent points to the 'defaults' in config

  llist_insert_tail (&entry->configitems, &item->lookup);
  return entry;
}


LumieraConfigitem
lumiera_config_lookup_remove (LumieraConfigLookup self, LumieraConfigitem item)
{
  TRACE (configlookup_dbg, "%s", item->line);
  REQUIRE (!llist_is_empty (&item->lookup), "item is not in a lookup");

  if (llist_is_single (&item->lookup))
    {
      /* last item in lookup, remove it from the splay tree */
      LumieraConfigLookupentry entry = LLIST_TO_STRUCTP (llist_next (&item->lookup), lumiera_config_lookupentry, configitems);
      llist_unlink (&item->lookup);
      psplay_delete_node (&self->tree, (PSplaynode)entry);
    }
  else
    {
      /* more than this item present in hash, just unlink this item */
      llist_unlink (&item->lookup);
    }

  return item;
}


LumieraConfigLookupentry
lumiera_config_lookup_find (LumieraConfigLookup self, const char* key)
{
  TRACE (configlookup_dbg, "%s", key);
  return (LumieraConfigLookupentry)psplay_find (&self->tree, key, 100);
}


LumieraConfigitem
lumiera_config_lookup_item_find (LumieraConfigLookup self, const char* key)
{
  TRACE (configlookup_dbg, "%s", key);

  LumieraConfigLookupentry entry =
    lumiera_config_lookup_find (self, key);

  if (entry && !llist_is_empty (&entry->configitems))
    return LLIST_TO_STRUCTP (llist_head (&entry->configitems), lumiera_configitem, lookup);

  return NULL;
}


LumieraConfigitem
lumiera_config_lookup_item_tail_find (LumieraConfigLookup self, const char* key)
{
  TRACE (configlookup_dbg, "%s", key);

  LumieraConfigLookupentry entry =
    lumiera_config_lookup_find (self, key);

  if (entry && !llist_is_empty (&entry->configitems))
    return LLIST_TO_STRUCTP (llist_tail (&entry->configitems), lumiera_configitem, lookup);

  return NULL;
}



/* === Lookup of entries === */

LumieraConfigLookupentry
lumiera_config_lookupentry_init (LumieraConfigLookupentry self, const char* key)
{
  TRACE (configlookup_dbg, "%s", key);
  if (self)
    {
      psplaynode_init (&self->node);
      llist_init (&self->configitems);
      self->full_key = lumiera_strndup (key, SIZE_MAX);
    }
  return self;
}


LumieraConfigLookupentry
lumiera_config_lookupentry_new (const char* key)
{
  return lumiera_config_lookupentry_init (lumiera_malloc (sizeof (lumiera_config_lookupentry)), key);
}


LumieraConfigLookupentry
lumiera_config_lookupentry_destroy (LumieraConfigLookupentry self)
{
  TRACE (configlookup_dbg);
  if (self)
    {
      REQUIRE (llist_is_empty (&self->configitems), "lookup node still in use");
      lumiera_free (self->full_key);
    }
  return self;
}


void
lumiera_config_lookupentry_delete (LumieraConfigLookupentry self)
{
  lumiera_free (lumiera_config_lookupentry_destroy (self));
}

static int
cmp_fn (const void* a, const void* b)
{
  return strcmp ((const char*)a, (const char*)b);
}

static void
delete_fn (PSplaynode node)
{
  lumiera_config_lookupentry_delete ((LumieraConfigLookupentry) node);
}


static const void*
key_fn (const PSplaynode node)
{
  return ((LumieraConfigLookupentry) node)->full_key;
}

/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
