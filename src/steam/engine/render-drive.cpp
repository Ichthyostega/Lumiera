/*
  RenderDrive  -  repetitively advancing a render calculation stream

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file render-drive.cpp
 ** Implementation elements of render process planning.
 ** @todo 4/2023 »Playback Vertical Slice« -- effort towards first integration of render process ////////////TICKET #1221
 */


#include "steam/engine/render-drive.hpp"
#include "steam/engine/frame-coord.hpp"
#include "steam/engine/job-ticket.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/frameid.hpp"
//#include "steam/state.hpp"

#include <boost/functional/hash.hpp>



namespace steam {
namespace engine {
  
  
  /** entry point (interface JobClosure): invoke the concrete job operation.
   *  In this case, the job operation is responsible for planning a chunk of actual render jobs.
   */
  void
  RenderDrive::invokeJobOperation (JobParameter parameter)
  {
    ASSERT (parameter.nominalTime == getTimings().getFrameStartAt (parameter.invoKey.frameNumber));
    
    this->performJobPlanningChunk (parameter.invoKey.frameNumber);
  }
  
  
  void
  RenderDrive::signalFailure (JobParameter parameter, JobFailureReason reason)
  {
    UNIMPLEMENTED ("what needs to be done when a planning continuation cant be invoked?");
  }
  
  
  bool
  RenderDrive::verify (Time nominalTime, InvocationInstanceID invoKey)  const
  {
    UNIMPLEMENTED ("the actual meat: advance the render process");
    return getTimings().isValid()
        && Time::MIN < nominalTime && nominalTime < Time::MAX
        && nominalTime == getTimings().getFrameStartAt (invoKey.frameNumber);
  }
  
  
  size_t
  RenderDrive::hashOfInstance (InvocationInstanceID invoKey) const
  {
    UNIMPLEMENTED ("the actual meat: advance the render process");
    return boost::hash_value (invoKey.frameNumber);
  }
  
  
  
  
  
  Job
  RenderDrive::prepareRenderPlanningFrom (FrameCnt startFrame)
  {
    InvocationInstanceID invoKey;
    invoKey.frameNumber = startFrame;
    Time nominalPlanningStartTime = getTimings().getFrameStartAt (startFrame);
    
    return Job(*this, invoKey, nominalPlanningStartTime);
  }
  
  
  void
  RenderDrive::performJobPlanningChunk(FrameCnt nextStartFrame)
  {
    TimeAnchor refPoint(getTimings(), nextStartFrame);
    UNIMPLEMENTED ("the actual meat: advance the render process");
  }
  
  
  Job
  RenderDrive::buildFollowUpJobFrom (TimeAnchor const& refPoint)
  {
    return this->prepareRenderPlanningFrom(
                   refPoint.getNextAnchorPoint());
  }
  
  
  
  
}} // namespace engine
