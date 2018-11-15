/*
  CalcPlanContinuation  -  closure for planning a chunk of jobs

  Copyright (C)         Lumiera.org
    2013,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file calc-plan-continuation.cpp
 ** Implementation elements of render process planning.
 ** @todo a draft created in 2013 and then stalled. This is not obsolete.
 */


#include "steam/engine/calc-plan-continuation.hpp"
#include "steam/engine/frame-coord.hpp"
#include "steam/engine/job-ticket.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/frameid.hpp"
//#include "steam/state.hpp"

#include <boost/functional/hash.hpp>



namespace proc {
namespace engine {
  
  
  /** entry point (interface JobClosure): invoke the concrete job operation.
   *  In this case, the job operation is responsible for planning a chunk of actual render jobs.
   */
  void
  CalcPlanContinuation::invokeJobOperation (JobParameter parameter)
  {
    ASSERT (parameter.nominalTime == timings_.getFrameStartAt (parameter.invoKey.frameNumber));
    
    this->performJobPlanningChunk (parameter.invoKey.frameNumber);
  }
  
  
  void
  CalcPlanContinuation::signalFailure (JobParameter parameter, JobFailureReason reason)
  {
    UNIMPLEMENTED ("what needs to be done when a planning continuation cant be invoked?");
  }
  
  
  bool
  CalcPlanContinuation::verify (Time nominalTime, InvocationInstanceID invoKey)  const
  {
    return timings_.isValid()
        && Time::MIN < nominalTime && nominalTime < Time::MAX
        && nominalTime == timings_.getFrameStartAt (invoKey.frameNumber);
  }
  
  
  size_t
  CalcPlanContinuation::hashOfInstance (InvocationInstanceID invoKey) const
  {
    return boost::hash_value (invoKey.frameNumber);
  }
  
  
  
  
  
  Job
  CalcPlanContinuation::prepareRenderPlanningFrom (FrameCnt startFrame)
  {
    InvocationInstanceID invoKey;
    invoKey.frameNumber = startFrame;
    Time nominalPlanningStartTime = timings_.getFrameStartAt (startFrame);
    
    return Job(*this, invoKey, nominalPlanningStartTime);
  }
  
  
  void
  CalcPlanContinuation::performJobPlanningChunk(FrameCnt nextStartFrame)
  {
    TimeAnchor refPoint(timings_, nextStartFrame);
    JobPlanningSequence jobs = dispatcher_.onCalcStream(modelPort_, channel_)
                                          .establishNextJobs(refPoint);
    
    Job nextChunkOfPlanning = buildFollowUpJobFrom (refPoint);
    
    UNIMPLEMENTED ("the actual meat: access the scheduler and fed those jobs");
  }
  
  
  Job
  CalcPlanContinuation::buildFollowUpJobFrom (TimeAnchor const& refPoint)
  {
    return this->prepareRenderPlanningFrom(
                   refPoint.getNextAnchorPoint());
  }
  
  
  
  
}} // namespace engine
