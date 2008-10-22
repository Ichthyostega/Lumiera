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
//#include "common/util.hpp"


//using util::isnil;
//using util::cStr;

namespace lib {
  
  class AllocationCluster::MemoryManager
    {
    public:
      MemoryManager(TypeInfo info);
      
      void* allocate();
      
      void commit (void* pendingAlloc);
    };

  
  AllocationCluster::MemoryManager::MemoryManager (TypeInfo info)
  {
    UNIMPLEMENTED ("create a new MemoryManager instance for the specific type given");
    //////////////////////////////////////////////////////TODO
  }
  
  void*
  AllocationCluster::MemoryManager::allocate()
  {
    UNIMPLEMENTED ("do the actual raw memory allocation");
    return 0; ////////////////////////////////////////////TODO
  }
  
  void
  AllocationCluster::MemoryManager::commit (void* pendingAlloc)
  {
    UNIMPLEMENTED ("commit a pending allocation to be valid");
    //////////////////////////////////////////////////////TODO
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
  }
  
  
  /** On shutdown of the AllocationCluster we need to assure a certain
   *  destruction order is maintained by explicitly invoking a cleanup
   *  operation on each of the low-level memory manager objects. 
   */
  AllocationCluster::~AllocationCluster()  throw()
  {
    try
      {
        
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
    TODO ("this is still a bit fishy, probably off by one and not threadsafe...!");
    
    if (!slot || slot >= typeHandlers_.size() || !typeHandlers_[slot])
      return 0;  // Memory manager not yet initialised
    else
      return typeHandlers_[slot]->allocate();
  }
  
  
  void*
  AllocationCluster::initiateAlloc (TypeInfo type, size_t& slot)
  {
    ASSERT (0 < slot && (slot >=typeHandlers_.size() || !typeHandlers_[slot]));

    if (slot >= typeHandlers_.size())
      typeHandlers_.resize(slot);
    if (!typeHandlers_[slot])
      typeHandlers_[slot].reset (new MemoryManager (type));
    
    ASSERT (typeHandlers_[slot]);
    
    return initiateAlloc(slot); 
    
  }
  
  
  void
  AllocationCluster::finishAlloc (size_t& slot, void* allocatedObj)
  {
    ASSERT (typeHandlers_[slot]);
    ASSERT (allocatedObj);
    
    typeHandlers_[slot]->commit(allocatedObj);
  }
  
  
  
  
} // namespace lib
