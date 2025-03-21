/*
  SCHEDULER-FRONTEND.hpp  -  access point to the scheduler within the renderengine

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file scheduler-frontend.hpp
 ** Scheduler service access point for higher layers.
 ** @todo WIP unfinished since 9/2013
 ** @warning as of 4/2023 Render-Engine integration work is underway ////////////////////////////////////////TICKET #1280
 ** @deprecated 8/23 obsoleted by rework for »Playback Vertical Slice«     //////////////////////////////////TICKET #1228
 ** @see activity-lang.hpp the emerging new interface
 ** 
 */


#ifndef VAULT_GEAR_SCHEDULER_FRONTEND_H
#define VAULT_GEAR_SCHEDULER_FRONTEND_H



#include "lib/depend.hpp"
#include "lib/time/timevalue.hpp"
#include "vault/gear/job.h"


namespace vault{
namespace gear {
  
  using lib::time::Time;
  
  
  /**
   * Access point to the scheduler service provided by the back-end.
   * Steam-Layer uses this service as the primary means of instructing
   * the vault; suitably prepared and wired frame render jobs are
   * handed over to the scheduler for time-bound or bandwidth-controlled
   * execution
   * 
   * @ingroup engine
   * @ingroup scheduler
   * @todo this is planned to become the frontend
   * to the render node network, which can be considered
   * at the lower end of the middle layer; the actual
   * render operations are mostly implemented by the vault layer
   * @todo define the low-level scheduler interface and hook in
   *       the necessary calls to implement this frontend.
   * ////////TODO WIP as of 9/2013
   */
  class SchedulerFrontend
    {
    public:
      /** access point to the Engine Interface.
       * @internal this is an facade interface for internal use
       *           by the player. Client code should use the Player.
       */
      static lib::Depend<SchedulerFrontend> instance;
      
      
      /**
       * Definition context for jobs to be scheduled.
       * This builder allows to specify individual jobs,
       * and to attach a transaction for prerequisite jobs.
       * When done, the #commit operation can be used
       * to activate all jobs defined this far.
       */
      class JobTransaction
        {
          SchedulerFrontend* sched_;
          
          
          JobTransaction (SchedulerFrontend* s)
            : sched_(s)
            { }
          
          friend class SchedulerFrontend;
          
          // using default copy operations
          
          
        public:
          /** finish this set of job definitions.
           *  All jobs attached to this transaction thus far,
           *  and all dependent transactions will be scheduled
           * @note transaction should not be used beyond this point;
           *       contents and data structures are cleared right away;
           */
          void
          commit()
            {
              UNIMPLEMENTED ("feed all the attached jobs and transactions to the scheduler");
            }
          
          /** define a render job
           *  for time-bound calculation */
          JobTransaction&
          addJob (Time deadline, Job const& job)
            {
              UNIMPLEMENTED ("a mock implementation for adding a single job; change this later to talk to the real scheduler");
              return *this;
            }
          
          /** define a job for background rendering. */
          JobTransaction&
          addBackground (Job const& job)
            {
              UNIMPLEMENTED ("a mock implementation for adding a single background job; change this later to talk to the real scheduler");
              return *this;
            }
          
          /** define a render job
           *  to be calculated as soon as resources permit.
           *  Typically this call is used for rendering final results.
           */
          JobTransaction&
          addFreewheeling (Job const& job)
            {
              UNIMPLEMENTED ("a mock implementation for adding a single job for immediate calculation; change this later to talk to the real scheduler");
              return *this;
            }
          
          /**
           * define a set of prerequisites of the current JobTransaction.
           * @param prerequisites a set of job definitions, which need to be executed
           *        successfully before any job of the current JobTransaction may be
           *        invoked.
           * @note prerequisites may be nested recursively, a prerequisite transaction
           *        might rely on further prerequisites
           */
          JobTransaction&
          attach (JobTransaction const& prerequisites)
            {
              UNIMPLEMENTED ("a mock implementation for adding a tree of prerequisite jobs; change this later to talk to the real scheduler");
              return *this;
            }
          
          JobTransaction
          startPrerequisiteTx()
            {
              return JobTransaction(sched_);
            }
          
        };
      
      
      JobTransaction
      startJobTransaction()
        {
          return JobTransaction(this);
        }
      
      
      ///// TODO: find out about further public operations
       
      
    protected:
      void activateTracing();
      void disableTracing(); ///< EX_FREE
      
      friend class SchedulerDiagnostics;
      
    private:
      
    };

}} // namespace vault::gear
#endif /*VAULT_GEAR_SCHEDULER_FRONTEND_H*/
