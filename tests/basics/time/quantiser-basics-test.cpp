/*
  QuantiserBasics(Test)  -  a demo quantiser to cover the basic quantiser API

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file quantiser-basics-test.cpp
 ** unit test \ref QuantiserBasics_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/time/quantiser.hpp"
#include "lib/util.hpp"
#include <cstdlib>

using lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE;
using util::isnil;
using std::rand;



namespace lib {
namespace time{
namespace test{
  
  namespace {
    
    const int MAX_FRAMES = 25*500;
    const int DIRT_GRAIN = 50;
    
    const FSecs F25(1,25); // duration of one PAL frame
    
    inline Time
    secs (int seconds)
    {
      return Time(FSecs(seconds));
    }
  }
  
  
  
  
  /****************************************************//**
   * @test cover the basic Quantiser API.
   * This test uses a standalone quantiser implementation
   * to demonstrate and verify the basic behaviour
   * and the usage corner cases of a quantiser.
   * 
   * In this most simple form, a quantiser is defined
   * by the time reference point (origin) to use, and
   * the frame rate (grid spacing). For each raw time
   * value, the quantiser yields a time value aligned
   * at the next lower frame bound. Besides that,
   * time values are confined to be within
   * the interval (Time::MIN, Time::Max)
   * 
   * @see TimeQuantisation_test
   */
  class QuantiserBasics_test : public Test
    {
    
      virtual void
      run (Arg) 
        {
          checkSimpleQuantisation ();
          coverQuantisationStandardCases();
          coverQuantisationCornerCases();
        }
      
      
      void
      checkSimpleQuantisation ()
        {
          FixedFrameQuantiser fixQ(25);
          
          int frames = (rand() % MAX_FRAMES);
          FSecs dirt  = (F25 / (2 + rand() % DIRT_GRAIN));
          
          Time rawTime = Time(frames*F25) + Duration(dirt);
          
          CHECK (Time( frames   *F25) <= rawTime);
          CHECK (Time((frames+1)*F25) >  rawTime);
          
          Time quantTime (fixQ.gridLocal (rawTime));
          
          CHECK (Time(frames*F25) == quantTime);
        }
      
      
      /** Test Quantiser
       *  allowing to use plain numbers.
       *  1 Frame == 3 micro ticks */
      struct TestQuant
        : FixedFrameQuantiser
        {
          TestQuant (int origin=0)
            : FixedFrameQuantiser( FrameRate(TimeValue::SCALE, 3 ), TimeValue(origin))
            { }
          
          int
          quant (int testPoint)
            {
              TimeVar quantised = this->gridLocal(TimeValue(testPoint));
              return int(quantised);
            }
        };
      
      void
      coverQuantisationStandardCases()
        {
          TestQuant q0;
          TestQuant q1(1);
          
          CHECK ( 6 == q0.quant(7) );
          CHECK ( 6 == q0.quant(6) );
          CHECK ( 3 == q0.quant(5) );
          CHECK ( 3 == q0.quant(4) );
          CHECK ( 3 == q0.quant(3) );
          CHECK ( 0 == q0.quant(2) );
          CHECK ( 0 == q0.quant(1) );
          CHECK ( 0 == q0.quant(0) );
          CHECK (-3 == q0.quant(-1));
          CHECK (-3 == q0.quant(-2));
          CHECK (-3 == q0.quant(-3));
          CHECK (-6 == q0.quant(-4));
          
          CHECK ( 6 == q1.quant(7) );
          CHECK ( 3 == q1.quant(6) );
          CHECK ( 3 == q1.quant(5) );
          CHECK ( 3 == q1.quant(4) );
          CHECK ( 0 == q1.quant(3) );
          CHECK ( 0 == q1.quant(2) );
          CHECK ( 0 == q1.quant(1) );
          CHECK (-3 == q1.quant(0) );
          CHECK (-3 == q1.quant(-1));
          CHECK (-3 == q1.quant(-2));
          CHECK (-6 == q1.quant(-3));
          CHECK (-6 == q1.quant(-4));
        }
      
      
      void
      coverQuantisationCornerCases()
        {
          // origin at lower end of the time range
          FixedFrameQuantiser case1 (1, Time::MIN);
          CHECK (secs(0)            == case1.gridLocal(Time::MIN  ));
          CHECK (secs(0)            == case1.gridLocal(Time::MIN +TimeValue(1) ));
          CHECK (secs(1)            == case1.gridLocal(Time::MIN +secs(1) ));
          CHECK (Time::MAX -secs(1) >  case1.gridLocal( secs(-1)  ));
          CHECK (Time::MAX -secs(1) <= case1.gridLocal( secs (0)  ));
          CHECK (Time::MAX          >  case1.gridLocal( secs (0)  ));
          CHECK (Time::MAX          == case1.gridLocal( secs(+1)  ));
          CHECK (Time::MAX          == case1.gridLocal( secs(+2)  ));
          
          // origin at upper end of the time range
          FixedFrameQuantiser case2 (1, Time::MAX);
          CHECK (secs( 0)           == case2.gridLocal(Time::MAX  ));
          CHECK (secs(-1)           == case2.gridLocal(Time::MAX -TimeValue(1) ));  // note: next lower frame
          CHECK (secs(-1)           == case2.gridLocal(Time::MAX -secs(1) ));      //        i.e. the same as a whole frame down
          CHECK (Time::MIN +secs(1) <  case2.gridLocal( secs(+2)  ));
          CHECK (Time::MIN +secs(1) >= case2.gridLocal( secs(+1)  ));
          CHECK (Time::MIN          <  case2.gridLocal( secs(+1)  ));
          CHECK (Time::MIN          == case2.gridLocal( secs( 0)  ));          //      note: because of downward truncating,
          CHECK (Time::MIN          == case2.gridLocal( secs(-1)  ));         //             resulting values will already exceed
          CHECK (Time::MIN          == case2.gridLocal( secs(-2)  ));        //              allowed range and thus will be clipped
          
          // use very large frame with size of half the time range
          Duration hugeFrame(Time::MAX);
          FixedFrameQuantiser case3 (hugeFrame);
          CHECK (Time::MIN          == case3.gridLocal(Time::MIN  ));
          CHECK (Time::MIN          == case3.gridLocal(Time::MIN +TimeValue(1) ));
          CHECK (Time::MIN          == case3.gridLocal( secs(-1)  ));
          CHECK (TimeValue(0)       == case3.gridLocal( secs( 0)  ));
          CHECK (TimeValue(0)       == case3.gridLocal( secs(+1)  ));
          CHECK (TimeValue(0)       == case3.gridLocal(Time::MAX -TimeValue(1) ));
          CHECK (Time::MAX          == case3.gridLocal(Time::MAX  ));
          
          // now displacing this grid by +1sec....
          FixedFrameQuantiser case4 (hugeFrame, secs(1));
          CHECK (Time::MIN          == case4.gridLocal(Time::MIN  ));
          CHECK (Time::MIN          == case4.gridLocal(Time::MIN +TimeValue(1) ));  // clipped...
          CHECK (Time::MIN          == case4.gridLocal(Time::MIN +secs(1) ));      //  but now exact (unclipped)
          CHECK (Time::MIN          == case4.gridLocal( secs(-1)  ));
          CHECK (Time::MIN          == case4.gridLocal( secs( 0)  ));
          CHECK (TimeValue(0)       == case4.gridLocal( secs(+1)  ));           //.....now exactly the frame number zero
          CHECK (TimeValue(0)       == case4.gridLocal(Time::MAX -TimeValue(1) ));
          CHECK (TimeValue(0)       == case4.gridLocal(Time::MAX  ));         //.......still truncated down to frame #0
          
          // think big...
          Duration superHuge{secs(12345) + hugeFrame};
          Duration extraHuge{2*hugeFrame};
          CHECK (extraHuge == Duration::MAX);
          
          // Time::MAX < superHuge < Duration::Max is possible, but we can accommodate only one
          FixedFrameQuantiser case5 (superHuge);
          CHECK (TimeValue(0)       == case5.gridLocal(Time::MAX  ));
          CHECK (TimeValue(0)       == case5.gridLocal(Time::MAX -TimeValue(1) ));
          CHECK (TimeValue(0)       == case5.gridLocal( secs( 1)  ));
          CHECK (TimeValue(0)       == case5.gridLocal( secs( 0)  ));
          CHECK (Time::MIN          == case5.gridLocal( secs(-1)  ));
          CHECK (Time::MIN          == case5.gridLocal(Time::MIN +TimeValue(1) ));
          CHECK (Time::MIN          == case5.gridLocal(Time::MIN  ));
          
          // now with offset
          FixedFrameQuantiser case6 (superHuge, Time::MAX-secs(1));
          CHECK (TimeValue(0)       == case6.gridLocal(Time::MAX  ));
          CHECK (TimeValue(0)       == case6.gridLocal(Time::MAX -TimeValue(1) ));
          CHECK (TimeValue(0)       == case6.gridLocal(Time::MAX -secs(1) ));
          CHECK (Time::MIN          == case6.gridLocal(Time::MAX -secs(2) ));
          CHECK (Time::MIN          == case6.gridLocal( secs( 1)          ));
          CHECK (Time::MIN          == case6.gridLocal( secs(-12345)      ));
          CHECK (Time::MIN          == case6.gridLocal( secs(-12345-1)    ));
          CHECK (Time::MIN          == case6.gridLocal( secs(-12345-2)    ));  // this would be one frame lower, but is clipped
          CHECK (Time::MIN          == case6.gridLocal(Time::MIN +TimeValue(1) ));
          CHECK (Time::MIN          == case6.gridLocal(Time::MIN  ));          // same... unable to represent time points before Time::MIN

          // maximum frame size is spanning the full time range
          FixedFrameQuantiser case7 (extraHuge, Time::MIN+secs(1));
          CHECK (TimeValue(0)       == case7.gridLocal(Time::MAX  ));          // rounded down one frame, i.e. to origin
          CHECK (TimeValue(0)       == case7.gridLocal( secs( 0)  ));
          CHECK (TimeValue(0)       == case7.gridLocal(Time::MIN+secs(2) ));
          CHECK (TimeValue(0)       == case7.gridLocal(Time::MIN+secs(1) ));   // exactly at origin
          CHECK (Time::MIN          == case7.gridLocal(Time::MIN         ));   // one frame further down, but clipped to Time::MIN
          
          // even larger frames aren't possible
          Duration not_really_larger(secs(10000) + extraHuge);
          CHECK (extraHuge == not_really_larger);
          
          // frame sizes below the time micro grid get trapped
          long subAtomic = 2*TimeValue::SCALE;                          // too small for this universe...
          VERIFY_ERROR (BOTTOM_VALUE, FixedFrameQuantiser quark(subAtomic) );
          VERIFY_ERROR (BOTTOM_VALUE, FixedFrameQuantiser quark(Duration (FSecs (1,subAtomic))) );
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (QuantiserBasics_test, "unit common");
  
  
  
}}} // namespace lib::time::test
