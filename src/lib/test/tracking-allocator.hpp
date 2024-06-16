/*
  TRACKING-ALLOCATOR.hpp  -  custom allocator for memory management diagnostics

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


/** @file tracking-allocator.hpp
 ** unittest helper code: a custom allocator to track memory usage.
 ** By registering each allocation and deallocation, correct memory handling
 ** can be verified and memory usage can be investigated in practice.
 ** \par TrackingAllocator
 ** The foundation is to allow raw memory allocations, which are attached
 ** and tracked within some memory pool, allowing to investigate the number
 ** of allocations, number of currently allotted bytes and a checksum.
 ** Moreover, all relevant actions are logged into an lib::test::EventLog.
 ** By default a common global MemoryPool is used, while it is possible
 ** to carry out all those operations also on a dedicated pool; the user
 ** visible »allocators« are actually shared-ownership smart-handles.
 ** \par TrackingFactory
 ** Built on top is a standard factory to create and destroy arbitrary
 ** object instances, with the corresponding allocations attached to
 ** the underlying MemoryPool
 ** \par C++ standard allocator adapter
 ** A variation of this is the TrackAlloc<TY> template, which can be
 ** used as custom allocator adapter within STL containers
 ** @remark these classes also work in concert with the building blocks
 **         from allocator-handle.hpp; notably it is possible to create
 **         a OwnUniqueAdapter front-end for fabricating `unique_ptr`
 ** @see TestTracking_test
 */


#ifndef LIB_TEST_TRACKING_ALLOCATOR_H
#define LIB_TEST_TRACKING_ALLOCATOR_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/nocopy.hpp"
#include "lib/hash-value.h"
#include "lib/test/event-log.hpp"
#include "lib/format-string.hpp"

#include <utility>
#include <memory>

using std::byte;


namespace lib {
namespace test {
  
  
  namespace {
    const Symbol GLOBAL{"GLOBAL"};
  }
  
  /** common registration table and memory pool */
  class MemoryPool;
  
  using PoolHandle = std::shared_ptr<MemoryPool>;
  
  
  
  /**
   * Generic low-level allocator attached to tracking MemoryPool.
   * Effectively this is a shared handle front-end to the MemoryPool, and
   * new distinct pools are generated (and discarded) on demand, keyed by a pool-ID.
   * A global (singleton) pool is used when no pool-ID is explicitly given.
   */
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
      Literal poolID ()        const;
      
      static HashVal checksum (Literal pool =GLOBAL);
      static size_t use_count (Literal pool =GLOBAL);
      static size_t numAlloc  (Literal pool =GLOBAL);
      static size_t numBytes  (Literal pool =GLOBAL);
      
      static EventLog log;
    };
  
  
  /**
   * C++ standard compliant _custom allocator adapter_
   * backed by the TrackingAllocator and the MemoryPool denoted at construction.
   * TrackAlloc adapters can be default constructed (thereby using the GLOBAL pool),
   * or created with a designated pool-ID or copy/move constructed from any other
   * TrackAlloc adapter (then using the same backing pool)
   * @tparam TY the type of intended product object, to determine the size
   */
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
   * Generic object factory backed by TrackingAllocator.
   */
  class TrackingFactory
    : public TrackingAllocator
    {
    public:
      using TrackingAllocator::TrackingAllocator;

      /** attach to the given TrackingAllocator and MemoryPool */
      TrackingFactory (TrackingAllocator const& anchor)
        : TrackingAllocator{anchor}
        { }
      
      /** create new element with an allocation registered in the backing MemoryPool */
      template<class TY, typename...ARGS>
      TY*
      create (ARGS&& ...args)
        {
          Location loc = allocate(sizeof(TY));
          log.call (poolID(),"create-"+util::typeStr<TY>(), std::forward<ARGS> (args)...);
          try {
              return new(loc) TY (std::forward<ARGS> (args)...);
            }
          catch (std::exception& mishap)
            {
              log.error (util::_Fmt{"CtorFail: type=%s, problem:%s"} % util::typeStr<TY>() % mishap.what());
              throw;
            }
          catch (...)
            {
              log.error ("CtorFail: unknown cause");
              throw;
            }
        }
      
      /** destroy the given element and discard the associated memory and registration */
      template<class TY>
      void
      dispose (TY* elm)  noexcept
        {
          if (not elm)
            {
              log.warn("dispose(nullptr)");
              return;
            }
          log.call (poolID(),"destroy-"+util::typeStr<TY>());
          elm->~TY();
          deallocate(elm, sizeof(TY));
        }
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
