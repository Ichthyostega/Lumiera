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
#include "lib/iter-explorer.hpp"
#include "lib/util.hpp"

#include <type_traits>
#include <cstring>
#include <utility>
#include <vector>



namespace lib {
  using std::vector;
  using std::forward;
  using std::move;
  using std::byte;
  
  namespace {// Allocation management policies
    
    struct HeapOwn
      {
        void*
        realloc (void* data, size_t oldSiz, size_t newSiz)
          {
            UNIMPLEMENTED ("adjust memory allocation"); ///////////////////////////OOO Problem Objekte verschieben
          }
      };
    
    using std::is_trivially_move_constructible_v;
    using std::is_trivially_destructible_v;
    using std::has_virtual_destructor_v;
    using std::is_same_v;
    using lib::meta::is_Subclass;
    
    template<class I, class E>
    struct MemStrategy
      {
        bool disposable :1 ;
        bool wild_move :1 ;
        
        template<typename TY>
        bool
        canDestroy()
          {
            return disposable
                or (is_trivially_destructible_v<TY> and is_trivially_destructible_v<I>)
                or (has_virtual_destructor_v<I> and is_Subclass<TY,I>())
                or (is_same_v<TY,E> and is_Subclass<E,I>());
          }
        
        template<typename TY>
        bool
        canDynGrow()
          {
            return is_same_v<TY,E>
                or (is_trivially_move_constructible_v<TY> and wild_move);
          }
        
        auto
        getDeleter()
          {
            if constexpr (disposable or
                          (is_trivially_destructible_v<E> and is_trivially_destructible_v<I>))
              return nullptr;
            if constexpr (has_virtual_destructor_v<I>)
              return nullptr;
            else
              return nullptr;
          }
      };
  }
  
  /**
   * Wrap a vector holding objects of a subtype and
   * provide array-like access using the interface type.
   */
  template<class I             ///< Interface or base type visible on resulting Several<I>
          ,class E   =I        ///< a subclass element element type (relevant when not trivially movable and destructible)
          ,class POL =HeapOwn  ///< Allocator policy
          >
  class SeveralBuilder
    : Several<I>
    , POL
    {
      using Col = Several<I>;
      
      size_t storageSiz_{0};
      
    public:
      SeveralBuilder() = default;
      
      /** start Several build using a custom allocator */
      template<typename...ARGS,                  typename = meta::enable_if<std::is_constructible<POL,ARGS...>>>
      SeveralBuilder (ARGS&& ...alloInit)
        : Several<I>{}
        , POL{forward<ARGS> (alloInit)...}
        { }
      
      
      SeveralBuilder&&
      reserve (size_t cntElm)
        {
          adjustStorage (cntElm, sizeof(I));
          return move(*this);
        }
      
      template<class IT>
      SeveralBuilder&&
      appendAll (IT&& data)
        {
          explore(data).foreach ([this](auto it){ emplaceElm(it); });
          return move(*this);
        }
      
      Several<I>
      build()
        {
          return move (*this);
        }
      
    private:
      void
      adjustStorage (size_t cnt, size_t spread)
        {
          if (cnt*spread > storageSiz_)
            {  //  need more storage...
              Col::data_ = static_cast<typename Col::Bucket> (POL::realloc (Col::data_, storageSiz_, cnt*spread));
              storageSiz_ = cnt*spread;
            }
          ENSURE (Col::data_);
          if (spread != Col::data_->spread)
            adjustSpread (spread);

          if (cnt*spread < storageSiz_)
            {  //  attempt to shrink storage
              Col::data_ = static_cast<typename Col::Bucket> (POL::realloc (Col::data_, storageSiz_, cnt*spread));
              storageSiz_ = cnt*spread;
            }
        }
      
      /** move existing data to accommodate spread */
      void
      adjustSpread (size_t newSpread)
        {
          REQUIRE (Col::size_);
          REQUIRE (Col::data_);
          REQUIRE (newSpread * Col::size_ <= storageSiz_);
          size_t oldSpread = Col::data_->spread;
          if (newSpread > oldSpread)
            // need to spread out
            for (size_t i=Col::size_-1; 0<i; --i)
              shiftStorage (i, oldSpread, newSpread);
          else
            // attempt to condense spread
            for (size_t i=1; i<Col::size_; ++i)
              shiftStorage (i, oldSpread, newSpread);
          // data elements now spaced by new spread
          Col::data_->spread = newSpread;
        }
      
      void
      shiftStorage (size_t idx, size_t oldSpread, size_t newSpread)
        {
          REQUIRE (idx);
          REQUIRE (oldSpread);
          REQUIRE (newSpread);
          REQUIRE (Col::data_);
          byte* oldPos = Col::data_->storage;
          byte* newPos = oldPos;
          oldPos += idx * oldSpread;
          newPos += idx * newSpread;
          std::memmove (newPos, oldPos, util::min (oldSpread,newSpread));
        }
      
      template<class IT>
      void
      emplaceElm (IT& dataSrc)
        {
          using Val = typename IT::value_type;
          size_t elmSiz = sizeof(Val);
          adjustStorage (Col::size_+1, requiredSpread(elmSiz));
          UNIMPLEMENTED ("emplace data");
        }
      
      size_t
      requiredSpread (size_t elmSiz)
        {
          size_t currSpread = Col::empty()? 0 : Col::data_->spread;
          return util::max (currSpread, elmSiz);
        }
    };
  
  
  
  
  template<typename X>
  SeveralBuilder<X>
  makeSeveral (std::initializer_list<X> ili)
  {
    return SeveralBuilder<X>{}
            .reserve (ili.size())
            .appendAll (ili);
  }
  
  
} // namespace lib
#endif /*LIB_SEVERAL_BUILDER_H*/
