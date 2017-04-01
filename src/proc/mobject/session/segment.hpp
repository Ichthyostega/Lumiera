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


#ifndef MOBJECT_SESSION_SEGMENT_H
#define MOBJECT_SESSION_SEGMENT_H

#include <list>

#include "proc/common.hpp"
#include "proc/mobject/explicitplacement.hpp"
#include "lib/time/timevalue.hpp"

using std::list;

namespace proc {
namespace mobject {
namespace session {
  
  
  /**
   * For the purpose of building and rendering, the fixture (for each timeline)
   * is partitioned such that each segment is <i>structurally constant</i>.
   * For each segment there is a RenderGraph (unit of the render engine) which
   * is able to render all ExitNodes for this segment.
   * 
   * @todo 1/2012 Just a Placeholder. The real thing is not yet implemented.
   * @see http://lumiera.org/wiki/renderengine.html#Fixture
   */
  class Segment
    {
    protected:
      typedef lib::time::TimeSpan Span;
      
      /** begin of this timeline segment. */
      Span span_;
      
      /** relevant MObjects comprising this segment. */
      list<ExplicitPlacement> elements;
      // TODO: actually necessary??
      // TODO: ownership??
      
      /////////////////////////////////////////////////TODO: placeholder code
    };
  
  
  
}}} // namespace proc::mobject::session
#endif
