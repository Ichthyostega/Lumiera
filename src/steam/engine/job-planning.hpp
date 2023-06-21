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
#include "steam/engine/frame-coord.hpp"
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
      
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 :: to be refactored...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1301 : likely to become obsolete  
      /** build a new JobPlanning object,
       * set to explore the prerequisites
       * at the given planning situation
       */
      JobPlanning
      discoverPrerequisites()  const
        {
          if (isnil (plannedOperations_))
            return JobPlanning();
          else
            return JobPlanning (plannedOperations_->discoverPrerequisites (0)  //////////////////////////////TICKET #1301 : was: point_to_calculate_.channelNr)
                               ,this->point_to_calculate_);
        }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1301 : likely to become obsolete  
      
      /** integrate another chain of prerequisites into the current evaluation line.
       *  Further evaluation will start to visit prerequisites from the new starting point,
       *  and return to the current evaluation chain later on exhaustion of the side chain.
       *  Especially in case the current evaluation is empty or already exhausted, the
       *  new starting point effectively replaces the current evaluation point */
      friend void
      integrate (JobPlanning const& newStartingPoint, JobPlanning& existingPlan)
      {
        if (isnil (existingPlan.plannedOperations_))
          { // current evaluation is exhausted: switch to new starting point
            existingPlan.point_to_calculate_ = newStartingPoint.point_to_calculate_;
          }
        existingPlan.plannedOperations_.push (newStartingPoint.plannedOperations_);
        existingPlan.plannedOperations_.markTreeLocation();
      }
      
      
      /* === Iteration control API for IterStateWrapper== */
      
      bool
      checkPoint ()  const
        {
          return not isnil (plannedOperations_);
        }
      
      JobPlanning&
      yield ()  const
        {
          REQUIRE (checkPoint());
          return unConst(*this);
        }
      
      void
      iterNext ()
        {
          plannedOperations_.pullNext();
          plannedOperations_.markTreeLocation();
        }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 :: to be refactored...
    };
  
  
  
  
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 :: to be refactored...
  /**
   * iterator, exposing a sequence of JobPlanning elements
   */
  class PlanningState
    : public lib::IterStateWrapper<JobPlanning>
    {
      typedef lib::IterStateWrapper<JobPlanning> _Iter;
      
      
    public:
      /** inactive evaluation */
      PlanningState()
        : _Iter()
        { }
      
      explicit
      PlanningState (JobPlanning const& startingPoint)
        : _Iter(startingPoint)   // note: invoking copy ctor on state core
        { }
      
      // using the standard copy operations 
      
      
      
      
      /* === API for JobPlanningSequence to expand the tree of prerequisites === */
      
      /** attach and integrate the given planning details into this planning state.
       *  Actually the evaluation proceeds depth-first with the other state,
       *  returning to the current position later for further evaluation */
      PlanningState &
      wrapping (JobPlanning const& startingPoint)
        {
          integrate (startingPoint, this->stateCore());
          return *this;
        }
      
      PlanningState &
      usingSequence (PlanningState const& prerequisites)
        {
          if (isnil (prerequisites))
            return *this;
          else
            return this->wrapping(*prerequisites);
               //  explanation: PlanningState represents a sequence of successive planning points.
              //                actually this is implemented by switching an embedded JobPlanning element
             //                 through a sequence of states. Thus the initial state of an investigation
            //                  (which is a JobPlanning) can stand-in for the sequence of prerequisites
        }
      
      
      
      /** Extension point to be picked up by ADL.
       *  Provides access for the JobPlanningSequence
       *  for combining and expanding partial results.
       */
      friend PlanningState&
      build (PlanningState& attachmentPoint)
      {
        return attachmentPoint;
      }
    };
    
  
  
  
  /** this is the core operation to drive planning ahead:
   *  discover the prerequisites of some operation -- here
   *  "prerequisites" are those operations to be performed
   *  within separate Jobs beforehand.
   * @note this function is intended to be flat-mapped (">>=")
   *       onto a tree-like monad representing the evaluation process.
   */
  inline PlanningState
  expandPrerequisites (JobPlanning const& calculationStep)
  {
    PlanningState newSubEvaluation(
                    calculationStep.discoverPrerequisites());
    return newSubEvaluation;
  }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 :: to be refactored...
  
  
  
  
  
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1301 : obsoleted by rework of Dispatcher-Pipeline  
  /**
   * Abstraction: a Facility to establish frame coordinates
   * and identify and access the execution plan for this frame.
   * @see Dispatcher the service interface actually used
   */
  class FrameLocator
    : public FrameSequencer
    {
    public:
      
      JobTicket&
      getJobTicketFor (FrameCoord const& location)
        {
          return accessJobTicket (location.modelPortIDX, location.absoluteNominalTime);
        }
      
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1301 : likely to become obsolete  
      bool canContinue (FrameCoord const& location)
        {
//        return not isEndOfChunk (location.absoluteFrameNumber,
//                                 location.modelPort);
        }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1301 : likely to become obsolete  
      
    protected:
      virtual JobTicket& accessJobTicket (size_t, TimeValue nominalTime)  =0;
      virtual bool       isEndOfChunk    (FrameCnt, ModelPort port)       =0;
    };
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1301 : obsoleted by rework of Dispatcher-Pipeline  
  
  
  
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 :: to be refactored...
  /**
   * Generate a sequence of starting points for Job planning,
   * based on the underlying frame grid. This sequence will be
   * used to seed a JobPlanningSequence for generating a chunk
   * of frame render jobs within a given CalcStream in the player.
   * Evaluation of that seed will then expand each starting point,
   * until all prerequisites for those frames are discovered,
   * resulting in a sequence of Jobs ready to be handed over
   * to the scheduler for time-bound activation.
   */
  class PlanningStepGenerator
    {
      FrameLocator* locationGenerator_;
      FrameCoord    currentLocation_;
      
      //////////////////////////////////////////TODO duplicated storage of a FrameCoord record
      //////////////////////////////////////////TODO nextEvaluation_ is only needed to initialise the "current" sequence 
      //////////////////////////////////////////TODO within the RecursiveSelfIntegration strategy. Maybe this storage could be collapsed?
      JobPlanning nextEvaluation_;
      
      void
      use_current_location_as_starting_point_for_planning()
        {
          JobTicket& processingPlan = locationGenerator_->getJobTicketFor (currentLocation_);
          
          nextEvaluation_ = JobPlanning(processingPlan.startExploration()
                                       ,currentLocation_);
        }
      
      
      
    public:
      typedef JobPlanning value_type;
      typedef JobPlanning& reference;
      typedef JobPlanning *  pointer;
      
      
      PlanningStepGenerator(FrameCoord startPoint, FrameLocator& locator)
        : locationGenerator_(&locator)
        , currentLocation_(startPoint)
        { 
          REQUIRE (startPoint.isDefined());
          use_current_location_as_starting_point_for_planning();
        }
      
      // default copyable
      
      
      /* === Iteration control API for IterStateWrapper== */
      
      bool
      checkPoint ()  const
        {
          return currentLocation_.isDefined();
        }     // might indicate end of this planning chunk (or of playback altogether)
      
      
      JobPlanning&
      yield ()  const
        {
          ENSURE (checkPoint());
          return unConst(this)->nextEvaluation_;
        }
      
      
      void
      iterNext ()
        {
          if (locationGenerator_->canContinue (currentLocation_))
            {
              currentLocation_ = locationGenerator_->getNextFrame (currentLocation_);
              this->use_current_location_as_starting_point_for_planning();
              ENSURE (this->checkPoint());
            }
          else 
            { // indicate end-of playback or a jump to another playback position
              currentLocation_ = FrameCoord();
            }
        }
    };
  
  
  
  /* type definitions for building the JobPlaningSequence */
  
  typedef PlanningState (*SIG_expandPrerequisites) (JobPlanning const&);
  
  typedef lib::IterExplorer<PlanningStepGenerator
                           ,lib::iter_explorer::RecursiveSelfIntegration>      JobPlanningChunkStartPoint;
  typedef JobPlanningChunkStartPoint::FlatMapped<SIG_expandPrerequisites>::Type  ExpandedPlanningSequence;
  
  
  
  /**
   * This iterator represents a pipeline to pull planned jobs from.
   * To dispatch individual frame jobs for rendering, this pipeline is generated
   * and wired internally such as to interpret the render node definitions.
   * 
   * \par Explanation of the structure
   * 
   * The JobPlanningSequence is constructed from several nested layers of functionality
   * - for the client, it is an iterator, exposing a sequence of JobPlanning elements
   * - a JobPlanning element allows to add a frame render job to the scheduler
   * - actually such an element can even be \em converted directly into a Job (descriptor)
   * - the sequence of such JobPlanning elements (that is, the iterator) is called a PlanningState,
   *   since evaluating this iterator effectively drives the process of job planning ahead
   * - this planning process is \em implemented as a recursive evaluation and exploration of
   *   a tree of prerequisites; these prerequisites are defined in the JobTicket datastructure
   * - there is an underlying grid of evaluation starting points, each corresponding to a
   *   single frame. Typically, each frame generates at least two jobs, one for fetching
   *   data, and one for the actual calculations. Depending on the actual render network,
   *   a lot of additional jobs might be necessary
   * - this basic frame grid is generated by the PlanningStepGenerator, which is
   *   effectively backed by the Dispatcher and thus the render node model. 
   * 
   * @remarks JobPlanningSequence is a monad, and the operation to explore the prerequisites
   *          is applied by the \c >>= (monad flat map operation). This approach allows us
   *          to separate the technicalities of exhausting tree exploration from the actual
   *          "business code" to deal with frame job dependencies
   */
  class JobPlanningSequence
    : public ExpandedPlanningSequence
    {
      
    public:
      JobPlanningSequence(engine::FrameCoord startPoint, FrameLocator& locator)
        : ExpandedPlanningSequence(
            JobPlanningChunkStartPoint(
                PlanningStepGenerator(startPoint,locator))
            
            >>= expandPrerequisites)                    // "flat map" (monad operation)
        { }
    };
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1276 :: to be refactored...
  
  
  
}} // namespace steam::engine
#endif
