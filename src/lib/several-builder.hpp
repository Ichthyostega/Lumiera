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


#include "lib/error.hpp"
#include "lib/several.hpp"
#include "include/limits.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

#include <type_traits>
#include <cstring>
#include <utility>
#include <vector>



namespace lib {
  namespace err = lumiera::error;
  
  using std::vector;
  using std::forward;
  using std::move;
  using std::byte;
  
  namespace {// Allocation management policies
    
    using util::max;
    using util::min;
    using util::_Fmt;
    
    template<class I, template<typename> class ALO>
    class ElementFactory
      : private ALO<std::byte>
      {
        using Allo = ALO<std::byte>;
        using AlloT = std::allocator_traits<Allo>;
        using Bucket = ArrayBucket<I>;
        
        Allo& baseAllocator() { return *this; }
        
        template<typename X>
        auto
        adaptAllocator()
          {
            using XAllo = typename AlloT::template rebind_alloc<X>;
            if constexpr (std::is_constructible_v<XAllo, Allo>)
              return XAllo{baseAllocator()};
            else
              return XAllo{};
          }
        
      public:
        ElementFactory (Allo allo = Allo{})
          : Allo{std::move (allo)}
          { }
        
        Bucket*
        create (size_t cnt, size_t spread)
          {
            size_t storageBytes = Bucket::requiredStorage (cnt, spread);
            std::byte* loc = AlloT::allocate (baseAllocator(), storageBytes);
            Bucket* bucket = reinterpret_cast<Bucket*> (loc);
            
            using BucketAlloT = typename AlloT::template rebind_traits<Bucket>;
            auto bucketAllo = adaptAllocator<Bucket>();
            try { BucketAlloT::construct (bucketAllo, bucket, storageBytes, spread); }
            catch(...)
              {
                AlloT::deallocate (baseAllocator(), loc, storageBytes);
                throw;
              }
            return bucket;
          };
        
        template<class E, typename...ARGS>
        E&
        createAt (Bucket* bucket, size_t idx, ARGS&& ...args)
          {
            REQUIRE (bucket);
            using ElmAlloT = typename AlloT::template rebind_traits<E>;
            auto elmAllo = adaptAllocator<E>();
            E* loc = & bucket->subscript (idx);
            ElmAlloT::construct (elmAllo, loc, forward<ARGS> (args)...);
            ENSURE (loc);
            return *loc;
          };
        
        template<class E>
        void
        destroy (ArrayBucket<I>* bucket)
          {
            REQUIRE (bucket);
            size_t cnt = bucket->cnt;
            size_t storageBytes = bucket->buffSiz;
            using ElmAlloT = typename AlloT::template rebind_traits<E>;
            auto elmAllo = adaptAllocator<E>();
            for (size_t idx=0; idx<cnt; ++idx)
              ElmAlloT::destroy (elmAllo, & bucket->subscript(idx));
            
            std::byte* loc = reinterpret_cast<std::byte*> (bucket);
            AlloT::deallocate (baseAllocator(), loc, storageBytes);
          };
      };
    
    
    template<class I, template<typename> class ALO>
    struct AllocationPolicy
      : ElementFactory<I, ALO>
      {
        using Fac = ElementFactory<I, ALO>;
        using Bucket = ArrayBucket<I>;
        
        using Fac::Fac; // pass-through ctor
        
        Bucket*
        realloc (Bucket* data, size_t demand)
          {
            size_t buffSiz{data? data->buffSiz : 0};
            if (demand == buffSiz)
              return data;
            if (demand > buffSiz)
              {// grow into exponentially expanded new allocation
                size_t spread = data? data->spread : sizeof(I);
                size_t safetyLim = LUMIERA_MAX_ORDINAL_NUMBER * spread;
                size_t expandAlloc = min (safetyLim
                                         ,max (2*buffSiz, demand));
                if (expandAlloc < demand)
                  throw err::State{_Fmt{"Storage expansion for Several-collection "
                                        "exceeds safety limit of %d bytes"} % safetyLim
                                  ,LERR_(SAFETY_LIMIT)};
                // allocate new storage block...
                size_t newCnt = demand / spread;
                if (newCnt * spread < demand) ++newCnt;
                Bucket* newBucket = Fac::create (newCnt, spread);
                // move (or copy) existing data...
                size_t cnt = data? data->cnt : 0;
                for (size_t idx=0; idx<cnt; ++idx)
                  Fac::template createAt<I> (newBucket, idx
                                            ,std::move_if_noexcept (data->subscript(idx)));
                                   ////////////////////////////////////////////////////////OOO schee... aba mia brauchn E, ned I !!!!!
                // discard old storage
                if (data)
                  Fac::template destroy<I> (data);
                return newBucket;
              }
            else
              {// shrink into precisely fitting new allocation
                Bucket* newBucket{nullptr};
                if (data)
                  {
                    size_t cnt{data->cnt}; 
                    ASSERT (cnt > 0);
                    newBucket = Fac::create (cnt, data->spread);
                    for (size_t idx=0; idx<cnt; ++idx)
                      Fac::template createAt<I> (newBucket, idx
                                                ,std::move_if_noexcept (data->subscript(idx)));    ////////////OOO selbes Problem: E hier
                    Fac::template destroy<I> (data);
                  }
                return newBucket;
              }
          }
      };
    
    template<class I>
    using HeapOwn = AllocationPolicy<I, std::allocator>;
    
    
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
  template<class I                ///< Interface or base type visible on resulting Several<I>
          ,class E   =I           ///< a subclass element element type (relevant when not trivially movable and destructible)
          ,class POL =HeapOwn<I>  ///< Allocator policy
          >
  class SeveralBuilder
    : Several<I>
    , util::MoveOnly
    , POL
    {
      using Coll = Several<I>;
      
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
          adjustStorage (cntElm, sizeof(E));
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
          size_t demand{cnt*spread};
          Coll::data_ = POL::realloc (Coll::data_, demand);
          ENSURE (Coll::data_);
          if (spread != Coll::data_->spread) ///////////////OOO API für spread?
            adjustSpread (spread);
        }
      
      void
      fitStorage()
        {
          if (not Coll::data_) return;
          size_t demand{Coll::size_ * Coll::data_->spread};
          Coll::data_ = POL::realloc (Coll::data_, demand);
        }
      
      /** move existing data to accommodate spread */
      void
      adjustSpread (size_t newSpread)
        {
          REQUIRE (Coll::data_);
          REQUIRE (newSpread * Coll::size_ <= storageSiz_);
          size_t oldSpread = Coll::data_->spread;
          if (newSpread > oldSpread)
            // need to spread out
            for (size_t i=Coll::size()-1; 0<i; --i)
              shiftStorage (i, oldSpread, newSpread);
          else
            // attempt to condense spread
            for (size_t i=1; i<Coll::size(); ++i)
              shiftStorage (i, oldSpread, newSpread);
          // data elements now spaced by new spread
          Coll::data_->spread = newSpread;
        }
      
      void
      shiftStorage (size_t idx, size_t oldSpread, size_t newSpread)
        {
          REQUIRE (idx);
          REQUIRE (oldSpread);
          REQUIRE (newSpread);
          REQUIRE (Coll::data_);
          byte* oldPos = Coll::data_->storage;
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
          adjustStorage (Coll::size()+1, requiredSpread(elmSiz));
          UNIMPLEMENTED ("emplace data");
        }
      
      size_t
      requiredSpread (size_t elmSiz)
        {
          size_t currSpread = Coll::empty()? 0 : Coll::data_->spread;
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
