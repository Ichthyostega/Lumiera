/*
  NODE-FACTORY.hpp  -  Service for creating processing nodes of various kinds

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file node-factory.hpp
 ** A factory to build processing nodes
 ** @todo draft from the early days of the Lumiera project, reworked in 2009
 **       Development activity in that area as stalled ever since. In the end,
 **       it is highly likely we'll _indeed need_ a node factory though...
 ** @todo about to be reworked for the »Playback Vertical Slice« in 2024
 */


#ifndef STEAM_ENGINE_NODE_FACTORY_H
#define STEAM_ENGINE_NODE_FACTORY_H

#include "steam/engine/proc-node.hpp"
#include "steam/mobject/placement.hpp"
//#include "steam/engine/nodewiring-obsolete.hpp"   /////////////////////////////////////////////////////////TICKET #1367 : Adapt to rework of Node Invocation


  ////////////////////////////////////TODO: make inclusions / forward definitions a bit more orthogonal....

namespace lib { class AllocationCluster; }

namespace steam {
namespace mobject {
namespace session {
  
  class Clip;
  class Effect;
  typedef Placement<Effect> PEffect;
  // TODO: class Transition;
  
}} // namespace mobject::session


namespace engine {

  using std::vector;
  using lib::AllocationCluster;
  
//  class WiringSituation;


  /**
   * Create processing nodes based on given objects of the high-level model.
   */
  class NodeFactory
    {
      AllocationCluster& alloc_;
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
      WiringFactory wiringFac_;
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
      
    public:
      NodeFactory (AllocationCluster& a)
        : alloc_(a)
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
        , wiringFac_(alloc_)
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
        { }
      
      
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
      PNode operator() (mobject::session::PEffect const&, WiringSituation&);
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
 
        
    };

}} // namespace steam::engine
#endif /*STEAM_ENGINE_NODE_FACTORY_H*/
