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

/** @file dispatcher-interface-test.cpp
 ** unit test \ref DispatcherInterface_test
 ** 
 ** @warning as of 4/2023 a complete rework of the Dispatcher is underway ///////////////////////////////////////////TICKET #1275
 */


#include "lib/test/run.hpp"
#include "lib/error.hpp"

//#include "steam/engine/procnode.hpp"
#include "steam/play/dummy-play-connection.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/engine/dispatcher.hpp"
#include "steam/engine/time-anchor.hpp"
#include "steam/play/timings.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/time/timequant.hpp"
//#include "lib/format-cout.hpp"
#include "lib/depend.hpp"
#include "lib/itertools.hpp"
#include "lib/util-coll.hpp"
#include "lib/util.hpp"

#include <functional>
#include <vector>

using test::Test;
using util::isnil;
using util::last;
using std::vector;
using std::function;
//using std::rand;


namespace steam {
namespace engine{
namespace test  {
  
  using lib::time::FrameRate;
  using lib::time::Duration;
  using lib::time::Offset;
  using lib::time::TimeVar;
  using lib::time::Time;
  using mobject::ModelPort;
  using play::Timings;
  
  namespace { // used internally
    
    using play::test::PlayTestFrames_Strategy;
    using play::test::ModelPorts;
    
    using DummyPlaybackSetup = play::test::DummyPlayConnection<PlayTestFrames_Strategy>;
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1274 replace by MockDispatcher !!!!
    class MockDispatcherTable
      : public Dispatcher
      {
        
        DummyPlaybackSetup dummySetup_;
        
        
        /* == mock Dispatcher implementation == */
        
        FrameCoord
        locateRelative (FrameCoord const&, FrameCnt frameOffset)  override
          {
            UNIMPLEMENTED ("dummy implementation of the core dispatch operation");
          }
        
        bool
        isEndOfChunk (FrameCnt, ModelPort port)  override
          {
            UNIMPLEMENTED ("determine when to finish a planning chunk");
          }
  
        size_t
        resolveModelPort (ModelPort modelPort)  override
          {
            UNIMPLEMENTED ("some Map lookup in a prepared table to find out the actual slot number");
          }

        JobTicket&
        accessJobTicket (size_t, TimeValue nominalTime)  override
          {
            UNIMPLEMENTED ("dummy implementation of the model backbone / segmentation");
          }
        
      public:
        
        ModelPort
        provideMockModelPort()
          {
            ModelPorts mockModelPorts = dummySetup_.getAllModelPorts();
            return *mockModelPorts;  // using just the first dummy port
          }
      };
    
    lib::Depend<MockDispatcherTable> mockDispatcher;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1274 replace by MockDispatcher !!!!
    
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #890
    ModelPort
    getTestPort()
    {
      return mockDispatcher().provideMockModelPort();
    }
    
    
    /* == test parameters == */
    
    const uint START_FRAME(10);
    const uint CHANNEL(0);
    
    bool continuation_has_been_triggered = false;
    
  } // (End) internal defs
  
  
  
  
  
  
  
  /***************************************************************//**
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
          ModelPort modelPort (getTestPort());
          Timings timings (FrameRate::PAL);
          ENSURE (START_FRAME == 10);
          
          TimeAnchor refPoint(timings, START_FRAME);
          CHECK (refPoint == Time::ZERO + Duration(10, FrameRate::PAL));
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1301
          FrameCoord coordinates = dispatcher.onCalcStream (modelPort,CHANNEL)
                                             .relativeFrameLocation (refPoint, 15);
          CHECK (coordinates.absoluteNominalTime == Time(0,1));
          CHECK (coordinates.absoluteFrameNumber == 25);
          CHECK (refPoint.remainingRealTimeFor(coordinates) <  Time(FSecs(25,25)));
          CHECK (refPoint.remainingRealTimeFor(coordinates) >= Time(FSecs(24,25)));
          CHECK (coordinates.modelPort == modelPort);
          CHECK (coordinates.channelNr == CHANNEL);
          
          JobTicket& executionPlan = dispatcher.getJobTicketFor (coordinates);
          CHECK (executionPlan.isValid());
          
          Job frameJob = executionPlan.createJobFor (coordinates);
          CHECK (frameJob.getNominalTime() == coordinates.absoluteNominalTime);
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1301
        }
      
      
      
      /** @test the standard invocation sequence
       * used within the engine for planning new jobs.
       * The actual implementation is mocked.
       */
      void
      verify_standardDispatcherUsage()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1301
          Dispatcher& dispatcher = mockDispatcher();
          ModelPort modelPort (getTestPort());
          Timings timings (FrameRate::PAL);
          
          TimeAnchor refPoint = TimeAnchor(timings, START_FRAME);
          
          JobPlanningSequence jobs = dispatcher.onCalcStream(modelPort,CHANNEL)
                                               .establishNextJobs(refPoint);
          
          // Verify the planned Jobs
          
          CHECK (!isnil (jobs));
          vector<Job> plannedChunk;
          lib::append_all (jobs, plannedChunk);
          
          Duration coveredTime (Offset(refPoint, last(plannedChunk).getNominalTime()));
          CHECK (coveredTime >= timings.getPlanningChunkDuration());
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1301
          
          ///TODO nachfolgendes muß komplett umgeschrieben werden
          ///TODO definieren, wie das scheduler-interface angesprochen wird
          ///TODO dann stub dafür bauen
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #903
          TimeVar frameStart (refPoint);
          InvocationInstanceID prevInvocationID(0);  ///////////////////////////////////////////////////////TICKET #1138 : C++17 requires explicit ctor for initialisation of union
          Offset expectedTimeIncrement (1, FrameRate::PAL);
          for (uint i=0; i < plannedChunk.size(); ++i )
            {
              Job& thisJob = plannedChunk[i];
              CHECK (prevInvocationID < thisJob.getInvocationInstanceID());
              prevInvocationID = thisJob.getInvocationInstanceID();
              
              if (frameStart != thisJob.getNominalTime())
                {
                  frameStart += expectedTimeIncrement;
                  CHECK (frameStart == thisJob.getNominalTime());
                }
            }
          // now, after having passed over the whole planned chunk
          CHECK (frameStart                        == Time(refPoint) + coveredTime);
          CHECK (frameStart                        >= Time(refPoint) + timings.getPlanningChunkDuration());
          CHECK (frameStart + expectedTimeIncrement > Time(refPoint) + timings.getPlanningChunkDuration());
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #903
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
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #903
          Dispatcher& dispatcher = mockDispatcher();
          ModelPort modelPort (getTestPort());
          Timings timings (FrameRate::PAL);
          
          // prepare the rest of this test to be invoked as "continuation"
          function<void(TimeAnchor)> testFunc = verify_invocation_of_Continuation;
          
          TimeAnchor refPoint = TimeAnchor::build (timings, START_FRAME);
          JobPlanningSequence jobs = dispatcher.onCalcStream(modelPort,CHANNEL)
                                               .establishNextJobs(refPoint)
                                               .prepareContinuation(testFunc);
          
          // an additional "continuation" Job has been prepared....
          Job continuation = lib::pull_last(jobs);
          CHECK (META_JOB == continuation.getKind());
          
          // the Continuation will be scheduled sufficiently ahead of the currently planned chunk's end
          CHECK (continuation.getNominalTime() < Time(refPoint) + timings.getPlanningChunkDuration());
          
          // now invoke the rest of this test, which has been embedded into the continuation job.
          // Since we passed testFunc as action for the continuation, we expect the invocation
          // of the function verify_invocation_of_Continuation()
          continuation_has_been_triggered = false;
          
          continuation.triggerJob();
          CHECK (continuation_has_been_triggered);
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #903
        }
      
      /** action used as "continuation" in #check_ContinuationBuilder
       *  This function expects to be invoked with a time anchor bumped up
       *  to point exactly behind the end of the previously planned chunk of Jobs
       */
      static void
      verify_invocation_of_Continuation (TimeAnchor nextRefPoint)
        {
          Timings timings (FrameRate::PAL);
          Duration frameDuration (1, FrameRate::PAL);
          Time startAnchor = Time::ZERO + START_FRAME*frameDuration;
          Duration time_to_cover = timings.getPlanningChunkDuration();
          
          CHECK (Time(nextRefPoint) >= startAnchor + time_to_cover);
          CHECK (Time(nextRefPoint) <  startAnchor + time_to_cover + 1*frameDuration);
          continuation_has_been_triggered = true;
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DispatcherInterface_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
