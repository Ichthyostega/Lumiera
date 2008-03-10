/*
  SEGMENT.hpp  -  Segment of the Timeline.
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/


#ifndef MOBJECT_SESSION_SEGMENT_H
#define MOBJECT_SESSION_SEGMENT_H

#include <list>

#include "lumiera.h"
#include "proc/mobject/explicitplacement.hpp"


using std::list;


namespace mobject
  {
  namespace session
    {


    /**
     * Used at the moment (7/07) for partitioning the timeline/fixture into segments
     * to be rendered by a specialized render node network for each, without the need
     * to change any connections within a given segment.
     * Note this concept may be superfluos alltogether; is a draft and the real
     * use still needs to be worked out...
     */
    class Segment
      {
      protected:
        typedef lumiera::Time Time;

        /** begin of this timeline segment. */
        Time start;
        /** duration (span) of this timeline segment. */
        Time length;

        /** relevant MObjects comprising this segment. */
        list<ExplicitPlacement *> elements;
        // TODO: actually necessary??

      };



  } // namespace mobject::session

} // namespace mobject
#endif
