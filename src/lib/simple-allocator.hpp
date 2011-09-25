/*
  SIMPLE-ALLOCATOR.hpp  -  frontend for plain explicit allocations 

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

//#include "pre.hpp"
#include "lib/error.hpp"
#include "lib/meta/generator.hpp"
#include "lib/format.hpp"
//#include "lib/typed-counter.hpp"
#include "include/logging.h"


//#include <tr1/memory>
#include <boost/static_assert.hpp>
#include <memory>



namespace lib {
  
//  using std::tr1::shared_ptr;
  using lumiera::typelist::InstantiateForEach;
  using lumiera::typelist::Types;
  
  namespace {
    using lumiera::typelist::Node;
    using lumiera::typelist::NullType;
    
    template<typename TYPES, typename TY>
    struct IsInList
      {
        enum{ value = false };
      };
    
    template<typename TY, typename TYPES>
    struct IsInList<Node<TY,TYPES>, TY>
      {
        enum{ value = true };
      };
    
    template<typename XX, typename TYPES, typename TY>
    struct IsInList<Node<XX,TYPES>, TY>
      {
        enum{ value = IsInList<TYPES,TY>::value };
      };
    
  }
  
  
  template<typename TY>
  class CustomAllocator
    : public std::allocator<TY>
    {
      
    };
  
    
  
  /**
   * Frontend for explicit allocations, using a custom allocator.
   * This template is to be instantiated for the collection of types
   * later to be allocated through this custom memory manager/model.
   * It provides convenience shortcuts for placement-construction
   * and releasing of target objects.
   * 
   * @todo currently (as of 8/09) the low-level pooled allocator
   *       isn't implemented; instead we do just heap allocations.
   *       see Ticket #835
   */
  template<typename TYPES>
  class SimpleAllocator
    : InstantiateForEach< typename TYPES::List     // for each of those types...         
                        , CustomAllocator         //  ...mix in the custom allocator 
                        >
    {
      
      /** forward plain memory allocation */
      template<class XX>
      XX *
      allocateSlot ()
        {
          TRACE (memory, "allocate %s", util::tyStr<XX>().c_str());
          return CustomAllocator<XX>::allocate (1);
        }
      
      template<class XX>
      void
      releaseSlot (XX* entry)
        {
          TRACE (memory, "release %s", util::tyStr<XX>().c_str());
          CustomAllocator<XX>::deallocate (entry, 1);
        }
      
      
      template<class XX>
      void
      ___assertSupportedType()
        {
          typedef typename TYPES::List PreconfiguredTypes;
          typedef IsInList<PreconfiguredTypes,XX> IsPreconfiguredType;
          
          BOOST_STATIC_ASSERT (IsPreconfiguredType::value);
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
              WARN (common_dbg, "dtor of %s failed: %s", util::tyStr(entry).c_str()
                                                       , errorID );
            }
          releaseSlot<XX> (entry);
        }
      
    };
  
  
  

} // namespace lib
#endif
