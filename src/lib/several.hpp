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
 ** employed allocator framework are _not exposed in the front-end's type signature._
 ** The container is single-ownership (move-asignable); some additional metadata
 ** and the data storage reside within an `ArrayBucket<I>`, managed by the allocator.
 ** In its simplest form, this storage is heap allocated and automatically deleted.
 ** 
 ** @warning WIP-WIP in rework 6/2025
 ** @see several-builder.hpp
 */


#ifndef LIB_SEVERAL_H
#define LIB_SEVERAL_H


#include "lib/nocopy.hpp"
#include "lib/iter-index.hpp"

#include <cstddef>
#include <functional>


namespace lib {
  
  namespace {// Storage header implementation details
    
    /**
     * Metadata record placed immediately before the data storage.
     * @remark SeveralBuilder uses a custom allocation scheme to acquire
     *         a sufficiently sized allocation for ArrayBucket + the data.
     */
    template<class I>
    struct ArrayBucket
      {
        ArrayBucket (size_t storageSize, size_t buffStart, size_t elmSize = sizeof(I))
          : cnt{0}
          , spread{elmSize}
          , buffSiz{storageSize - buffStart}
          , buffOffset{buffStart}
          , deleter{nullptr}
          { }
        
        using Deleter = std::function<void(ArrayBucket*)>;
        
        size_t cnt;
        size_t spread;
        size_t buffSiz;
        size_t buffOffset;
        Deleter deleter;
        
        static constexpr size_t storageOffset = sizeof(ArrayBucket);
        
        /** data storage area starts immediately behind the ArrayBucket */
        std::byte*
        storage()
          {
            return reinterpret_cast<std::byte*>(this) + buffOffset;
          }
        
        /** perform unchecked access into the storage area */
        I&
        subscript (size_t idx)
          {
            std::byte* elm = storage() + (idx * spread);
            ENSURE (storage() <= elm and elm < storage()+buffSiz);
            return * std::launder (reinterpret_cast<I*> (elm));
          }
        
        void
        destroy()
          {
            if (deleter)
              deleter (this);
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
        try { if (data_) data_->destroy(); }
        ERROR_LOG_AND_IGNORE (progress, "clean-up Several data")
      
      /// Move-Assignment allowed...
      Several (Several&& rr)
        {
          std::swap (data_, rr.data_);
        }
      Several& operator= (Several&& rr)
        {
          std::swap (data_, rr.data_);
          return *this;
        }
      
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
      
      I const&
      operator[] (size_t idx)  const
        {
          return const_cast<Several&>(*this).operator[] (idx);
        }
      
      I& front() { return operator[] (0);                         }
      I& back()  { return operator[] (data_? data_->cnt-1 : 0);   }
      
      using iterator       = lib::IterIndex<Several>;
      using const_iterator = lib::IterIndex<const Several>;
      
      iterator begin()             { return iterator{*this};      }
      iterator end()               { return iterator{};           }
      const_iterator begin() const { return const_iterator{*this};}
      const_iterator end()   const { return const_iterator{};     }
      
      friend auto begin (Several& svl)       { return svl.begin();}
      friend auto end   (Several& svl)       { return svl.end();  }
      friend auto begin (Several const& svl) { return svl.begin();}
      friend auto end   (Several const& svl) { return svl.end();  }
      
      
    protected:
      size_t
      spread()  const
        {
          return data_? data_->spread : sizeof(I);
        }
      
      size_t
      storageBuffSiz()  const
        {
          return data_? data_->buffSiz : 0;
        }
      
      bool
      hasReserve (size_t requiredSize, size_t newElms =1)  const
        {
          if (requiredSize < spread())
            requiredSize = spread();
          size_t extraSize{requiredSize * newElms};
          if (requiredSize > spread())
            extraSize += (requiredSize - spread())*size();
          return data_
             and data_->buffSiz >= size()*spread() + extraSize;
        }
      
    private:
    };
  
  
} // namespace lib
#endif
