/*
  REF-ARRAY.hpp  -  abstraction providing array-like access to a list of references

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file ref-array.hpp
 ** Abstraction interface: array-like access by subscript
 ** @todo as of 2016, this concept seems very questionable: do we _really_ want
 **       to abstract over random access, or do we _actually_ want for-iteration??
 ** @warning in rework 5/2025
 */


#ifndef LIB_REF_ARRAY_H
#define LIB_REF_ARRAY_H


#include "lib/nocopy.hpp"


namespace lib {
  
  /**
   * Abstraction: Array of const references.
   * Typically the return type is an interface, 
   * and the Implementation wraps some datastructure
   * holding subclasses.
   * @todo ouch -- a collection that isn't iterable...  ///////////////////////TICKET #1040
   * @warning in rework 5/2025
   */
  template<class T>
  class RefArray
    : util::NonCopyable
    {
    public:
      virtual ~RefArray() {}  ///< this is an interface
      
      virtual T const& operator[] (size_t i)  const =0;
      virtual size_t size()                   const =0;
    };
  
  
} // namespace lib
#endif
