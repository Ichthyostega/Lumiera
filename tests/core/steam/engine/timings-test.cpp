/*
  Timings(Test)  -  document and verify basic frame step timings

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
 ** unit test \ref Timings_test
 ** 
 ** @warning as of 5/2023 planning-chunk generation is reworked ///////////////////////////////////////////TICKET #1301 : factor out RenderDrive
 */


#include "lib/test/run.hpp"
#include "lib/error.hpp"

//#include "steam/engine/procnode.hpp"
#include "steam/play/dummy-play-connection.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/engine/dispatcher.hpp"
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
  
  namespace { // Test fixture...
    
    /* == test parameters == */
    
    const uint START_FRAME(10);
    
    const uint DUMMY_CHANNEL(0); /////////////////////////////////////////////////////////////TICKET #1297 : get rid of the channels (use different ModelPort)
    
    
    ModelPort
    getTestPort()
    {
      using play::test::ModelPorts;
      using play::test::PlayTestFrames_Strategy;
      using DummyPlaybackSetup = play::test::DummyPlayConnection<PlayTestFrames_Strategy>;
      
      DummyPlaybackSetup dummySetup;
      ModelPorts mockModelPorts = dummySetup.provide_testModelPorts();
      return *mockModelPorts;  // using just the first dummy port
    }

  } // (End) Test fixture
  
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1301
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1301
  
  
  
  
  
  
  /***************************************************************//**
   * @test document and verify the engine::Dispatcher interface, used
   *       to translate a CalcStream into individual node jobs.
   *       This test covers the definition of the interface itself,
   *       together with the supporting types and the default
   *       implementation of the basic operations.
   *       It creates and uses  a mock Dispatcher implementation.
   */
  class Timings_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           verify_simpleFrameStep();
           verify_next_startPoint();
        }
      
      
      /** @test perform the basic dispatch step
       *  and verify the generated frame coordinates
       */
      void
      verify_simpleFrameStep()
        {
          ModelPort modelPort (getTestPort());
          Timings timings (FrameRate::PAL);
          ENSURE (START_FRAME == 10);
          
          TimeAnchor refPoint(timings, START_FRAME);
          CHECK (refPoint == Time::ZERO + Duration(10, FrameRate::PAL));
          
          FrameCoord frame; /////////////////TODO get / implement offset-by-#-Frames
          FrameCnt frameOffset = 15;
          
          frame.absoluteNominalTime = refPoint;
          frame.absoluteFrameNumber = refPoint.getStartFrame();
          frame.absoluteRealDeadline = refPoint.establishDeadlineFor (frameOffset);
          frame.modelPort = modelPort;
          frame.channelNr = DUMMY_CHANNEL;
          
          ENSURE (frame.isDefined());
//        frame = dispatcher_->locateRelative (frame, frameOffset); /////////////////////////////OOO MAGIC FAIRY DUST HERE
          
          CHECK (frame.absoluteNominalTime == Time(0,1));     ///////////////////////////////////OOO Boooooom
          CHECK (frame.absoluteFrameNumber == 25);
          CHECK (refPoint.remainingRealTimeFor(frame) <  Time(FSecs(25,25)));
          CHECK (refPoint.remainingRealTimeFor(frame) >= Time(FSecs(24,25)));
          CHECK (frame.modelPort == modelPort);
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1301
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1301
        }
      
      
      /** action used as "continuation" in #check_ContinuationBuilder
       *  This function expects to be invoked with a time anchor bumped up
       *  to point exactly behind the end of the previously planned chunk of Jobs
       */
      void
      verify_next_startPoint()
        {
          Timings timings (FrameRate::PAL);
          TimeAnchor nextRefPoint(timings, START_FRAME);  //////////////////////////////////////OOO : how is this actually determined (not yet implemented....)
          UNIMPLEMENTED ("How to determine the start point of the next planning chunk");
          
          Duration frameDuration (1, FrameRate::PAL);
          Time startAnchor = Time::ZERO + START_FRAME*frameDuration;
          Duration time_to_cover = timings.getPlanningChunkDuration();
          
          CHECK (Time(nextRefPoint) >= startAnchor + time_to_cover);
          CHECK (Time(nextRefPoint) <  startAnchor + time_to_cover + 1*frameDuration);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (Timings_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
