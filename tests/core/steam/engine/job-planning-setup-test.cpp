/*
  JobPlanningSetup(Test)  -  structure and setup of the job-planning pipeline

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

* *****************************************************/

/** @file job-planning-setup-test.cpp
 ** unit test \ref CalcStream_test
 */


#include "lib/test/run.hpp"
#include "lib/error.hpp"
#include "steam/engine/mock-dispatcher.hpp"
#include "vault/engine/dummy-job.hpp"

#include "lib/format-cout.hpp"///////////////////////TODO

//#include "steam/engine/job-planning.hpp"

//#include <ctime>

using test::Test;
//using std::rand;


namespace steam {
namespace engine{
namespace test  {
  
  using vault::engine::DummyJob;

  namespace { // test fixture...
    
  } // (End) test fixture
  
  
  
  /****************************************************************************//**
   * @test demonstrate interface, structure and setup of the job-planning pipeline.
   *       - using a frame step as base tick
   *       - invoke the dispatcher to retrieve the top-level JobTicket
   *       - expander function to explore prerequisite JobTickets
   *       - integration: generate a complete sequence of (dummy)Jobs
   *       - scaffolding and mocking used for this test 
   * 
   * @todo WIP-WIP-WIP 4/2023
   * 
   * @see DispatcherInterface_test
   * @see Dispatcher
   * @see CalcStream
   * @see RenderDriveS
   */
  class JobPlanningSetup_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          demonstrateScaffolding();
          UNIMPLEMENTED ("shape the interface of the job-planning pipeline");
          buildBaseTickGenerator();
          accessTopLevelJobTicket();
          exploreJobTickets();
          integration();
        }
      
      
      /** @test document and verify the mock setup used for this test */
      void
      demonstrateScaffolding()
        {
          Time nominalTime = lib::test::randTime();
          int additionalKey = rand() % 5000;
          Job mockJob = DummyJob::build (nominalTime, additionalKey);
          CHECK (mockJob.getNominalTime() == nominalTime);
          CHECK (not DummyJob::was_invoked (mockJob));
          
          mockJob.triggerJob();
          CHECK (    DummyJob::was_invoked (mockJob));
          CHECK (RealClock::wasRecently (DummyJob::invocationTime (mockJob)));
          CHECK (nominalTime   == DummyJob::invocationNominalTime (mockJob) );
          CHECK (additionalKey == DummyJob::invocationAdditionalKey(mockJob));
          
          Time prevInvocation = DummyJob::invocationTime (mockJob);
          mockJob.triggerJob();
          CHECK (prevInvocation < DummyJob::invocationTime (mockJob));                 // invoked again, recorded new invocation time
          CHECK (nominalTime   == DummyJob::invocationNominalTime (mockJob) );         // all other Job parameter recorded again unaltered
          CHECK (additionalKey == DummyJob::invocationAdditionalKey(mockJob));
          
          MockJobTicket mockTick;
          CHECK (mockTick.discoverPrerequisites().empty());
          
          MockSegmentation mockSeg;
          UNIMPLEMENTED ("how to mock and fake");
          /////////////////////////////////////////////////////////////////////////////TODO: extract from DispatcherInterface_test
          /////////////////////////////////////////////////////////////////////////////TODO: design a job-ticket-mock
          /////////////////////////////////////////////////////////////////////////////TODO: create a scheme for mock-jobs
        }
      
      
      /** @test use the Dispatcher interface (mocked) to generate a frame »beat«
       *  @remark this is the foundation to generate top-level frame render jobs
       */
      void
      buildBaseTickGenerator()
        {
          UNIMPLEMENTED ("foundation of state core");
        }
      
      
      /** @test use the base tick to access the corresponding JobTicket
       *        through the Dispatcher interface (mocked here).
       */
      void
      accessTopLevelJobTicket()
        {
          UNIMPLEMENTED ("transform into job ticket access");
        }
      
      
      /** @test build and verify the exploration function to discover job prerequisites */
      void
      exploreJobTickets()
        {
          UNIMPLEMENTED ("exploration function");
        }
      
      
      /** @test Job-planning pipeline integration test
       *  @remark generating dummy jobs for verification
       */
      void
      integration()
        {
          UNIMPLEMENTED ("integration incl. generation of dummy jobs");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (JobPlanningSetup_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
