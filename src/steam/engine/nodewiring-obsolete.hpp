/*
  NODEWIRING.hpp  -  Implementation of the node network and operation control

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file nodewiring.hpp
 ** Mechanism to wire ProcNode instances for a render network
 ** @todo unfinished draft from 2009 regarding the render process
 ** @deprecated 2024 this header will likely be obsoleted
 ** @see node-wiring-builder.hpp for the rewritten node-builder
 */


#ifndef ENGINE_NODEWIRING_OBSOLETE_H
#define ENGINE_NODEWIRING_OBSOLETE_H


#include "steam/engine/connectivity-obsolete.hpp"
#include "lib/allocation-cluster.hpp"
#include "lib/ref-array.hpp"
#include "lib/util-foreach.hpp"
#include "lib/nocopy.hpp"

#include <memory>



namespace steam {
namespace engine {
  
  
  class WiringFactory;
  
  namespace config { class WiringFactoryImpl; }
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : inlined here and then dropped in node-wiring-builder.hpp
  using lib::RefArray;
  
  
  /**
   * Finding out about a concrete way of wiring up a
   * ProcNode about to be built. Such a (temporary) setup object
   * is used while building the low-level model. It is loaded with
   * information concerning the intended connections to be made
   * and then used to initialise the wiring descriptor, which
   * in turn allows us to setup the ProcNode.
   * 
   * \par intended usage pattern
   * The goal is to describe the constellation of a new node to be built.
   * Thus, we start with one or several existing nodes, specifying which
   * output should go to which input pin of the yet-to-be created new node.
   * When intending to create a source node, a default WiringSituation
   * should be used, without adding any connection information.
   *  
   * @deprecated WIP-WIP-WIP 2024 Node-Invocation is reworked from ground up for the »Playback Vertical Slice«
   */
  class WiringSituation
    : util::NonCopyable
    {
      long flags_;
      asset::Proc::ProcFunc* function_;
      
    public: /* === API for querying collected data === */
      RefArray<ChannelDescriptor>&
      makeOutDescriptor()  const
        {
          UNIMPLEMENTED ("build new output descriptors for the node under construction");  //////////TODO: where to get the information about the output channels???
        }
      
      RefArray<InChanDescriptor>&
      makeInDescriptor()   const
        {
          UNIMPLEMENTED ("build new input descriptors for the node under construction");
        }
      
      Connectivity::ProcFunc*
      resolveProcessingFunction()  const
        {
          REQUIRE (function_);
          return function_;
        }
      
      lumiera::NodeID const&
      createNodeID()  const
        {
          UNIMPLEMENTED ("initiate generation of a new unique node-ID"); // see rendergraph.cpp
        }
      
      
      
    public: /* === API for specifying the desired wiring === */
      
      /** A default WiringSituation doesn't specify any connections.
       *  It can be used as-is for building a source node, or augmented
       *  with connection information later on.
       */
      WiringSituation()
        : flags_(0)
        , function_(0)
        {
          UNIMPLEMENTED ("representation of the intended wiring");
        }
      
      
      /** Continue the wiring by hooking directly into the output
       *  of an existing predecessor node
       */
      WiringSituation (PNode predecessor)
        : flags_(0)
        , function_(0)
        {
          UNIMPLEMENTED ("wiring representation; hook up connections 1:1");
          REQUIRE (predecessor);
          
          //////////////////////////TODO: see Ticket 254
          // for_each (predecessor->outputs(), .....   see also Ticket 183 (invoking member fun in for_each)
          
        }
      
      
      /** set up a connection leading to a specific input pin of the new node */
      WiringSituation&
      defineInput (uint inPin, PNode pred, uint outPin)
        {
          UNIMPLEMENTED ("wiring representation; define new connection");
          return *this;
        }
      
      /** set up the next input connection,
       *  originating at a specific output pin of the predecessor */
      WiringSituation&
      defineInput (PNode pred, uint outPin)
        {
          UNIMPLEMENTED ("wiring representation; define new connection");
          return *this;
        }
      
      /** set up the next input connection to a specific input pin,
       *  originating at a the next/sole output pin of the predecessor */
      WiringSituation&
      defineInput (uint inPin, PNode pred)
        {
          UNIMPLEMENTED ("wiring representation; define new connection");
          return *this;
        }
      
      
      /** set detail flags regarding the desired node operation mode */
      WiringSituation&
      setFlag (long code)
        {
          flags_ |= code;
          return *this;
        }
      
      long getFlags ()  const { return flags_; }
      
      
      /** trigger resolving of the actual processing function */
      WiringSituation&
      resolveProcessor (asset::Proc const& procAsset)
        {
          function_ = procAsset.resolveProcessor();
          ENSURE (function_);
        }
      
    };
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : (END)inlined here and then dropped in node-wiring-builder.hpp
  
  
  
  
  /**
   * Actual implementation of the link between nodes,
   * also acting as "track switch" for the execution path
   * chosen while operating the node network for rendering.
   * @param STATE Invocation state object controlling the
   *        behaviour of callDown() while rendering.
   * @see StateAdapter
   * @see NodeFactory
   */
  template<class STATE>
  class NodeWiring
    : public Connectivity
    {
      
    public:
      NodeWiring(WiringSituation const& setup)
        : Connectivity(setup.makeOutDescriptor(),
                           setup.makeInDescriptor(),
                           setup.resolveProcessingFunction(),
                           setup.createNodeID())
        { }
      
    private:
      virtual BuffHandle
      callDown (StateClosure_OBSOLETE& currentProcess, uint requestedOutputNr)  const
        {
          STATE thisStep (currentProcess, *this, requestedOutputNr);
          return thisStep.retrieve (); // fetch or calculate results
        }
    };
  
  
  
  class WiringFactory
    {
      lib::AllocationCluster& alloc_;
      std::unique_ptr<config::WiringFactoryImpl> pImpl_;
      
    public:
      WiringFactory (lib::AllocationCluster& a);
     ~WiringFactory ();
      
      Connectivity&
      operator() (WiringSituation const& setup);
    };
  
  
  
}} // namespace steam::engine
#endif
