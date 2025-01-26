/*
  HETERO-DATA.hpp  -  handle chain of heterogeneous data blocks

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file hetero-data.hpp
 ** Maintain a chained sequence of heterogeneous data blocks without allocation.
 ** This building block for low-level memory management allows to build up a collection
 ** of entirely arbitrary data placed into existing and possibly distributed storage.
 ** The safety of storage and lifetime must be ensured by other means, since data access
 ** proceeds without further bound checks. However, a type-safe compile-time overlay of
 ** accessor marker types is provided, allowing to integrate such a storage layout into
 ** an overall memory safe arrangement.
 ** 
 ** A usage scenario would be gradually to build up an assortment of data elements directly
 ** in local automatic storage within an elaborate recursive call stack unfolding recursively.
 ** Notably the accessor marker types can be assembled independently from the provision of
 ** actual storage, as the connection between accessor and actual storage address is
 ** _established late,_ on actual _data access._ Obviously, data access in such an arrangement
 ** requires traversal in several steps, which, on the other hand, can be justified by a good
 ** cache locality of recently used stack frames, thereby avoiding heap allocations altogether.
 ** 
 ** # Usage
 ** @warning it is essential to understand where actual storage resides!
 ** A HeteroData chain is built-up gradually, starting with a front-block
 ** - the front-block is usually placed at an _anchor location_ and populated with data
 ** - retrieve a _chain constructor type_ from the _type_ of the front-block,
 **   i.e `HeteroData<D1,D2,...>::Chain<D21,...>`
 ** - use this chain constructor to create a follow-up data block elsewhere
 ** - need to link this data block explicitly into the front
 ** - get _accessor types_ from the _chain constructor_
 ** - use these to work with individual data elements _through the front-block._
 ** @example
 ** \code
 **   using Front = lib::HeteroData<uint,double>;
 **   auto h1 = Front::build (1,2.3);
 **   using Cons1 = Front::Chain<bool,string>;
 **   auto b2 = Cons1::build (true, "Ψ");
 **   b2.linkInto(h1);
 **   auto& [d1,d2,d3,d4] = Cons1::recast(h1);
 **   CHECK (d1 == 1);
 **   CHECK (d2 == 2.3);
 **   CHECK (d3 == true);
 **   CHECK (d4 == "Ψ");
 **   Cons1::AccessorFor<string> get4;
 **   CHECK (get4(h1) == "Ψ");
 ** \endcode
 ** 
 ** @todo WIP-WIP this is the draft of a design sketch regarding the render node network,
 **       which seems to be still pretty much in flux as of 12/2024
 ** @see HeteroData_test
 ** @see steam::engine::TurnoutSystem (use case)
 **
 */



#ifndef LIB_HETERO_DATA_H
#define LIB_HETERO_DATA_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/typeseq-util.hpp"

#include <utility>
#include <tuple>


namespace lib {
  
  /**
   * A setup with chained data tuples residing in distributed storage.
   * A HeteroData-chain is started from a front-end block and can later be
   * extended by a linked list of further data blocks allocated elsewhere.
   * @warning this is a low-level memory layout without storage management.
   * @see HeteroData_test
   */
  template<typename...DATA>
  class HeteroData;
  
  /** linked list of StorageFrame elements */
  struct StorageLoc
    : util::NonCopyable
    {
      StorageLoc* next{nullptr};
    };
  
  /** individual storage frame in a chain, holding a data tuple */
  template<size_t seg, typename...DATA>
  struct StorageFrame
    : protected StorageLoc
    , std::tuple<DATA...>
    {
      using Tuple = std::tuple<DATA...>;
      
      using Tuple::tuple;
      
      template<typename SPEC>
      void linkInto (HeteroData<SPEC>&);
      template<typename SPEC>
      void detachFrom (HeteroData<SPEC>&);
      
      template<size_t slot> auto& get() noexcept { return std::get<slot>(*this); }
      template<typename X>  auto& get() noexcept { return std::get<X>(*this);    }
    };
  
  
  /**
   * @internal implementation specialisation to manage a sublist of StorageFrame elements
   * @tparam seg  a type tag to mark the position of StorageFrame elements
   * @tparam DATA tuple element types residing in the first segment
   * @tparam TAIL recursive Loki-style type list to describe the rest of the chain
   */
  template<size_t seg, typename...DATA, class TAIL>
  class HeteroData<meta::Node<StorageFrame<seg, DATA...>,TAIL>>
    : StorageFrame<seg, DATA...>
    {
      using _Self = HeteroData;
      using _Tail = HeteroData<TAIL>;
      using Tuple = std::tuple<DATA...>;
      using Frame = StorageFrame<seg, DATA...>;
      
      static constexpr size_t localSiz = sizeof...(DATA);
      
      template<size_t slot>
      static constexpr bool isLocal = slot < localSiz;
      
      template<size_t slot>
      using PickType = std::conditional_t<isLocal<slot>, std::tuple_element<slot,Tuple>
                                                       , typename _Tail::template PickType<slot-localSiz>>;
                                                      // need to use this helper to prevent eager evaluation on Elm_t<i>
      _Tail&
      accessTail()
        {
          if (Frame::next == nullptr) // Halt system by breaking noexcept
            throw lumiera::error::Fatal{"HeteroData storage logic broken: follow-up extent not(yet) allocated"};
          return * reinterpret_cast<_Tail*> (Frame::next);
        }
      
      template<typename...SPEC>
      static _Self&
      recast (HeteroData<SPEC...>& frontChain)
        {
          return reinterpret_cast<_Self&> (frontChain);
        }
      template<typename...SPEC>
      static _Self const&
      recast (HeteroData<SPEC...> const& frontChain)
        {
          return reinterpret_cast<_Self const&> (frontChain);
        }
      
      
      template<typename...XX>
      friend class HeteroData;  ///< allow chained types to use recursive type definitions
      
      using Frame::Frame;       ///< data elements shall be populated through the builder front-ends
      
      
    public:
      HeteroData() = default;
      
      static constexpr size_t
      size()
        {
          return localSiz + _Tail::size();
        }
      
      /** access type to reside in the given slot of the _complete chain_ */
      template<size_t slot>
      using Elm_t = typename PickType<slot>::type;
      
      
      /** access data elements within _complete chain_ by index pos */
      template<size_t slot>
      Elm_t<slot>&
      get()  noexcept
        {
          static_assert (slot < size(), "HeteroData access index beyond defined data");
          if constexpr (slot < localSiz)
            return std::get<slot> (*this);
          else
            return accessTail().template get<slot-localSiz>();
        }
      
      template<size_t slot>
      Elm_t<slot> const&
      get()  const noexcept
      {
        return const_cast<HeteroData*>(this)->get<slot>();
      }
      
      
      /**
       * Accessor-functor to get at the data residing within some tuple element
       * Using the enclosing typed scope to ensure safe storage access
       * @tparam slot number of the data element, counting from zero over the full chain
       * @note this functor holds no data, but shall be applied to some existing HeteroData.
       */
      template<size_t slot>
      struct Accessor
        {
          using Type = Elm_t<slot>;
          
          template<class SPEC>
          static Type&
          get (HeteroData<SPEC>& frontEnd)
            {
              auto& fullChain = _Self::recast (frontEnd);
              return fullChain.template get<slot>();
            }
          
          template<typename HH>
          Type& operator() (HH& frontEnd) const { return Accessor::get(frontEnd); }
        };
      
      /**
       * Constructor-functor to build an extra data segment, which can then be linked to the chain.
       * @tparam VALS data types to use in the extra storage tuple
       * @note Using this functor is the only safe path to create and add new data blocks.
       *       Each such data block can be linked in once, and only if the base chain matches
       *       the structure embedded into the type of the enclosing scope.
       *     - storage frames can be default constructed, but not copied / moved thereafter
       *     - the #build() function can be used to create the block and init the data
       *     - after creating a frame, it must be explicitly linked in by invoking NewFrame::linkInto()
       *     - the #recast() function will re-interpret _any_ `HeteroData&` into the storage structure
       *       which can be expected after building the extension frame (use with care!)
       *     - the nested template ChainExtent is a follow-up constructor-functor to add a further block
       *     - the nested template Accessor shall be used for any type-save access to data values
       *     - if all types are distinct, the Accessor can also be selected by-type
       */
      template<typename...VALS>
      struct Chain
        {
          using Segments = meta::Node<Frame,TAIL>; // ◁———this type describes current chain structure
          using NewFrame = StorageFrame<meta::count<Segments>::value, VALS...>;
          using ChainType = HeteroData<typename meta::Append<Segments,NewFrame>::List>;
                                                // ...and this would be the extended chain structure
          template<typename...INIT>
          static NewFrame
          build (INIT&& ...initArgs)
            {
              return {initArgs ...}; // Note: NewFrame is non-copyable
            }
          template<typename...INIT>
          static NewFrame&
          emplace (void* storage, INIT&& ...initArgs)   ///< placement-new flavour of the builder notation
            {
              return * new(storage) NewFrame{initArgs ...};
            }
          
          template<class HET>
          static auto&
          recast (HET& frontChain)
            {
              return ChainType::recast (frontChain);
            }
          
          template<typename...XVALS>
          using ChainExtent = typename ChainType::template Chain<XVALS...>;
          
          template<size_t slot>
          using Accessor = typename ChainType::template Accessor<_Self::size()+slot>;
          
          template<typename X>
          using AccessorFor = Accessor<meta::indexOfType<X,VALS...>()>;
        };
    };
  
  /**
   * @internal implementation specialisation to mark the end of a chain
   */
  template<>
  class HeteroData<meta::NullType>
    {
    public:
      static size_t constexpr size() { return 0; }
      
      template<size_t>
      using Elm_t = void;
      template<size_t>
      using PickType = void;
    };
  
  /*************************************************************************//**
   * @remark this is the front-end for regular usage
   *       - create and populate with the #build operation
   *       - data access with the `get<i>` member function (inherited)
   *       - use `HeteroData<DATA...>::Chain<TY...>` to build follow-up segments
   */
  template<typename...DATA>
  class HeteroData
    : public HeteroData<meta::Node<StorageFrame<0, DATA...>, meta::NullType>>
    {
      using _Front = HeteroData<meta::Node<StorageFrame<0, DATA...>, meta::NullType>>;
      
    public:
      using NewFrame = typename _Front::Frame;
      using ChainType = _Front;
      
      template<typename...INIT>
      static _Front
      build (INIT&& ...initArgs)
        {
          return {initArgs ...};
        }
    };
  
  
  namespace {
    /**
     * @internal helper for safety-check when attaching segments.
     *  New segments are created by a constructor functor, guided by a
     *  type signature describing the complete chain. When attaching new
     *  segments, we can not verify that the base chain to extend does really
     *  match the presumed chain structure as encoded into the type (since this
     *  base chain does not store any meta data). But at least we can verify
     *  that the number of real segment-links matches the assumed structure.
     *  Notably it does not really matter what is stored in the base chain,
     *  as long as this segment count matches, because accessor functors
     *  generated by a `HeteroData::Chain` constructor will always address
     *  only their own (newly added) segment.
     */
    inline StorageLoc*&
    checkedTraversal (size_t segments, StorageLoc* last)
    {
      REQUIRE(last);
      while (segments and last->next)
        {
          last = last->next;
          --segments;
        }
      ASSERT (last->next == nullptr and segments == 1
             ,"Failure to attach new data segment to HeteroData: "
              "assumed type structure does not match real connectivity, "
              "end-of-chain encountered with %d type segment(s) remaining"
             , segments);
      return last->next;
    }
    
    /**
     * @internal detach the HeteroData-chain at the link to the given chainBlock.
     *  This enables some additional data sanity, because the internal chain can
     *  thus be severed when an extension data block is known to go out of scope.
     *  If somehow a store accessor is used after that point, the system will
     *  be halted when attempting to navigate to the (now defunct) data block.
     */
    inline void
    checkedDetach (size_t segments, StorageLoc* seg, void* chainBlock)
    {
      REQUIRE(seg);
      while (segments and seg->next)
        if (segments == 1 and seg->next == chainBlock)
          {
            seg->next = nullptr;
            return;
          }
        else
          {
            seg = seg->next;
            --segments;
          }
      NOTREACHED ("Failure to detach a data segment from HeteroData: "
                  "assumed type structure does not match real connectivity");
    }
  }//(End)helper
  
  
  /**
   * Attach a new storage frame at the end of an existing HeteroData-chain.
   * @tparam seg the number of the separate data segment, must match target
   * @param  prefixChain with `seg - 1` existing chained tuple-segments
   * @remark The core function actually to extend a chain with a new segment,
   *         which should have been built using a suitable nested `HeteroData::Chain`
   *         constructor type. Further segments can be defined working from there,
   *         since each such constructor in turn has a member type `ChainExtension`
   * @note   Always use this strongly typed extension and access path, to prevent
   *         out-of-bounds memory access. The actual HeteroData stores no run time
   *         type information, and thus a force-cast is necessary internally to
   *         access the follow-up data tuple frames. The typing, and especially
   *         the `seg` template parameter used to mark each StorageFrame is
   *         the only guard-rail provided, and ensures safe data access.
   */
  template<size_t seg, typename...DATA>
  template<typename SPEC>
  inline void
  StorageFrame<seg,DATA...>::linkInto (HeteroData<SPEC>& prefixChain)
  {
    StorageLoc*  firstSeg = reinterpret_cast<StorageLoc*> (&prefixChain);
    StorageLoc*& lastLink = checkedTraversal (seg, firstSeg);
    ENSURE (lastLink == nullptr);
    lastLink = this;
  }
  
  /** cleanly detach this storage frame from the HeteroData prefix-chain. */
  template<size_t seg, typename...DATA>
  template<typename SPEC>
  inline void
  StorageFrame<seg,DATA...>::detachFrom (HeteroData<SPEC>& prefixChain)
  {
    StorageLoc*  firstSeg = reinterpret_cast<StorageLoc*> (&prefixChain);
    checkedDetach (seg, firstSeg, this);
  }
}// namespace lib




namespace std { // Specialisation to support C++ »Tuple Protocol« and structured bindings.
  
  /** determine compile-time fixed size of a HeteroData */
  template<typename...DATA>
  struct tuple_size<lib::HeteroData<DATA...> >
    : integral_constant<size_t, lib::HeteroData<DATA...>::size()>
    { };
  
  /** expose the type of the I-th element of a HeteroData chain */
  template<size_t I, typename...DATA>
  struct tuple_element<I, lib::HeteroData<DATA...> >
    {
      using type = typename lib::HeteroData<DATA...>::template Elm_t<I>;
    };
  template<size_t I>
  struct tuple_element<I, lib::HeteroData<lib::meta::NullType> >
    {
      static_assert ("accessing element-type of an empty HeteroData block");
    };
  
  // Note: deliberately NOT providing a free get<i> function.
  //       Overload resolution would fail, since it attempts to instantiate std::get<i>(tuple) as a candidate,
  //       which triggers an assertion failure when using an index valid only for the full chain, not the base tuple
  
  
  /** determine compile-time fixed size of a StorageFrame */
  template<size_t seg, typename...DATA>
  struct tuple_size<lib::StorageFrame<seg,DATA...> >
    : tuple_size<typename lib::StorageFrame<seg,DATA...>::Tuple>
    { };

  /** delegate to the type access of a StorageFrame's underlying tuple */
  template<size_t I, size_t seg, typename...DATA>
  struct tuple_element<I, lib::StorageFrame<seg,DATA...> >
    : tuple_element<I, typename lib::StorageFrame<seg,DATA...>::Tuple>
    { };
  
  // no need to define an overload for std::get<i>
  // (other than a template specialisation, it will use base-type conversion to std::tuple on its argument;
  
}// namespace std
#endif /*LIB_HETERO_DATA_H*/
