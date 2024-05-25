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
 ** \par low-level trickery
 **      The StorageManager implementation exploits object layout knowledge in order to
 **      operate with the bare minimum of administrative overhead; notably the next allocation
 **      is always located _within_ the current extent and by assuming that the remaining size
 **      is tracked correctly, the start of the current extent can always be re-discovered;
 **      the sequence of extents is managed as a linked list, where the `next*` resides in the
 **      first »slot« within each Extent; this pointer is _dressed up_ (reinterpreted) as a
 **      lib::LinkedElements with a heap allocator, which ends up performing the actual
 **      allocation in blocks of EXTENT_SIZ.
 */


#include "lib/allocation-cluster.hpp"
#include "lib/linked-elements.hpp"
#include "lib/util.hpp"


using util::unConst;
using util::isnil;
using std::byte;


namespace lib {
  namespace {// Configuration constants
    const size_t EXTENT_SIZ  = 256;
    const size_t OVERHEAD    = 2 * sizeof(void*);
    const size_t STORAGE_SIZ = EXTENT_SIZ - OVERHEAD;
     
    using HeapAlloc = std::allocator<std::byte>;
    using Alloc     = std::allocator_traits<HeapAlloc>;
    
    HeapAlloc heap;
    
    void*
    allocate (size_t bytes) ////////////////////////OOO obsolete ... find a way to relocate this as custom allocator within LinkedElements!!!
    {
      return Alloc::allocate (heap, bytes);
    }
    
    void
    destroy (void* storage)
    {
      Alloc::destroy (heap, static_cast<std::byte *> (storage));
    }

    /**
     * Special allocator-policy for lib::LinkedElements
     * - does not allow to allocate new elements
     * - can hook up elements allocated elsewhere
     * - ensure the destructor of all elements is invoked
     */
    struct PolicyInvokeDtor
      : lib::linked_elements::NoOwnership
      {
        /**
         * while this policy doesn't take ownership,
         * it ensures the destructor is invoked
         */
        template<class X>
        void
        destroy (X* elm)
          {
            REQUIRE (elm);
            elm->~X();
          }
      };

  }
  
  
  /**
   * An _overlay view_ for the AllocationCluster to add functionality
   * for adding / clearing extents and registering optional deleter functions.
   * @warning this is a tricky construct to operate each Allocation Cluster
   *          with the absolute minimum of organisational overhead necessary.
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
      
      using Destructors = lib::LinkedElements<Destructor, PolicyInvokeDtor>;
      
      /** Block of allocated storage */
      struct Extent
        : util::NonCopyable
        {
          Extent* next;
          Destructors dtors;
          std::byte storage[STORAGE_SIZ];
        };
      using Extents = lib::LinkedElements<Extent>;
      
      union ManagementView
        {
          Storage storage;
          Extents extents;
        };
      
      ManagementView view_;
      
      StorageManager()  = delete;    ///< @note used as _overlay view_ only, never created
      
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
      
      void
      discardAll()
        {
          closeCurrentBlock();
          view_.extents.clear();
        }
      
      void
      attach (Destructor& dtor)
        {
          getCurrentBlockStart()->dtors.push (dtor);
        }
      
      
      bool
      empty()  const
        {
          return nullptr == view_.storage.pos;
        }
      
      size_t
      determineExtentCnt()  const
        {
          return empty()? 0
                        : lib::asLinkedElements (getCurrentBlockStart())
                              .size();
        }
      
      size_t
      calcAllocInCurrentBlock()  const
        {
          ENSURE (STORAGE_SIZ >= view_.storage.rest);
          return STORAGE_SIZ - view_.storage.rest;
        }
      
      
    private:
      Extent*
      getCurrentBlockStart()  const
        {
          REQUIRE (not empty());
          void* pos = static_cast<byte*>(view_.storage.pos)
                                       + view_.storage.rest
                                       - EXTENT_SIZ;
          return static_cast<Extent*> (pos);
        }
      
      void
      closeCurrentBlock()
        {
          if (empty()) return;
          // relocate the pos-pointer to the start of the block
          view_.storage.pos = getCurrentBlockStart();
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
  
  
  
  
  
  /**
   * Prepare a new clustered allocation to be expanded by extents of size
   * EXTENT_SIZ, yet discarded all at once when the dtor is called.
   * The constructor does not allocate anything immediately.
   */
  AllocationCluster::AllocationCluster()
    : storage_{}
    {
      TRACE (memory, "new AllocationCluster");
    }
  
  
  /**
   * The shutdown of an AllocationCluster walks all extents and invokes all
   * registered deleter functions and then discards the complete storage.
   * @note it is possible to allocate objects as _disposable_ — meaning
   *       that no destructors will be enrolled and called for such objects.
   */
  AllocationCluster::~AllocationCluster()  noexcept
  try
    {
      TRACE (memory, "shutting down AllocationCluster");
      StorageManager::access(*this).discardAll();
    }
  ERROR_LOG_AND_IGNORE (progress, "discarding AllocationCluster")
  
  
  /** virtual dtor to cause invocation of the payload's dtor on clean-up */
  AllocationCluster::Destructor::~Destructor() { };
  
  
  
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
    StorageManager::access(*this).addBlock();
  }
  
  
  void
  AllocationCluster::registerDestructor (Destructor& dtor)
  {
    StorageManager::access(*this).attach (dtor);
  }
  
  
  
  
  /* === diagnostics helpers === */
  
  bool
  AllocationCluster::_is_within_limits (size_t size, size_t align)
  {
    auto isPower2 = [](size_t n){ return !(n & (n-1)); };
    return 0 < size
       and 0 < align
       and size <= STORAGE_SIZ
       and align <= STORAGE_SIZ
       and isPower2 (align);
  }
  
  
  size_t
  AllocationCluster::numExtents()  const
  {
    return StorageManager::access (unConst(*this)).determineExtentCnt();
  }

  /**
   * @warning whenever there are more than one extent,
   *   the returned byte count is guessed only (upper bound), since
   *   actually allocated size is not tracked to save some overhead.
   */
  size_t
  AllocationCluster::numBytes()  const
  {
    size_t extents = numExtents();
    if (not extents) return 0;
    size_t bytes = StorageManager::access (unConst(*this)).calcAllocInCurrentBlock();
    return (extents - 1) * STORAGE_SIZ + bytes;
  }
  
  
} // namespace lib
