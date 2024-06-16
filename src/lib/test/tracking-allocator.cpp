/*
  TrackingAllocator  -  test dummy objects for tracking ctor/dtor calls

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file tracking-allocator.cpp
 ** Implementation of the common storage backend for the tracking test allocator.
 ** 
 ** @see tracking-allocator.hpp
 ** @see TestTracking_test#demonstrate_checkAllocator()
 ** 
 */


//#include "lib/test/test-helper.hpp"
#include "lib/test/tracking-allocator.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/unique-malloc-owner.hpp"
#include "lib/depend.hpp"
#include "lib/uninitialised-storage.hpp"
#include "lib/util.hpp"


//#include <string>
#include <unordered_map>

//using util::_Fmt;
using std::make_shared;
using std::make_pair;
//using std::string;
using util::contains;
using util::joinDash;

namespace lib {
namespace test{
  
  
  /**
   * @internal registration and tracking of memory allocations handed out.
   */
  class MemoryPool
    : util::NonCopyable
    {
      // using the allocated memory location as key
      using Location = void*;
      struct LocationHash
        {
          HashVal operator() (Location const& loc) const { return HashVal(loc); }
        };
      
      /** registration entry to maintain a single allocation */
      struct Allocation
        : util::MoveOnly
        {
          UninitialisedDynBlock<byte> buff{};
        };
      
      using AllocTab = std::unordered_map<const Location, Allocation, LocationHash>;
      
      
      Literal poolID_;
      AllocTab allocs_{};
      
    public:
      MemoryPool (Literal id)
        : poolID_{id}
        , allocs_{}
        { }
      
      Allocation& addAlloc (size_t bytes);
      void discardAlloc (void* loc);
    };
  
  
  namespace { // implementation details for common memory management
    
    /* === Logging primitives === */
    
    inline EventLog& log() { return TrackingAllocator::log; }
    
    template<typename...XS>
    inline void
    logAlarm (XS const& ...xs)
    {
      log().error (joinDash(xs...));
    }
    
    template<typename...ARGS>
    inline void
    logAlloc (Literal pool, string fun, ARGS const& ...args)
    {
      log().call (pool,fun,args...);
    }
    
    
    class PoolRegistry
      : util::NonCopyable
      {
        std::unordered_map<Literal, std::weak_ptr<MemoryPool>> pools_{};
      public:
        
        static PoolHandle locate (Literal poolID);
        
      private:
        PoolHandle fetch_or_create (Literal poolID);
      };
    
    Depend<MemoryPool> globalPool;
    Depend<PoolRegistry> poolReg;
    
    
    /** static access front-end : locate (or create) a MemoryPool */
    PoolHandle
    PoolRegistry::locate (Literal poolID)
    {
      return poolReg().fetch_or_create (poolID);
    }
    
    
    PoolHandle
    PoolRegistry::fetch_or_create (Literal poolID)
    {
      auto entry = pools_[poolID];
      auto handle = entry.lock();
      if (handle) return handle;
      
      // create a new pool and enrol it for given ID
      PoolHandle newPool = make_shared<MemoryPool> (poolID);
      entry = newPool;
      return newPool;
    }
  }//(End)Implementation memory pools and registration
  
  
  
  
  
  TrackingAllocator::TrackingAllocator()
    : mem_{PoolRegistry::locate(GLOBAL)}
    { }
  
  TrackingAllocator::TrackingAllocator (Literal id)
    : mem_{PoolRegistry::locate(id)}
    { }
  
  
  void*
  TrackingAllocator::allocate (size_t n)
  {
    UNIMPLEMENTED ("allocate memory block of size n");
  }
  
  void
  TrackingAllocator::deallocate (void* loc) noexcept
  {
    UNIMPLEMENTED ("allocate memory block of size n");
  }
  
  MemoryPool::Allocation&
  MemoryPool::addAlloc (size_t bytes)
  {
    Allocation newAlloc;
    newAlloc.buff.allocate (bytes);
    Location loc = newAlloc.buff.front();
    ASSERT (not contains (allocs_, loc));
    logAlloc (poolID_, "allocate", bytes, loc);
    return allocs_.emplace (loc, move(newAlloc))
                  .first->second;
  }
  
  void
  MemoryPool::discardAlloc (void* loc)
  {
    if (contains (allocs_, loc))
      {
        auto& entry = allocs_[loc];
        ASSERT (entry.buff);
        ASSERT (entry.buff.front() == loc);
        logAlloc (poolID_, "deallocate", entry.buff.size());
        allocs_.erase(loc);
      }
    else // deliberately no exception here (for better diagnostics)
      logAlarm("FreeUnknown", loc);
  }
  
  
  
  /* ===== Diagnostics ===== */
  
  EventLog TrackingAllocator::log{"test::TrackingAllocator"};
  
  
  HashVal
  TrackingAllocator::checksum (Literal pool)
  {
    UNIMPLEMENTED ("get Checksum for mem-pool");
  }
  
  size_t
  TrackingAllocator::numAlloc  (Literal pool)
  {
    UNIMPLEMENTED ("get allocation count for mem-pool");
  }
  
  size_t
  TrackingAllocator::numBytes  (Literal pool)
  {
    UNIMPLEMENTED ("calc allotted Bytes for mem-pool");
  }
  

  
  
}} // namespace lib::test
