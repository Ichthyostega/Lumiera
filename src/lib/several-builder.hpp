/*
  SEVERAL-BUILDER.hpp  -  builder for a limited fixed collection of elements

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

*/

/** @file several-builder.hpp
 ** Some (library-) implementations of the RefArray interface.
 ** 
 ** Being an array-like object exposing just a const ref, it is typically used
 ** on interfaces, and the type of the array "elements" usually is a ABC or interface.
 ** The actual implementation typically holds a subclass, and is either based on a vector,
 ** or a fixed storage contained within the implementation. The only price to pay is
 ** a virtual call on element access.
 ** 
 ** For advanced uses it would be possible to have a pointer-array or even an embedded
 ** storage of variant-records, able to hold a mixture of subclasses. (the latter cases
 ** will be implemented when needed).
 ** 
 ** @warning WIP and in rework 5/2025 -- not clear yet where this design leads to...
 ** @see several-builder-test.cpp
 ** 
 */


#ifndef LIB_SEVERAL_BUILDER_H
#define LIB_SEVERAL_BUILDER_H


#include "lib/several.hpp"
#include "include/logging.h"

#include <utility>
#include <vector>

using std::vector;
using std::move;



namespace lib {
  
  namespace {// Allocation managment policies
    
    struct HeapOwn
      {
        
      };
  }
  
  /**
   * Wrap a vector holding objects of a subtype and
   * provide array-like access using the interface type.
   */
  template<class I, class POL =HeapOwn>
  class SeveralBuilder
    : Several<I>
    , POL
    {

    public:
      Several<I>
      build()
        {
          return move (*this);
        }
    };
  
  template<typename X>
  SeveralBuilder<X>
  makeSeveral (std::initializer_list<X> ili)
  {
    UNIMPLEMENTED ("start building a Several-Container");
  }
  
  
} // namespace lib
#endif /*LIB_SEVERAL_BUILDER_H*/
