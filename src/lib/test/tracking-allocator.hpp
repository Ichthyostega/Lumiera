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
  
  
  namespace {
    const Symbol GLOBAL{"GLOBAL"};
  }
  
  /** common registration table and memory pool */
  class MemoryPool;
  
  using PoolHandle = std::shared_ptr<MemoryPool>;
  
  
  
  class TrackingAllocator
    {
      PoolHandle mem_;
      
    public:
      /** can be default created to attach to a common pool */
      TrackingAllocator();
      
      /** create a separate, marked memory pool */
      TrackingAllocator (Literal id);
      
      // standard copy operations acceptable
      
      using Location = void*;
      
      [[nodiscard]] Location allocate (size_t n);
      void deallocate (Location, size_t =0) noexcept;
      
      
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
      
      bool manages (Location)  const;
      size_t getSize(Location) const;
      HashVal getID (Location) const;
      
      static HashVal checksum (Literal pool =GLOBAL);
      static size_t use_count (Literal pool =GLOBAL);
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
   * C++ standard allocator API : allot raw memory for \a cnt elements of type \a TY
   */
  template<typename TY>
  TY*
  TrackAlloc<TY>::allocate (size_t cnt)
  {
    return static_cast<TY*> (TrackingAllocator::allocate (cnt * sizeof(TY)));
  }
  
  /**
   * C++ standard allocator API : clear an existing allocation,
   * which must have been allocated into the same pool, with given element cnt.
   */
  template<typename TY>
  void
  TrackAlloc<TY>::deallocate (TY* loc, size_t cnt) noexcept
  {
    TrackingAllocator::deallocate (loc, cnt * sizeof(TY));
  }
  
  
  
  
}} // namespace lib::test
#endif /*LIB_TEST_TRACKING_ALLOCATOR_H*/
