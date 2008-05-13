/*
  mrucache.h  -  most recent used cache

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

#ifndef LUMIERA_CACHE_H
#define LUMIERA_CACHE_H

#include "lib/llist.h"

#include <nobug.h>

/**
 * @file Most recent used cache
 * Elements (addressed by a LList node) are either in the cache and thereby subject of aging
 * or checked out under control of the user.
 */

/**
 * Callback function used to destruct/cleanup aged elements.
 * shall clean the element sufficiently up to be ready for being freed or reused
 * @param node the llist node used to link cache elements (will be empty at call)
 * @return pointer to the begin of the element.
 */
typedef void* (*lumiera_cache_destructor_fn)(LList node);

struct lumiera_mrucache_struct
{
  llist cache_list;
  size_t cached;
  lumiera_cache_destructor_fn destructor_cb;
};
typedef struct lumiera_mrucache_struct lumiera_mrucache;
typedef lumiera_mrucache* LumieraMruCache;

/**
 * Initialize a mrucache
 */
LumieraMruCache
lumiera_mrucache_init (LumieraMruCache self, lumiera_cache_destructor_fn destructor_cb);


/**
 * Destroy a mrucache
 */
LumieraMruCache
lumiera_mrucache_destroy (LumieraMruCache self);


/**
 * add an element to a mrucache
 */
static inline void
lumiera_mrucache_add (LumieraMruCache self, LList node)
{
  REQUIRE (self);
  REQUIRE (node && llist_is_empty (node));
  llist_insert_head (&self->cache_list, node);
  ++self->cached;
}


/**
 * Remove an element from a mrucache.
 */
static inline void
lumiera_mrucache_remove (LumieraMruCache self, LList node)
{
  REQUIRE (self);
  REQUIRE (node && llist_is_member (&self->cache_list, node));   /* speedup loop warning :P, this check is costly */
  llist_unlink (node);
  --self->cached;
}

/**
 * destroy the oldest element and return a pointer to it for reuse.
 */
static inline void*
lumiera_mrucache_pop (LumieraMruCache self)
{
  REQUIRE (self);
  if (llist_is_empty (&self->cache_list))
    return NULL;

  LList node =  llist_tail (&self->cache_list);
  llist_unlink (node);
  --self->cached;

  if (self->destructor_cb)
    return self->destructor_cb (node);
  else
    return node;
}


/**
 * destroy and free the nelem oldest elements
 */
int
lumiera_mrucache_age (LumieraMruCache self, int nelem);


#endif
