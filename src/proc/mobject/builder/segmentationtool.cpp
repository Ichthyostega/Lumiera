/*
  SegmentationTool  -  Tool for creating a partitioning of the current timeline
 
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
 
* *****************************************************/


#include "proc/mobject/builder/segmentationtool.hpp"
#include "proc/mobject/buildable.hpp"
#include "proc/mobject/session/clip.hpp"
#include "proc/mobject/session/effect.hpp"
#include "proc/mobject/session/segment.hpp"

using proc::mobject::Buildable;
using proc::mobject::session::Clip;
using proc::mobject::session::Effect;


namespace proc
  {
  namespace mobject
    {
    namespace builder
      {



      void
      SegmentationTool::treat (Buildable& something)
      {
      }


      void
      SegmentationTool::treat (Clip& clip)
      {
      }


      void
      SegmentationTool::treat (Effect& effect)
      {
      }



    } // namespace proc::mobject::builder

  } // namespace proc::mobject

} // namespace proc
