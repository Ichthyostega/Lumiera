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
#include "lib/safeclib.h"

#include "backend/config_lookup.h"
#include "backend/config.h"

/* we only use one fatal error for now, when allocation in the config system fail, something else is pretty wrong */
LUMIERA_ERROR_DEFINE (CONFIG_LOOKUP, "config lookup failure");


static size_t
h1 (const void* item, const uint32_t r);

static size_t
h2 (const void* item, const uint32_t r);

static size_t
h3 (const void* item, const uint32_t r);

static int
cmp (const void* keya, const void* keyb);

static void
dtor (void* item);

static void
mov (void* dest, void* src, size_t size);



/**
 * @file
 * Implementation of the lookup of configuration keys using a cuckoo hash.
 */

LumieraConfigLookup
lumiera_config_lookup_init (LumieraConfigLookup self)
{
  TRACE (config_lookup);
  self->hash = cuckoo_new (sizeof (lumiera_config_lookupentry), &(struct cuckoo_vtable){h1, h2, h3, cmp, dtor, mov});

  if (!self->hash)
    LUMIERA_DIE (CONFIG_LOOKUP);

  return self;
}


LumieraConfigLookup
lumiera_config_lookup_destroy (LumieraConfigLookup self)
{
  TRACE (config_lookup);
  if (self)
    cuckoo_delete (self->hash);
  return self;
}


LumieraConfigLookupentry
lumiera_config_lookup_insert (LumieraConfigLookup self, LumieraConfigitem item)
{
  TRACE (config_lookup);
  REQUIRE (self);
  REQUIRE (item);
  REQUIRE (item->key);
  REQUIRE (item->key_size);

  lumiera_config_lookupentry tmp;
  FIXME ("implement section prefix/suffix for the key");
  lumiera_config_lookupentry_init (&tmp, lumiera_tmpbuf_strcat3 (NULL, 0, item->key, item->key_size, NULL, 0));

  LumieraConfigLookupentry entry = cuckoo_find (self->hash, &tmp);

  if (!entry)
    entry = cuckoo_insert (self->hash, &tmp);

  if (entry)
    llist_insert_head (&entry->configitems, &item->lookup);
  else
    LUMIERA_DIE (CONFIG_LOOKUP);

  return entry;
}


LumieraConfigLookupentry
lumiera_config_lookup_insert_default (LumieraConfigLookup self, LumieraConfigitem item)
{
  TRACE (config_lookup);
  REQUIRE (self);
  REQUIRE (item);
  REQUIRE (item->key);
  REQUIRE (item->key_size);

  lumiera_config_lookupentry tmp;
  lumiera_config_lookupentry_init (&tmp, lumiera_tmpbuf_snprintf (SIZE_MAX, "%.*s", item->key_size, item->key));

  LumieraConfigLookupentry entry = cuckoo_find (self->hash, &tmp);

  if (!entry)
    entry = cuckoo_insert (self->hash, &tmp);

  if (entry)
    llist_insert_tail (&entry->configitems, &item->lookup);
  else
    LUMIERA_DIE (CONFIG_LOOKUP);

  return entry;
}


LumieraConfigitem
lumiera_config_lookup_remove (LumieraConfigLookup self, LumieraConfigitem item)
{
  TRACE (config_lookup);
  REQUIRE (!llist_is_empty (&item->lookup), "item is not in a lookup hash");

  if (llist_is_single (&item->lookup))
    {
      /* last item in lookup, remove it from hash */
      LumieraConfigLookupentry entry = (LumieraConfigLookupentry)llist_next (&item->lookup);
      llist_unlink (&item->lookup);
      cuckoo_remove (self->hash, entry);
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
  TRACE (config_lookup);

  /* fast temporary init without strdup */
  lumiera_config_lookupentry tmp;
  tmp.full_key = (char*)key;

  return cuckoo_find (self->hash, &tmp);
}


LumieraConfigitem
lumiera_config_lookup_item_find (LumieraConfigLookup self, const char* key)
{
  TRACE (config_lookup);

  LumieraConfigLookupentry entry =
    lumiera_config_lookup_find (self, key);

  if (entry && !llist_is_empty (&entry->configitems))
    return LLIST_TO_STRUCTP (llist_head (&entry->configitems), lumiera_configitem, lookup);

  return NULL;
}

LumieraConfigitem
lumiera_config_lookup_item_tail_find (LumieraConfigLookup self, const char* key)
{
  TRACE (config_lookup);

  LumieraConfigLookupentry entry =
    lumiera_config_lookup_find (self, key);

  if (entry && !llist_is_empty (&entry->configitems))
    return LLIST_TO_STRUCTP (llist_tail (&entry->configitems), lumiera_configitem, lookup);

  return NULL;
}



/*
  Hash table entries
*/

LumieraConfigLookupentry
lumiera_config_lookupentry_init (LumieraConfigLookupentry self, const char* key)
{
  TRACE (config_lookup, "key = %s", key);
  self->full_key = lumiera_strndup (key, SIZE_MAX);
  llist_init (&self->configitems);
  return self;
}


void
lumiera_config_lookupentry_delete (LumieraConfigLookupentry self)
{
  TRACE (config_lookup);
  REQUIRE (llist_is_empty (&self->configitems), "lookup node still in use");
  lumiera_free (self->full_key);
}




/*
  Support functions for the cuckoo hash
*/

static size_t
h1 (const void* item, const uint32_t r)
{
  REQUIRE (item);
  REQUIRE (((LumieraConfigLookupentry)item)->full_key);
  size_t hash = r;
  for (const char* s = ((LumieraConfigLookupentry)item)->full_key; *s; ++s)
    hash = *s ^ ~(*s << 5) ^ (hash << 3) ^ (hash >> 7);
  return hash;
}

static size_t
h2 (const void* item, const uint32_t r)
{
  REQUIRE (item);
  REQUIRE (((LumieraConfigLookupentry)item)->full_key);
  size_t hash = r;
  for (const char* s = ((LumieraConfigLookupentry)item)->full_key; *s; ++s)
    hash = *s ^ ~(*s << 7) ^ (hash << 3) ^ (hash >> 5);
  return hash;
}

static size_t
h3 (const void* item, const uint32_t r)
{
  REQUIRE (item);
  REQUIRE (((LumieraConfigLookupentry)item)->full_key);
  size_t hash = r;
  for (const char* s = ((LumieraConfigLookupentry)item)->full_key; *s; ++s)
    hash = *s ^ ~(*s << 3) ^ (hash << 5) ^ (hash >> 7);
  return hash;
}

static int
cmp (const void* keya, const void* keyb)
{
  REQUIRE (keya);
  REQUIRE (((LumieraConfigLookupentry)keya)->full_key);
  REQUIRE (keyb);
  REQUIRE (((LumieraConfigLookupentry)keyb)->full_key);
  return !strcmp (((LumieraConfigLookupentry)keya)->full_key, ((LumieraConfigLookupentry)keyb)->full_key);
}

static void
dtor (void* item)
{
  if (((LumieraConfigLookupentry)item)->full_key)
    {
      REQUIRE (llist_is_empty (&((LumieraConfigLookupentry)item)->configitems), "lookup node still in use");
      lumiera_free (((LumieraConfigLookupentry)item)->full_key);
    }
}

static void
mov (void* dest, void* src, size_t size)
{
  ((LumieraConfigLookupentry)dest)->full_key = ((LumieraConfigLookupentry)src)->full_key;
  llist_init (&((LumieraConfigLookupentry)dest)->configitems);
  llist_insertlist_next (&((LumieraConfigLookupentry)dest)->configitems, &((LumieraConfigLookupentry)src)->configitems);
}


/*
// Local Variables:
// mode: C
// c-file-style: "gnu"
// indent-tabs-mode: nil
// End:
*/
