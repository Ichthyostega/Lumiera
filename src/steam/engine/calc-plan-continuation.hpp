/*
  CALC-PLAN-CONTINUATION.hpp  -  closure for planning a chunk of jobs

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

*/


/** @file calc-plan-continuation.hpp
 ** A specialised render job to care for the planning of the calculation process itself.
 ** Rendering is seen as an open-ended, ongoing process, and thus the management and planning
 ** of the render process itself is performed chunk wise and embedded into the other rendering
 ** calculations. The _"rendering-as-it-is-planned-right-now"_ can be represented as a closure
 ** to the jobs, which perform and update this plan on the go. And in fact, the head of the
 ** calculation process, the CalcStream, holds onto such a closure to access current planning.
 ** 
 ** @deprecated 4/2023 »Playback Vertical Slice« -- reworked into the RenderDrive   /////////////////////////TICKET #1221
 */


#ifndef STEAM_ENGINE_CALC_PLAN_CONTINUATION_H
#define STEAM_ENGINE_CALC_PLAN_CONTINUATION_H

#include "steam/common.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/engine/time-anchor.hpp"
#include "steam/engine/dispatcher.hpp"
#include "steam/play/timings.hpp"
#include "vault/engine/job.h"


namespace steam {
namespace engine {
  
//  using std::function;
  using vault::engine::JobParameter;
  using vault::engine::JobClosure;
  using mobject::ModelPort;
//  using lib::time::TimeSpan;
//  using lib::time::FSecs;
//  using lib::time::Time;
  using lib::time::FrameCnt;
  using lib::HashVal;
  
  
  /**
   * Special job to perform the job planning.
   * This closure extends the existing planning of frame jobs to add a chunk
   * of additional future jobs. Included with this chunk will be a recursive
   * self re-invocation to trigger planning of the next chunk. Overall, this
   * planning process is determined and controlled by the CalcStream owning
   * this closure.
   * 
   * @deprecated 4/2023 »Playback Vertical Slice« -- reworked into the RenderDrive //////////////////////////TICKET #1221
   */
  class CalcPlanContinuation
    : public JobClosure
    {
      
      play::Timings const& timings_;
      Dispatcher& dispatcher_;
      const ModelPort modelPort_;
      const uint channel_;
      
      
      /* === JobClosure Interface === */
      
      JobKind
      getJobKind()  const override
        {
          return META_JOB;
        }
      
      bool verify (Time, InvocationInstanceID)       const override;
      size_t hashOfInstance (InvocationInstanceID)   const override;
      InvocationInstanceID buildInstanceID (HashVal) const override;
      void invokeJobOperation (JobParameter)               override;
      void signalFailure (JobParameter, JobFailureReason)  override;
      
      
      
      
    public:
      /**
       * @todo
       */
      CalcPlanContinuation(play::Timings const& timings
                          ,Dispatcher& dispatcher
                          ,ModelPort modelPort
                          ,uint channel)
        : timings_(timings)
        , dispatcher_(dispatcher)
        , modelPort_(modelPort)
        , channel_(channel)
        { }
      
      /** create the "start trigger job"
       *  Scheduling this job will effectively get a calculation stream
       *  into active processing, since it causes the first chunk of job planning
       *  plus the automated scheduling of follow-up planning jobs. The relation
       *  to real (wall clock) time will be established when the returned job
       *  is actually invoked
       * @param startFrame where to begin rendering, relative to the nominal
       *        time grid implicitly given by the ModelPort to be pulled
       */
      Job prepareRenderPlanningFrom (FrameCnt startFrame);
      
      
    private:
      void performJobPlanningChunk(FrameCnt nextStartFrame);
      Job buildFollowUpJobFrom (TimeAnchor const& refPoint);
    };
  
  
  
}} // namespace steam::engine
#endif
