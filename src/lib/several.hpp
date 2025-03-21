/*
  SEVERAL.hpp  -  abstraction providing a limited fixed number of elements

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
 ** @see several-builder.hpp
 */


#ifndef LIB_SEVERAL_H
#define LIB_SEVERAL_H


#include "lib/nocopy.hpp"
#include "lib/index-iter.hpp"

#include <cstddef>
#include <functional>

namespace util {
  template<class COLL>
  std::string toStringBracket (COLL&& coll);
}

namespace lib {
  
  namespace several {// Storage header implementation details
    
    /** @internal mix-in for self-destruction capabilities
     *  @remark the destructor function is assumed to perform deallocation;
     *          thus the functor is moved in the local stack frame, where it
     *          can be invoked safely; this also serves to prevent re-entrance.
     */
    template<class TAR>
    class SelfDestructor
      {
        std::function<void(TAR*)> dtor_{};
        
      public:
        template<class FUN>
        void
        installDestructor (FUN&& dtor)
          {
            dtor_ = std::forward<FUN> (dtor);
          }
        
       ~SelfDestructor()
          {
            if (isArmed())
              {
                auto destructionFun = std::move(dtor_);
                ENSURE (not dtor_);
                destructionFun (target());
              }
          }
       
        bool isArmed() const { return bool(dtor_); }
        auto getDtor() const { return dtor_;       }
        void destroy()       { target()->~TAR();   }
        TAR* target()        { return static_cast<TAR*>(this); }
      };
    
    /**
     * Metadata record placed immediately before the data storage.
     * @remark SeveralBuilder uses a custom allocation scheme to acquire
     *         a sufficiently sized allocation for ArrayBucket + the data.
     */
    template<class I>
    struct ArrayBucket
      : SelfDestructor<ArrayBucket<I>>
      {
        ArrayBucket (size_t storageSize, size_t buffStart, size_t elmSize = sizeof(I))
          : cnt{0}
          , spread{elmSize}
          , buffSiz{storageSize - buffStart}
          , buffOffset{buffStart}
          { }
        
        using Deleter = std::function<void(ArrayBucket*)>;
        
        size_t cnt;
        size_t spread;
        size_t buffSiz;
        size_t buffOffset;
        
        static constexpr size_t storageOffset = sizeof(ArrayBucket);
        size_t getAllocSize()  const { return buffOffset +buffSiz; }
        
        
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
        
      };
    
  }//(End)implementation details
  
  
  
  /************************************************//**
   * Abstraction: Fixed array of elements.
   * Typically the return type is an interface,
   * and the Implementation wraps some datastructure
   * holding subclasses.
   * @note may only be populated through SeveralBuilder
   */
  template<class I>
  class Several
    : util::MoveAssign
    {
    protected:
      using Bucket = several::ArrayBucket<I>*;
      
      Bucket data_{nullptr};
      
    public:
      Several() =default; ///< usually to be created through SeveralBuilder
      
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
      
      explicit
      operator std::string()  const;
      
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
      
      using iterator       = lib::IndexIter<Several>;
      using const_iterator = lib::IndexIter<const Several>;
      
      iterator begin()             { return iterator{*this};      }
      iterator end()               { return iterator{};           }
      const_iterator begin() const { return const_iterator{*this};}
      const_iterator end()   const { return const_iterator{};     }
      
      friend auto begin (Several& svl)       { return svl.begin();}
      friend auto end   (Several& svl)       { return svl.end();  }
      friend auto begin (Several const& svl) { return svl.begin();}
      friend auto end   (Several const& svl) { return svl.end();  }
      
      using value_type = typename meta::RefTraits<I>::Value;
      using reference  = typename meta::RefTraits<I>::Reference;
      using const_reference = value_type const&;
      
      
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
    };
  
  
  /** generic string rendering delegated to `util::toString(elm)` */
  template<class I>
  inline
  Several<I>::operator std::string()  const
  {
    return util::toStringBracket (*this);
  }
  
} // namespace lib
#endif
