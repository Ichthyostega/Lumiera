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
 ** Builder to create and populate instances of the lib::Several container.
 ** For mere usage, inclusion of several.hpp should be sufficient, since the
 ** container front-end is generic and intends to hide most details of allocation
 ** and element placement. It is an array-like container, but may hold subclass
 ** elements, while exposing only a reference to the interface type.
 ** 
 ** # Implementation data layout
 ** 
 ** The front-end container lib::Several<I> is actually just a smart-ptr referring
 ** to the actual data storage, which resides within an _array bucket._ Typically
 ** the latter is placed into memory managed by a custom allocator, most notably
 ** lib::AllocationCluster. However, by default, the ArrayBucket<I> will be placed
 ** into heap memory. All further meta information is also maintained alongside
 ** this data allocation, including a _deleter function_ to invoke all element
 ** destructors and de-allocate the bucket itself. Neither the type of the
 ** actual elements, nor the type of the allocator is revealed.
 ** 
 ** Since the actual data elements can (optionally) be of a different type than
 ** the exposed interface type \a I, additional storage and spacing is required
 ** in the element array. The field ArrayBucket<I>::spread defines this spacing
 ** and thus the offset used for subscript access.
 ** 
 ** @todo this is a first implementation solution from 6/2025 — and was deemed
 **       _roughly adequate_ at that time, yet should be revalidated once more
 **       observations pertaining real-world usage are available...
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
    using std::is_trivially_move_constructible_v;
    using std::is_trivially_destructible_v;
    using std::has_virtual_destructor_v;
    using std::is_trivially_copyable_v;
    using std::is_object_v;
    using std::is_volatile_v;
    using std::is_const_v;
    using std::is_same_v;
    using lib::meta::is_Subclass;

    
    /**
     * Helper to determine the »spread« required to hold
     * elements of type \a TY in memory _with proper alignment._
     * @warning assumes that the start of the buffer is also suitably aligned,
     *          which _may not be the case_ for **over-aligned objects** with
     *          `alignof(TY) > alignof(void*)`
     */
    template<typename TY>
    size_t inline constexpr
    reqSiz()
      {
        size_t quant = alignof(TY);
        size_t siz = max (sizeof(TY), quant);
        size_t req = (siz/quant) * quant;
        if (req < siz)
          req += quant;
        return req;
      }
    
    
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
            try { BucketAlloT::construct (bucketAllo, bucket, cnt*spread, spread); }
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
            E* loc = reinterpret_cast<E*> (& bucket->subscript (idx));
            ElmAlloT::construct (elmAllo, loc, forward<ARGS> (args)...);
            ENSURE (loc);
            return *loc;
          };
        
        template<class E>
        void
        destroy (ArrayBucket<I>* bucket)
          {
            REQUIRE (bucket);
            if (not is_trivially_destructible_v<E>)
              {
                size_t cnt = bucket->cnt;
                using ElmAlloT = typename AlloT::template rebind_traits<E>;
                auto elmAllo = adaptAllocator<E>();
                for (size_t idx=0; idx<cnt; ++idx)
                  {
                    E* elm = reinterpret_cast<E*> (& bucket->subscript (idx));
                    ElmAlloT::destroy (elmAllo, elm);
                  }
              }
            size_t storageBytes = Bucket::requiredStorage (bucket->buffSiz);
            std::byte* loc = reinterpret_cast<std::byte*> (bucket);
            AlloT::deallocate (baseAllocator(), loc, storageBytes);
          };
      };
    
    
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
                E& oldElm = reinterpret_cast<E&> (src->subscript (idx));
                Fac::template createAt<E> (tar, idx
                                          ,std::move_if_noexcept (oldElm));
              }
            tar->cnt = idx+1; // mark fill continuously for proper clean-up after exception
          }
      };
    
    template<class I, class E>
    using HeapOwn = AllocationPolicy<I, E, std::allocator>;
    
  }//(End)implementation details
  
  
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
      
      using Bucket = ArrayBucket<I>;
      using Deleter = typename Bucket::Deleter;
      
    public:
      SeveralBuilder() = default;
      
      /** start Several build using a custom allocator */
      template<typename...ARGS,                  typename = meta::enable_if<std::is_constructible<POL,ARGS...>>>
      SeveralBuilder (ARGS&& ...alloInit)
        : Several<I>{}
        , POL{forward<ARGS> (alloInit)...}
        { }
      
      
      /* ===== Builder API ===== */
      
      SeveralBuilder&&
      reserve (size_t cntElm)
        {
          adjustStorage (cntElm, reqSiz<E>());
          return move(*this);
        }
      
      template<typename VAL, typename...VALS>
      SeveralBuilder&&
      append (VAL&& val, VALS&& ...vals)
        {
          emplace<VAL> (forward<VAL> (val));
          if constexpr (0 < sizeof...(VALS))
            return append (forward<VALS> (vals)...);
          else
            return move(*this);
        }
      
      template<class IT>
      SeveralBuilder&&
      appendAll (IT&& data)
        {
          explore(data).foreach ([this](auto it){ emplaceCopy(it); });
          return move(*this);
        }
      
      template<class X>
      SeveralBuilder&&
      appendAll (std::initializer_list<X> ili)
        {
          using Val = typename meta::Strip<X>::TypeReferred;
          for (Val const& x : ili)
            emplaceNewElm<Val> (x);
          return move(*this);
        }
      
      template<typename...ARGS>
      SeveralBuilder&&
      fillElm (size_t cntNew, ARGS&& ...args)
        {
          for ( ; 0<cntNew; --cntNew)
            emplaceNewElm<E> (forward<ARGS> (args)...);
          return move(*this);
        }
      
      template<class TY, typename...ARGS>
      SeveralBuilder&&
      emplace (ARGS&& ...args)
        {
          using Val = typename meta::Strip<TY>::TypeReferred;
          emplaceNewElm<Val> (forward<ARGS> (args)...);
          return move(*this);
        }
      
      
      /**
       * Terminal Builder: complete and lock the collection contents.
       * @note the SeveralBuilder is sliced away, effectively
       *       returning only the pointer to the ArrayBucket.
       */
      Several<I>
      build()
        {
          return move (*this);
        }
      
      size_t size() const { return Coll::size(); }
      bool empty()  const { return Coll::empty();}
      
      
    private:
      template<class IT>
      void
      emplaceCopy (IT& dataSrc)
        {
          using Val = typename IT::value_type;
          emplaceNewElm<Val> (*dataSrc);
        }
      
      template<class TY, typename...ARGS>
      void
      emplaceNewElm (ARGS&& ...args)
        {
          static_assert (is_object_v<TY> and not (is_const_v<TY> or is_volatile_v<TY>));
              
          // mark when target type is not trivially movable
          probeMoveCapability<TY>();
          
          // ensure sufficient element capacity or the ability to adapt element spread
          if (Coll::spread() < reqSiz<TY>() and not (Coll::empty() or canWildMove()))
            throw err::Invalid{_Fmt{"Unable to place element of type %s (size=%d)"
                                    "into container for element size %d."}
                                   % util::typeStr<TY>() % reqSiz<TY>() % Coll::spread()};
          
          // ensure sufficient storage or verify the ability to re-allocate
          if (not (Coll::empty() or Coll::hasReserve(reqSiz<TY>())
                   or POL::canExpand(reqSiz<TY>())
                   or canDynGrow()))
            throw err::Invalid{_Fmt{"Unable to accommodate further element of type %s "}
                                   % util::typeStr<TY>()};
          
          size_t elmSiz = reqSiz<TY>();
          size_t newPos = Coll::size();
          size_t newCnt = Coll::empty()? INITIAL_ELM_CNT : newPos+1;
          adjustStorage (newCnt, max (elmSiz, Coll::spread()));
          ENSURE (Coll::data_);
          ensureDeleter<TY>();
          POL::template createAt<TY> (Coll::data_, newPos, forward<ARGS> (args)...);
          Coll::data_->cnt = newPos+1;
        }
      
      template<class TY>
      void
      ensureDeleter()
        {
          // ensure clean-up can be handled properly
          Deleter deleterFunctor = selectDestructor<TY>();
          if (Coll::data_->deleter) return;
          Coll::data_->deleter = deleterFunctor;
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
          if (canWildMove() and spread != Coll::spread())
            adjustSpread (spread);
        }
      
      void
      fitStorage()
        {
          if (not Coll::data)
            return;
          if (not canDynGrow())
            throw err::Invalid{"Unable to shrink storage for Several-collection, "
                               "since at least one element can not be moved."};
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
      
      
      
      /* ==== Logic do decide about possible element handling ==== */
      
      enum DestructionMethod{ UNKNOWN
                            , TRIVIAL
                            , ELEMENT
                            , VIRTUAL
                            };
      static Literal
      render (DestructionMethod m)
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
      
      
      /**
       * Select a suitable method for invoking the element destructors
       * and build a λ-object to be stored as deleter function alongside
       * with the data; this includes a _copy_ of the embedded allocator,
       * which in many cases is a monostate empty base class.
       * @note this collection is _primed_ by the first element added,
       *       causing to lock into one of the possible destructor schemes;
       *       the reason is, we do not retain the information of the individual
       *       element types and thus we must employ one coherent scheme for all.
       */
      template<typename TY>
      Deleter
      selectDestructor ()
        {
          typename POL::Fac& factory(*this);
          
          if (is_Subclass<TY,I>() and has_virtual_destructor_v<I>)
            {
              __ensureMark<TY> (VIRTUAL);
              return [factory](ArrayBucket<I>* bucket){ unConst(factory).template destroy<I> (bucket); };
            }
          if (is_trivially_destructible_v<TY>)
            {
              __ensureMark<TY> (TRIVIAL);
              return [factory](ArrayBucket<I>* bucket){ unConst(factory).template destroy<TY> (bucket); };
            }
          if (is_same_v<TY,E> and is_Subclass<E,I>())
            {
              __ensureMark<TY> (ELEMENT);
              return [factory](ArrayBucket<I>* bucket){ unConst(factory).template destroy<E> (bucket); };
            }
          throw err::Invalid{_Fmt{"Unsupported kind of destructor for element type %s."}
                                 % util::typeStr<TY>()};
        }
      
      template<typename TY>
      void
      __ensureMark (DestructionMethod requiredKind)
        {
          if (destructor != UNKNOWN and destructor != requiredKind)
            throw err::Invalid{_Fmt{"Unable to handle (%s-)destructor for element type %s, "
                                    "since this container has been primed to use %s-destructors."}
                                   % render(requiredKind)
                                   % util::typeStr<TY>()
                                   % render(destructor)};
          destructor = requiredKind;
        }
      
      
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
      
      bool
      canDynGrow()
        {
          return not lock_move;
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
  
  template<typename I, typename E =I>
  SeveralBuilder<I,E>
  makeSeveral()
  {
    return SeveralBuilder<I,E>{};
  }
  
  
} // namespace lib
#endif /*LIB_SEVERAL_BUILDER_H*/
