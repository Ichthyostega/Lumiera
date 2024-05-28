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
#include "lib/iter-adapter.hpp"

#include <cstddef>


namespace lib {
  
  namespace {// Storage implementation details
    
    template<class I>
    struct ArrayBucket
      {
        union Manager
          {
            typedef void (*Deleter) (void*, size_t);
            
            bool unmanaged :1;
            Deleter deleter;
          };
        
        Manager manager;
        size_t spread;
        
        /** mark start of the storage area */
        alignas(I)
          std::byte storage[sizeof(I)];
        
        /** perform unchecked access into the storage area
         * @note typically reaching behind the nominal end of this object
         */
        I&
        subscript (size_t idx)
          {
            std::byte* elm = storage;
            size_t off = idx * spread;
            elm += off;
            return * std::launder (reinterpret_cast<I*> (elm));
          }
      };
    
  }//(End)implementation details
  
  
  
  /************************************************//**
   * Abstraction: Fixed array of elements.
   * Typically the return type is an interface, 
   * and the Implementation wraps some datastructure
   * holding subclasses.
   * @warning in rework 5/2025
   */
  template<class I>
  class Several
    : util::MoveAssign
    {
    protected:
      using Bucket = ArrayBucket<I>*;
      
      size_t size_{0};
      Bucket data_{nullptr};
      
    public:
      
      size_t
      size()  const
        {
          return size_;
        }
      
      bool
      empty()  const
        {
          return not (size_ and data_);
        }
      
      I&
      operator[] (size_t idx)
        {
          REQUIRE (data_);
          return data_->subscript (idx);
        }
      
      I& front() { return operator[] (size_-1); }
      I& back()  { return operator[] (0);       }
      
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
