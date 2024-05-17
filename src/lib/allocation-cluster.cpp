/*
  AllocationCluster  -  allocating and owning a pile of objects

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file allocation-cluster.cpp
 ** Implementation of [memory management helper functions](\ref allocation-cluster.hpp)
 ** for the render engine model. Here, in the actual translation unit, the generic part
 ** of these functions is emitted, while the corresponding header provides a strictly
 ** typed front-end, based on templates, which forward to the implementation eventually.
 */


#include "lib/allocation-cluster.hpp"
#include "lib/linked-elements.hpp"
#include "lib/util.hpp"

#include <cstddef>
#include <memory>

using util::isnil;


namespace lib {
  namespace {// Configuration constants
    const size_t EXTENT_SIZ  = 256;
    const size_t OVERHEAD    = 2 * sizeof(void*);
    const size_t STORAGE_SIZ = EXTENT_SIZ - OVERHEAD;
     
    using HeapAlloc = std::allocator<std::byte>;
    using Alloc     = std::allocator_traits<HeapAlloc>;
    
    HeapAlloc heap;
    
    void*
    allocate (size_t bytes)
    {
      return Alloc::allocate (heap, bytes);
    }
    
    void
    destroy (void* storage)
    {
      Alloc::destroy (heap, static_cast<std::byte *> (storage));
    }
  }
  
  /**
   * An _overlay view_ for the AllocationCluster to add functionality
   * for adding / clearing extents and registering optional deleter functions.
   * @warning this is a tricky construct to operate each Allocation Cluster
   *          with the absolute necessary minimum of organisational overhead.
   *          The key point to note is that StorageManager is layout compatible
   *          with AllocationCluster itself — achieved through use of the union
   *          ManagementView, which holds a Storage descriptor member, but
   *          also an alternate view to manage a chain of extents as
   *          intrusive linked list (lib::LinkedElements).
   * @remark  this trick relies on `std::align(pos,rest)` to manage the storage
   *          coordinates coherently, allowing to re-establish the begin
   *          of each storage block always, using pointer arithmetics.
   */
  class AllocationCluster::StorageManager
    {
      struct Destructor
        {
          Destructor* next;
          ///////////////////////////////////////////////OOO store deleter function here
        };
      using Destructors = lib::LinkedElements<Destructor>;
      
      struct Extent
        {
          Extent* next;
          Destructors dtors;
          std::byte storage[STORAGE_SIZ];
        };
      using Extents = lib::LinkedElements<Extent>;
      
      union ManagementView
        {
          Storage storage{};
          Extents extents;
        };
      
      ManagementView view_;
      
    public:
      static StorageManager&
      access (AllocationCluster& clu)
        {
          return reinterpret_cast<StorageManager&> (clu);
        }
      
      void
      addBlock()
        {
          closeCurrentBlock();
          prependNextBlock();
        }
      
    private:
      void
      closeCurrentBlock()
        {
          ASSERT (view_.storage.pos);
          // relocate the pos-pointer to the start of the block
          view_.storage.pos += view_.storage.rest - EXTENT_SIZ;
          view_.storage.rest = 0;
        }
      
      void
      prependNextBlock()
        {
          view_.extents.emplace();
          view_.storage.pos = & view_.extents.top().storage;
          view_.storage.rest = STORAGE_SIZ;
        }
    };
  
  
  
  /** creating a new AllocationCluster prepares a table capable
   *  of holding the individual object families to come. Each of those
   *  is managed by a separate instance of the low-level memory manager.
   */
  AllocationCluster::AllocationCluster()
    {
      TRACE (memory, "new AllocationCluster");
    }
  
  
  /** On shutdown of the AllocationCluster we need to assure a certain
   *  destruction order is maintained by explicitly invoking a cleanup
   *  operation on each of the low-level memory manager objects.
   */
  AllocationCluster::~AllocationCluster()  noexcept
  try
    {
      
      TRACE (memory, "shutting down AllocationCluster");
      
    }
  ERROR_LOG_AND_IGNORE (progress, "discarding AllocationCluster")
  
  
  /**
   * Expand the alloted storage pool by a block,
   * suitable to accommodate at least the indicated request.
   * @remark Storage blocks are organised as linked list,
   *         allowing to de-allocate all blocks together.
   */
  void
  AllocationCluster::expandStorage (size_t allocRequest)
  {
    ENSURE (allocRequest + OVERHEAD <= EXTENT_SIZ);
    void* newBlock = allocate (EXTENT_SIZ); ////////////////////////OOO obsolete ... use ManagementView instead!!!
    StorageManager::access(*this).addBlock();
  }

  
  
  /* === diagnostics helpers === */
  
  bool
  AllocationCluster::_is_within_limits (size_t size, size_t align)
  {
    UNIMPLEMENTED ("size limits"); ///////////////////////////OOO enforce maximum size limits
  }
  
  
  
} // namespace lib
