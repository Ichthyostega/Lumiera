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
#include "proc/engine/job.hpp"
#include "proc/play/timings.hpp"
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
      
      Dispatcher* dispatcher_;
      TimeAnchor refPoint_;
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
          UNIMPLEMENTED ("representation of the job functor, especially the invocation instance ID for this planning chunk");
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
                          ,int64_t startFrame
                          ,uint channel)
        : dispatcher_(&dispatcher)
        , refPoint_(TODO)
        , modelPort_(modelPort)
        , channel_(channel)
        {
          UNIMPLEMENTED ("how to set up the initial playback start point"); 
        }
      
      Job
      initiateRenderPlanning ()
        {
          UNIMPLEMENTED ("setup of the initial planning job");
        }
      
      
    private:
      void
      performJobPlanningChunk()
        {
          JobPlanningSequence jobs = dispatcher_->onCalcStream(modelPort_, channel_)
                                                 .establishNextJobs(refPoint_);
          
          UNIMPLEMENTED ("the actual meat: access the scheduler and fed those jobs");
        }
      
      Job
      buildFollowUpJobFromThis()
        {
          refPoint_.setNextAnchorPoint();
          
          UNIMPLEMENTED ("create the follow-up job, wired with this closure");
        }
    };
  
  
  
}} // namespace proc::engine
#endif
