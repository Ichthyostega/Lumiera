/*
  TIME-ANCHOR.hpp  -  current render evaluation time point closure

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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


#ifndef PROC_ENGINE_TIME_ANCHOR_H
#define PROC_ENGINE_TIME_ANCHOR_H

#include "proc/common.hpp"
//#include "proc/state.hpp"
#include "lib/time/timevalue.hpp"



namespace proc {
namespace engine {
  
  using lib::time::TimeSpan;
  using lib::time::FSecs;
  using lib::time::Time;
//  
//  class ExitNode;
  
  /**
   * The process of playback or rendering is a continued series of
   * exploration and evaluation. The outline of what needs to be calculated
   * is determined continuously, proceeding in chunks of evaluation.
   * Each of these continued partial evaluations establishes a distinct
   * anchor or breaking point in time: everything before this point
   * is settled and planned thus far. Effectively, this time point
   * acts as a <i>evaluation closure</i>, to be picked up on the
   * next partial evaluation. More specifically, the TimeAnchor
   * closure is the definitive binding between the abstract
   * logical time of the session timeline, and the real
   * wall-clock time forming the deadline for rendering.
   * 
   * @todo 1/12 WIP-WIP-WIP just emerging as a concept
   */
  class TimeAnchor
    {
      
    public:
      TimeAnchor()
        {
          UNIMPLEMENTED ("anything regarding the Engine backbone");
        }
      
      // using default copy operations
    };
  
  
  
}} // namespace proc::engine
#endif
