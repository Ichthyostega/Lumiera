/*
  RENDERGRAPH.hpp  -  render network corresponding to one segment of the timeline

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

*/


/** @file rendergraph.hpp
 ** @deprecated a pile of crap, leftovers and started drafts, stalled since 2011 -- but maybe here to stay...
 */


#ifndef STEAM_ENGINE_RENDERGRAPH_H
#define STEAM_ENGINE_RENDERGRAPH_H

#include "steam/common.hpp"
#include "steam/state.hpp"
#include "lib/time/timevalue.hpp"


/////////////////////////////TODO 7/11 this is a piece of debris, left over from the first attempt to complete the render nodes network.
/////////////////////////////TODO Meanwhile the intention is to treat the render nodes network more like a data structure,
/////////////////////////////TODO consequently this will become some kind of root or anchor point for this network


namespace proc {
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
  
  
  
}} // namespace proc::engine
#endif /*STEAM_ENGINE_RENDERGRAPH_H*/
