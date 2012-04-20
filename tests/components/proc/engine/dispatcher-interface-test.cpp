/*
  DispatcherInterface(Test)  -  document and verify dispatcher for frame job creation

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

* *****************************************************/


#include "lib/test/run.hpp"
#include "lib/error.hpp"

//#include "proc/engine/procnode.hpp"
#include "proc/play/dummy-play-connection.hpp"
#include "proc/mobject/model-port.hpp"
#include "proc/engine/dispatcher.hpp"
#include "proc/play/timings.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"
#include "lib/singleton.hpp"
#include "lib/itertools.hpp"
#include "lib/util.hpp"

//#include <boost/scoped_ptr.hpp>
//#include <iostream>
#include <tr1/functional>
#include <vector>

using test::Test;
using util::isnil;
using std::vector;
using std::tr1::function;
//using std::cout;
//using std::rand;


namespace proc {
namespace engine{
namespace test  {
  
  using lib::time::QuTime;
  using lib::time::FrameRate;
  using lib::time::Duration;
  using lib::time::Offset;
  using lib::time::TimeVar;
  using lib::time::Time;
  using mobject::ModelPort;
  using play::Timings;
  
  namespace { // used internally
    
    using play::PlayTestFrames_Strategy;
    using play::ModelPorts;
    
    typedef play::DummyPlayConnection<play::PlayTestFrames_Strategy> DummyPlaybackSetup;
    
    
    class MockDispatcherTable
      : public Dispatcher
      {
        
        DummyPlaybackSetup dummySetup_;
        
        
        /* == mock Dispatcher implementation == */
        
        FrameCoord
        locateFrameNext (uint frameCountOffset, TimeAnchor refPoint)
          {
            UNIMPLEMENTED ("dummy implementation of the core dispatch operation");
          }

      public:
        
        ModelPort
        provideMockModelPort()
          {
            ModelPorts mockModelPorts = dummySetup_.provide_testModelPorts();
            return *mockModelPorts;  // using just the first dummy port
          }
      };
    
    lib::Singleton<MockDispatcherTable> mockDispatcher;
    
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #890
    ModelPort
    getTestPort()
    {
      return mockDispatcher().provideMockModelPort();
    }
      
    
  } // (End) internal defs
  
  
  
  /*******************************************************************
   * @test document and verify the engine::Dispatcher interface, used
   *       to translate a CalcStream into individual node jobs.
   *       This test covers the definition of the interface itself,
   *       together with the supporting types and the default
   *       implementation of the basic operations.
   *       It creates and uses  a mock Dispatcher implementation.
   */
  class DispatcherInterface_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
           verify_basicDispatch();
           verify_standardDispatcherUsage();
           check_ContinuationBuilder();
        }
      
      
      /** @test perform the basic dispatch step
       *  and verify the generated frame coordinates
       */
      void
      verify_basicDispatch()
        {
          Dispatcher& dispatcher = mockDispatcher();
          Timings timings (FrameRate::PAL);
          ModelPort modelPort (getTestPort());
          uint startFrame(10);
          uint channel(0);
          
          TimeAnchor refPoint = TimeAnchor::build (timings, startFrame);
          CHECK (refPoint == Time::ZERO + Duration(10, FrameRate::PAL));
          
          FrameCoord coordinates = dispatcher.onCalcStream (modelPort,channel)
                                             .relativeFrameLocation (refPoint, 15);
          CHECK (coordinates.absoluteNominalTime == Time(0,1));
          CHECK (coordinates.absoluteFrameNumber == 25);
          CHECK (coordinates.remainingRealTime() <  Time(FSecs(25,25)));
          CHECK (coordinates.remainingRealTime() >= Time(FSecs(24,25)));
          CHECK (coordinates.modelPort == modelPort);
          CHECK (coordinates.channelNr == channel);
          
          JobTicket& executionPlan = dispatcher.accessJobTicket (coordinates);
          CHECK (executionPlan.isValid());
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
          Job frameJob = executionPlan.createJobFor (coordinates);
          CHECK (frameJob.getNominalTime() == coordinates.absoluteNominalTime);
          CHECK (0 < frameJob.getInvocationInstanceID());
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
        }
      
      
      /** @test the standard invocation sequence
       * used within the engine for planning new jobs.
       * The actual implementation is mocked.
       */
      void
      verify_standardDispatcherUsage()
        {
          Dispatcher& dispatcher = mockDispatcher();
          Timings timings (FrameRate::PAL);
          ModelPort modelPort (getTestPort());
          uint startFrame(10);
          uint channel(0);
          
          TimeAnchor refPoint = TimeAnchor::build (timings, startFrame);
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
          JobTicket::JobsPlanning jobs = dispatcher.onCalcStream(modelPort,channel)
                                                   .establishNextJobs(refPoint);
          
          // Verify the planned Jobs
          
          CHECK (!isnil (jobs));
          vector<Job> plannedChunk;
          lib::append_all (jobs, plannedChunk);
          
          uint chunksize = plannedChunk.size();
          CHECK (chunksize == timings.getPlanningChunkSize());
          
          TimeVar nextFrameStart = refPoint;
          Offset expectedTimeIncrement (1, FrameRate::PAL);
          for (uint i=0; i < chunksize; ++i )
            {
              Job& thisJob = plannedChunk[i];
              CHECK (nextFrameStart == thisJob.getNominalTime());
              CHECK (prevInvocationID < thisJob.getInvocationInstanceID());
              
              prevInvocationID = thisJob.getInvocationInstanceID();
              nextFrameStart += expectedTimeIncrement;
            }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
        }
      
      
      /** @test usually at the end of each standard invocation,
       * after scheduling a chunk of new Jobs, an additional
       * continuation job is created to re-invoke this
       * scheduling step.
       * - the refPoint gets bumped beyond the planned segment
       * - the continuation job embodies a suitable closure,
       *   usable for self-re-invocation
       */
      void
      check_ContinuationBuilder()
        {
          Dispatcher& dispatcher = mockDispatcher();
          Timings timings (FrameRate::PAL);
          ModelPort modelPort (getTestPort());
          uint startFrame(10);
          uint channel(0);
          
          // prepare the rest of this test to be invoked as "continuation"
          function<void(TimeAnchor)> testFunc = verify_invocation_of_Continuation;
          
          TimeAnchor refPoint = TimeAnchor::build (timings, startFrame);
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
          JobTicket::JobsPlanning jobs = dispatcher.onCalcStream(modelPort,channel)
                                                   .establishNextJobs(refPoint)
                                                   .prepareContinuation(testFunc);
          
          // an additional "continuation" Job has been prepared....
          Job continuation = lib::pull_last(jobs);
          CHECK (META_JOB = continuation.getKind());
          
          uint nrJobs = timings.getPlanningChunkSize();
          Duration frameDuration (1, FrameRate::PAL);
          
          // the Continuation will be scheduled sufficiently ahead of the planning end
          CHECK (continuation.getNominalTime() < Time(refPoint) + (nrJobs-1) * frameDuration);
          
          // now invoke the rest of this test, which has been embedded into the continuation job.
          // Since we passed testFunc as action for the continuation, we expect the invocation
          // of the function verify_invocation_of_Continuation()
          continuation.triggerJob();
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
        }
      
      /** action used as "continuation" in #check_ContinuationBuilder
       *  This function expects to be invoked with a time anchor bumped up
       *  to point exactly behind the end of the previously planned chunk of Jobs
       */
      static void
      verify_invocation_of_Continuation (TimeAnchor nextRefPoint)
        {
          Timings timings (FrameRate::PAL);
          uint startFrame(10);
          uint nrJobs = timings.getPlanningChunkSize();
          Duration frameDuration (1, FrameRate::PAL);
          
          CHECK (Time(nextRefPoint) == Time::ZERO + (startFrame + nrJobs) * frameDuration);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DispatcherInterface_test, "unit engine");
  
  
  
}}} // namespace proc::engine::test
