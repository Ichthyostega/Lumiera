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
#include "lib/test/test-helper.hpp"

#include "steam/play/timings.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/time/timequant.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/util-coll.hpp"
#include "lib/util.hpp"

//#include <functional>
//#include <vector>

using test::Test;
using util::isnil;
//using std::vector;
//using std::function;
using std::rand;


namespace steam {
namespace engine{
namespace test  {
  
  using lib::time::FSecs;
  using lib::time::FrameCnt;
  using lib::time::FrameRate;
  using lib::time::Duration;
  using lib::time::Offset;
  using lib::time::Time;
  using play::Timings;
  
  namespace { // Test fixture...
    
    /* == test parameters == */
    
    const uint START_FRAME(10);
    
    
    
    FSecs
    randTicks()
    {
      return FSecs{1 + rand() % 600, 1 + rand() % 600};
    }

  } // (End) Test fixture
  
  
  
  
  /***************************************************************//**
   * @test document and verify frame timing calculations, which are
   *       used in the Player / engine::Dispatcher, to translate a CalcStream
   *       into individual node jobs.
   * @see TimingConstraints_test
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
          Timings timings (FrameRate::PAL);
          CHECK (timings.getOrigin() == Time::ZERO);
          
          ENSURE (START_FRAME == 10);
          CHECK (timings.getFrameStartAt(START_FRAME)   == Time::ZERO + Duration(10, FrameRate::PAL));
          CHECK (timings.getFrameStartAt(START_FRAME+1) == Time::ZERO + Duration(11, FrameRate::PAL));
        }
      
      
      /** @test detect boundaries of frame planning chunks for arbitrary chunk duration.
       */
      void
      verify_next_startPoint()
        {
          Timings timings (FrameRate::PAL);
          Time refPoint{randTicks()};
          
          FrameCnt startFrame = timings.getBreakPointAfter (refPoint);
          Time frameStart = timings.getFrameStartAt(startFrame);
          
          Duration frameDuration (1, FrameRate::PAL);
          
          CHECK (frameStart >= refPoint);
          CHECK (frameStart <  refPoint + frameDuration);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (Timings_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
