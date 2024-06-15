/*
  TRACKING-ALLOCATOR.hpp  -  test dummy objects for tracking ctor/dtor calls

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file tracking-allocator.hpp
 ** unittest helper code: a custom allocator to track memory usage.
 ** By registering each allocation and deallocation, correct memory handling
 ** can be verified and memory usage can be investigated in practice.
 */


#ifndef LIB_TEST_TRACKING_ALLOCATOR_H
#define LIB_TEST_TRACKING_ALLOCATOR_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/hash-value.h"
#include "lib/symbol.hpp"
#include "lib/test/event-log.hpp"

//#include <cstddef>
#include <utility>
#include <memory>
#include <list>

using std::byte;


namespace lib {
namespace test {
  
    
  namespace { // common memory management for the TrackingAllocator
    
    const Symbol GLOBAL{"GLOBAL"};
    
    /** common registration table and memory pool */
    class MemoryPool;
  }
  
  
  class TrackingAllocator
    {
      std::shared_ptr<MemoryPool> mem_;
      
    public:
      /** can be default created to attach to a common pool */
      TrackingAllocator();
      
      /** create a separate, marked memory pool */
      TrackingAllocator (Literal id);
      
      // standard copy operations acceptable
      
      
      [[nodiscard]] void* allocate (size_t n);
      void deallocate (void*) noexcept;
      
      
      friend bool
      operator== (TrackingAllocator const& a1, TrackingAllocator const& a2)
      {
        return a1.mem_ == a2.mem_;
      }
      friend bool
      operator!= (TrackingAllocator const& a1, TrackingAllocator const& a2)
      {
        return not (a1 == a2);
      }
      
      
      /* ===== Diagnostics ===== */
      
      static HashVal checksum (Literal pool =GLOBAL);
      static size_t numAlloc  (Literal pool =GLOBAL);
      static size_t numBytes  (Literal pool =GLOBAL);
      
      static EventLog log;      
    };
  
  
  template<typename TY>
  class TrackAlloc
    : public TrackingAllocator
    {
    public:
      using TrackingAllocator::TrackingAllocator;

      /** cross-building for another type, using a common pool */
      template<typename X>
      TrackAlloc (TrackAlloc<X> const& anchor)
        : TrackingAllocator{anchor}
        { }
      
      
      /* ===== C++ standard allocator interface ===== */
        
      using value_type = TY;
      
      [[nodiscard]] TY* allocate (size_t cnt);
      void deallocate (TY*, size_t) noexcept;
    };
  
  
  /**
   * 
   */
  template<typename TY>
  TY*
  TrackAlloc<TY>::allocate (size_t cnt)
  {
    UNIMPLEMENTED ("type-sized alloc");
  }
  
  /**
   * 
   */
  template<typename TY>
  void
  TrackAlloc<TY>::deallocate (TY* loc, size_t cnt) noexcept
  {
    UNIMPLEMENTED ("type-sized de-alloc");
  }
  
  /**
   * Placeholder implementation for a custom allocator
   * @todo shall be replaced by an AllocationCluster eventually
   * @todo 5/2024 to be reworked and aligned with a prospective C++20 Allocator Concept /////////////////////TICKET #1366
   * @remark using `std::list` container, since re-entrant allocation calls are possible,
   *         meaning that further allocations will be requested recursively from a ctor.
   *         Moreover, for the same reason we separate the allocation from the ctor call,
   *         so we can capture the address of the new allocation prior to any possible
   *         re-entrant call, and handle clean-up of allocation without requiring any
   *         additional state flags.....
   */
  template<typename TY>
  class AllocatorHandle
    {
      struct Allocation
        {
          alignas(TY)
            std::byte buf_[sizeof(TY)];
          
          template<typename...ARGS>
          TY&
          create (ARGS&& ...args)
            {
              return *new(&buf_) TY {std::forward<ARGS> (args)...};
            }
          
          TY&
          access()
            {
              return * std::launder (reinterpret_cast<TY*> (&buf_));
            }
          void
          discard() /// @warning strong assumption made here: Payload was created
            {
              access().~TY();
            }
        };
      
      std::list<Allocation> storage_;
      
    public:
      template<typename...ARGS>
      TY&
      operator() (ARGS&& ...args)
        {                  // EX_STRONG
          auto pos = storage_.emplace (storage_.end());  ////////////////////////////////////////////////////TICKET #230 : real implementation should care for concurrency here
          try {
              return pos->create (std::forward<ARGS> (args)...);
            }
          catch(...)
            {
              storage_.erase (pos); // EX_FREE
              
              const char* errID = lumiera_error();
              ERROR (memory, "Allocation failed with unknown exception. "
                             "Lumiera errorID=%s", errID?errID:"??");
              throw;
            }
        }
      
      /** @note need to do explicit clean-up, since a ctor-call might have been failed,
       *        and we have no simple way to record this fact internally in Allocation,
       *        short of wasting additional memory for a flag to mark this situation */
     ~AllocatorHandle()
        try {
            for (auto& alloc : storage_)
              alloc.discard();
          }
        ERROR_LOG_AND_IGNORE (memory, "clean-up of custom AllocatorHandle")
    };
  
  
  
}} // namespace lib::test
#endif /*LIB_TEST_TRACKING_ALLOCATOR_H*/
