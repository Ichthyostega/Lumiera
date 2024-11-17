/*
  RENDERGRAPH.hpp  -  render network corresponding to one segment of the timeline

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file rendergraph.hpp
 ** @deprecated a pile of crap, leftovers and started drafts, stalled since 2011 -- but maybe here to stay...
 */


#ifndef STEAM_ENGINE_RENDERGRAPH_H
#define STEAM_ENGINE_RENDERGRAPH_H

#include "steam/common.hpp"
#include "steam/engine/state-closure-obsolete.hpp"
#include "lib/time/timevalue.hpp"


/////////////////////////////TODO 7/11 this is a piece of debris, left over from the first attempt to complete the render nodes network.
/////////////////////////////TODO Meanwhile the intention is to treat the render nodes network more like a data structure,
/////////////////////////////TODO consequently this will become some kind of root or anchor point for this network


namespace steam {
namespace engine {
  
  using lib::time::TimeSpan;
  using lib::time::FSecs;
  using lib::time::Time;
  
  class ExitNode;
  
  /**
   * @todo likely to be reworked into the engine backbone  /////////////TODO WIP as of 12/2010
   */
  class RenderGraph
    {
    protected:
      ExitNode * output;
      
      /** timerange covered by this RenderGraph */
      TimeSpan segment_;
      
    public:
      RenderGraph()
        : segment_(Time::ZERO, FSecs(5))
        {
          UNIMPLEMENTED ("anything regarding the Fixture datastructure");
        }
      
    };
  
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_RENDERGRAPH_H*/
