/*
  ACTIVITY-LANG.hpp  -  definition language framework for scheduler activities

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file activity-lang.hpp
 ** A language framework to define and interconnect scheduler activity verbs.
 ** This is an interface to build valid activity descriptors, connect them with
 ** other activities and provide parameter data. The resulting definition terms
 ** can be passed to the scheduler's messaging queue to cause the described
 ** activities to be performed in the context of the Lumiera render engine.
 ** 
 ** @see SchedulerActivity_test
 ** @see activity.hpp definition of verbs
 **
 */


#ifndef SRC_VAULT_GEAR_ACTIVITY_LANG_H_
#define SRC_VAULT_GEAR_ACTIVITY_LANG_H_


#include "vault/gear/activity.hpp"
#include "vault/gear/block-flow.hpp"
#include "vault/gear/activity-term.hpp"


namespace vault{
namespace gear {
  
  using BlockFlowAlloc = BlockFlow<blockFlow::RenderConfig>;
  
  
  
  /*************************************************************************************//**
   * Term builder and execution framework to perform chains of _scheduler Activities_.
   * These are the verbs of a low-level execution language for render jobs; individual
   * \ref Activity records are managed by the \ref BlockFlow allocation scheme and maintained
   * until expiration of their deadline. To enact a render job, a connection of Activities
   * will be suitably wired, and the entry point to start execution can be instructed
   * into the Scheduler.
   * 
   * The ActivityLang object provides an _construction and execution service_
   * - it provides a builder API to construct _terms_ of properly wired Activity records
   * - the activity::Term serves as a transient object for wiring and configuration and
   *   can be discarded after enqueuing the entry point of a chain.
   * - the static function ActivityLang::dispatchChain() provides the execution logic
   *   for _activating_ a chain of Activities successively.
   * - for real usage, this execution environment needs some functionality implemented
   *   in the scheduler, which — for the purpose of Activity activation — is abstracted
   *   as an *Execution Context* with the following operations
   *   ** λ-post : hand over a chain of Activities for (time bound) activation
   *   ** λ-work : signal start of media processing and then leave »management mode«
   *   ** λ-done : signal completion of media processing
   *   ** λ-tick : activate an internal heartbeat and scheduler maintenance hook 
   * 
   * @see Activity
   * @see SchedulerActivity_test
   */
  class ActivityLang
    {
      BlockFlowAlloc& mem_;
      
    public:
      explicit
      ActivityLang (BlockFlowAlloc& memManager)
        : mem_{memManager}
        { }
      
      // using default copy/assignment
      
      
      /**
       * Builder-API: initiate definition of render activities for a media calculation job.
       */
      activity::Term
      buildCalculationJob (Job job, Time start, Time deadline)
        {
          return setupActivityScheme (activity::Term::CALC_JOB, job, start, deadline);
        }
      
      /**
       * Builder-API: initiate definition of IO data loading activities.
       */
      activity::Term
      buildAsyncLoadJob (Job job, Time start, Time deadline)
        {
          return setupActivityScheme (activity::Term::LOAD_JOB, job, start, deadline);
        }
      
      /**
       * Builder-API: initiate definition of internal/planning job.
       */
      activity::Term
      buildMetaJob (Job job, Time start, Time deadline)
        {
          return setupActivityScheme (activity::Term::META_JOB, job, start, deadline);
        }
      
      
      /**
       * Execution Framework: dispatch performance of a chain of Activities.
       */
      template<class EXE>
      static activity::Proc
      dispatchChain (Activity* chain, EXE& executionCtx)
        {
          if (!chain) return activity::PASS;
          activity::Proc res = chain->dispatch (executionCtx.getSchedTime(), executionCtx);
          if (activity::PASS == res)
            res = activateChain (chain->next, executionCtx);
          else if (activity::SKIP == res)
            res = activity::PASS;
          return res;
        }
      
      /**
       * Execution Framework: successive activation of Activities in a chain.
       */
      template<class EXE>
      static activity::Proc
      activateChain (Activity* chain, EXE& executionCtx)
        {
          activity::Proc res{activity::PASS};
          while (chain and activity::PASS == res)
            {
              res = chain->activate (executionCtx.getSchedTime(), executionCtx);
              chain = chain->next;
            }
          if (activity::SKIP == res)// SKIP has been handled
            res = activity::PASS;  //  just by aborting the loop
          return res;
        }
      
    private:
      /** @internal generate the builder / configurator term */
      activity::Term
      setupActivityScheme (activity::Term::Template schemeKind, Job job, Time start, Time after)
        {
          return activity::Term{ mem_.until(after)
                               , schemeKind
                               , start
                               , after
                               , job
                               };
        }
    };
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_ACTIVITY_LANG_H_*/
