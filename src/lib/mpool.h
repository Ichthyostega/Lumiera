/*
    mpool.h - memory pool for constant sized objects

  Copyright (C)         Lumiera.org
    2009,               Christian Thaeter <ct@pipapo.org>

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

#include <stdint.h>
#include <nobug.h>
#include "lib/llist.h"
#include "include/logging.h"

/*
//mpool Memory Pools
//mpool ------------
//mpool
//mpool This memory pools are implemented as clusters of fixed sized elements. New clusters
//mpool are allocated on demand or manually preallocated with a `reserve()` operation.
//mpool Some efforts are taken to ensure (cache) locality of the provided memory.
//mpool All functions are reentrant but not threadsafe, if this is desired it is advised to
//mpool care for proper locking elsewhere.
//mpool
*/


/*
//index.mpool_destroy_fn xref:mpool_destroy_fn[mpool_destroy_fn]:: function prototype for destroying elements
//mpool [[mpool_destroy_fn]]
//mpool .mpool_destroy_fn
//mpool When a memory pool gets destroyed it can call a destructor for any element which is still in the pool.
//mpool This destructor is optional.
//mpool
//mpool  typedef void (*mpool_destroy_fn)(void* self)
//mpool
//mpool  `self`::
//mpool         element to be destroyed
//mpool
*/
typedef void (*mpool_destroy_fn)(void* self);


/*
//index.struct_mpool xref:struct_mpool[mpool (struct)]:: the memory pool management structure
//mpool [[struct_mpool]]
//mpool .mpool
//mpool  typedef struct mpool_struct mpool
//mpool  typedef mpool* MPool
//mpool  typedef const mpool* const_MPool
//mpool
//mpool This structure should be considered opaque.
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
};


/*
//index.mpool_init xref:mpool_init[mpool_init()]:: initialize a new memory pool
//mpool [[mpool_init]]
//mpool .mpool_init
//mpool Initialize a memory pool, memory pools must be initialized before being used. One can supply
//mpool an optional destructor function for elements, this will be used to destroy elements which are still
//mpool in the pool when it gets destroyed itself. The destructor is _NOT_ called when elemented are freed.
//mpool
//mpool  MPool mpool_init (MPool self, size_t elem_size, unsigned elements_per_cluster, mpool_move_fn mv, mpool_destroy_fn dtor)
//mpool
//mpool  `self`::
//mpool         pointer to the memory pool structure to be initialized
//mpool  `elem_size`::
//mpool         size for a single element
//mpool  `elements_per_cluster`::
//mpool         how many elements to put into a cluster
//mpool  `dtor`::
//mpool         pointer to an optional destructor function or NULL
//mpool  return::
//mpool         self
//mpool
*/
MPool
mpool_init (MPool self, size_t elem_size, unsigned elements_per_cluster, mpool_destroy_fn dtor);


/*
//index.mpool_destroy xref:mpool_destroy[mpool_destroy()]:: destroy a memory pool
//mpool [[mpool_destroy]]
//mpool .mpool_destroy
//mpool A memory pool is not used anymore it should be destroyed. This frees all memory allocated with it.
//mpool When a destructor was provided at construction time, then this destructor is used on all non free elements
//mpool before before the clusters are freed. If no destructor was given then the clusters are just freed.
//mpool The destroyed memory pool behaves as if it was freshly initialized and can be used again, this is some kindof
//mpool exceptional behaviour.
//mpool
//mpool  MPool mpool_destroy (MPool self)
//mpool
//mpool  `self`::
//mpool         pointer to an initialized memory pool to be destroyed.
//mpool  return::
//mpool         self
//mpool
//mpool
*/
MPool
mpool_destroy (MPool self);

/*
//index.mpool_purge xref:mpool_purge[mpool_purge()]:: free unused clusters
//mpool [[mpool_purge]]
//mpool .mpool_purge
//mpool
//mpool TODO
//mpool
//mpool
*/
MPool
mpool_purge (MPool self);


/*
//index.mpool_available xref:mpool_available[mpool_available()]:: query number of free elements
//mpool [[mpool_available]]
//mpool .mpool_available
//mpool One can check how much elements are available without a new cluster allocation in a memory pool.
//mpool
//mpool  unsigned mpool_available (MPool self)
//mpool
//mpool  `self`::
//mpool         pointer to the memory pool to be queried
//mpool  return::
//mpool         number of available elements
//mpool
*/
static inline unsigned
mpool_available (MPool self)
{
  return self->elements_free;
}


/*
//index.mpool_reserve xref:mpool_reserve[mpool_reserve()]:: preallocate elements
//mpool [[mpool_reserve]]
//mpool .mpool_reserve
//mpool Resize the pool that at least nelements become available without cluster reallocations
//mpool
//mpool  unsigned mpool_reserve (MPool self, unsigned nelements)
//mpool
//mpool  `self`::
//mpool         pointer to the memory pool
//mpool  `nelements`::
//mpool         minimum number of elements to preallocate
//mpool  return::
//mpool         self on success or NULL on error
//mpool
*/
MPool
mpool_reserve (MPool self, unsigned nelements);


/*
//index.mpool_alloc xref:mpool_alloc[mpool_alloc()]:: allocate one element
//mpool [[mpool_alloc]]
//mpool .mpool_alloc
//mpool Allocates on element from a mpool. To improve cache locality allocations
//mpool are grouped close together to recent allocations.
//mpool
//mpool  void* mpool_alloc (MPool self)
//mpool
//mpool  `self`::
//mpool         pointer to the memory pool
//mpool  return::
//mpool         pointer to the allocated memory on success or NULL on error
//mpool         will never fail when enough space was preallocated
//mpool
*/
void*
mpool_alloc (MPool self);


/*
//index.mpool_alloc_near xref:mpool_alloc_near[mpool_alloc_near()]:: allocate one element, w/ locality
//mpool [[mpool_alloc_near]]
//mpool .mpool_alloc_near
//mpool Allocates on element from a mpool. To improve cache locality the allocation
//mpool tries to get an element close to another.
//mpool
//mpool  void* mpool_alloc_near (MPool self, void* near)
//mpool
//mpool  `self`::
//mpool         pointer to the memory pool
//mpool  `near`::
//mpool         reference to another element which should be close to the returned element (hint only)
//mpool  return::
//mpool         pointer to the allocated memory on success or NULL on error
//mpool         will never fail when enough space was preallocated
//mpool
*/
void*
mpool_alloc_near (MPool self, void* near);


/*
//index.mpool_free xref:mpool_free[mpool_free()]:: free one element
//mpool [[mpool_free]]
//mpool .mpool_free
//mpool Frees the given element and puts it back into the pool for furhter allocations.
//mpool
//mpool  void mpool_free (MPool self, void* element)
//mpool
//mpool  `self`::
//mpool         pointer to the memory pool
//mpool  `element`::
//mpool         element to be freed
//mpool
*/
void
mpool_free (MPool self, void* element);



void
nobug_mpool_dump (const_MPool self,
                  const int depth,
                  const struct nobug_context dump_context);


/*
//      Local Variables:
//      mode: C
//      c-file-style: "gnu"
//      indent-tabs-mode: nil
//      End:
*/
