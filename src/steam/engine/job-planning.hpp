/*
  JOB-PLANNING.hpp  -  steps to prepare and build render jobs

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

/** @file job-planning.hpp
 ** Aggregation of planning data to generate actual frame calculation jobs.
 ** These render jobs are generated periodically by an ongoing process while rendering is underway.
 ** For this purpose, each CalcStream of the play/render process operates a RenderDrive with a
 ** _job-planning pipeline_, rooted at the »master beat« as defined by the frame grid from the
 ** Timings spec of the current render process. This pipeline will assemble the specifications
 ** for the render jobs and thereby possibly discover prerequisites, which must be calculated first.
 ** From a usage point of view, the _job-planning pipeline_ is an _iterator:_ for each independent
 ** calculation step a new JobPlanning record appears at the output side of the pipeline, holding
 ** all collected data, sufficient to generate the actual job definition, which can then be
 ** handed over to the Scheduler.
 ** 
 ** # Implementation of the Job-Planning pipeline
 ** 
 ** JobPlanning acts as _working data aggregator_ within the Job-Planning pipeline; for this reason
 ** all data fields are references, and the optimiser is expected to elide them, since after template
 ** instantiation, JobPlanning becomes part of the overall assembled pipeline object, stacked on top
 ** of the Dispatcher::PipeFrameTick, which holds and increments the current frame number. The
 ** underlying play::Timings will provide a _frame grid_ to translate these frame numbers into
 ** the _nominal time values_ used throughout the rest of the render calculations.
 ** 
 ** There is one tricky detail to note regarding the handling of calculation prerequisites. The
 ** typical example would be the loading and decoding of media data, which is an IO-bound task and
 ** must be complete before the main frame job can be started. Since the Job-Planning pipeline is
 ** generic, this kind of detail dependency is modelled as _prerequisite JobTicket,_ leading to
 ** an possibly extended depth-first tree expansion, starting from the »master frame ticket« at
 ** the root. This _tree exploration_ is implemented by the TreeExplorer::Expander building block,
 ** which obviously has to maintain a stack of expanded child dependencies. This leads to the
 ** observation, that at any point of this dependency processing, for the complete path from the
 ** child prerequisite up to the root tick there is a sequence of JobPlanning instances placed
 ** into this stack in the Explorer object (each level in this stack is actually an iterator
 ** and handles one level of child prerequisites). The deadline calculation directly exploits
 ** this known arrangement, insofar each JobPlanning has a pointer to its parent (sitting in
 ** the stack level above). See the [IterExplorer unit test](\ref lib::IterTreeExplorer_test::verify_expandOperation)
 ** to understand this recursive on-demand processing in greater detail.
 ** 
 ** @see JobPlanning_test
 ** @see JobTicket
 ** @see Dispatcher
 ** @see EngineService
 **
 */


#ifndef STEAM_ENGINE_JOB_PLANNING_H
#define STEAM_ENGINE_JOB_PLANNING_H

#include "steam/common.hpp"
#include "vault/engine/job.h"
#include "steam/engine/job-ticket.hpp"
#include "steam/play/output-slot.hpp"
#include "steam/play/timings.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/itertools.hpp"
#include "lib/nocopy.hpp"



namespace steam {
namespace engine {
  
  namespace error = lumiera::error;

  using play::DataSink;
  using play::Timings;
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::Duration;
  
  
  
  /**
   * View on the execution planning for a single calculation step.
   * When this view-frontend becomes accessible, behind the scenes all
   * the necessary information has been pulled and collected from the
   * low-level model and the relevant rendering/playback configuration.
   * Typically, clients will materialise this planning into a Job (descriptor)
   * ready to be entered into the scheduler.
   * 
   * JobPlanning is indeed a view; the represented planning information is not
   * persisted (other then in the job to be created). The implementation draws
   * on a recursive exploration of the corresponding JobTicket, which acts as
   * a general blueprint for creating jobs within this segment of the timeline.
   */
  class JobPlanning
    : util::MoveOnly
    {
      JobTicket&      jobTicket_;
      TimeVar  const& nominalTime_;
      FrameCnt const& frameNr_;

    public:
      JobPlanning (JobTicket& ticket, TimeVar const& nominalTime, FrameCnt const& frameNr)
        : jobTicket_{ticket}
        , nominalTime_{nominalTime}
        , frameNr_{frameNr}
        { }
      
      // move construction is possible
      
      
      JobTicket& ticket()     { return jobTicket_; }
      bool isTopLevel() const { return not dependentPlan_; }
      
      
      /**
       * Connect and complete the planning information assembled thus far
       * to create a frame job descriptor, ready to be scheduled.
       */
      Job
      buildJob()
        {
          Job job = jobTicket_.createJobFor (Time{nominalTime_});
                                                       //////////////////////////////////////////////////////TICKET #1295 : somehow package and communicate the DataSink info
          return job;
        }
      
      /**
       * Calculate the latest time point when to _start_ the job,
       * so it can still possibly reach the timing goal.
       * @return time point in wall-clock-time, or Time::ANYTIME if unconstrained
       */
      Time
      determineDeadline(Timings const& timings)
        {
          switch (timings.playbackUrgency)
            {
            case play::ASAP:
            case play::NICE:
              return Time::ANYTIME;
            
            case play::TIMEBOUND:
              return doCalcDeadline (timings);
            }
          NOTREACHED ("unexpected playbackUrgency");
        }
      
      /**
       * Determine a timing buffer for flexibility to allow starting the job
       * already before its deadline; especially for real-time playback this leeway
       * is rather limited, and constrained by the earliest time the target buffer
       * is already allotted and ready to receive data.
       * @return tolerance duration
       *         - Duration::NIL if deadline has to be matched with maximum precision
       *         - Duration::MAX for unlimited leeway to start anytime before the deadline
       */
      Duration
      determineLeeway(Timings const&)
        {
          UNIMPLEMENTED ("Job planning logic to establish Leeway for scheduling");
        }
      
      
      /**
       * Build a sequence of dependent JobPlanning scopes for all prerequisites
       * of this current JobPlanning, and internally linked back to `*this`
       * @return an iterator which explores the prerequisites of the JobTicket.
       * @remark typical example would be to load data from file, or to require
       *         the results from some other extended media calculation.
       * @see Dispatcher::PipelineBuilder::expandPrerequisites()
       */
      auto
      buildDependencyPlanning()
        {
          return lib::transformIterator (jobTicket_.getPrerequisites()
                                        ,[this](JobTicket& prereqTicket)
                                                {
                                                  return JobPlanning{*this, prereqTicket};
                                                });
        }

    private:
      /** link to a dependent JobPlanning, for planning of prerequisites */
      JobPlanning* dependentPlan_{nullptr};
      
      /**
       * @internal construct a chained prerequisite JobPlanning,
       *  attached to the dependent »parent« JobPlanning, using the same
       *  frame data, but chaining up the deadlines, so that a Job created
       *  from this JobPlanning needs to be completed before the »parent«
       *  Job (which uses the generated data) can start
       * @see #buildDependencyPlanning()
       * @see JobPlanning_test::setupDependentJob()
       */
      JobPlanning (JobPlanning& parent, JobTicket& prerequisite)
        : jobTicket_{prerequisite}
        , nominalTime_{parent.nominalTime_}
        , frameNr_{parent.frameNr_}
        , dependentPlan_{&parent}
        { }
      
      
      Time
      doCalcDeadline(Timings const& timings)
        {
          if (isTopLevel())
            return timings.getTimeDue(frameNr_)                  // anchor at timing grid
                 - jobTicket_.getExpectedRuntime()               // deduce the presumably runtime
                 - timings.engineLatency                         // and the generic engine overhead
                 - timings.outputLatency;                        // Note: output latency only on top-level job
          else
            return dependentPlan_->determineDeadline (timings)   ////////////////////////////////////////////TICKET #1310 : WARNING - quadratic in the depth of the dependency chain
                 - jobTicket_.getExpectedRuntime()
                 - timings.engineLatency;
        }
    };
  
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_JOB_PLANNING_H*/
