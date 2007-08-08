/*
  SEGMENTATIONTOOL.hpp  -  Tool for creating a partitioning of the current timeline
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


#ifndef PROC_MOBJECT_BUILDER_SEGMENTATIONTOOL_H
#define PROC_MOBJECT_BUILDER_SEGMENTATIONTOOL_H

#include <list>

#include "proc/mobject/buildable.hpp"
#include "proc/mobject/builder/tool.hpp"
#include "proc/mobject/session/segment.hpp"

using std::list;



namespace proc
  {
  namespace mobject
    {
    namespace session
      { 
      // Forward declarations
      class Clip;
      class Effect;
      }
    
    namespace builder
      {


      /**
       * Tool implementation for deriving a partitioning of the current 
       * timeline, such that each segement has a constant configuration. 
       * "Constant" means here, that any remaining changes over time
       * can be represented by automation solely, without the need 
       * to change the node connections.
       */
      class SegmentationTool : public Tool
        {
        public:
          void treat (proc::mobject::session::Clip& clip) ;
          void treat (proc::mobject::session::Effect& effect) ;

          void treat (proc::mobject::Buildable& something) ;

        protected:
          typedef proc::mobject::session::Segment Segment;
          
          /** Partitioning of the Timeline to be created by this tool. */
          list<Segment*> segments;
          // TODO handle alloc!!!!

        };
        
        

    } // namespace proc::mobject::builder

  } // namespace proc::mobject

} // namespace proc
#endif
