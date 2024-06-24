/*
  NODE-WIRING-BUILDER.hpp  -  Setup of render nodes connectivity

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file node-wiring-builder.hpp
 ** Helper for defining the desired wiring and operation mode for a render node.
 ** During the Builder run, the render nodes network is wired up starting from the
 ** source (generating) nodes up to the exit nodes. As the wiring is implemented through
 ** a const engine::Connectivity, when a new node gets fabricated, all of the connections
 ** to its predecessors need to be completely settled; similarly, any information pertaining
 ** the desired operation mode of this node need to be available. Thus we use this temporary
 ** information record to assemble all these pieces of information.
 ** 
 ** @deprecated WIP-WIP-WIP 2024 Node-Invocation is reworked from ground up -- not clear yet what happens with the builder
 ** 
 ** @see steam::engine::NodeFactory
 ** @see nodewiring.hpp
 ** @see node-basic-test.cpp
 ** 
 */


#ifndef ENGINE_NODE_WIRING_BUILDER_H
#define ENGINE_NODE_WIRING_BUILDER_H


//#include "steam/engine/proc-node.hpp"    //////////////////////////////////////////////////////////////////TICKET #1367 : switch to new Node Invocation scheme
#include "steam/engine/connectivity-obsolete.hpp"
#include "lib/ref-array.hpp"
#include "lib/util-foreach.hpp"
#include "lib/nocopy.hpp"



namespace steam {
namespace engine {
  
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
  
  class NodeWiringBuilder
    : util::MoveOnly
    {
    public:
      
      /****************************************************//**
       * Terminal: complete the Connectivity defined thus far.
       */
//    Connectivity
      void*   /////////////////////////////////////////////////OOO Connectivity no longer needs to be abstract
      build()
        {
          UNIMPLEMENTED("Node-Connectivity Setup");
        }
    };
  
  
  /**
   * Entrance point for building node Connectivity
   */
  template<class ONT>
  auto
  buildPatternFor()
  {
    UNIMPLEMENTED("instantiate Domain Ontology Facade and outfit the NodeWiringBuilder");
  }
  
  
}} // namespace steam::engine
#endif /*ENGINE_NODE_WIRING_BUILDER_H*/
