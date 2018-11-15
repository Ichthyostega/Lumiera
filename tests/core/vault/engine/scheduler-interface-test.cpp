/*
  SchedulerInterface(Test)  -  verify invocation structure of the scheduler interface
  
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

* *****************************************************/

/** @file scheduler-interface-test.cpp
 ** unit test \ref SchedulerInterface_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "vault/real-clock.hpp"
#include "vault/engine/scheduler-frontend.hpp"
#include "vault/engine/scheduler-diagnostics.hpp"
#include "vault/engine/dummy-job.hpp"


namespace vault {
namespace engine {
namespace test {
  
  using util::isSameObject;
  
  using lib::time::Duration;
  using lib::time::Offset;
  using lib::time::FSecs;
  
  
  namespace { // test fixture: scheduling a dummy job operation...
    
    
    Time TEST_START_TIME (vault::RealClock::now());
    const Duration TEST_FRAME_DURATION(FSecs(1,2));
    
    inline Offset
    dummyFrameStart (uint frameNr)
    {
      return frameNr * TEST_FRAME_DURATION;
    }
  
  } //(End) test fixture
  
  
  
  
  
  typedef SchedulerFrontend::JobTransaction JobTransaction;
  
  
  /***********************************************************************//**
   * @test verify and demonstrate the organisation of the high-level interface
   *       for defining jobs to be invoked by the scheduler.
   *       
   * @see SchedulerFrongend
   * @see DispatcherInterface_test
   */
  class SchedulerInterface_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          SchedulerFrontend& scheduler = SchedulerFrontend::instance();
          
          verify_simple_job_specification (scheduler);
          verify_job_specification_variations (scheduler);
          demonstrate_nested_job_specification (scheduler);
        }
      
      
      void
      verify_simple_job_specification (SchedulerFrontend& scheduler)
        {
          SchedulerDiagnostics monitor(scheduler);
          
          Job job = DummyJob::build();
          Time deadline(TEST_START_TIME);
          
          scheduler.startJobTransaction()
                   .addJob(deadline, job)
                   .commit();
          
          CHECK ( monitor.is_scheduled_timebound (job));
          CHECK (!monitor.is_scheduled_background (job));
          CHECK (!monitor.is_scheduled_freewheeling (job));
        }
      
      
      void
      verify_job_specification_variations (SchedulerFrontend& scheduler)
        {
          SchedulerDiagnostics monitor(scheduler);
          
          JobTransaction tx = scheduler.startJobTransaction();
          
          Job job1 = DummyJob::build();
          Job job2 = DummyJob::build();
          
          tx.addFreewheeling(job1);
          tx.addBackground (job2);
          
          CHECK (!monitor.is_scheduled_timebound (job1));
          CHECK (!monitor.is_scheduled_timebound (job2));
          CHECK (!monitor.is_scheduled_background (job1));
          CHECK (!monitor.is_scheduled_background (job2));
          CHECK (!monitor.is_scheduled_freewheeling (job1));
          CHECK (!monitor.is_scheduled_freewheeling (job2));
          
          tx.commit();
          
          CHECK (!monitor.is_scheduled_timebound (job1));
          CHECK (!monitor.is_scheduled_timebound (job2));
          
          CHECK ( monitor.is_scheduled_background (job1));
          CHECK ( monitor.is_scheduled_freewheeling (job2));
        }
      
      
      /** @test demonstrate how a tree of dependent render jobs
       * can be handed over to the scheduler within a single "transaction"
       * 
       * @remarks in the real usage situation, the definition of jobs will be
       *      driven by the exploration of a tree-like structure (the JobTicket).
       *      For the purpose of this interface demonstration test this recursive
       *      invocation structure is just emulated by a simple tail recursion.
       * @see HierarchyOrientationIndicator_test#demonstrate_tree_rebuilding
       */
      void
      demonstrate_nested_job_specification (SchedulerFrontend& scheduler)
        {
          SchedulerDiagnostics monitor(scheduler);
          
          JobTransaction startTx = scheduler.startJobTransaction();
          
          uint dummyLevel = 5;
          specifyJobs (startTx, dummyLevel);
          
          startTx.commit();
          
          for (uint i=0; i <=5; ++i)
            {
              Time nominalTime(dummyFrameStart (i));
              Time deadline(TEST_START_TIME + nominalTime);
              
              CHECK (monitor.has_job_scheduled_at (deadline));
              CHECK (nominalTime == monitor.job_at(deadline).parameter.nominalTime);
            }
        }
      
      /** recursive helper function to add several levels of prerequisites
       *  It is crucial for this function to be recursive: this allows us to represent
       *  a complete tree navigation as a sequence of job definitions to be "pulled"
       *  out from some opaque source
       */
      static void
      specifyJobs (JobTransaction& currentTx, uint dummyLevel)
        {
          uint frameNr = dummyLevel;
          Time nominalTime(dummyFrameStart(frameNr));
          Time deadline(TEST_START_TIME + nominalTime);
          
          Job job = DummyJob::build(nominalTime, frameNr);
          
          currentTx.addJob (deadline, job);
          
          if (0 < dummyLevel)
            {
              JobTransaction dependentTx = currentTx.startPrerequisiteTx();
              specifyJobs (dependentTx, dummyLevel-1);
              currentTx.attach (dependentTx);
            }
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER(SchedulerInterface_test, "unit engine");
  
}}} // namespace vault::engine::test
