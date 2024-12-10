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
#include "lib/meta/typeseq-util.hpp"

//#include <algorithm>
//#include <vector>
#include <tuple>


namespace lib {
  
  struct StorageLoc
    : util::NonCopyable
    {
      StorageLoc* next{nullptr};
    };
  
  template<typename...DATA>
  struct StorageFrame
    : StorageLoc
    , std::tuple<DATA...>
    {
      using std::tuple<DATA...>::tuple;
    };
  
  /**
   */
  template<class SPEC>
  class HeteroData
    {
      /////////////////////////OOO do we need this as marker for an unspecified front-end?
    };
  
  template<class TAIL, typename...DATA>
  class HeteroData<meta::Node<StorageFrame<DATA...>,TAIL>>
    : StorageFrame<DATA...>
    {
      using _Self = HeteroData;
      using _Tail = HeteroData<TAIL>;
      using Frame = StorageFrame<DATA...>;
      
      static constexpr size_t localSiz = sizeof...(DATA);
      
      template<size_t slot>
      static constexpr bool isLocal = slot < localSiz;
      
      _Tail&
      accessTail()
        {
          REQUIRE (Frame::next, "HeteroData storage logic broken: follow-up extent not yet allocated");
          return * reinterpret_cast<_Tail*> (Frame::next);
        }
      
    public:
      static constexpr size_t
      size()
        {
          return localSiz + _Tail::size();
        }
      
      template<size_t slot>
      using Elm_t = std::conditional<isLocal<slot>, std::tuple_element_t<slot,Frame>
                                                  , typename _Tail::template Elm_t<slot-localSiz>>;
      
      template<size_t slot>
      Elm_t<slot>&
      get()
        {
          static_assert (slot < size(), "HeteroData access index beyond defined data");
          if constexpr (slot < localSiz)
            return std::get<slot> (*this);
          else
            return accessTail().template get<slot-localSiz>();
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
      struct Constructor
        {
          using NewFrame = StorageFrame<VALS...>;
          using ChainType = meta::Append<meta::Node<Frame,TAIL>,NewFrame>;
          
          template<typename...INIT>
          NewFrame
          operator() (INIT&& ...initArgs)
            {
              return {initArgs ...};
            }
          
          template<typename...XVALS>
          using ChainConstructor = typename ChainType::template Constructor<XVALS...>;
          
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
      
      template<typename...DATA>
      struct Constructor
        {
          using Chain = HeteroData<meta::Node<StorageFrame<DATA...>, meta::NullType>>;
          
          template<typename...INIT>
          typename Chain::Frame
          operator() (INIT&& ...initArgs)
            {
              return {initArgs ...};
            }
        };
    };
  
  
} // namespace lib
#endif /*LIB_HETERO_DATA_H*/
