/*
  TURNOUT.hpp  -  Fixed standard scheme to generate data within a Render Node

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file turnout.hpp
 ** Fixed standard setup used in each Port of the Render Node to generate data.
 ** Organise the state related to the invocation of s single ProcNode::pull() call
 ** This header defines part of the "glue" which holds together the render node network
 ** and enables to pull result frames from the nodes. Doing so requires some invocation
 ** local state to be maintained, especially a table of buffers used to carry out the
 ** calculations. Further, getting the input buffers filled requires to issue recursive
 ** \c pull() calls, which on the whole creates a stack-like assembly of local invocation
 ** state.
 ** The actual steps to be carried out for a \c pull() call are dependent on the configuration
 ** of the node to pull. Each node has been preconfigured by the builder with a Connectivity
 ** descriptor and a concrete type of a StateAdapter. The actual sequence of steps is defined
 ** in the header nodeoperation.hpp out of a set of basic operation steps. These steps all use
 ** the passed in Invocation object (a sub-interface of StateAdapter) to access the various
 ** aspects of the invocation state.
 ** 
 ** # composition of the Invocation State
 ** 
 ** For each individual ProcNode#pull() call, the WiringAdapter#callDown() builds an StateAdapter
 ** instance directly on the stack, managing the actual buffer pointers and state references. Using this
 ** StateAdapter, the predecessor nodes are pulled. The way these operations are carried out is encoded
 ** in the actual StateAdapter type known to the NodeWiring (WiringAdapter) instance. All of these actual
 ** StateAdapter types are built as implementing the engine::StateClosure interface.
 ** 
 ** @todo relies still on an [obsoleted implementation draft](\ref bufftable-obsolete.hpp)
 ** @see engine::ProcNode
 ** @see engine::StateProxy
 ** @see engine::FeedManifold
 ** @see nodewiring.hpp interface for building/wiring the nodes
 ** 
 ** @warning as of 4/2023 a complete rework of the Dispatcher is underway ///////////////////////////////////////////TICKET #1275
 ** 
 */


#ifndef STEAM_ENGINE_TURNOUT_H
#define STEAM_ENGINE_TURNOUT_H

#include "steam/common.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/engine/turnout-system.hpp"
#include "steam/engine/feed-manifold.hpp"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
//#include "vault/gear/job.h"
//#include "steam/engine/exit-node.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/linked-elements.hpp"
#include "lib/several.hpp"
//#include "lib/util-foreach.hpp"
//#include "lib/iter-adapter.hpp"
#include "lib/meta/function.hpp"
//#include "lib/itertools.hpp"
//#include "lib/util.hpp"      ////////OOO wegen manifoldSiz<FUN>()

#include <utility>
#include <array>
//#include <stack>


namespace steam {
namespace engine {
  
  using std::forward;
  using lib::Several;
  
  
  /**
   * Definition to emulate a _Concept_ for the *Invocation Adapter*.
   * For each Proc-Asset, the corresponding Library Adapter must provide
   * such adapters to access the input and result buffers and finally to
   * invoke the processing functions from this library.
   * - `connect(fanIn,fanOut)` access the _Feed Manifold_ and link the buffers
   * - `invoke()` invoke the processing function, passing the connected buffers
   */
  template<class ADA>
  constexpr bool
  _verify_usable_as_InvocationAdapter()
  {
    ASSERT_MEMBER_FUNCTOR (&ADA::connect, void(uint, uint));
    ASSERT_MEMBER_FUNCTOR (&ADA::invoke, void());
    return sizeof(ADA);
  }
  
  
  
  /**
   * Standard implementation for a _Weaving Pattern_ to connect
   * the input and output data feeds (buffers) into a processing function.
   * @tparam INVO a configuration / policy base class to _adapt for invocation_
   * @note assumptions made regarding the overall structure
   *     - `INVO::Feed` defines an _invocation adapter_ for the processing function
   *     - `INVO::buildFeed()` is a functor to (repeatedly) build `Feed` instances
   *     - the _invocation adapter_ in turn embeds a `FeedManifold<N>` to hold
   *       + an array of input buffer pointers
   *       + an array of output buffer pointers
   *       + `INVO::MAX_SIZ` limits both arrays
   */
  template<class INVO>
  struct SimpleWeavingPattern
    : INVO
    {
      using Feed = typename INVO::Feed;
      
      static_assert (_verify_usable_as_InvocationAdapter<Feed>());
      
      Several<PortRef>   leadPort;
      Several<BuffDescr> outTypes;
      
      uint resultSlot{0};
      
      /** forwarding-ctor to provide the detailed input/output connections */
      template<typename...ARGS>
      SimpleWeavingPattern (Several<PortRef>&&   pr
                           ,Several<BuffDescr>&& dr
                           ,uint resultIdx
                           ,ARGS&& ...args)
        : INVO{forward<ARGS>(args)...}
        , leadPort{move(pr)}
        , outTypes{move(dr)}
        , resultSlot{resultIdx}
        { }
      
      
      Feed
      mount()
        {
          return INVO::buildFeed();
        }
      
      void
      pull (Feed& feed, TurnoutSystem& turnoutSys)
        {
          for (uint i=0; i<leadPort.size(); ++i)
            {
              BuffHandle inputData = leadPort[i].get().weave (turnoutSys);
              feed.inBuff.createAt(i, move(inputData));
            }
        }
      
      void
      shed (Feed& feed, OptionalBuff outBuff)
        {
          for (uint i=0; i<outTypes.size(); ++i)
            {
              BuffHandle resultData =
                i == resultSlot and outBuff? *outBuff
                                           : outTypes[i].lockBuffer();
              feed.outBuff.createAt(i, move(resultData));
            }
          feed.connect (leadPort.size(),outTypes.size());
        }
      
      void
      weft (Feed& feed)
        {
          feed.invoke();                 // process data
        }
      
      BuffHandle
      fix (Feed& feed)
        {
          for (uint i=0; i<leadPort.size(); ++i)
            {
              feed.inBuff[i].release();
            }
          for (uint i=0; i<outTypes.size(); ++i)
            {
              feed.outBuff[i].emit();    // state transition: data ready
              if (i != resultSlot)
                feed.outBuff[i].release();
            }
          ENSURE (resultSlot < INVO::MAX_SIZ, "invalid result buffer configured.");
          return feed.outBuff[resultSlot];
        }
      
    };
  
  
  /**
   * Processing structure to activate a Render Node and produce result data.
   * @tparam PAT a _Weaving Pattern,_ which defines in detail how data is retrieved,
   *             combined and processed to yield the results; actually this implementation
   *             is assembled from several building blocks, in accordance to the specific
   *             situation as established by the _Builder_ for a given render node.
   */
  template<class PAT>
  class Turnout
    : public Port
    , public PAT
//    , util::MoveOnly
    {
      using Feed = typename PAT::Feed;
      
    public:
      template<typename...INIT>
      Turnout (ProcID& id, INIT&& ...init)
        : Port{id}
        , PAT{forward<INIT> (init)...}
        { }
      
      /**
       * Entrance point to the next recursive step of media processing.
       * @param turnoutSys anchor context with parameters and services
       * @return a BuffHandle exposing the generated result data
       */
      BuffHandle
      weave (TurnoutSystem& turnoutSys, OptionalBuff outBuff =std::nullopt)  override
        {
          Feed feed = PAT::mount();
          PAT::pull(feed, turnoutSys);
          PAT::shed(feed, outBuff);
          PAT::weft(feed);
          return PAT::fix (feed);
        }
    };

  
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_TURNOUT_H*/
