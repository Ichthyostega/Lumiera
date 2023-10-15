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
#include "lib/error.hpp"
#include "lib/util.hpp"
#include "lib/sync.hpp"

#include <deque>

using util::isnil;


namespace lib {
  
  /**
   * "Low-level" Memory manager for allocating small objects of a fixed size.
   * The usage pattern is definite: Objects will be allocated in the course of
   * a build process and then live until all Objects will be purged in one sway.
   * Allocations will be requested one by one and immediately committed after
   * successful ctor call of the object being allocated. Allocations and commits
   * can be assumed to come in pairs, thus if an allocation immediately follows
   * another one (without commit), the previous allocation can be considered
   * a failure and can be dropped silently. After an allocation succeeds
   * (i.e. was committed), the MemoryManager is in charge for the lifecycle
   * of the object within the allocated space and has to guarantee calling
   * it's dtor, either on shutdown or on explicit #purge() -- the type info
   * structure handed in on initialisation provides a means for invoking
   * the dtor without actually knowing the object's type.
   * 
   * @todo this is a preliminary or pseudo-implementation based on 
   * a vector of raw pointers,  i.e. actually the objects are heap 
   * allocated. What actually should happen is for the MemoryManager to
   * allocate raw memory chunk wise, sub partition it and place the objects
   * into this private memory buffer. Further, possibly we could maintain
   * a pool of raw memory chunks used by all MemoryManager instances. I am
   * skipping those details for now (10/2008) because they should be based
   * on real-world measurements, not guessing.
   */
  class AllocationCluster::MemoryManager
    : public Sync<RecursiveLock_NoWait>
    {
      typedef std::deque<char*> MemTable;
      TypeInfo type_;
      MemTable mem_;
      size_t top_;   ///< index of the next slot available for allocation
      
    public:
      MemoryManager(TypeInfo info) : top_(0) { reset(info); }
     ~MemoryManager()                        { purge(); }
      
      size_t size()  const;
      
      void purge();
      void reset(TypeInfo info);
      
      void* allocate();
      
      void commit (void* pendingAlloc);
      
    private:
      void clearStorage();
    };
  
  
  
  /** the top_ index always points at the next slot 
   *  not yet holding a finished, committed allocation.
   *  Index is zero based, thus top_ == count of living objects
   */
  size_t
  AllocationCluster::MemoryManager::size()  const
  {
    return top_;
  }
  
  
  void
  AllocationCluster::MemoryManager::reset (TypeInfo info)
  {
    Lock sync{this};
    
    if (0 < mem_.size()) purge();
    type_ = info;
    
    ENSURE (0==top_);
    ENSURE (isnil (mem_));
    ENSURE (0 < type_.allocSize);
    ENSURE (type_.killIt);
  }
  
  
  void
  AllocationCluster::MemoryManager::purge()
  {
    Lock sync{this};
    
    REQUIRE (type_.killIt, "we need a deleter function");
    REQUIRE (0 < type_.allocSize, "allocation size unknown");
    REQUIRE (top_ == mem_.size() || (top_+1) == mem_.size());
    
    while (top_)
      type_.killIt (mem_[--top_]);
    clearStorage();
  }// note: unnecessary to kill pending allocations
  
  
  inline void
  AllocationCluster::MemoryManager::clearStorage()
  {
    for (size_t i=mem_.size(); 0 < i; )
      delete[] mem_[--i];
    mem_.clear();
  }
  
  
  inline void*
  AllocationCluster::MemoryManager::allocate()
  {
    Lock sync{this};
    
    REQUIRE (0 < type_.allocSize);
    REQUIRE (top_ <= mem_.size());
    
    if (top_==mem_.size())
      mem_.resize(top_+1);
    
    if (!mem_[top_]) // re-use existing allocation, if any
      mem_[top_] = new char[type_.allocSize];
    
    ENSURE (top_ < mem_.size());
    ENSURE (mem_[top_]);
    return mem_[top_];
  }
  
  
  inline void
  AllocationCluster::MemoryManager::commit (void* pendingAlloc)
  {
    Lock sync{this};
    
    REQUIRE (pendingAlloc);
    ASSERT (top_ < mem_.size());
    ASSERT (pendingAlloc == mem_[top_], "allocation protocol violated");
    
    ++top_;
    
    ENSURE (top_ == mem_.size());
  }
  
  
  
  /** storage for static bookkeeping of type allocation slots */
  size_t AllocationCluster::maxTypeIDs;
  
  
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
  {
    try
      {                                         // avoiding a per-instance lock for now.
        ClassLock<AllocationCluster> guard;    //  (see note in the class description)
        
        TRACE (memory, "shutting down AllocationCluster");
        for (size_t i = typeHandlers_.size(); 0 < i; --i)
          if (handler(i))
            handler(i)->purge();
        
        typeHandlers_.clear();
        
      }
    catch (lumiera::Error & ex)
      {
        WARN (progress, "Exception while closing AllocationCluster: %s", ex.what());
        const char* errID = lumiera_error();
        TRACE (debugging, "Error flag was: %s", errID);
      }
    catch (...)
      {
        ALERT (progress, "Unexpected fatal Exception while closing AllocationCluster.");
        lumiera::error::lumiera_unexpectedException(); // terminate
      }
  }
  
  
  
  void*
  AllocationCluster::initiateAlloc (size_t& slot)
  {
    if (!slot or slot > typeHandlers_.size() or !handler(slot) )
      return nullptr;  // Memory manager not yet initialised
    else
      return handler(slot)->allocate();
  }
  
  
  void*
  AllocationCluster::initiateAlloc (TypeInfo type, size_t& slot)
  {
    ASSERT (0 < slot);
    
      {                                         // avoiding a per-instance lock for now.
        ClassLock<AllocationCluster> guard;    //  (see note in the class description)
        
        if (slot > typeHandlers_.size())
          typeHandlers_.resize(slot);
        if (not handler(slot))
          handler(slot).reset (new MemoryManager (type));
        
      }
    
    ASSERT (handler(slot));
    return initiateAlloc(slot);
  }
  
  
  void
  AllocationCluster::finishAlloc (size_t& slot, void* allocatedObj)
  {
    ASSERT (handler(slot));
    ASSERT (allocatedObj);
    
    handler(slot)->commit(allocatedObj);
  }
  
  
  /* === diagnostics helpers === */
  
  /** @return total number of objects
   *          currently managed by this allocator */
  size_t
  AllocationCluster::size()  const
  {
    size_t size(0);
    typedef ManagerTable::const_iterator Iter;
    
    for (Iter ii= typeHandlers_.begin(); ii != typeHandlers_.end(); ++ii )
      if (*ii)
        size += (*ii)->size();
    
    return size;
  }
  
  
  size_t
  AllocationCluster::countActiveInstances (size_t& slot)  const
  {
    if (handler (slot))
      return handler(slot)->size();
    else
      return 0;
  }
  
  
  
} // namespace lib
