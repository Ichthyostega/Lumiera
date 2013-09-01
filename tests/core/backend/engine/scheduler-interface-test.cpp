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


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "proc/play/timings.hpp"
#include "lib/time/timevalue.hpp"
#include "backend/engine/job.h"
#include "backend/engine/scheduler-frontend.hpp"
#include "backend/engine/scheduler-diagnostics.hpp"


namespace backend {
namespace engine {
namespace test {
  
  using util::isSameObject;
  
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::Duration;
  using lib::time::FSecs;
  
  using backend::engine::JobClosure;
  using backend::engine::JobParameter;
  
  
  namespace { // test fixture: a dummy job operation...
    
    class DummyClosure
      : public JobClosure
      {
        void
        invokeJobOperation (JobParameter parameter)
          {
            UNIMPLEMENTED ("conjure a verifiable dummy job operation");
          }
        
        void
        signalFailure (JobParameter,JobFailureReason)
          {
            NOTREACHED ("Job failure is not subject of this test");
          }
        
        JobKind
        getJobKind()  const
          {
            return META_JOB;
          }
        
        bool
        verify (Time nominalJobTime)  const
          {
            UNIMPLEMENTED ("what the hell do we need to mock for this operation????");
          }
        
        size_t
        hashOfInstance(InvocationInstanceID invoKey) const
          {
            UNIMPLEMENTED ("how to interpret the invoKey to create a hash value");
          }
      };
    
    /** actual instance of the test dummy job operation */
    DummyClosure dummyClosure;
    
    
    /**
     * @todo this is a draft and shall be incorporated into the SchedulerFrontend
     * Definition context for jobs to be scheduled.
     * Allows to specify individual jobs, and to attach a transaction for prerequisite jobs
     */
    class JobTransaction
      {
        
      public:
        JobTransaction()
          {
            UNIMPLEMENTED ("suitable representation, link to the actual scheduler?");
          }
        
        // using default copy operations
        
        
        /** define a render job
         *  for time-bound calculation
         */
        void
        addJob (Time deadline, Job const& job)
          {
            UNIMPLEMENTED ("a mock implementation for adding a single job; change this later to talk to the real scheduler");
          }
        
        /** define a job for background rendering. */
        void
        addBackground (Job const& job)
          {
            UNIMPLEMENTED ("a mock implementation for adding a single background job; change this later to talk to the real scheduler");
          }
        
        /** define a render job
         *  to be calculated as soon as resources permit.
         *  Typically this call is used for rendering final results.
         */
        void
        addFreewheeling (Job const& job)
          {
            UNIMPLEMENTED ("a mock implementation for adding a single job for immediate calculation; change this later to talk to the real scheduler");
          }
        
        void
        attach (JobTransaction const& prerequisites)
          {
            UNIMPLEMENTED ("a mock implementation for adding a tree of prerequisite jobs; change this later to talk to the real scheduler");
          }
        
        JobTransaction startPrerequisiteTx();
        
      };
    
    JobTransaction
    JobTransaction::startPrerequisiteTx()
      {
        UNIMPLEMENTED ("how to start a nested job definition context");
      }
    
    
    TimeVar testStartTime(Time::ZERO);
    const Duration TEST_FRAME_DURATION(FSecs(1,2));
  
  } //(End) test fixture
  
  
  
  /***************************************************************************
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
      verify_simple_job_specification (SchedulerFronend& scheduler)
        {
          SchedulerDiagnostics monitor(scheduler);
          
          InvocationInstanceID invoKey;
          invoKey.frameNumber = 111;
          
          Job job(dummyClosure, invoKey, Time::ZERO);
          
          
          JobTransaction definitionContext;
          scheduler.startJobTransaction()
                   .addJob(job)
                   .commit();
          
          CHECK ( monitor.is_scheduled_timebound (job));
          CHECK (!monitor.is_scheduled_background (job));
          CHECK (!monitor.is_scheduled_freewheeling (job));
        }
      
      
      void
      verify_job_specification_variations (SchedulerFronend& scheduler)
        {
          SchedulerDiagnostics monitor(scheduler);
          
          InvocationInstanceID invoKey;
          invoKey.frameNumber = 111;
          
          Job job(dummyClosure, invoKey, Time::ZERO);
          
          JobTransaction tx = scheduler.startJobTransaction();
          
          tx.addFreewheeling(job);
          tx.addBackground (job);
          
          CHECK (!monitor.is_scheduled_timebound (job));
          CHECK (!monitor.is_scheduled_background (job));
          CHECK (!monitor.is_scheduled_freewheeling (job));
          
          tx.commit();
          
          CHECK (!monitor.is_scheduled_timebound (job));
          CHECK ( monitor.is_scheduled_background (job));
          CHECK ( monitor.is_scheduled_freewheeling (job));
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
      demonstrate_nested_job_specification (SchedulerFronend& scheduler)
        {
          SchedulerDiagnostics monitor(scheduler);
          
          JobTransaction startTx = scheduler.startJobTransaction();
          uint dummyLevel = 5;
          
          specifyJobs (startTx, dummyLevel);
          
          startTx.commit();
          
          for (uint i=0; i <=5; ++i)
            {
              Time nominalTime(dummyLevel*TEST_FRAME_DURATION); 
              Time deadline(testStartTime + i*TEST_FRAME_DURATION);
              
              CHECK (monitor.has_job_scheduled_at (deadline));
              CHECK (isSameObject (dummyClosure, monitor.job_at(deadline).jobClosure));
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
          InvocationInstanceID invoKey;
          invoKey.frameNumber = dummyLevel;
          
          Time nominalTime(dummyLevel*TEST_FRAME_DURATION); 
          Time deadline(testStartTime + dummyLevel*TEST_FRAME_DURATION);
          
          Job job(dummyClosure, invoKey, nominalTime);
          
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
  LAUNCHER(SchedulerInterface_test, "unit common");
  
}}} // namespace backend::engine::test
