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

#include <nobug.h>
#include "lib/llist.h"
#include "include/logging.h"

/*
//mpool Memory Pools
//mpool ------------
//mpool
//mpool This memory pools are implemented as clusters of fixed sized elements. New clusters
//mpool are allocated on demand or manually preallocated with a `reserve()` operation. The pool
//mpool can be optimized by a `optimize()` operation which sorts the freelist by the addresses of
//mpool free elements and optionally moves elements (when a move function is provided) to lower
//mpool to fill up holes and improve cache locality.
//mpool Clusters can be reclaimed with a `collect()` operation. Optimization and Collection are
//mpool optional and are never automatically called.
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
//mpool This structure should be considered opaque except for the documented members
//mpool which may be read.
//mpool
//mpool  `unsigned mpool.elements_free`::
//mpool         number of free elements in the pool
//mpool
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
  unsigned elements_free;               /* a counter of free elements is the price we pay to support a reserve() operation */
  void* locality;
  mpool_destroy_fn destroy;
};


/*
//index.mpool_init xref:mpool_init[mpool_init]:: initialize a new memory pool
//mpool [[mpool_init]]
//mpool .mpool_init
//mpool Memory pools must be initialized before being used.
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
//index.mpool_destroy xref:mpool_destroy[mpool_destroy]:: destroy a memory pool
//mpool [[mpool_destroy]]
//mpool .mpool_destroy
//mpool A memory pool is not used anymore it should be destroyed. This frees all memory allocated with it.
//mpool When a destructor was provided at construction time, then this destructor is used on all non free elements
//mpool before before the memory is freed. If no destructor was given then the memory is just freed.
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
  query how much free elements are available
*/
unsigned
mpool_available (MPool self);


/*
  resize the pool that at least nelements become available without cluster reallocations
*/
MPool
mpool_reserve (MPool self, unsigned nelements);


/*
  allocate and initialize a new cluster (internal)
*/
MPool
mpool_cluster_alloc_ (MPool self);


/*
  alloc one element from the pool
*/
void* mpool_alloc (MPool self);


/*
  put a element back on the pool
*/
void
mpool_free (MPool self, void* element);



void
nobug_mpool_dump (const_MPool self,
                  const int depth,
                  const char* file,
                  const int line,
                  const char* func);
