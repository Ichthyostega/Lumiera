/*
  SEGMENTATION.hpp  -  Partitioning of a timeline for organising the render graph.

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


/** @file segmentation.hpp
 ** Part of the Fixture datastructure to manage time segments of constant structure.
 ** The Fixture is result of the build process and separation between high-level and
 ** low-level model. It's kind of an effective resulting timeline, and split into segments
 ** of constant wiring structure: whenever the processing nodes need to be wired differently
 ** for some timespan, we start a new segment of the timeline. This might be for the duration
 ** of a clip, or just for the duration of a transition, when the pipes of both clips need to
 ** be wired up in parallel.
 ** 
 ** Within the Fixture, a Segment of the timeline is used as attachment point for all the
 ** render nodes relevant for rendering this segment. Thus, the Segmentation defines the
 ** index and access datastructure to get at any point of the render node network.
 ** Moreover, the segments are used as foundation for render node memory management
 ** 
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 ** 
 ** @see Fixture
 ** @see ModelPort
 */


#ifndef MOBJECT_SESSION_SEGMENTATION_H
#define MOBJECT_SESSION_SEGMENTATION_H


#include "steam/mobject/session/segment.hpp"

#include <list>

using std::list;


namespace proc {
namespace mobject {
namespace session {
  
  
  /**
   * For the purpose of building and rendering, the fixture (for each timeline) 
   * is partitioned such that each segment is <i>structurally constant</i>. 
   * The Segmentation defines and maintains this partitioning. Further,
   * it is the general entry point for accessing the correct part of the engine
   * responsible for a given timeline time point.
   * @see SegmentationTool actually calculating the Segmentation
   * 
   * @todo 1/2012 Just a Placeholder. The real thing is not yet implemented.
   * @see http://lumiera.org/wiki/renderengine.html#Fixture
   */
  class Segmentation
    {
      /////////////////////////////////////////////////TODO: placeholder code
      
      /** segments of the engine in ordered sequence. */
      list<Segment> segments_;
      
    };
  
  
  
}}} // namespace proc::mobject::session
#endif
