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
 ** The "mechanics" of discovering and planning frame calculation jobs.
 ** This is a rather abstract chunk of code, to deal especially with the technicalities
 ** of \em organising the discovery of prerequisites and of joining all the discovered operations
 ** into a sequence of planning steps. The net result is to present a <i>sequence of job planing</i>
 ** to the user, while actually encapsulating a depth-first tree exploration, which proceeds on demand.
 ** 
 ** # participating elements
 ** All of these job planning operations are implemented on top of the JobTicket. This is where to look
 ** for "actual" implementation code. Here, within this header, the following entities cooperate to
 ** create a simple sequence out of this implementation level tasks:
 ** - JobPlanningSequence is the entry point for client code: it allows to generate a sequence of jobs
 ** - JobPlanning is a view on top of all the collected planning information for a single job
 ** - PlanningState is an iterator, successively exposing a sequence of JobPlanning views
 ** - steam::engine::expandPrerequisites(JobPlanning const&) is the operation to explore further prerequisite Jobs recursively
 ** - PlanningStepGenerator yields the underlying "master beat": a sequence of frame locations to be planned
 ** 
 ** # how the PlanningState (sequence) is advanced
 ** PlanningState is an iterator to expose a sequence of JobPlanning elements. On the implementation level,
 ** there is always just a single JobPlanning element, which represents the \em current element; this element
 ** lives as "state core" within the PlanningState object. Advancing to the next JobPlanning element (i.e. to
 ** consider the next job or prerequisite job to be planned for scheduling) is performed through the iteration
 ** control API exposed by JobPlanning (the functions `checkPoint()`, `yield()` and `iterNext()`. Actually,
 ** these functions are invoked through the depth-first tree exploration performed by JobPlaningSequence.
 ** The implementation of these invocations can be found within the IterExplorer strategy
 ** lib::iter_explorer::RecursiveSelfIntegration. The net result is
 ** - the current element is always accessed through `yield()`
 ** - advancing to the next element happens \em either
 **   
 **   - by invoking `iterNext()` (when processing a sequence of sibling job prerequisites)
 **   - by invoking `integrate()` (when starting to explore the next level of children) 
 ** 
 ** 
 ** @see DispatcherInterface_test simplified usage examples
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
#include "lib/time/timevalue.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/util.hpp"



namespace steam {
namespace engine {
  
  namespace error = lumiera::error;
  
  using lib::time::TimeValue;
  using util::unConst;
  using util::isnil;
  
  
  
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
   * 
   * @remarks on the implementation level, JobPlanning is used as "state core"
   *          for a PlanningState iterator, to visit and plan subsequently all
   *          the individual operations necessary to render a timeline chunk.
   */
  class JobPlanning
    {
      JobTicket::ExplorationState plannedOperations_;
      FrameCoord                 point_to_calculate_;
     
    public:
      /** by default create the bottom element of job planning,
       *  which happens to to plan no job at all. It is represented
       *  using an inactive state core (default constructed)
       */
      JobPlanning()
        { }
      
      /** further job planning can be initiated by continuing off a given previous planning state.
       *  This is how the forks are created, expanding into a multitude of prerequisites for
       *  the job in question.
       */
      JobPlanning (JobTicket::ExplorationState const& startingPoint, FrameCoord requestedFrame)
        : plannedOperations_(startingPoint)
        , point_to_calculate_(requestedFrame)
        { }
      
      // using the standard copy operations
      
      
      /** cast and explicate this job planning information
       *  to create a frame job descriptor, ready to be scheduled
       */
      operator Job()
        {
          if (isnil (plannedOperations_))
            throw error::Logic("Attempt to plan a frame-Job based on a missing, "
                               "unspecified, exhausted or superseded job description"
                              ,error::LUMIERA_ERROR_BOTTOM_VALUE);
          
          return plannedOperations_->createJobFor (point_to_calculate_);
        }
      
      
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
            return JobPlanning (plannedOperations_->discoverPrerequisites (point_to_calculate_.channelNr)
                               ,this->point_to_calculate_);
        }
      
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
    };
  
  
  
  
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
          return accessJobTicket (location.modelPort, location.absoluteNominalTime);
        }
      
      bool canContinue (FrameCoord const& location)
        {
          return not isEndOfChunk (location.absoluteFrameNumber,
                                   location.modelPort);
        }
      
    protected:
      virtual JobTicket& accessJobTicket (ModelPort, TimeValue nominalTime)  =0;
      virtual bool       isEndOfChunk    (FrameCnt, ModelPort port)          =0;
    };
  
  
  
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
  
  
  
}} // namespace steam::engine
#endif
