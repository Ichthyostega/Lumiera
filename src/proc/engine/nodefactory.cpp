/*
  NodeFactory  -  Service for creating processing nodes of various kinds

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file nodefactory.cpp
 ** Implementation of render node setup and wiring
 ** @todo draft from the early days of the Lumiera project, reworked in 2009
 **       Development activity in that area as stalled ever since. In the end,
 **       it is highly likely we'll _indeed need_ a node factory though...
 */


#include "proc/engine/nodefactory.hpp"
#include "proc/mobject/session/effect.hpp"
#include "lib/allocation-cluster.hpp"

#include "proc/engine/nodewiring.hpp"

namespace proc {
namespace engine {
  
  namespace { // Details of node fabrication

    ////////////////////////////////////////////////TODO: still needed?
  
  } // (END) Details of node fabrication
  
  
  using mobject::Placement;
  using mobject::session::Effect;
  
  
  /** create a processing node able to render an effect
   * @todo as of 2016 this is the only (placeholder) implementation
   *       to indicate how building and wiring of nodes was meant to happen
   */
  PNode
  NodeFactory::operator() (Placement<Effect> const& effect, WiringSituation& intendedWiring)
  {
    intendedWiring.resolveProcessor(effect->getProcAsset());
    WiringDescriptor& wiring = wiringFac_(intendedWiring);
    
    ProcNode& newNode = alloc_.create<ProcNode> (wiring);
    ENSURE (newNode.isValid());
    return &newNode;
  }


}} // namespace engine
