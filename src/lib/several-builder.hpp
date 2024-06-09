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
#include <functional>
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
    
    /** number of storage slots to open initially;
     *  starting with an over-allocation similar to `std::vector`
     */
    const uint INITIAL_ELM_CNT = 10;
    
    
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
    
    using std::is_trivially_copyable_v;
    
    template<class I, class E, template<typename> class ALO>
    struct AllocationPolicy
      : ElementFactory<I, ALO>
      {
        using Fac = ElementFactory<I, ALO>;
        using Bucket = ArrayBucket<I>;
        
        using Fac::Fac; // pass-through ctor
        
        const bool isDisposable{false};    ///< memory must be explicitly deallocated
         
        bool canExpand(size_t){ return false; }
        
        Bucket*
        realloc (Bucket* data, size_t cnt, size_t spread)
          {
            Bucket* newBucket = Fac::create (cnt, spread);
            if (data)
              try {
                  newBucket->deleter = data->deleter;
                  size_t elms = min (cnt, data->cnt);
                  for (size_t idx=0; idx<elms; ++idx)
                    moveElem(idx, data, newBucket);
                  data->destroy();
                }
              catch(...)
                { newBucket->destroy(); }
            return newBucket;
/*            
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
  */        
//          // ensure sufficient storage or verify the ability to re-allocate
//          if (not (Coll::hasReserve(sizeof(TY))
//                   or POL::canExpand(sizeof(TY))
//                   or handling_.template canDynGrow<TY>()))
//            throw err::Invalid{_Fmt{"Unable to accommodate further element of type %s "}
//                                   % util::typeStr<TY>()};
          }
        
        void
        moveElem (size_t idx, Bucket* src, Bucket* tar)
          {
            if constexpr (is_trivially_copyable_v<E>)
              {
                void* oldPos = & src->subscript(idx);
                void* newPos = & tar->subscript(idx);
                size_t amount = min (src->spread, tar->spread);
                std::memmove (newPos, oldPos, amount);
              }
            else
              {
                Fac::template createAt<E> (tar, idx
                                          ,std::move_if_noexcept (src->subscript(idx)));
              }
            tar->cnt = idx+1; // mark fill continuously for proper clean-up after exception   
          }
      };
    
    template<class I, class E>
    using HeapOwn = AllocationPolicy<I, E, std::allocator>;
    
    
    using std::is_trivially_move_constructible_v;
    using std::is_trivially_destructible_v;
    using std::has_virtual_destructor_v;
    using std::is_trivially_copyable_v;
    using std::is_same_v;
    using lib::meta::is_Subclass;
    
    template<class I, class E>
    struct HandlingStrategy
      {
        enum DestructionMethod { UNKNOWN
                               , TRIVIAL
                               , ELEMENT
                               , VIRTUAL
                               };
        static Literal
        render(DestructionMethod m)
          {
            switch (m)
              { 
                case TRIVIAL: return "trivial";
                case ELEMENT: return "fixed-element-type";
                case VIRTUAL: return "virtual-baseclass";
                default:
                  throw err::Logic{"unknown DestructionMethod"};
              }
          }
        
        DestructionMethod destructor{UNKNOWN};
        bool              lock_move{false};
        
        /** mark that we're about to accept an otherwise unknown type,
         *  which can not be trivially moved. This irrevocably disables
         *  relocations by low-level `memove` for this container instance */
        template<typename TY>
        void
        probeMoveCapability()
          {
            if (not (is_same_v<TY,E> or is_trivially_copyable_v<TY>))
              lock_move = true;
          }
        
        bool
        canWildMove()
          {
            return is_trivially_copyable_v<E> and not lock_move;
          }
        
        
        template<typename TY>
        std::function<void(I&)>
        selectDestructor()
          {
            if (is_trivially_destructible_v<TY>)
              {
                __ensureMark (TRIVIAL, util::typeStr<TY>());
                return [](auto){ /* calmness */ };
              }
            if (is_same_v<TY,E> and is_Subclass<E,I>())
              {
                __ensureMark (ELEMENT, util::typeStr<TY>());
                return [](I& elm){ reinterpret_cast<E&>(elm).~E(); };
              }
            if (is_Subclass<TY,I>() and has_virtual_destructor_v<I>)
              {
                __ensureMark (VIRTUAL, util::typeStr<TY>());
                return [](I& elm){ elm.~I(); };
              }
            throw err::Invalid{_Fmt{"Unsupported kind of destructor for element type %s."}
                                   % util::typeStr<TY>()};
          }
        
        void
        __ensureMark(DestructionMethod expectedKind, string typeID)
          {
            if (destructor != UNKNOWN and destructor != expectedKind)
              throw err::Invalid{_Fmt{"Unable to handle destructor for element type %s, "
                                      "since this container has been primed to use %s-deleters."}
                                     % typeID % render(expectedKind)};
            destructor = expectedKind;
          }
      };
  }
  
  /**
   * Wrap a vector holding objects of a subtype and
   * provide array-like access using the interface type.
   */
  template<class I                  ///< Interface or base type visible on resulting Several<I>
          ,class E   =I             ///< a subclass element element type (relevant when not trivially movable and destructible)
          ,class POL =HeapOwn<I,E>  ///< Allocator policy
          >
  class SeveralBuilder
    : Several<I>
    , util::MoveOnly
    , POL
    {
      using Coll = Several<I>;
      
      HandlingStrategy<I,E> handling_{};
      
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
          explore(data).foreach ([this](auto it){ emplaceCopy(it); });
          return move(*this);
        }
      
      Several<I>
      build()
        {
          return move (*this);
        }
      
    private:
      template<class IT>
      void
      emplaceCopy (IT& dataSrc)
        {
          using Val = std::remove_cv_t<typename IT::value_type>;
          emplaceElm<Val> (*dataSrc);
        }
      
      template<class TY, typename...ARGS>
      void
      emplaceElm (ARGS&& ...args)
        {
          // ensure proper configuration of clean-up for the container
          auto invokeDestructor = handling_.template selectDestructor<TY>();
          
          // mark when target type is not trivially movable
          handling_.template probeMoveCapability<TY>();
          
          // ensure sufficient element capacity or the ability to adapt element spread
          if (Coll::spread() < sizeof(TY) and not (Coll::empty() or handling_.canWildMove()))
            throw err::Invalid{_Fmt{"Unable to place element of type %s (size=%d)"
                                    "into container for element size %d."}
                                   % util::typeStr<TY>() % sizeof(TY) % Coll::spread()};
          
          size_t elmSiz = sizeof(TY);
          size_t newPos = Coll::size();
          size_t newCnt = Coll::empty()? INITIAL_ELM_CNT : newPos+1;
          adjustStorage (newCnt, max (elmSiz, Coll::spread()));
          ENSURE (Coll::data_);
          if (not Coll::data_->deleter)
            UNIMPLEMENTED ("setup deleter trampoline");
          Coll::data_->cnt = newPos+1;
          POL::template createAt<TY> (Coll::data_, newPos, forward<ARGS> (args)...);
        }
      
      void
      adjustStorage (size_t cnt, size_t spread)
        {
          size_t demand{cnt*spread};
          size_t buffSiz{Coll::data_? Coll::data_->buffSiz : 0};
          if (demand == buffSiz)
            return;
          if (demand > buffSiz)
            {// grow into exponentially expanded new allocation
              size_t safetyLim = LUMIERA_MAX_ORDINAL_NUMBER * Coll::spread();
              size_t expandAlloc = min (safetyLim
                                       ,max (2*buffSiz, demand));
              if (expandAlloc < demand)
                throw err::State{_Fmt{"Storage expansion for Several-collection "
                                      "exceeds safety limit of %d bytes"} % safetyLim
                                ,LERR_(SAFETY_LIMIT)};
              // allocate new storage block...
              size_t newCnt = expandAlloc / spread;
              if (newCnt * spread < expandAlloc) ++newCnt;
              Coll::data_ = POL::realloc (Coll::data_, newCnt,spread);
            }
          ENSURE (Coll::data_);
          if (handling_.canWildMove() and spread != Coll::spread())
            adjustSpread (spread);
        }
      
      void
      fitStorage()
        {
          if (handling_.lock_move or not Coll::data_)
            return;
          Coll::data_ = POL::realloc (Coll::data_, Coll::size(), Coll::spread());
        }
      
      /** move existing data to accommodate spread */
      void
      adjustSpread (size_t newSpread)
        {
          REQUIRE (Coll::data_);
          REQUIRE (newSpread * Coll::size() <= Coll::data_->buffSiz);
          size_t oldSpread = Coll::spread();
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
