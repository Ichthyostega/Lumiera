/*
  FRAME-COORD.hpp  -  unique distinct coordinates of a frame to be calculated

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


#ifndef PROC_ENGINE_FRAME_COORD_H
#define PROC_ENGINE_FRAME_COORD_H

#include "proc/common.hpp"
//#include "proc/state.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"



namespace proc {
namespace engine {
  
//using lib::time::TimeSpan;
  using lib::time::Duration;
//using lib::time::FSecs;
  using lib::time::Time;
//  
//  class ExitNode;
  
  /**
   * effective coordinates of a frame to be calculated.
   * Frame coordinates are produced as result of the Dispatcher call,
   * thus forming the foundation of an actual ProcNode invocation
   * A frame render job can be characterised by
   * - the nominal (timeline) time of the frame
   * - the corresponding frame-number
   * - the real wall-clock time of expected delivery
   * - timing constraints (e.g. latency to observe) //////////////TODO : not clear if we repeat this information here
   * - the actual node to pull data from
   * - the segment holding that node                //////////////TODO : is this information really required??
   * 
   * @todo 1/12 WIP-WIP-WIP defining the invocation sequence and render jobs
   */
  class FrameCoord
    {
      
    public:
      Time absoluteNominalTime;
      int64_t absoluteFrameNumber;
      
      FrameCoord()
        {
          UNIMPLEMENTED ("anything regarding the Node Invocation");
        }
      
      // using default copy operations
      
      
      Duration remainingRealTime()
        {
          UNIMPLEMENTED ("deterine the real wall clock time amount left until deadline");
        }

      
    };
  
  
  
}} // namespace proc::engine
#endif
