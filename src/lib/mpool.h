/*
    mpool.h - memory pool for constant sized objects

  Copyright (C)         Lumiera.org
    2009,               Christian Thaeter <ct@pipapo.org>

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
*/


/** @file mpool.h
 ** Pooled memory allocator for constant sized objects
 ** *Memory Pools* are implemented as clusters of fixed sized elements. New clusters
 ** are allocated on demand or manually preallocated with a mpool_reserve() operation.
 ** Some efforts are taken to ensure (cache) locality of the provided memory.
 ** All functions are reentrant but not threadsafe, if this is desired it is advised to
 ** care for proper locking elsewhere.
 ** @warning as of 2016, this is a stalled development effort towards a pooling allocator.
 **          An initial working draft was created in 2009, but never challenged by any
 **          widespread use beyond some test code. We acknowledge that there _will be_
 **          some kind of optimised allocator -- yet for the time being we rely on
 **          several preliminary front-ends, which _could be attached_ to such an
 **          allocator eventually, but use plain flat heap allocations right now.
 */

#include <stdint.h>
#include <nobug.h>
#include "lib/llist.h"
#include "include/logging.h"

/**
 * Function prototype for destroying elements.
 * When a memory pool gets destroyed it can call a destructor for any element which is still in the pool.
 * Use of such a destructor is optional.
 * @param self element to be destroyed
 */
typedef void (*mpool_destroy_fn)(void* self);


/**
 * memory pool management structure.
 * This structure should be considered opaque.
 */
typedef struct mpool_struct mpool;
typedef mpool* MPool;
typedef const mpool* const_MPool;

struct mpool_struct
{
  llist freelist;
  llist clusters;
  size_t elem_size;
  unsigned elements_per_cluster;
  uintptr_t cluster_size;
  unsigned elements_free;               /* a counter of free elements is the price we pay to support a reserve() operation */
  void* locality;
  mpool_destroy_fn destroy;
  void *(*malloc_hook)(size_t);
  void (*free_hook)(void *);
  void* udata;                          /* free to use by the user, resourcecollector stuff in lumiera*/
};


extern void *(*mpool_malloc_hook)(size_t size);
extern void (*mpool_free_hook)(void *ptr);

/** called after a mpool got initialised */
extern void (*mpool_init_hook) (MPool self);
/** called before a mpool gets destroyed */
extern void (*mpool_destroy_hook) (MPool self);

/**
 * Initialise a new memory pool.
 * Memory pools must be initialised before being used. One can supply
 * an optional destructor function for elements, this will be used to destroy elements which are still
 * in the pool when it gets destroyed itself. The destructor is _NOT_ called when elements are freed.
 * @param self pointer to the memory pool structure to be initialised
 * @param elem_size size for a single element
 * @param elements_per_cluster how many elements to put into a cluster
 * @param dtor pointer to an optional destructor function, may be `NULL`
 * @return `self` pointer to the initialised object
 */
MPool
mpool_init (MPool self, size_t elem_size, unsigned elements_per_cluster, mpool_destroy_fn dtor);


/**
 * destroy a memory pool.
 * A memory pool not used anymore should be destroyed. This frees all memory allocated with it.
 * When a destructor was provided at construction time, then this destructor is used on all non free elements
 * before before the clusters are freed. If no destructor was given then the clusters are just freed.
 * The destroyed memory pool behaves as if it was freshly initialised and can be used again, this is
 * some kind of exceptional behaviour.
 * @param self pointer to an initialised memory pool to be destroyed.
 * @return `self`
 */
MPool
mpool_destroy (MPool self);

/**
 * free unused clusters.
 */
MPool
mpool_purge (MPool self);


/**
 * query number of free elements.
 * @param self pointer to the memory pool to be queried
 * @return number of elements available in the pool _without allocating a new cluster_
 */
static inline unsigned
mpool_available (MPool self)
{
  return self->elements_free;
}


/**
 * preallocate elements.
 * Resize the pool that at least the given number of elements are available without cluster reallocations.
 * @param self pointer to the memory pool
 * @param nelements minimum number of elements to preallocate
 * @return self on success or `NULL` on error
 */
MPool
mpool_reserve (MPool self, unsigned nelements);


/**
 * allocate one element from a MPool. To improve cache locality allocations
 * are grouped close together to recent allocations.
 * @param self pointer to the memory pool
 * @return pointer to the allocated memory on success or `NULL` on error
 * @note will never fail when enough space was preallocated
 */
void*
mpool_alloc (MPool self);


/**
 * allocate one element close to the given reference element.
 * To improve cache locality the allocation tries to get an element close by.
 * @param self pointer to the memory pool
 * @param near reference to another element which should be close to the returned element _(hint only)_
 * @return pointer to the allocated memory on success or `NULL` on error
 * @note will never fail when enough space was preallocated
 */
void*
mpool_alloc_near (MPool self, void* near);


/**
 * free one element.
 * Frees the given element and puts it back into the pool for further allocations.
 * @param self pointer to the memory pool
 * @param element element to be freed
 */
void
mpool_free (MPool self, void* element);



/** diagnostic dump of MPool allocation */
void
nobug_mpool_dump (const_MPool self,
                  const int depth,
                  const struct nobug_context dump_context,
                  void* extra);

