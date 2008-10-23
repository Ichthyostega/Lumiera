/*
  AllocationCluster  -  allocating and owning a pile of objects 
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
* *****************************************************/


#include "lib/allocationcluster.hpp"
#include "common/error.hpp"
#include "common/util.hpp"


using util::isnil;
//using util::cStr;

namespace lib {
  
  /**
   * "Low-level" Memory manager for allocating small objects of a fixed size.
   * The usage pattern is definite: Objects will be allocated in the course of
   * a build process and then live until all Objects will be purged in one sway.
   * Allocations will be requested one by one and immediately committed after
   * successful ctor call of the object being allocated. Allocations and commits
   * can be assumed to come in pairs, thus if an allocation immediately follows
   * another one (without commit), the previous allocation can be considered
   * a failure and can be dropped silently. After an allocation has succeeds
   * (i.e. was committed), the MemoryManager is in charge for the lifecycle
   * of the object within the allocated spaces and has to guarantee calling
   * it's dtor, either on shutdown or on explicit #purge() -- the type info
   * structure handed in on initialisation provides a means for invoking
   * the dtor without actually knowing the object's type.
   * 
   * @todo this is a preliminary or pseudo-implementation based on 
   * a vector of smart pointers,  i.e. actually the objects are heap 
   * allocated. What actually should happen is for the MemoryManager to
   * allocate raw memory chunk wise, sub partition it and place the objects
   * into this private memory buffer. Further, possibly we could maintain
   * a pool of raw memory chunks used by all MemoryManager instances. I am
   * skipping those details for now (10/2008) because they should be based
   * on real-world measurements, not guessing.
   */
  class AllocationCluster::MemoryManager
    {
      typedef std::vector<char*> MemTable;
      TypeInfo type_;
      MemTable mem_;
      size_t top_;
      
    public:
      MemoryManager(TypeInfo info) { reset(info); }
      ~MemoryManager()             { purge(); }
      
      void purge();
      void reset(TypeInfo info);
      
      void* allocate();
      
      void commit (void* pendingAlloc);
    };

  
  
  void
  AllocationCluster::MemoryManager::reset (TypeInfo info)
  {
    Thread::Lock<MemoryManager> guard   SIDEEFFECT;
    
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
    Thread::Lock<MemoryManager> guard   SIDEEFFECT;

    REQUIRE (type_.killIt, "we need a deleter function");
    REQUIRE (0 < type_.allocSize, "allocation size unknown");
    REQUIRE (top_ == mem_.size() || (top_+1) == mem_.size());
    
    while (top_)
      type_.killIt (&mem_[--top_]);
    mem_.resize(0);
  }// note: unnecessary to kill pending allocations
  
  
  inline void*
  AllocationCluster::MemoryManager::allocate()
  {
    Thread::Lock<MemoryManager> guard   SIDEEFFECT;
    
    REQUIRE (0 < type_.allocSize);
    REQUIRE (top_ <= mem_.size());

    if (top_==mem_.size())
      mem_.resize(top_+1);
    mem_[top_] = new char[type_.allocSize];
    
    ENSURE (top_ < mem_.size());
    return mem_[top_];
  }
  
  
  inline void
  AllocationCluster::MemoryManager::commit (void* pendingAlloc)
  {
    Thread::Lock<MemoryManager> guard   SIDEEFFECT;

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
//    : configParam_  (new Configmap),
  {
    TRACE (buildermem, "new AllocationCluster");
  }
  
  
  /** On shutdown of the AllocationCluster we need to assure a certain
   *  destruction order is maintained by explicitly invoking a cleanup
   *  operation on each of the low-level memory manager objects. 
   */
  AllocationCluster::~AllocationCluster()  throw()
  {
    try
      {
        Thread::Lock<AllocationCluster> guard   SIDEEFFECT
        
        TRACE (buildermem, "shutting down AllocationCluster");
        for (size_t i = typeHandlers_.size(); 0 < i; --i)
          handler(i)->purge();
        
        typeHandlers_.resize(0);
        
      }
    catch (lumiera::Error & ex)
      {
        WARN (oper, "Exception while closing AllocationCluster: %s",ex.what());
      }
    catch (...)
      {
        ERROR (NOBUG_ON, "Unexpected fatal Exception while closing AllocationCluster.");
        lumiera::error::lumiera_unexpectedException(); // terminate
      }
  }
  
  
  
  void*
  AllocationCluster::initiateAlloc (size_t& slot)
  {
    if (!slot || slot > typeHandlers_.size() || !handler(slot) )
      return 0; // Memory manager not yet initialised
    else
      return handler(slot)->allocate();
  }
  
  
  void*
  AllocationCluster::initiateAlloc (TypeInfo type, size_t& slot)
  {
    ASSERT (0 < slot);
    
      {
        Thread::Lock<AllocationCluster> guard   SIDEEFFECT;   /////TODO: decide tradeoff: lock just the instance, or lock the AllocationCluster class?
        
        if (slot > typeHandlers_.size())
          typeHandlers_.resize(slot);
        if (!handler(slot))
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
  
  
  
  
} // namespace lib
