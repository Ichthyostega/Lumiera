/*
  WEAVING-PATTERN.hpp  -  Base patterns to organise data connections and render processing invocation

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file weaving-pattern.hpp
 ** Construction set to assemble and operate a data processing scheme within a Render Node.
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
 ** @warning as of 12/2024 first complete integration round of the Render engine ////////////////////////////TICKET #1367
 ** 
 */


#ifndef STEAM_ENGINE_WEAVING_PATTERN_H
#define STEAM_ENGINE_WEAVING_PATTERN_H

#include "steam/common.hpp"
#include "steam/engine/turnout.hpp"
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
     * Standard implementation for a _Weaving Pattern_ to connect
     * the input and output data feeds (buffers) into a processing function.
     * @tparam INVO a configuration / policy base class to _adapt for invocation_
     * @note assumptions made regarding the overall structure
     *     - `INVO::Feed` defines an _invocation adapter_ for the processing function
     *     - `INVO::buildFeed()` is a functor to (repeatedly) build `Feed` instances
     *     - the _invocation adapter_ in turn embeds a `FeedManifold<FUN>` to hold
     *       + a setup of output buffer pointers (single, tuple or array)
     *       + (optionally) a similar setup for input buffer pointers
     *       + (optionally) a parameter or parameter tuple
     *       + storage to configure BuffHandle entries for each «slot»
     *       + storage to hold the actual processing functor
     */
  template<class INVO>
  struct MediaWeavingPattern
    : INVO
    {
      using Feed = typename INVO::Feed;
      
      static_assert (_verify_usable_as_InvocationAdapter<Feed>());
      
      Several<PortRef>   leadPort;
      Several<BuffDescr> outTypes;
      
      uint resultSlot{0};
      
      /** forwarding-ctor to provide the detailed input/output connections */
      template<typename...ARGS>
      MediaWeavingPattern (Several<PortRef>&&   pr
                          ,Several<BuffDescr>&& dr
                          ,uint resultIdx
                          ,ARGS&& ...args)
        : INVO{forward<ARGS>(args)...}
        , leadPort{move(pr)}
        , outTypes{move(dr)}
        , resultSlot{resultIdx}
        { }
      
      
      Feed
      mount (TurnoutSystem& turnoutSys)
        {
          ENSURE (leadPort.size() <= INVO::FAN_I);
          ENSURE (outTypes.size() <= INVO::FAN_O);
          return INVO::buildFeed (turnoutSys);
        }
      
      void
      pull (Feed& feed, TurnoutSystem& turnoutSys)
        {
          if constexpr (Feed::hasInput())
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
          feed.connect();
        }
      
      void
      weft (Feed& feed)
        {
          feed.invoke();                 // process data
        }
      
      BuffHandle
      fix (Feed& feed)
        {
          if constexpr (Feed::hasInput())
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
          ENSURE (resultSlot < INVO::FAN_O, "invalid result buffer configured.");
          return feed.outBuff[resultSlot];
        }
      
    };
  
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_WEAVING_PATTERN_H*/
