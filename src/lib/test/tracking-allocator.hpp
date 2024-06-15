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

#include <cstddef>
#include <utility>
#include <list>



namespace lib {
  namespace allo {///< Concepts and Adaptors for custom memory management
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1366 : define Allocator Concepts here
    /// TODO the following Concepts can be expected here (with C++20)
    /// - Allocator : for the bare memory allocation
    /// - Factory : for object fabrication and disposal
    /// - Handle : a functor front-end to be dependency-injected
    
    
    /**
     * Adapter to implement the *Factory* concept based on a `std::allocator`
     * @tparam ALO a std::allocator instance or anything compliant to [Allocator]
     * [Allocator]: https://en.cppreference.com/w/cpp/named_req/Allocator
     * @note in addition to the abilities defined by the standard, this adapter
     *       strives to provide some kind of _lateral leeway,_ attempting to
     *       create dedicated allocators for other types than the BaseType
     *       implied by the given \a ALO (standard-allocator).
     *       - this is possible if the rebound allocator can be constructed
     *         from the given base allocator
     *       - alternatively, an attempt will be made to default-construct
     *         the rebound allocator for the other type requested.
     * @warning Both avenues for adaptation may fail,
     *         which could lead to compilation or runtime failure.
     * @remark deliberately this class inherits from the allocator,
     *         allowing to exploit empty-base-optimisation, since
     *         usage of monostate allocators is quite common.
     */
    template<class ALO>
    class StdFactory
      : private ALO
      {
        using Allo  = ALO;
        using AlloT = std::allocator_traits<Allo>;
        using BaseType = typename Allo::value_type;
        
        Allo& baseAllocator() { return *this; }
        
        template<typename X>
        auto
        adaptAllocator()
          {
            using XAllo = typename AlloT::template rebind_alloc<X>;
            if constexpr (std::is_constructible_v<XAllo, Allo>)
              return XAllo{baseAllocator()};
            else
              return XAllo{};
          }
        
        template<class ALOT, typename...ARGS>
        typename ALOT::pointer
        construct (typename ALOT::allocator_type& allo, ARGS&& ...args)
          {
            auto loc = ALOT::allocate (allo, 1);
            try { ALOT::construct (allo, loc, std::forward<ARGS>(args)...); }
            catch(...)
              {
                ALOT::deallocate (allo, loc, 1);
                throw;
              }
            return loc;
          }
        
        template<class ALOT>
        void
        destroy (typename ALOT::allocator_type& allo, typename ALOT::pointer elm)
          {
            ALOT::destroy (allo, elm);
            ALOT::deallocate (allo, elm, 1);
          }
        
        
      public:
        /**
         * Create an instance of the adapter factory,
         * forwarding to the embedded standard conforming allocator
         * for object creation and destruction and memory management.
         * @param allo (optional) instance of the C++ standard allocator
         *        used for delegation, will be default constructed if omitted.
         * @remark the adapted standard allocator is assumed to be either a copyable
         *        value object, or even a mono-state; in both cases, a dedicated
         *        manager instance residing »elsewhere« is referred, rendering
         *        all those front-end instances exchangeable.
         */
        StdFactory (Allo allo = Allo{})
          : Allo{std::move (allo)}
          { }
        
        template<class XALO>
        bool constexpr operator== (StdFactory<XALO> const& o)  const
          {
            return baseAllocator() == o.baseAllocator();
          }
        template<class XALO>
        bool constexpr operator!= (StdFactory<XALO> const& o)  const
          {
            return not (*this == o);
          }
        
        
        /** create new element using the embedded allocator */
        template<class TY, typename...ARGS>
        TY*
        create (ARGS&& ...args)
          {
            if constexpr (std::is_same_v<TY, BaseType>)
              {
                return construct<AlloT> (baseAllocator(), std::forward<ARGS>(args)...);
              }
            else
              {
                using XAlloT = typename AlloT::template rebind_traits<TY>;
                auto xAllo = adaptAllocator<TY>();
                return construct<XAlloT> (xAllo, std::forward<ARGS>(args)...);
              }
          }
        
        /** destroy the given element and discard the associated memory */
        template<class TY>
        void
        dispose (TY* elm)
          {
            if constexpr (std::is_same_v<TY, BaseType>)
              {
                destroy<AlloT> (baseAllocator(), elm);
              }
            else
              {
                using XAlloT = typename AlloT::template rebind_traits<TY>;
                auto xAllo = adaptAllocator<TY>();
                destroy<XAlloT> (xAllo, elm);
              }
          }
      };
  }
  
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1366 : the following code becomes obsolete in the long term
  
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
  
  
  
} // namespace lib
#endif /*LIB_TEST_TRACKING_ALLOCATOR_H*/
