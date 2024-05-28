/*
  SEVERAL.hpp  -  abstraction providing a limited fixed number of elements

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
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


/** @file several.hpp
 ** Abstraction interface: array-like access by subscript
 ** @todo as of 2016, this concept seems very questionable: do we _really_ want
 **       to abstract over random access, or do we _actually_ want for-iteration??
 ** @warning WIP-WIP-WIP in rework 5/2025
 ** @see several-builder.hpp
 */


#ifndef LIB_SEVERAL_H
#define LIB_SEVERAL_H


#include "lib/nocopy.hpp"


namespace lib {
  
  /**
   * Abstraction: Array of const references.
   * Typically the return type is an interface, 
   * and the Implementation wraps some datastructure
   * holding subclasses.
   * @todo ouch -- a collection that isn't iterable...  ///////////////////////TICKET #1040     //////////OOO cookie jar
   * @warning in rework 5/2025
   */
  template<class I>
  class Several
    : util::MoveOnly
    {
    public:
      
      size_t
      size()  const
        {
          UNIMPLEMENTED ("determine storage size");
        }
      
      I&
      operator[] (size_t i)
        {
          UNIMPLEMENTED ("subscript");
        }
      
      I&
      back()
        {
          UNIMPLEMENTED ("storage access");
        }
      
      using iterator = I*;
      using const_iterator = I const*;
      
      iterator begin() { UNIMPLEMENTED ("iteration"); }
      iterator end()   { UNIMPLEMENTED ("iteration"); }
      const_iterator begin() const { UNIMPLEMENTED ("iteration"); }
      const_iterator end()   const { UNIMPLEMENTED ("iteration"); }
      
      friend auto begin (Several const& svl) { return svl.begin();}
      friend auto end   (Several const& svl) { return svl.end();  }
    };
  
  
} // namespace lib
#endif
