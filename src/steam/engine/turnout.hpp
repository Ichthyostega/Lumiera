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
 ** This header defines part of the "glue" which holds together the Render Node network
 ** and enables to pull result frames from the nodes. Doing so requires some local state
 ** to be maintained, especially a collection of buffers used to hold data for computation.
 ** Furthermore, getting the input buffers filled with prerequisite data leads to the issuance
 ** of recursive `weave()` calls, together creating a stack-like assembly of local invocation
 ** state.
 ** 
 ** The actual steps to be carried out for a `weave()` call are broken down into
 ** a fixed arrangement of steps, in accordance to the _weaving metaphor:_
 ** - `mount()` establish the framework of operation
 ** - `pull()` recurse into predecessors to retrieve input data
 ** - `shed()` allocate output buffers and spread out all connections
 ** - `weft()` pass invocation to the processing operation
 ** - `fix()`  detach from input, mark and commit results and pas output
 ** As arranged in the Turnout template, the necessary interconnections are prepared
 ** and this standard sequence of operations is issued, while delegating the actual
 ** implementation of these steps into a **Weaving Pattern**, integrated as mix-in
 ** base template. Notably an implementation data scheme is expected as a definition
 ** nested into the weaving pattern, designated as `PAT::Feed`, and  created
 ** _on the stack for each invocation_ by the `mount()` call. »The Feed«
 ** is conceived both as an _Invocation Adapter_ and a _Pipe Manifold:_
 ** - embedding an adapted processing-functor and a parameter-functor
 ** - providing storage slots for \ref BuffHandle management entries
 ** @note typically, a \ref MediaWeavingPattern is used as default implementation.
 ** @remark The name »Turnout« plays upon the overlay of several metaphors, notably
 **   the [Railroad Turnout]. A »Turnout System« may thus imply either a system for
 **   generating and collecting turnout, or the complex interwoven system of tracks
 **   and switches found in large railway stations.
 ** @see \ref proc-node.hpp "Overview of Render Node structures"
 ** @see turnout-system.hpp
 ** @see media-weaving-pattern.hpp
 ** @see weaving-pattern-builder.hpp
 ** [Railroad Turnout]: https://en.wikipedia.org/wiki/Railroad_turnout
 */


#ifndef STEAM_ENGINE_TURNOUT_H
#define STEAM_ENGINE_TURNOUT_H

#include "steam/common.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/engine/turnout-system.hpp"

#include <utility>


namespace steam {
namespace engine {
  
  using std::forward;
  
  
  /**
   * Definition to emulate a _Concept_ for the **Invocation Adapter**.
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
    // also: array-like outBuff
    // optionally array-like inBuff
    ASSERT_MEMBER_FUNCTOR (&ADA::connect, void());
    ASSERT_MEMBER_FUNCTOR (&ADA::invoke, void());
    return sizeof(ADA);
  }
  
  
  template<class PAT>
  constexpr bool
  _verify_usable_as_WeavingPattern()
  {
    using Feed = typename PAT::Feed;
    ASSERT_MEMBER_FUNCTOR (&PAT::mount, Feed(TurnoutSystem&));
    ASSERT_MEMBER_FUNCTOR (&PAT::pull, void(Feed&, TurnoutSystem&));
    ASSERT_MEMBER_FUNCTOR (&PAT::shed, void(Feed&, TurnoutSystem&, OptionalBuff));
    ASSERT_MEMBER_FUNCTOR (&PAT::weft, void(Feed&, TurnoutSystem&));
    ASSERT_MEMBER_FUNCTOR (&PAT::fix,  BuffHandle(Feed&, TurnoutSystem&));
    return sizeof(PAT);
  }
  
  
  
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
    {
      static_assert (_verify_usable_as_WeavingPattern<PAT>());
      
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
          Feed feed = PAT::mount (turnoutSys);
          PAT::pull (feed, turnoutSys);
          PAT::shed (feed, turnoutSys, outBuff);
          PAT::weft (feed, turnoutSys);
          return PAT::fix (feed, turnoutSys);
        }
    };

  
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_TURNOUT_H*/
