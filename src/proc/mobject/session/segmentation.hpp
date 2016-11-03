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
 ** TODO segmentation.hpp
 */


#ifndef MOBJECT_SESSION_SEGMENTATION_H
#define MOBJECT_SESSION_SEGMENTATION_H


#include "proc/mobject/session/segment.hpp"

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
