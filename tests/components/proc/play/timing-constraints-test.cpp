/*
  TimingConstraints(Test)  -  verify timing constraint specifications

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

#include "proc/play/timings.hpp"
//#include "proc/engine/buffhandle.hpp"
//#include "proc/engine/testframe.hpp"
//#include "lib/time/control.hpp"



namespace proc {
namespace play {
namespace test {
  
  namespace time = lib::time;
  
//using proc::engine::BuffHandle;
//using proc::engine::test::testData;
//using proc::engine::test::TestFrame;
  
//using lumiera::Play;
  
//typedef time::Control<time::Duration> DurationControl;
//
//typedef DummyPlayConnection<PlayTestFrames_Strategy> DummyPlayer;
  
  
  /*******************************************************************
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
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #831
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #831
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimingConstraints_test, "unit player");
  
  
  
}}} // namespace proc::play::test
