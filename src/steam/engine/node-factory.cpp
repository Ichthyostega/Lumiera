/*
  NodeFactory  -  Service for creating processing nodes of various kinds

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file node-factory.cpp
 ** Implementation of render node setup and wiring
 ** @todo draft from the early days of the Lumiera project, reworked in 2009
 **       Development activity in that area as stalled ever since. In the end,
 **       it is highly likely we'll _indeed need_ a node factory though...
 ** @todo WIP-WIP 2024 to be reworked from ground up for the »Playback Vertical Slice«      
 */


#include "steam/engine/exit-node.hpp"
#include "steam/engine/node-factory.hpp"
#include "steam/mobject/session/effect.hpp"
#include "lib/allocation-cluster.hpp"

//#include "steam/engine/nodewiring-obsolete.hpp"

namespace steam {
namespace engine {
  
  /// storage for the »inactive« ExitNode marker
  ExitNode ExitNode::NIL{};
  
  
  namespace { // Details of node fabrication

    ////////////////////////////////////////////////TODO: still needed?
  
  } // (END) Details of node fabrication
  
  


}} // namespace engine
