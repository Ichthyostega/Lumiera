/*
  TrackingAllocator  -  custom allocator for memory management diagnostics

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
 ** - PoolRegistry maintains a common hashtable with all MemoryPool instances;
 **   when using the standard accessors, registration and pool creating happens
 **   automatically.
 ** - in this setup, the pool with ID="GLOBAL" is just one further pool, which
 **   will be established when a front-end function is used with the default
 **   pool-ID (i.e. without explicit pool specification). Note however, that
 **   such a factory is still statefull (since it embeds a shared-ownership
 **   smart-pointer)
 ** - each MemoryPool contains a hashtable, where each active allocation is
 **   stored, using the storage-location as hashtable key. Each such entry
 **   gets a further consecutive internal ID, which is visible in the EventLog
 ** @see tracking-allocator.hpp
 ** @see TestTracking_test#demonstrate_checkAllocator()
 */


#include "lib/test/tracking-allocator.hpp"
#include "lib/uninitialised-storage.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/depend.hpp"
#include "lib/util.hpp"


#include <string>
#include <unordered_map>

using std::string;
using std::make_shared;
using std::make_pair;
using util::contains;
using util::joinDash;
using util::showAdr;


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
          size_t entryID{0};
        };
      
      using AllocTab = std::unordered_map<const Location, Allocation, LocationHash>;
      
      
      Literal poolID_;
      AllocTab allocs_;
      HashVal checksum_;
      size_t entryNr_;
      
    public:
      MemoryPool (Literal id)
        : poolID_{id}
        , allocs_{}
        , checksum_{0}
        , entryNr_{0}
        { }
      
      Allocation& addAlloc (size_t bytes);
      void discardAlloc (void* loc, size_t);
      
      Allocation const* findAlloc (Location)  const;
      
      HashVal getChecksum()     const;
      size_t getAllocationCnt() const;
      size_t calcAllocSize()    const;
      
      Literal getPoolID()  const { return poolID_; }
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
    
    
    /** keep track of any distinct memory pools used */
    class PoolRegistry
      : util::NonCopyable
      {                                          // note: obsolete entries never discarded
        using PoolTab = std::unordered_map<Literal, std::weak_ptr<MemoryPool>>;
        
        PoolTab pools_{};
        
      public:
        static PoolHandle locate (Literal poolID);
        
      private:
        PoolHandle fetch_or_create (Literal poolID);
      };
    
    /** singleton for default pool */
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
      auto& entry = pools_[poolID];
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
  
  
  /**
   * Allot a memory block of given size \a bytes.
   * This allocation is recorded in the associated MemoryPool
   * and proper deallocation can thus be verified.
   * @return a `void*` to the start of the bare memory location
   */
  TrackingAllocator::Location
  TrackingAllocator::allocate (size_t bytes)
  {
    ENSURE (mem_);
    return mem_->addAlloc (bytes)
                    .buff.front();
  }
  
  /**
   * Discard and forget an allocation created through this allocator.
   * The \a bytes argument serves as sanity check (since the actual allocation
   * size is recorded anyway); a mismatch is logged as error, yet silently ignored.
   */
  void
  TrackingAllocator::deallocate (Location loc, size_t bytes) noexcept
  {
    ENSURE (mem_);
    mem_->discardAlloc (loc, bytes);
  }
  
  MemoryPool::Allocation&
  MemoryPool::addAlloc (size_t bytes)
  {
    Allocation newAlloc;
    newAlloc.buff.allocate (bytes);
    Location loc = newAlloc.buff.front();
    ASSERT (not contains (allocs_, loc));
    newAlloc.entryID = ++entryNr_;
    logAlloc (poolID_, "allocate", bytes, newAlloc.entryID, showAdr(loc));
    checksum_ += newAlloc.entryID * bytes;
    return allocs_.emplace (loc, move(newAlloc))
                  .first->second;
  }
  
  void
  MemoryPool::discardAlloc (Location loc, size_t bytes)
  {
    if (contains (allocs_, loc))
      {
        auto& entry = allocs_[loc];
        ASSERT (entry.buff);
        ASSERT (entry.buff.front() == loc);
        if (entry.buff.size() != bytes)     // *deliberately* tolerating wrong data, but log incident to support diagnostics
          logAlarm ("SizeMismatch", bytes, "≠", entry.buff.size(), entry.entryID, showAdr(loc));
        
        logAlloc (poolID_, "deallocate", bytes, entry.entryID, bytes, showAdr(loc));
        checksum_ -= entry.entryID * bytes;  // Note: using the given size (if wrong ⟿ checksum mismatch)
        allocs_.erase(loc);
      }
    else // deliberately no exception here (for better diagnostics)
      logAlarm ("FreeUnknown", bytes, showAdr(loc));
  }
  
  
  MemoryPool::Allocation const*
  MemoryPool::findAlloc (Location loc)  const
  {
    return contains (allocs_, loc)? & allocs_.at(loc)
                                  : nullptr;
  }
  
  
  HashVal
  MemoryPool::getChecksum()  const
  {
    return checksum_;
  }
  
  size_t
  MemoryPool::getAllocationCnt()  const
  {
    return allocs_.size();
  }
  
  size_t
  MemoryPool::calcAllocSize()  const
  {
    return explore(allocs_)
              .transform ([](auto& it){ return it->second.buff.size(); })
              .resultSum();
  }
  
  
  
  /* ===== Diagnostics ===== */
  
  EventLog TrackingAllocator::log{"test::TrackingAllocator"};
  
  
  /** get Checksum for specific mem-pool */
  HashVal
  TrackingAllocator::checksum (Literal poolID)
  {
    PoolHandle pool = PoolRegistry::locate (poolID);
    return pool->getChecksum();
  }
  
  /** determine number of active front-end handles */
  size_t
  TrackingAllocator::use_count (Literal poolID)
  {
    PoolHandle pool = PoolRegistry::locate (poolID);
    return pool.use_count() - 1;
  }
  
  /** get active allocation count for mem-pool */
  size_t
  TrackingAllocator::numAlloc  (Literal poolID)
  {
    PoolHandle pool = PoolRegistry::locate (poolID);
    return pool->getAllocationCnt();
  }
  
  /** calculate currently allotted Bytes for mem-pool */
  size_t
  TrackingAllocator::numBytes  (Literal poolID)
  {
    PoolHandle pool = PoolRegistry::locate (poolID);
    return pool->calcAllocSize();
  }
  
  
  /** probe if this allocator pool did allocate the given memory location */
  bool
  TrackingAllocator::manages (Location memLoc)  const
  {
    ENSURE (mem_);
    return bool(mem_->findAlloc (memLoc));
  }
  
  /** retrieve the registered size of this allocation, if known. */
  size_t
  TrackingAllocator::getSize(Location memLoc) const
  {
    ENSURE (mem_);
    auto* entry = mem_->findAlloc (memLoc);
    return entry? entry->buff.size()
                : 0;
  }
  
  /** retrieve the internal registration ID for this allocation. */
  HashVal
  TrackingAllocator::getID (Location memLoc) const
  {
    ENSURE (mem_);
    auto* entry = mem_->findAlloc (memLoc);
    return entry? entry->entryID
                : 0;
  }
  
  Literal
  TrackingAllocator::poolID()  const
  {
    ENSURE (mem_);
    return mem_->getPoolID();
  }

  
  
}} // namespace lib::test
