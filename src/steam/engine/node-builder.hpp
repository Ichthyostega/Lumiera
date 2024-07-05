/*
  NODE-BUILDER.hpp  -  Setup of render nodes connectivity

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
    2024    ,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file node-builder.hpp
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


#ifndef ENGINE_NODE_BUILDER_H
#define ENGINE_NODE_BUILDER_H


#include "steam/engine/proc-node.hpp"
#include "lib/nocopy.hpp"

#include <utility>


namespace steam {
namespace engine {
  
  using std::move;
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
  
  class PortBuilder
    : util::MoveOnly
    {
    public:
      
      PortBuilder
      inSlots (uint s)
        {
          UNIMPLEMENTED ("define number of predecessor-source slots");
          return move(*this);
        }
      
      PortBuilder
      outSlots (uint r)
        {
          UNIMPLEMENTED ("define number of result slots");
          return move(*this);
        }
      
      template<class ILA, typename...ARGS>
      PortBuilder
      createBuffers (ARGS&& ...args)
        {
          UNIMPLEMENTED ("define builder for all buffers to use");
          return move(*this);
        }
      
      /****************************************************//**
       * Terminal: complete the Port wiring and return to the node level.
       */
      void //////////////////////////////////////////////////////////OOO return type
      completePort()
        {
          UNIMPLEMENTED("finish and link-in port definition");
        }
    };
  
  
  
  class NodeBuilder
    : util::MoveOnly
    {
    public:
      
      NodeBuilder
      inSlots (uint s)
        {
          UNIMPLEMENTED ("define number of predecessor-source slots");
          return move(*this);
        }
      
      NodeBuilder
      outSlots (uint r)
        {
          UNIMPLEMENTED ("define number of result slots");
          return move(*this);
        }
      
      void //////////////////////////////////////////////////////////OOO return type
      preparePort ()
        {
          UNIMPLEMENTED ("recursively enter detailed setup of a single processing port");
//        return move(*this);
        }
      
      /****************************************************//**
       * Terminal: complete the Connectivity defined thus far.
       */
      Connectivity
      build()
        {
          UNIMPLEMENTED("Node-Connectivity Setup");
        }
    };
  
  
  class ProcBuilder
    : util::MoveOnly
    {
    public:
      
      void //////////////////////////////////////////////////////////OOO return type
      requiredSources ()
        {
          UNIMPLEMENTED ("enumerate all source feeds required");
//        return move(*this);
        }
      
      void //////////////////////////////////////////////////////////OOO return type
      retrieve (void* streamType)
        {
          UNIMPLEMENTED ("recursively define a predecessor feed");
//        return move(*this);
        }
      
      /****************************************************//**
       * Terminal: trigger the Level-3 build walk to produce a ProcNode network.
       */
      void //////////////////////////////////////////////////////////OOO return type
      build()
        {
          UNIMPLEMENTED("Level-3 build-walk");
        }
    };
  
  
  class LinkBuilder
    : util::MoveOnly
    {
    public:
      
      void //////////////////////////////////////////////////////////OOO return type
      from (void* procAsset)
        {
          UNIMPLEMENTED ("recursively enter definition of processor node to produce this feed link");
//        return move(*this);
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
    return NodeBuilder{};
  }
  
  
}} // namespace steam::engine
#endif /*ENGINE_NODE_BUILDER_H*/
