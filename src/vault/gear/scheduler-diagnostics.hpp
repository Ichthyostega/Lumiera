/*
  SCHEDULER-DIAGNOSTICS.hpp  -  diagnostic facility to investigate scheduler operation

   Copyright (C)
     2013,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file scheduler-diagnostics.hpp
 ** An facility to check and monitor the internal workings of the scheduler.
 ** Once created, an SchedulerDiagnostics object connects to the scheduler implementation
 ** through the SchedulerFrontend interface to activate additional diagnostic facilities.
 ** This allows to verify the operation of the scheduler from within unit-tests;
 ** typically doing so incurs a performance overhead. 
 ** 
 ** @deprecated 8/23 obsoleted by rework for »Playback Vertical Slice«     //////////////////////////////////TICKET #1228
 ** @see SchedulerFrontend
 ** @see scheduler-interface-test.cpp
 ** @see EngineServiceMock
 */


#ifndef VAULT_GEAR_SCHEDULER_DIAGNOSTICS_H
#define VAULT_GEAR_SCHEDULER_DIAGNOSTICS_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/hash-value.h"
#include "lib/time/timevalue.hpp"
#include "vault/gear/scheduler-frontend.hpp"


namespace vault{
namespace gear {

//    using std::string;
  using lib::time::Time;
  using lib::HashVal;
  
  
  
  
  /***************************************************************//**
   * Render engine diagnostic facility. Creating an instance
   * will activate additional tracing and diagnostic facilities
   * within the scheduler implementation; results may be investigated
   * through SchedulerDiagnostics public functions.
   * The object acts like a smart handle, i.e. the tracing facilities
   * will be disabled and disconnected when going out of scope.
   * @warning not reentrant, no reference-counting.
   *          At any given time, at most a single instance
   *          of SchedulerDiagnostics may be used.  
   */
  class SchedulerDiagnostics
    : util::NonCopyable
    {
      SchedulerFrontend& scheduler_;
      
    public:
      SchedulerDiagnostics (SchedulerFrontend& sch)
        : scheduler_(sch)
        {
          scheduler_.activateTracing();
        }
      
     ~SchedulerDiagnostics()
        {
          scheduler_.disableTracing();
        }
      
      /** */
     
      bool
      is_scheduled_timebound (HashVal jobID)
        {
          UNIMPLEMENTED ("query the scheduler to determine if the given job is planned for time-bound operation");
        }
      
      bool
      is_scheduled_freewheeling (HashVal jobID)
        {
          UNIMPLEMENTED ("query the scheduler to determine if the given job is planned for freewheeling operation");
        }
     
      bool
      is_scheduled_background (HashVal jobID)
        {
          UNIMPLEMENTED ("query the scheduler to determine if the given job is planned for background execution");
        }
      
      bool
      is_scheduled_timebound (Job const& job)
        {
          return is_scheduled_timebound (hash_value (job));
        }
     
      bool
      is_scheduled_freewheeling (Job const& job)
        {
          return is_scheduled_freewheeling (hash_value (job));
        }
      
      bool
      is_scheduled_background (Job const& job)
        {
          return is_scheduled_background (hash_value (job));
        }
      
      bool
      has_job_scheduled_at (Time deadline)
        {
          ///////////////TODO this would be a classical use case for some kind of Maybe monad.
          ///////////////TODO I am considering to add such a helper since quite some time, maybe really do it now??
          
          UNIMPLEMENTED ("query for job scheduled for specific deadline");
        }
      
      Job const&
      job_at (Time deadline)
        {
          UNIMPLEMENTED ("query for job scheduled for specific deadline");
        }
    };
  
  
  
  
  
  
}} // namespace vault::gear
#endif /*VAULT_GEAR_SCHEDULER_DIAGNOSTICS_H*/
