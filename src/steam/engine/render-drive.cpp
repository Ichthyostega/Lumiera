/*
  RenderDrive  -  repetitively advancing a render calculation stream

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file render-drive.cpp
 ** Implementation elements of render process planning.
 ** @todo 4/2023 »Playback Vertical Slice« -- effort towards first integration of render process ////////////TICKET #1221
 */


#include "steam/engine/render-drive.hpp"
#include "steam/engine/job-ticket.hpp"
#include "lib/time/timevalue.hpp"

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
  
  
  InvocationInstanceID
  RenderDrive::buildInstanceID (HashVal seed)  const
  {
    UNIMPLEMENTED ("systematically generate an invoKey, distinct for the nominal time");
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
    invoKey.frameNumber = startFrame;/////////////////////////////////////////////////////////TICKET #1301 : Job should be created similar to what JobTicket does
    Time nominalPlanningStartTime = getTimings().getFrameStartAt (startFrame);
    
    return Job(*this, invoKey, nominalPlanningStartTime);
  }
  
  
  void
  RenderDrive::performJobPlanningChunk(FrameCnt nextStartFrame)
  {
    UNIMPLEMENTED ("the actual meat: advance the render process");
  }
  
  
  Job
  RenderDrive::buildFollowUpJobFrom (Time refPoint)
  {
    UNIMPLEMENTED ("create a follow-up job to pick up job-planning at or after the refPoint");
  }
  
  
  
  
}} // namespace engine
