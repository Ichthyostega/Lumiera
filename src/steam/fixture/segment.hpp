/*
  SEGMENT.hpp  -  Segment of the timeline for rendering.

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


/** @file segment.hpp
 ** Building block of the backbone of the low-level (render node) model
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#ifndef STEAM_FIXTURE_SEGMENT_H
#define STEAM_FIXTURE_SEGMENT_H

#include <list>

#include "steam/common.hpp"
#include "steam/mobject/explicitplacement.hpp"
#include "steam/engine/job-ticket.hpp"
#include "lib/time/timevalue.hpp"

using std::list;

namespace steam {
namespace fixture {
  
  using mobject::ExplicitPlacement;
  using lib::time::TimeSpan;
  using lib::time::Time;
  
  /**
   * For the purpose of building and rendering, the fixture (for each timeline)
   * is partitioned such that each segment is _structurally constant._
   * For each segment there is a RenderGraph (unit of the render engine)
   * which is able to render all ExitNodes for this segment.
   * 
   * @ingroup fixture
   * @todo 1/2012 Just a Placeholder. The real thing is not yet implemented.
   * @todo WIP-WIP as of 4/2023 -- about to pull up the engine backbone
   * @see http://lumiera.org/wiki/renderengine.html#Fixture
   */
  class Segment
    {
    protected:
      
      /** begin of this timeline segment. */
      TimeSpan span_;
      
      /** render plan / blueprint to use for this segment */
      const engine::JobTicket* jobTicket_;
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #725 : placeholder code
      /** relevant MObjects comprising this segment. */
      list<ExplicitPlacement> elements;
      // TODO: actually necessary??
      // TODO: ownership??
      ///////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #725 : placeholder code
    public:
      Segment (TimeSpan covered =TimeSpan::ALL)
        : span_{covered}
        , jobTicket_{&engine::JobTicket::NOP}
      { }
      
      // default copy acceptable
      
      Time start() const { return span_.start(); }
      Time end()   const { return span_.end(); }
      
      engine::JobTicket const&
      jobTicket()  const
        {
          REQUIRE (jobTicket_);
          return *jobTicket_;
        }
    };
  
  
  
}} // namespace steam::fixture
#endif /*STEAM_FIXTURE_SEGMENT_H*/
