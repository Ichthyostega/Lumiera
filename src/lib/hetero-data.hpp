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
 ** @todo WIP-WIP this is the draft of a design sketch regarding the render node network,
 **       which seems to be still pretty much in flux as of 12/2024
 ** @see HeteroData_test
 ** @see steam::engine::TurnoutSystem (use case)
 **
 */



#ifndef LIB_HETERO_DATA_H
#define LIB_HETERO_DATA_H


#include "lib/error.hpp"
//#include "lib/symbol.hpp"
#include "lib/nocopy.hpp"
//#include "lib/linked-elements.hpp"
#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-manip.hpp"
//#include "lib/meta/typelist-util.hpp"
#include "lib/meta/typeseq-util.hpp"
#include "lib/test/test-helper.hpp"

//#include <algorithm>
//#include <vector>
#include <utility>
#include <tuple>


namespace lib {
  
  /**
   */
  template<typename...DATA>
  class HeteroData;
  
  
  struct StorageLoc
    : util::NonCopyable
    {
      StorageLoc* next{nullptr};
    };
  
  template<size_t seg, typename...DATA>
  struct StorageFrame
    : StorageLoc
    , std::tuple<DATA...>
    {
      using Tuple = std::tuple<DATA...>;
      
      using Tuple::tuple;
      
      template<typename SPEC>
      void linkInto (HeteroData<SPEC>&);
    };
  
  
  template<size_t seg, typename...DATA, class TAIL>
  class HeteroData<meta::Node<StorageFrame<seg, DATA...>,TAIL>>
//  : StorageFrame<seg, DATA...>
//    : util::NonCopyable
    {
      using _Self = HeteroData;
      using _Tail = HeteroData<TAIL>;
      using Tuple = std::tuple<DATA...>;
      using Frame = StorageFrame<seg, DATA...>;
      
      Frame frame_;
      
      static constexpr size_t localSiz = sizeof...(DATA);
      
      template<size_t slot>
      static constexpr bool isLocal = slot < localSiz;
      
      template<size_t slot>
      using PickType = std::conditional_t<isLocal<slot>, std::tuple_element<slot,Tuple>
                                                       , typename _Tail::template PickType<slot-localSiz>>;
      
      _Tail&
      accessTail()
        {
          REQUIRE (frame_.next, "HeteroData storage logic broken: follow-up extent not yet allocated");
          return * reinterpret_cast<_Tail*> (frame_.next);
        }
      
      template<typename...XX>
      friend class HeteroData;  ///< allow chained types to use recursive type definitions
      
      template<typename...INIT>
      HeteroData (INIT&& ...initArgs)
        : frame_{std::forward<INIT> (initArgs)...}
        { }
      
    public:
      HeteroData() = default;
      
      static constexpr size_t
      size()
        {
          return localSiz + _Tail::size();
        }
      
      template<size_t slot>
      using Elm_t = typename PickType<slot>::type;
      
      
      template<size_t slot>
      Elm_t<slot>&
      get()  noexcept
        {
          static_assert (slot < size(), "HeteroData access index beyond defined data");
          if constexpr (slot < localSiz)
            return std::get<slot> (frame_);
          else
            return accessTail().template get<slot-localSiz>();
        }
      
      template<size_t slot>
      Elm_t<slot> const&
      get()  const noexcept
      {
        return const_cast<HeteroData*>(this)->get<slot>();
      }
      
      template<size_t slot>
      struct Accessor
        {
          using Type = Elm_t<slot>;
          
          template<class SPEC>
          Type&
          get (HeteroData<SPEC>& frontEnd)
            {
              auto& fullChain = reinterpret_cast<_Self&> (frontEnd);
              return fullChain.template get<slot>();
            }
        };
      
      template<typename...VALS>
      struct Chain
        {
          using NewFrame = StorageFrame<seg+1, VALS...>;
          using ChainType = HeteroData<typename meta::Append<meta::Node<Frame,TAIL>,NewFrame>::List>;
          
          template<typename...INIT>
          static NewFrame
          build (INIT&& ...initArgs)
            {
              return {initArgs ...};
            }
          
          template<typename...XVALS>
          using ChainExtension = typename ChainType::template Chain<XVALS...>;
          
          template<size_t slot>
          using Accessor = typename ChainType::template Accessor<_Self::size()+slot>;
          
          template<typename X>
          using AccessorFor = Accessor<meta::indexOfType<X,VALS...>()>;
        };
    };
  
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
  }
  
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
  
} // namespace lib

namespace std {// Specialisation to support C++ »Tuple Protocol«
  
  /** determine compile-time fixed size of a HeteroData */
  template<typename...DATA>
  struct tuple_size<lib::HeteroData<DATA...> >
    : std::integral_constant<std::size_t, lib::HeteroData<DATA...>::size()>
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
  
  /** access by reference to the I-th data value held in a HeteroData chain */
  template<size_t I, typename...DATA>
  constexpr tuple_element_t<I, lib::HeteroData<DATA...>>&
  get (lib::HeteroData<DATA...> & heDa) noexcept
  {
    return heDa.template get<I>();
  }
  template<size_t I, typename...DATA>
  constexpr tuple_element_t<I, lib::HeteroData<DATA...>> const&
  get (lib::HeteroData<DATA...> const& heDa) noexcept
  {
    return heDa.template get<I>();
  }
  template<size_t I, typename...DATA>
  constexpr tuple_element_t<I, lib::HeteroData<DATA...>>&&
  get (lib::HeteroData<DATA...>&& heDa) noexcept
  {
    using ElmType = tuple_element_t<I, lib::HeteroData<DATA...>>;
    return forward<ElmType&&> (heDa.template get<I>());
  }
  template<std::size_t I, typename...DATA>
  constexpr std::tuple_element_t<I, lib::HeteroData<DATA...>> const &&
  get (lib::HeteroData<DATA...> const && heDa) noexcept
  {
    using ElmType = tuple_element_t<I, lib::HeteroData<DATA...>>;
    return forward<ElmType const&&> (heDa.template get<I>());
  }
  
  
  /** determine compile-time fixed size of a StorageFrame */
  template<size_t seg, typename...DATA>
  struct tuple_size<lib::StorageFrame<seg,DATA...> >
    : std::tuple_size<typename lib::StorageFrame<seg,DATA...>::Tuple>
    { };

  /** delegate to the type access of a StorageFrame's underlying tuple */
  template<size_t I, size_t seg, typename...DATA>
  struct tuple_element<I, lib::StorageFrame<seg,DATA...> >
    : std::tuple_element<I, typename lib::StorageFrame<seg,DATA...>::Tuple>
    { };

  /** delegate to the element data access of a StorageFrame's underlying tuple */
//  template<size_t I, size_t seg, typename...DATA>
//  std::tuple_element_t<I, lib::StorageFrame<seg,DATA...>>&
//  get (lib::StorageFrame<seg,DATA...>& block)
//  {
//    return std::get
//  };
  
}// namespace std
#endif /*LIB_HETERO_DATA_H*/
