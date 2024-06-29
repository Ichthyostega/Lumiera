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


#include "steam/engine/proc-node.hpp"
#include "lib/nocopy.hpp"



namespace steam {
namespace engine {
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
  
  class NodeWiringBuilder
    : util::MoveOnly
    {
    public:
      
      /****************************************************//**
       * Terminal: complete the Connectivity defined thus far.
       */
      Connectivity
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
    return NodeWiringBuilder{};
  }
  
  
}} // namespace steam::engine
#endif /*ENGINE_NODE_WIRING_BUILDER_H*/
