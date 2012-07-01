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
  
//using lib::time::TimeSpan;
//using lib::time::Duration;
//using lib::time::FSecs;
//using lib::time::Time;
//using lib::LinkedElements;
  using util::isnil;
//  
//class ExitNode;
  
  class JobPlanning;

#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #827
  class Evaluation
    {
      JobTicket::ExplorationStack focusPoint_;
      FUN& explore_;
      
    public:
      /** inactive evaluation */
      Evaluation()
        {
          UNIMPLEMENTED ("how to represent an inactive evaluation");
        }
      
      
      JobPlanning &
      getFeed ()
        {
          while (focusPoint_ && focusPoint_.currentLevelExhausted())
            focusPoint_.pop();
            
          return dressUp(this); /////////////TODO: bad smell
        }
      
      void
      recurse ()
        {
          focusPoint_.visitPrerequisites();
        }

      
      /* === Iteration control API for IterStateWrapper== */
      
      friend bool
      checkPoint (Evaluation const& eval)
      {
        return bool(seq.feed());
      }
      
      friend JobPlanning&
      yield (Evaluation const& eval)
      {
        return *(seq.feed());
      }
      
      friend void
      iterNext (Evaluation & eval)
      {
        seq.iterate();
      }      
    };
  
  
  /** 
   */ 
  class JobPlanning
    : public lib::IterStateWrapper<JobPlanning, Evaluation>
    {
      typedef lib::IterStateWrapper<JobPlanning, Evaluation> _Iter;
      
    public:
      /** by default create the bottom element of job planning,
       *  which happens to to plan no job at all. It is represented
       *  using an inactive state core (default constructed)
       */
      JobPlanning()
        {
          UNIMPLEMENTED ("create the bottom job planning");
        }
      
      /** further job planning can be initiated by continuing
       * off a given previous planning state. This is how
       * the forks are created, expanding into a multitude
       * of prerequisites of a given job
       */
      JobPlanning (Evaluation const& startingPoint)
        : _Iter(startingPoint)
        { }
      
//      operator Evaluation&()
//        {
//          return stateCore();
//        }
      
      operator Job()
        {
          UNIMPLEMENTED ("yield a Job descriptor to reflect this planning information");
        }
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
      /* === Iteration control API for IterStateWrapper== */
      
      friend bool
      checkPoint (PlanningStepGenerator const& gen)
      {
        return bool(seq.feed());
      }
      
      friend JobPlanning&
      yield (PlanningStepGenerator const& gen)
      {
        return *(seq.feed());
      }
      
      friend void
      iterNext (PlanningStepGenerator & gen)
      {
        seq.iterate();
      }      
    };
  
  
  inline JobPlanning
  explorePrerequisites(JobPlanning step)
  {
    Evaluation planningState_(step);
    return planningState_.explorePrerequisites();
  }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #827
  
  
  
  /**
   * @todo 6/12 WIP-WIP-WIP how to prepare jobs for scheduling
   */
  class JobPlanningSequence
    {
      
    public:
      
    };
  
  
  
}} // namespace proc::engine
#endif
