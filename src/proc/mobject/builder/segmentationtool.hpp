/*
  SEGMENTATIONTOOL.hpp  -  Tool for creating a partitioning of the current timeline
 
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


#ifndef MOBJECT_BUILDER_SEGMENTATIONTOOL_H
#define MOBJECT_BUILDER_SEGMENTATIONTOOL_H


#include "proc/mobject/builder/applicablebuildertargettypes.hpp"

#include "proc/mobject/session/segment.hpp"
#include "proc/mobject/session/fixture.hpp"   //////TODO really on the header??


#include <list>
using std::list;



namespace mobject {
  namespace builder {


    /**
     * Tool implementation for deriving a partitioning of the current 
     * timeline, such that each segement has a constant configuration. 
     * "Constant" means here, that any remaining changes over time
     * can be represented by automation solely, without the need 
     * to change the node connections.
     */
    class SegmentationTool 
      : public ApplicableBuilderTargetTypes<SegmentationTool>
      {
        
        SegmentationTool (session::Fixture &) ;
        
      public:
        void treat (mobject::session::Clip& clip) ;
        void treat (mobject::session::Effect& effect) ;

        void treat (mobject::Buildable& something) ;
        
        //////////////////////////////////////////////////////////TODO make it respond to the util::isnil test!

      protected:
        typedef mobject::session::Segment Segment;

        /** Partitioning of the Timeline to be created by this tool. */
        list<Segment*> segments;
        // TODO handle alloc!!!!

      };



  } // namespace mobject::builder

} // namespace mobject
#endif
