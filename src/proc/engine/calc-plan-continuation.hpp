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


#ifndef PROC_ENGINE_CALC_PLAN_CONTINUATION_H
#define PROC_ENGINE_CALC_PLAN_CONTINUATION_H

#include "proc/common.hpp"
#include "proc/mobject/model-port.hpp"
#include "proc/engine/time-anchor.hpp"
#include "proc/engine/frame-coord.hpp"
#include "proc/engine/job-ticket.hpp"
#include "proc/engine/dispatcher.hpp"
#include "proc/play/timings.hpp"
#include "backend/engine/job.h"
#include "lib/time/timevalue.hpp"

#include <boost/noncopyable.hpp>
//#include <tr1/functional>


namespace proc {
namespace engine {
  
//  using std::tr1::function;
  using mobject::ModelPort;
//  using lib::time::TimeSpan;
//  using lib::time::FSecs;
//  using lib::time::Time;
  
  
  /**
   * Special job to perform the job planning.
   * This closure extends the existing planning of frame jobs to add a chunk
   * of additional future jobs. Included with this chunk will be a recursive
   * self re-invocation to trigger planning of the next chunk. Overall, this
   * planning process is determined and controlled by the CalcStream owning
   * this closure.
   * 
   * @todo 5/13 still WIP -- write type comment
   */
  class CalcPlanContinuation
    : public JobClosure
    , boost::noncopyable
    {
      
      play::Timings const& timings_;
      Dispatcher& dispatcher_;
      const ModelPort modelPort_;
      const uint channel_;
      
      
      /* === JobClosure Interface === */
      
      JobKind
      getJobKind()  const
        {
          return META_JOB;
        }
      
      bool
      verify (Time nominalJobTime)  const
        {
          UNIMPLEMENTED ("verify the planning coordinates");
          return false;
        }
      
      void
      invokeJobOperation (JobParameter parameter)
        {
          ASSERT (parameter.nominalTime == timings_.getFrameStartAt (parameter.invoKey.frameNumber));
          
          this->performJobPlanningChunk (parameter.invoKey.frameNumber);
        }
      
      
      void
      signalFailure (JobParameter parameter)
        {
          UNIMPLEMENTED ("what needs to be done when a planning continuation cant be invoked?");
        }
      
      
      
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
       *        time grid implicitly related to the ModelPort to be pulled
       */
      Job
      prepareRenderPlanningFrom (int64_t startFrame)
        {
          InvocationInstanceID invoKey;
          invoKey.frameNumber = startFrame;
          Time nominalPlanningStartTime = timings_.getFrameStartAt (startFrame);
          
          return Job(*this, invoKey, nominalPlanningStartTime);
        }
      
      
    private:
      void
      performJobPlanningChunk(int64_t nextStartFrame)
        {
          TimeAnchor refPoint(timings_, nextStartFrame);
          JobPlanningSequence jobs = dispatcher_.onCalcStream(modelPort_, channel_)
                                                .establishNextJobs(refPoint);
          
          Job nextChunkOfPlanning = buildFollowUpJobFrom (refPoint);
          
          UNIMPLEMENTED ("the actual meat: access the scheduler and fed those jobs");
        }
      
      
      Job
      buildFollowUpJobFrom (TimeAnchor const& refPoint)
        {
          return this->prepareRenderPlanningFrom(
                         refPoint.getNextAnchorPoint());
        }
    };
  
  
  
}} // namespace proc::engine
#endif
