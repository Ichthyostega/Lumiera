/*
  DummyPlayConnection(Test)  -  create and verify a simplified test render engine setup

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

/** @file dummy-play-connection-test.cpp
 ** unit test \ref DummyPlayConnection_test
 */


#include "lib/test/run.hpp"

#include "steam/play/dummy-play-connection.hpp"
#include "include/play-facade.hpp"
#include "lib/time/control.hpp"



namespace steam {
namespace play {
namespace test {
  
  namespace time = lib::time;
  
  using steam::engine::BuffHandle;
//using steam::engine::test::testData;
//using steam::engine::test::TestFrame;
  
  using lumiera::Play;
  
  typedef time::Control<time::Duration> DurationControl;
  
  typedef DummyPlayConnection<PlayTestFrames_Strategy> DummyPlayer;
  
  
  /***************************************************************//**
   * @test verify the OutputSlot interface and base implementation
   *       by performing full data exchange cycle. This is a
   *       kind of "dry run" for documentation purposes,
   *       both the actual OutputSlot implementation
   *       as the client using this slot are Mocks.
   */
  class DummyPlayConnection_test : public Test
    {
      virtual void
      run (Arg) 
        {
          verify_simulatedPlayback();
        }
      
      
      void
      verify_simulatedPlayback()
        {
          DummyPlayer dummy;
          CHECK (!dummy.isWired());
          
          Play::Controller player
            = Play::facade().perform ( dummy.getAllModelPorts()
                                     , dummy.provide_testOutputSlot());
          CHECK ( dummy.isWired());
          
          DurationControl playDuration;
          player.controlDuration (playDuration);
          
          // configure the controller to playback only for a fixed time duration
          playDuration (dummy.getPlannedTestDuration());
          
          CHECK (!player.is_playing());
          
          player.play(true);                // hit the start button
          CHECK (player.is_playing());
          
          dummy.waitUntilDue();             // test helper: block waiting until planned test should be done
          
          CHECK (!player.is_playing());     // should have returned to pause, since we've set a fixed playback duration
          CHECK (dummy.isWired());
          
          player.close();
          CHECK (!dummy.isWired());
          CHECK (dummy.gotCorrectOutput());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DummyPlayConnection_test, "unit play");
  
  
  
}}} // namespace steam::play::test
