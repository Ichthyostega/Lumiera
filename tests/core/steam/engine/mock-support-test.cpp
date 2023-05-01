/*
  MockSupport(Test)  -  verify test support for fixture and job dispatch

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

/** @file mock-support-test.cpp
 ** unit test \ref MockSupport_test
 */


#include "lib/test/run.hpp"
#include "lib/error.hpp"
#include "steam/engine/mock-dispatcher.hpp"
#include "vault/engine/dummy-job.hpp"
#include "lib/util.hpp"
#include "vault/engine/nop-job-functor.hpp"

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
  
  
  
  /**********************************************************************//**
   * @test validate test support for render job planning and dispatch.
   *       - creating and invoking mock render jobs
   *       - a mocked JobTicket, generating mock render jobs
   *       - configurable test setup for a mocked Segmentation datastructure
   * 
   * @todo WIP-WIP-WIP 4/2023
   * 
   * @see JobPlanningSetup_test
   * @see Dispatcher
   * @see vault::engine::Job
   * @see steam::fixture::Segmentation
   */
  class MockSupport_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          simpleUsage();
          verify_MockJob();
          verify_MockJobTicket();
          verify_MockSegmentation();
        }
      
      
      /** @test simple usage example of the test helpers */
      void
      simpleUsage()
        {
          TODO ("simple usage example");
        }
      
      
      /** @test document and verify usage of a mock render job */
      void
      verify_MockJob()
        {
          Time nominalTime = lib::test::randTime();
          int additionalKey = rand() % 5000;
          Job mockJob = DummyJob::build (nominalTime, additionalKey);
          CHECK (mockJob.getNominalTime() == nominalTime);
          CHECK (not DummyJob::was_invoked (mockJob));
          
          mockJob.triggerJob();
          CHECK (DummyJob::was_invoked (mockJob));
          CHECK (RealClock::wasRecently (DummyJob::invocationTime (mockJob)));
          CHECK (nominalTime   == DummyJob::invocationNominalTime (mockJob) );
          CHECK (additionalKey == DummyJob::invocationAdditionalKey(mockJob));
          
          Time prevInvocation   = DummyJob::invocationTime (mockJob);
          mockJob.triggerJob();
          CHECK (prevInvocation < DummyJob::invocationTime (mockJob));                 // invoked again, recorded new invocation time
          CHECK (nominalTime   == DummyJob::invocationNominalTime (mockJob) );         // all other Job parameter recorded again unaltered
          CHECK (additionalKey == DummyJob::invocationAdditionalKey(mockJob));
        }
      
      
      /** @test document and verify usage of a mock JobTicket for frame dispatch */
      void
      verify_MockJobTicket()
        {
          FrameCoord coord;
          coord.absoluteNominalTime = lib::test::randTime();
          
          // build a render job to do nothing....
          Job nopJob = JobTicket::NOP.createJobFor(coord);
          CHECK (INSTANCEOF (vault::engine::NopJobFunctor, static_cast<JobClosure*> (nopJob.jobClosure)));   //////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
          CHECK (nopJob.parameter.nominalTime == coord.absoluteNominalTime);
          InvocationInstanceID empty; ///////////////////////////////////////////////////////////////////////TICKET #1287 : temporary workaround until we get rid of the C base structs
          CHECK (lumiera_invokey_eq (&nopJob.parameter.invoKey, &empty));
          
          MockJobTicket mockTicket;
          CHECK (mockTicket.discoverPrerequisites().empty());
          Job mockJob = mockTicket.createJobFor (coord);
          CHECK (    mockTicket.verify_associated (mockJob));                          // proof by invocation hash : is indeed backed by this JobTicket
          CHECK (not mockTicket.verify_associated (nopJob));                           // ...while some random other job is not related
          TODO ("cover details of MockJobTicket");
        }
      
      
      /** @test document and verify usage of a complete mocked Segmentation to back frame dispatch */
      void
      verify_MockSegmentation()
        {
          FrameCoord coord;
          Time someTime = lib::test::randTime();
          coord.absoluteNominalTime = someTime;
          {
            MockSegmentation mockSeg;
            CHECK (1 == mockSeg.size());
            JobTicket const& ticket = mockSeg[someTime].jobTicket();
            CHECK (util::isSameObject (ticket, JobTicket::NOP));
          }
          {
            MockSegmentation mockSeg{MakeRec().genNode()};
            CHECK (1 == mockSeg.size());
            JobTicket const& ticket = mockSeg[someTime].jobTicket();
            CHECK (not util::isSameObject (ticket, JobTicket::NOP));
            
            Job someJob = ticket.createJobFor(coord);
            CHECK (MockJobTicket::isAssociated (someJob, ticket));
            CHECK (not DummyJob::was_invoked (someJob));
            
            someJob.triggerJob();
            CHECK (DummyJob::was_invoked (someJob));
            CHECK (RealClock::wasRecently (DummyJob::invocationTime (someJob)));
            CHECK (someTime == DummyJob::invocationNominalTime (someJob));
          }
          
          
          TODO ("cover details of MockSegmentation");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MockSupport_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
