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
 ** Abstraction interface: array-like random access by subscript.
 ** 
 ** # Design
 ** 
 ** This is a data structure abstraction suitable for performance critical code.
 ** It is used pervasively in the backbone of the Lumiera »Render Node Network«.
 ** - usage is clear and concise, allowing to hide implementation details
 ** - adaption and optimisation for various usage patterns is possible
 ** - suitably fast read access with a limited amount of indirections
 ** \par why not `std::vector`?
 ** The most prevalent STL container _almost_ fulfils the above mentioned criteria,
 ** and thus served as a blueprint for design and implementations. Some drawbacks
 ** however prevent its direct use for this purpose. Notably, `std::vector` leaks
 ** implementation details of the contained data and generally exposes way too much
 ** operations; it is not possible to abstract away the concrete element type.
 ** Moreover, using `vector` with a custom allocator is surprisingly complicated,
 ** requires to embody the concrete allocator type into the container type and
 ** requires to store an additional back-link whenever the allocator is not
 ** a _monostate._ The intended use case calls for a large number of small
 ** collection elements, which are repeatedly bulk- allocated and deallocated.
 ** 
 ** The lib::Several container is a smart front-end and exposes array-style
 ** random access through references to a interface type. It can only be created
 ** and populated through a builder, and is immutable during lifetime, while it
 ** can hold non-const element data. The actual implementation data types and the
 ** allocator framework used are _not exposed in the front-end's type signature._
 ** The container is single-ownership (move-asignable); some additional metadata
 ** and the data storage reside in an `ArrayBucket<I>`, managed by the allocator.
 ** In its simplest form, this storage is heap allocated and automatically deleted.
 ** 
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
        ArrayBucket (size_t bytes, size_t elmSize = sizeof(I))
          : cnt{0}
          , spread{elmSize}
          , buffSiz{bytes}
          , deleter{nullptr}
          { }
        
        typedef void (*Deleter) (ArrayBucket*);
        
        size_t cnt;
        size_t spread;
        size_t buffSiz;
        Deleter deleter;
        
        /** mark start of the storage area */
        alignas(I)
          std::byte storage[sizeof(I)];
        
        
        static size_t
        requiredStorage (size_t cnt, size_t spread)
          {
            return sizeof(ArrayBucket) - sizeof(storage)
                 + cnt * spread;
          }
        
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
      
      Bucket data_{nullptr};
      
      Several() =default; ///< may only be created through SeveralBuilder
      
    public:
     ~Several()  noexcept
        try { discardData(); }
        ERROR_LOG_AND_IGNORE (progress, "clean-up Several data")
      
      /// Move-Assignment allowed...
      Several (Several&&)            =default;
      Several& operator= (Several&&) =default;
      
      size_t
      size()  const
        {
          return data_? data_->cnt : 0;
        }
      
      bool
      empty()  const
        {
          return not data_;
        }
      
      I&
      operator[] (size_t idx)
        {
          REQUIRE (data_);
          return data_->subscript (idx);
        }
      
      I& front() { return operator[] (data_? data_->size_-1 : 0); }
      I& back()  { return operator[] (0);                         }
      
      using iterator = I*;
      using const_iterator = I const*;
      
      iterator begin() { UNIMPLEMENTED ("iteration"); }
      iterator end()   { UNIMPLEMENTED ("iteration"); }
      const_iterator begin() const { UNIMPLEMENTED ("iteration"); }
      const_iterator end()   const { UNIMPLEMENTED ("iteration"); }
      
      friend auto begin (Several const& svl) { return svl.begin();}
      friend auto end   (Several const& svl) { return svl.end();  }
      
      
    protected:
      size_t
      spread()  const
        {
          return data_? data_->spread : sizeof(I);
        }
      
      bool
      hasReserve (size_t extraSize)  const
        {
          if (extraSize > spread())
            extraSize += (extraSize - spread())*size();
          return data_
             and data_->buffSiz >= size()*spread() + extraSize;
        }
      
    private:
      void
      discardData()
        {
          if (data_ and data_->deleter)
            (*data_->deleter) (data_);
        }
    };
  
  
} // namespace lib
#endif
