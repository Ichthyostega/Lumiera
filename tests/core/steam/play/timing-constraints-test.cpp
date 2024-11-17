/*
  TimingConstraints(Test)  -  verify timing constraint specifications

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file timing-constraints-test.cpp
 ** unit test \ref TimingConstraints_test
 */


#include "lib/test/run.hpp"

#include "steam/play/timings.hpp"
#include "lib/time/timevalue.hpp"
//#include "steam/engine/buffhandle.hpp"
//#include "steam/engine/testframe.hpp"
//#include "lib/time/control.hpp"



namespace steam {
namespace play {
namespace test {
  
  using lib::time::Time;
  using lib::time::Duration;
  using lib::time::FrameRate;

//using steam::engine::BuffHandle;
//using steam::engine::test::testData;
//using steam::engine::test::TestFrame;
  
//using lumiera::Play;
  
//typedef time::Control<time::Duration> DurationControl;
//
//typedef DummyPlayConnection<PlayTestFrames_Strategy> DummyPlayer;
  
  
  /***************************************************************//**
   * @test document and verify the definition and combining of
   *       timing constraints for playback and rendering.
   */
  class TimingConstraints_test : public Test
    {
      virtual void
      run (Arg) 
        {
          define_basicTimingConstraints();
        }
      
      
      void
      define_basicTimingConstraints()
        {
          Timings timings (FrameRate::PAL);
          
          Time t(0,10);
          Duration lengthPAL (1, FrameRate::PAL);
          
          CHECK (timings.getOrigin() == Time::ZERO);
          CHECK (timings.getFrameDurationAt(t) == lengthPAL);
          CHECK (ASAP == timings.playbackUrgency);
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #831
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #831
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimingConstraints_test, "unit play");
  
  
  
}}} // namespace steam::play::test
