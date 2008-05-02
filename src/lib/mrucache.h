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
 * Elements (addressed by a LList node) are either checked in the cache and thereby subject of aging
 * or checked out under control of the user. Most operations require that the chache is locked. This locking
 * must be done from elsewhere.
 */

/**
 * Callback function used to destruct/cleanup aged elements.
 * shall clean the element sufficiently up to be ready for being freed or reused,
 * this callback function must be reentrant and prepared to be called twice.
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
 * Initialize a cache.
 * @param self cache to be initialized
 * @param destructor_cb function for destructing a cache node, preparing for reuse.
 * @return self
 */
LumieraMruCache
lumiera_mrucache_init (LumieraMruCache self, lumiera_cache_destructor_fn destructor_cb);

/**
 * Destroy a cache.
 * frees all checked in items.
 * @param self cache to be destroyed
 * @return self
 */
LumieraMruCache
lumiera_mrucache_destroy (LumieraMruCache self);


/**
 * Add an element to a mrucache.
 * When added the element is subject of aging and must not be accessed anymore.
 * To acces elements they have to be checked out again.
 * Checkin and checkout operations must be protected by a lock over the cache.
 * @param self cache where to checkin
 * @param node a llist member in the cached data which is used as queue in the cache
 */
static inline void
lumiera_mrucache_checkin (LumieraMruCache self, LList node)
{
  REQUIRE (self);
  REQUIRE (node && llist_is_empty (node));
  llist_insert_head (&self->cache_list, node);
  ++self->cached;
}


/**
 * Schedule an element for fast aging.
 * When an element is not longer needed it can be placed at the end of the chache
 * aging queue and thus become the first one to be reused when a new element is queried.
 * This can be done on a checked out element as well as on an element which is in the cache.
 * The cache must be locked for this operation.
 * @param self cache
 * @param node a llist member in the cached data which is used as queue in the cache
 */
static inline void
lumiera_mrucache_drop (LumieraMruCache self, LList node)
{
  REQUIRE (self);
  REQUIRE (node);

  if (llist_is_empty (node))
    {
      /* was not in list, we need to count it */
      ++self->cached;
    }
  else
    {
      /* speedup loop warning :P, this check is costly */
      REQUIRE (llist_is_member (&self->cache_list, node), "node must be empty or member of cache");
    }
  llist_insert_tail (&self->cache_list, node);

  if (self->destructor_cb)
    self->destructor_cb (node);
}


/**
 * Checkout an element from a cache.
 * A checked out element is not under cache control anymore until is gets checked in again.
 * The code which checked the element out takes ownership of the element.
 * The cache must be locked for this operation.
 * @param self cache
 * @param node a llist member in the cached data which is used as queue in the cache
 */
static inline void
lumiera_mrucache_checkout (LumieraMruCache self, LList node)
{
  REQUIRE (self);
  REQUIRE (node && llist_is_member (&self->cache_list, node));   /* speedup loop warning :P, this check is costly */
  llist_unlink (node);
  --self->cached;
}


/**
 * Destroy the oldest element from the cache and return it.
 * This function is used to get a new element by deleting the oldest least used one from the cache.
 * The cache must be locked for this operation.
 * @param self cache
 * @return pointer to the uninitialized memory ready for being reused
 */
static inline void*
lumiera_mrucache_pop (LumieraMruCache self)
{
  REQUIRE (self);
  if (llist_is_empty (&self->cache_list))
    return NULL;

  LList node = llist_tail (&self->cache_list);
  llist_unlink (node);
  --self->cached;

  if (self->destructor_cb)
    return self->destructor_cb (node);
  else
    return node;
}


/**
 * Destroy and free the nelem oldest elements.
 * Used to free up resources and memory.
 * @param self cache where to free elements.
 * @param nelem number of elements wished to be freed
 * @return nelem-(numer of elements which got freed), that is 0 if all requested elements got freed
 */
int
lumiera_mrucache_age (LumieraMruCache self, int nelem);


#endif
