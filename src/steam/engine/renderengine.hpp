/*
  RENDERENGINE.hpp  -  a complete network of processing nodes usable for rendering

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file renderengine.hpp
 ** (Planned) access point to the render engine as service.
 ** @deprecated a pile of crap, leftovers and started drafts, stalled since 2011 -- but maybe here to stay...
 */


#ifndef STEAM_ENGINE_RENDERENGINE_H
#define STEAM_ENGINE_RENDERENGINE_H

#include "steam/engine/rendergraph.hpp"

#include <list>



/////////////////////////////TODO 7/11 this is a piece of debris, left over from the first attempt to complete the render nodes network.
/////////////////////////////TODO Meanwhile the intention is to treat the render nodes network more like a data structure,
/////////////////////////////TODO consequently this will become some kind of root or anchor point for this network

//////////TODO for the "real" engine API: look at engine-serivce.hpp

namespace steam {
namespace engine {
  
  using std::list;
  
  /**
   * @todo this was planned to become the frontend
   * to the render node network, which can be considered
   * at the lower end of the middle layer; the actual 
   * render operations are mostly implemented by the vault layer
   * ////////TODO WIP as of 12/2010
   */
  class RenderEngine
    : public RenderGraph
    {
    public:
       ///// TODO: find out about the public operations
       // note: the play controller lives in the steam-layer,
       //       but is a subsystem separate of the session.
      RenderEngine();
      
    private:
      list<RenderGraph> renderSegments;
      
    };

}} // namespace steam::engine
#endif /*STEAM_ENGINE_RENDERENGINE_H*/
