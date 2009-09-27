/*
  TYPED-COUNTER.hpp  -  maintain a set of type based contexts 
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
*/


/** @file typed-counter.hpp
 ** Creating sets of type-based contexts.
 ** The idea is to get a "slot" for any given type, so we can build
 ** tables or families of implementations based on these types. Currently,
 ** the slot allocation is based on static variables and thus is global.
 ** This leads to a waste of slots, as the various clients of this service
 ** typically will utilise different sets of types.
 ** @todo WIP WIP... this is the first, preliminary version of a facility,
 **       which is expected to get quite important for custom allocation management.
  ** 
 ** @see typed-counter-test.cpp
 ** @see TypedAllocationManager
 ** @see AllocationCluster (custom allocation scheme using a similar idea inline)
 **
 */



#ifndef LIB_TYPED_COUNTER_H
#define LIB_TYPED_COUNTER_H

//#include "pre.hpp"
#include "lib/error.hpp"
//#include "lib/format.hpp"
//#include "include/logging.h"


//#include <tr1/memory>



namespace lib {
  
//using std::tr1::shared_ptr;
  
  
  
  
  /** 
   * Helper providing a set of counters, each tied to a specific type. 
   */
  class TypedCounter
    {
    public:
      template<class X>
      size_t 
      get()  const
        {
          UNIMPLEMENTED ("get typed count");
        }
      
      template<class X>
      void 
      inc()
        {
          UNIMPLEMENTED ("increment typed count");
        }
      
      template<class X>
      void 
      dec()
        {
          UNIMPLEMENTED ("decrement typed count");
        }
    };
  
  
  
} // namespace lib
#endif
