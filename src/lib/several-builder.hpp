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
 ** and thus the offset used for subscript access. The actual data storage starts
 ** immediately behind the ArrayBucket, which thus acts as a metadata header.
 ** This arrangement requires a sufficiently sized raw memory allocation to place
 ** the ArrayBucket and the actual data into. Moreover, the allocation code in
 ** ElementFactory::create() is responsible to ensure proper alignment of the
 ** data storage, especially when the payload data type has alignment requirements
 ** beyond `alignof(void*)`, which is typically used by the standard heap allocator;
 ** additional headroom is added proactively in this case, to be able to shift the
 ** storage buffer ahead to the next alignment boundrary.
 ** 
 ** # Handling of data elements
 ** 
 ** The ability to emplace a mixture of data types into the storage exposed through
 ** the lib::Several front-end creates some complexities related to element handling.
 ** The implementation relies on a rules and criteria based approach to decide on
 ** a case by case base if some given data content is still acceptable. This allows
 ** for rather tricky low-level usages, but has the downside to detect errors only
 ** at runtime — which in this case is ameliorated by the limitation that elements
 ** must be provided completely up-front, through the SeveralBuilder.
 ** - in order to handle any data element, we must be able to invoke its destructor
 ** - an arbitrary mixture of types can thus only be accepted if either we can
 **   rely on a common virtual base class destructor, or if all data elements
 **   are trivially destructible; these properties can be detected at compile
 **   time with the help of the C++ `<type_traits>` library
 ** - this container can accommodate _non-copyable_ data types, under the proviso
 **   that the complete storage required is pre-allocated (using `reserve()` from
 **   the builder API)
 ** - otherwise, data can be filled in dynamically, expanding the storage as needed,
 **   given that all existing elements can be safely re-located by move or copy
 **   constructor into a new, larger storage buffer.
 ** - alternatively, when data elements are even ''trivially copyable'' (e.g. POD data),
 **   then it is even possible to increase the placement spread in the storage at the
 **   point when the requirement to do so is discovered dynamically; objects can be
 **   shifted to other locations by `std::memmove()` in this case.
 ** - notably, lib::AllocationCluster has the ability to dynamically adapt an allocation,
 **   but only if this happens to be currently the last allocation handed out; it can
 **   thus be arranged even for an unknown number of non-copyable objects to be emplaced
 **   when creating the suitable operational conditions.
 ** A key point to note is the fact that the container does not capture and store the
 ** actual data types persistently. Thus, the above rules must be applied in a way
 ** to always ensure safe handling of the contained data. Typically, the first element
 ** actually added will »prime« the container for a certain usage style, and after that,
 ** some other usage patterns may be rejected.
 ** 
 ** @todo this is a first implementation solution from 6/2025 — and was deemed
 **       _roughly adequate_ at that time, yet should be revalidated once more
 **       observations pertaining real-world usage are available...
 ** @warning there is a known problem with _over-alligned-types,_ which becomes
 **       relevant when the _interface type_ has only lower alignment requirement,
 **       but an individual element is added with higher alignment requirements.
 **       In this case, while the spread is increased, still the placement of
 **       the interface-type is used as anchor, possibly leading to misalignment.
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
    using util::positiveDiff;
    using std::is_nothrow_move_constructible_v;
    using std::is_trivially_move_constructible_v;
    using std::is_trivially_destructible_v;
    using std::has_virtual_destructor_v;
    using std::is_trivially_copyable_v;
    using std::is_copy_constructible_v;
    using std::is_object_v;
    using std::is_volatile_v;
    using std::is_const_v;
    using std::is_same_v;
    using lib::meta::is_Subclass;

    
    /**
     * Helper to determine the »spread« required to hold
     * elements of type \a TY in memory _with proper alignment._
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
    
    size_t inline constexpr
    alignRes (size_t alignment)
      {
        return positiveDiff (alignment, alignof(void*));
      }
    
    
    template<class I, template<typename> class ALO>
    class ElementFactory
      : protected ALO<std::byte>
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
        create (size_t cnt, size_t spread, size_t alignment =alignof(I))
          {
            REQUIRE (cnt);
            REQUIRE (spread);
            size_t storageBytes = Bucket::storageOffset + cnt*spread;
            storageBytes += alignRes (alignment);  // over-aligned data => reserve for alignment padding
            // Step-1 : acquire the raw storage buffer
            std::byte* loc = AlloT::allocate (baseAllocator(), storageBytes);
            ENSURE (0 == size_t(loc) % alignof(void*));
            
            size_t offset = (size_t(loc) + Bucket::storageOffset) % alignment;
            if (offset)     // padding needed to next aligned location
              offset = alignment - offset;
            offset += Bucket::storageOffset;
            ASSERT (storageBytes - offset >= cnt*spread);
            Bucket* bucket = reinterpret_cast<Bucket*> (loc);
            
            using BucketAlloT = typename AlloT::template rebind_traits<Bucket>;
            auto bucketAllo = adaptAllocator<Bucket>();
            // Step-2 : construct the Bucket metadata       | ▽ ArrayBucket ctor arg ▽  
            try { BucketAlloT::construct (bucketAllo, bucket, storageBytes, offset, spread); }
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
            if (bucket->isArmed())
              { // ensure the bucket's destructor is invoked
                // and in turn itself invokes this function
                bucket->destroy();
                return;
              }
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
            size_t storageBytes = bucket->getAllocSize();
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
        
        /** by default assume that memory is practically unlimited... */
        size_t static constexpr ALLOC_LIMIT = size_t(-1) / sizeof(E);
        
        bool canExpand(Bucket*, size_t){ return false; }
        
        Bucket*
        realloc (Bucket* data, size_t cnt, size_t spread)
          {
            Bucket* newBucket = Fac::create (cnt, spread, alignof(E));
            if (data)
              try {
                  newBucket->installDestructor (data->getDtor());
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
            if constexpr (is_nothrow_move_constructible_v<E>
                          or is_copy_constructible_v<E>)
              {
                E& oldElm = reinterpret_cast<E&> (src->subscript (idx));
                Fac::template createAt<E> (tar, idx
                                          ,std::move_if_noexcept (oldElm));
              }
            else
              {
                NOTREACHED("realloc immovable type (neither trivially nor typed movable)");
                // this alternative code section is very important, because it allows
                // to instantiate this code even for »noncopyable« types, assuming that
                // sufficient storage is reserved beforehand, and thus copying is irrelevant.
                // For context: the std::vector impl. from libStdC++ is lacking this option.
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
    : private Several<I>
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
      
      /** cross-builder to use a custom allocator for the lib::Several container */
      template<template<typename> class ALO  =std::void_t
              ,typename...ARGS>
      auto withAllocator (ARGS&& ...args);
      
      
      /** ensure up-front that a desired capacity is allocated */
      template<typename TY =E>
      SeveralBuilder&&
      reserve (size_t cntElm =1
              ,size_t elmSiz =reqSiz<TY>())
        {
          size_t extraElm = positiveDiff (cntElm, Coll::size());
          ensureElementCapacity<TY> (elmSiz);
          ensureStorageCapacity<TY> (elmSiz,extraElm);
          elmSiz = max (elmSiz, Coll::spread());
          adjustStorage (cntElm, elmSiz);
          return move(*this);
        }
      
      /** append copies of one or several arbitrary elements */
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
      
      /** append a copy of all values exposed through an iterator */
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
      
      /** emplace a number of elements of the defined element type \a E */
      template<typename...ARGS>
      SeveralBuilder&&
      fillElm (size_t cntNew, ARGS&& ...args)
        {
          for ( ; 0<cntNew; --cntNew)
            emplaceNewElm<E> (forward<ARGS> (args)...);
          return move(*this);
        }
      
      /** create a new content element within the managed storage */
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
      
      size_t size()       const { return Coll::size(); }
      bool empty()        const { return Coll::empty();}
      size_t capacity()   const { return Coll::storageBuffSiz() / Coll::spread(); }
      size_t capReserve() const { return capacity() - size(); }
      
      
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
              
          probeMoveCapability<TY>();   // mark when target type is not (trivially) movable
          ensureElementCapacity<TY>(); // sufficient or able to adapt spread
          ensureStorageCapacity<TY>(); // sufficient or able to grow buffer
          
          size_t elmSiz = reqSiz<TY>();
          size_t newPos = Coll::size();
          size_t newCnt = Coll::empty()? INITIAL_ELM_CNT : newPos+1;
          adjustStorage (newCnt, max (elmSiz, Coll::spread()));
          ENSURE (Coll::data_);
          ensureDeleter<TY>();
          POL::template createAt<TY> (Coll::data_, newPos, forward<ARGS> (args)...);
          Coll::data_->cnt = newPos+1;
        }
      
      /** ensure clean-up can be handled properly.
       * @throw err::Invalid when \a TY requires a different style
       *        of deleter than was established for this instance */
      template<class TY>
      void
      ensureDeleter()
        {
          Deleter deleterFunctor = selectDestructor<TY>();
          if (Coll::data_->isArmed()) return;
          Coll::data_->installDestructor (move (deleterFunctor));
        }
      
      /** ensure sufficient element capacity or the ability to adapt element spread */
      template<class TY>
      void
      ensureElementCapacity (size_t requiredSiz =reqSiz<TY>())
        {
          if (Coll::spread() < requiredSiz and not (Coll::empty() or canWildMove()))
            throw err::Invalid{_Fmt{"Unable to place element of type %s (size=%d)"
                                    "into Several-container for element size %d."}
                                   % util::typeStr<TY>() % requiredSiz % Coll::spread()};
        }
      
      /** ensure sufficient storage reserve for \a newElms or verify the ability to re-allocate */
      template<class TY>
      void
      ensureStorageCapacity (size_t requiredSiz =reqSiz<TY>(), size_t newElms =1)
        {
          if (not (Coll::empty()
                   or Coll::hasReserve (requiredSiz, newElms)
                   or POL::canExpand (Coll::data_, requiredSiz*newElms)
                   or canDynGrow()))
            throw err::Invalid{_Fmt{"Several-container is unable to accommodate further element of type %s; "
                                    "storage reserve (%d bytes ≙ %d elms) exhausted and unable to move "
                                    "elements of mixed unknown detail type, which are not trivially movable." }
                                   % util::typeStr<TY>() % Coll::storageBuffSiz() % capacity()};
        }
      
      
      /** possibly grow storage and re-arrange elements to accommodate desired capacity */
      void
      adjustStorage (size_t cnt, size_t spread)
        {
          size_t demand{cnt*spread};
          size_t buffSiz{Coll::storageBuffSiz()};
          if (demand == buffSiz)
            return;
          if (demand > buffSiz)
            {// grow into exponentially expanded new allocation
              if (spread > Coll::spread())
                cnt = max (cnt, buffSiz / Coll::spread()); // retain reserve
              size_t overhead = sizeof(Bucket) + alignRes(alignof(E));
              size_t safetyLim = LUMIERA_MAX_ORDINAL_NUMBER * Coll::spread();
              size_t expandAlloc = min (positiveDiff (min (safetyLim
                                                          ,POL::ALLOC_LIMIT)
                                                     ,overhead)
                                       ,max (2*buffSiz, cnt*spread));
              // round down to an even number of elements
              size_t newCnt = expandAlloc / spread;
              expandAlloc = newCnt * spread;
              if (expandAlloc < demand)
                throw err::State{_Fmt{"Storage expansion for Several-collection "
                                      "exceeds safety limit of %d bytes"} % safetyLim
                                ,LERR_(SAFETY_LIMIT)};
              // allocate new storage block...
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
          REQUIRE (newSpread * Coll::size() <= Coll::storageBuffSiz());
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
          byte* oldPos = Coll::data_->storage();
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
      selectDestructor()
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
  
  
  
  
  /* ===== Helpers and convenience-functions for creating SeveralBuilder ===== */
  
  
  namespace allo { // Setup for custom allocator policies
    
    template<template<typename> class ALO, typename...ARGS>
    struct SetupSeveral;
    
    template<template<typename> class ALO>
    struct SetupSeveral<ALO>
      {
        template<class I, class E>
        using Policy = AllocationPolicy<I,E,ALO>;
      };
    
    template<template<typename> class ALO, typename X>
    struct SetupSeveral<ALO, ALO<X>>
      {
        template<class I, class E>
        struct Policy
          : AllocationPolicy<I,E,ALO>
          {
            Policy (ALO<X> refAllocator)
              : AllocationPolicy<I,E,ALO>(move(refAllocator))
              { }
          };
      };
    //
  }//(End)Allocator configuration
  
  
  
  /**
   * @remarks this builder notation configures the new lib::Several container
   *     to perform memory management through a standard conformant allocation adapter.
   *     Moreover, optionally the behaviour can be configured through an extension point
   *     lib::allo::SetupSeveral, for which the custom allocator may provide an explicit
   *     template specialisation.
   * @tparam ALO a C++ standard conformant allocator template, which can be instantiated
   *     for creating various data elements. Notably, this will be instantiated as
   *     `ALO<std::byte>` to create and destroy the memory buffer for content data
   * @param args optional dependency wiring arguments, to be passed to the allocator
   * @return a new empty SeveralBuilder, configured to use the custom allocator.
   */
  template<class I, class E, class POL>
  template<template<typename> class ALO, typename...ARGS>
  inline auto
  SeveralBuilder<I,E,POL>::withAllocator (ARGS&& ...args)
  {
    if (not empty())
      throw err::Logic{"lib::Several builder withAllocator() must be invoked "
                       "prior to adding any elements to the container"};
    
    using Setup = allo::SetupSeveral<ALO,ARGS...>;
    using PolicyForAllo = typename Setup::template Policy<I,E>;
    using BuilderWithAllo = SeveralBuilder<I,E,PolicyForAllo>;
    
    return BuilderWithAllo(forward<ARGS> (args)...);
  }
  
  
  
  
  
  /*********************************************************//**
   * Entrance Point: start building a lib::Several instance
   * @tparam I Interface type to use for element access
   * @tparam E (optional) standard element implementation type
   * @return a builder instance with methods to create or copy
   *     data elements to populate the container...
   */
  template<typename I, typename E =I>
  SeveralBuilder<I,E>
  makeSeveral()
  {
    return SeveralBuilder<I,E>{};
  }
  
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
