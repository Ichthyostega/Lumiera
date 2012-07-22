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


#ifndef PROC_ENGINE_JOB_PLANNING_H
#define PROC_ENGINE_JOB_PLANNING_H

#include "proc/common.hpp"
//#include "proc/state.hpp"
#include "proc/engine/job.hpp"
#include "proc/engine/job-ticket.hpp"
//#include "proc/engine/frame-coord.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/time/timequant.hpp"
//#include "lib/meta/function.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/iter-explorer.hpp"
//#include "lib/linked-elements.hpp"
#include "lib/util.hpp"

//#include <boost/noncopyable.hpp>


namespace proc {
namespace engine {
  
  namespace error = lumiera::error;
  
//using lib::time::TimeSpan;
//using lib::time::Duration;
//using lib::time::FSecs;
//using lib::time::Time;
//using lib::LinkedElements;
  using util::isnil;
  using util::unConst;
//  
//class ExitNode;
  

  /** 
   */ 
  class JobPlanning
    {
      JobTicket::iterator plannedOperations_;
      FrameCoord point_to_calculate_;
     
    public:
      /** by default create the bottom element of job planning,
       *  which happens to to plan no job at all. It is represented
       *  using an inactive state core (default constructed)
       */
      JobPlanning()
        { }
      
      /** further job planning can be initiated by continuing
       * off a given previous planning state. This is how
       * the forks are created, expanding into a multitude
       * of prerequisites of a given job
       */
      JobPlanning (JobTicket::iterator const& startingPoint, FrameCoord requestedFrame)
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
            return JobPlanning (plannedOperations_->discoverPrerequisites()
                               ,this->point_to_calculate_);
        }
      
      friend void
      integrate (JobPlanning const& newStartingPoint, JobPlanning existingPlan)
      {
        existingPlan.plannedOperations_.push (newStartingPoint.plannedOperations_);
      }
      
      
      /* === Iteration control API for IterStateWrapper== */
      
      friend bool
      checkPoint (JobPlanning const& plan)
      {
        return !isnil (plan.plannedOperations_);
      }
      
      friend JobPlanning&
      yield (JobPlanning const& plan)
      {
        REQUIRE (checkPoint (plan));
        return unConst(plan);
      }
      
      friend void
      iterNext (JobPlanning & plan)
      {
        ++(plan.plannedOperations_);
      }      
    };
  
  
  
  
    
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
       *  returning later on to the current position for further evaluation */
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
             //                 through a sequence of states. Thus the initial state of the investigation
            //                  (which is a JobPlanning) can stand-in for the sequence of prerequisites
        }
      
      ///TODO what elements are accepted by the explorationStack? provide conversion operator to extract those from stateCore()
      
      
      
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
    
  
  
  inline PlanningState
  expandPrerequisites (PlanningState const& calculationStep)
  {
    PlanningState newSubEvaluation(
                    calculationStep->discoverPrerequisites());
    return newSubEvaluation;
  }
  
  

    
    
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
    public:
      typedef JobPlanning value_type;
      typedef JobPlanning& reference;
      typedef JobPlanning *  pointer;
      
      
      /* === Iteration control API for IterStateWrapper== */
      
      friend bool
      checkPoint (PlanningStepGenerator const& gen)
      {
        UNIMPLEMENTED ("determine planing chunk size"); /// return bool(seq.feed());
      }
      
      friend JobPlanning&
      yield (PlanningStepGenerator const& gen)
      {
        UNIMPLEMENTED ("generate a single frame job planning tree");  ///return *(seq.feed());
      }
      
      friend void
      iterNext (PlanningStepGenerator & gen)
      {
        UNIMPLEMENTED ("proceed to next frame");  ///////seq.iterate();
      }      
    };
  
  
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #827
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #827
  
  
  
  /**
   * @todo 6/12 WIP-WIP-WIP how to prepare jobs for scheduling
   */
  class JobPlanningSequence
    : public lib::IterExplorer<PlanningStepGenerator
                              ,lib::iter_explorer::RecursiveSelfIntegration>
    {
      
    public:
      
    };
  
  
  
}} // namespace proc::engine
#endif
