/*
  SIMPLE-ALLOCATOR.hpp  -  frontend for plain explicit allocations 

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file simple-allocator.hpp
 ** Frontend and marker interface for allocating small objects explicitly.
 ** Contrary to the TypedAllocationManager, the SimpleAllocator doesn't provide any
 ** ref-counting or tracking facilities, nor does he support bulk de-allocation.
 ** Each object needs to be allocated and released by explicit call.
 ** The advantage over using std::allocator  directly is the shortcut for (placement) construction,
 ** and -- of course -- the ability to exchange the memory model at one central location.
 ** 
 ** SimpleAllocator instances will be defined for a specific collection of types; for each of those
 ** types, there will be an embedded dedicated custom allocator (currently as of 9/2011, just
 ** implemented as std::allocator<TY>). Objects of these preconfigured types can be constructed
 ** and destroyed through this allocator instance. Each call needs to be done explicitly, with
 ** the precise, concrete type to be created or destroyed. This is especially important for
 ** the releasing of objects: there is \em no support for any kind of virtual destruction.
 ** 
 ** @see engine::BufferMetadata
 ** @see TypedAllocationManager (another more elaborate custom allocation scheme)
 **
 */



#ifndef LIB_SIMPLE_ALLOCATOR_H
#define LIB_SIMPLE_ALLOCATOR_H

#include "lib/error.hpp"
#include "lib/meta/generator.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/util.hpp"
#include "lib/typed-counter.hpp"
#include "include/logging.h"

#include <boost/static_assert.hpp>
#include <memory>



namespace lib {
  
  using lib::meta::Types;
  using lib::meta::IsInList;
  using lib::meta::InstantiateForEach;
  
  
  
  /* === Policies for simple custom allocator  === */
  
  /**
   * Policy: use just plain heap allocations
   * @waring whenever you define a specialisation,
   *         _you_ are responsible for proper alignment
   * @see TICKET #1204
   */
  template<typename TY>
  class CustomAllocator
    : public std::allocator<TY>
    { };
  
  
  /**
   * Policy: maintain explicit per type instance count
   * @note this imposes additional locking
   */
  struct UseInstantiationCounting
    {
      template<class XX>
      size_t
      allocationCount()  const
        {
          return allocCnt_.get<XX>();
        }
      
      template<class XX>
      void
      incrementCount()
        {
          allocCnt_.inc<XX>();
        }
      
      template<class XX>
      void
      decrementCount()
        {
          allocCnt_.dec<XX>();
        }
      
    private:
      lib::TypedCounter allocCnt_;
    };
  
  /**
   * Policy: no additional instantiation accounting
   */
  struct NoInstantiationCount
    {
      template<class XX>  size_t allocationCount()  const { return 0; }
      template<class XX>  void    incrementCount()  { /* NOP */ }
      template<class XX>  void    decrementCount()  { /* NOP */ }
    };
    
  
  
  
  
  
  /* === Allocator frontend === */
  
  /**
   * Frontend for explicit allocations, using a custom allocator.
   * This template is to be instantiated for the collection of types
   * later to be allocated through this custom memory manager/model.
   * It provides convenience shortcuts for placement-construction
   * and releasing of target objects.
   * 
   * @todo currently (as of 8/09) the low-level pooled allocator
   *       isn't implemented; instead we do just heap allocations.
   *       ////////////////////////////////////////////////////////////////////////////////////////////Ticket #835
   */
  template<typename TYPES
          ,class COUNTER = NoInstantiationCount       ///< Policy: support instance accounting?
          >
  class SimpleAllocator
    : InstantiateForEach< typename TYPES::List     // for each of those types...
                        , CustomAllocator         //  ...mix in the custom allocator
                        >
    , COUNTER                                   // ...Instantiation accounting policy
    {
      
      /** forward plain memory allocation */
      template<class XX>
      XX *
      allocateSlot ()
        {
          TRACE (memory, "allocate «%s»", util::typeStr<XX>().c_str());
          XX * newStorage = CustomAllocator<XX>::allocate (1);
          COUNTER::template incrementCount<XX>();
          return newStorage;
        }
      
      template<class XX>
      void
      releaseSlot (XX* entry)
        {
          TRACE (memory, "release «%s»", util::typeStr<XX>().c_str());
          CustomAllocator<XX>::deallocate (entry, 1);
          COUNTER::template decrementCount<XX>();
        }
      
      
      template<class XX>
      void
      ___assertSupportedType()
        {
          typedef typename TYPES::List PreconfiguredTypes;
          typedef IsInList<XX, PreconfiguredTypes> IsSupportedType;
          
          BOOST_STATIC_ASSERT (IsSupportedType::value);
        }
      
      
      
      
      public: /* ==== build objects with managed allocation ==== */
      
#define _EXCEPTION_SAFE_INVOKE(_CTOR_)                      \
                                                             \
          ___assertSupportedType<XX>();                       \
          XX* storage = allocateSlot<XX>();                    \
          try                                                   \
            {                                                    \
              return (new(storage) _CTOR_ );                     \
            }                                                    \
          catch(...)                                             \
            {                                                    \
              releaseSlot<XX>(storage);                          \
              throw;                                             \
            }
      
      template< class XX>
      XX*                                                                              //_____________________
      create ()                                                                       ///< invoke default ctor
        {
          _EXCEPTION_SAFE_INVOKE ( XX() )
        }
      
      
      template< class XX, typename P1>
      XX*                                                                              //___________________
      create (P1& p1)                                                                 ///< invoke 1-arg ctor
        {
          _EXCEPTION_SAFE_INVOKE ( XX (p1) )
        }
      
      
      template< class XX
              , typename P1
              , typename P2
              >
      XX*                                                                              //___________________
      create (P1& p1, P2& p2)                                                         ///< invoke 2-arg ctor
        {
          _EXCEPTION_SAFE_INVOKE ( XX (p1,p2) )
        }
      
      
      template< class XX
              , typename P1
              , typename P2
              , typename P3
              >
      XX*                                                                              //___________________
      create (P1& p1, P2& p2, P3& p3)                                                 ///< invoke 3-arg ctor
        {
          _EXCEPTION_SAFE_INVOKE ( XX (p1,p2,p3) )
        }
      
      
      template< class XX
              , typename P1
              , typename P2
              , typename P3
              , typename P4
              >
      XX*                                                                              //___________________
      create (P1& p1, P2& p2, P3& p3, P4& p4)                                         ///< invoke 4-arg ctor
        {
          _EXCEPTION_SAFE_INVOKE ( XX (p1,p2,p3,p4) )
        }
      
      
      template< class XX
              , typename P1
              , typename P2
              , typename P3
              , typename P4
              , typename P5
              >
      XX*                                                                              //___________________
      create (P1& p1, P2& p2, P3& p3, P4& p4, P5& p5)                                 ///< invoke 5-arg ctor
        {
          _EXCEPTION_SAFE_INVOKE ( XX (p1,p2,p3,p4,p5) )
        }
      
#undef _EXCEPTION_SAFE_INVOKE
      
      
      
      template<class XX>
      void
      destroy (XX* entry)
        {
          if (!entry) return;
          ___assertSupportedType<XX>();
          try
            {
              entry->~XX();
            }
          catch(...)
            {
              lumiera_err errorID = lumiera_error();
              WARN (common_dbg, "dtor of «%s» failed: %s", util::typeStr(entry).c_str()
                                                         , errorID );
            }
          releaseSlot<XX> (entry);
        }
      
      
      /** diagnostics */
      template<class XX>
      size_t
      numSlots()  const
        {
          return COUNTER::template allocationCount<XX>();
        }
    };
  
  
  
  
} // namespace lib
#endif
