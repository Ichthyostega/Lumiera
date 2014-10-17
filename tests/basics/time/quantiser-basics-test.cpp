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
    
    const uint MAX_FRAMES = 25*500;
    const uint DIRT_GRAIN = 50;
    
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
          
          uint frames = (rand() % MAX_FRAMES);
          FSecs dirt  = (F25 / (2 + rand() % DIRT_GRAIN));
          
          Time rawTime = Time(frames*F25) + Duration(dirt);            ////////////////TICKET #939 : should better use 64bit base type for FSecs ??
          
          CHECK (Time( frames   *F25) <= rawTime);
          CHECK (Time((frames+1)*F25) >  rawTime);
          
          Time quantTime (fixQ.gridAlign (rawTime));
          
          CHECK (Time(frames*F25) == quantTime);
        }
      
      
      /** Test Quantiser
       *  allowing to use plain numbers.
       *  1 Frame == 3 micro ticks */
      struct TestQuant
        : FixedFrameQuantiser
        {
          TestQuant (int origin=0)
            : FixedFrameQuantiser( FrameRate(GAVL_TIME_SCALE, 3 ), TimeValue(origin))
            { }
          
          int
          quant (int testPoint)
            {
              TimeVar quantised = this->gridAlign(TimeValue(testPoint));
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
          CHECK (secs(0)            == case1.gridAlign(Time::MIN  ));
          CHECK (secs(0)            == case1.gridAlign(Time::MIN +TimeValue(1) ));
          CHECK (secs(1)            == case1.gridAlign(Time::MIN +secs(1) ));
          CHECK (Time::MAX -secs(1) >  case1.gridAlign( secs(-1)  ));
          CHECK (Time::MAX -secs(1) <= case1.gridAlign( secs (0)  ));
          CHECK (Time::MAX          >  case1.gridAlign( secs (0)  ));
          CHECK (Time::MAX          == case1.gridAlign( secs(+1)  ));
          CHECK (Time::MAX          == case1.gridAlign( secs(+2)  ));
          
          // origin at upper end of the time range
          FixedFrameQuantiser case2 (1, Time::MAX);
          CHECK (secs( 0)           == case2.gridAlign(Time::MAX  ));
          CHECK (secs(-1)           == case2.gridAlign(Time::MAX -TimeValue(1) ));  // note: next lower frame
          CHECK (secs(-1)           == case2.gridAlign(Time::MAX -secs(1) ));      //        i.e. the same as a whole frame down
          CHECK (Time::MIN +secs(1) <  case2.gridAlign( secs(+2)  ));
          CHECK (Time::MIN +secs(1) >= case2.gridAlign( secs(+1)  ));
          CHECK (Time::MIN          <  case2.gridAlign( secs(+1)  ));
          CHECK (Time::MIN          == case2.gridAlign( secs( 0)  ));          //      note: because of downward truncating,
          CHECK (Time::MIN          == case2.gridAlign( secs(-1)  ));         //             resulting values will already exceed
          CHECK (Time::MIN          == case2.gridAlign( secs(-2)  ));        //              allowed range and thus will be clipped
          
          // maximum frame size is half the time range
          Duration hugeFrame(Time::MAX);
          FixedFrameQuantiser case3 (hugeFrame);
          CHECK (Time::MIN          == case3.gridAlign(Time::MIN  ));
          CHECK (Time::MIN          == case3.gridAlign(Time::MIN +TimeValue(1) ));
          CHECK (Time::MIN          == case3.gridAlign( secs(-1)  ));
          CHECK (TimeValue(0)       == case3.gridAlign( secs( 0)  ));
          CHECK (TimeValue(0)       == case3.gridAlign( secs(+1)  ));
          CHECK (TimeValue(0)       == case3.gridAlign(Time::MAX -TimeValue(1) ));
          CHECK (Time::MAX          == case3.gridAlign(Time::MAX  ));
          
          // now displacing this grid by +1sec....
          FixedFrameQuantiser case4 (hugeFrame, secs(1));
          CHECK (Time::MIN          == case4.gridAlign(Time::MIN  ));
          CHECK (Time::MIN          == case4.gridAlign(Time::MIN +TimeValue(1) ));  // clipped...
          CHECK (Time::MIN          == case4.gridAlign(Time::MIN +secs(1) ));      //  but now exact (unclipped)
          CHECK (Time::MIN          == case4.gridAlign( secs(-1)  ));
          CHECK (Time::MIN          == case4.gridAlign( secs( 0)  ));
          CHECK (TimeValue(0)       == case4.gridAlign( secs(+1)  ));           //.....now exactly the frame number zero
          CHECK (TimeValue(0)       == case4.gridAlign(Time::MAX -TimeValue(1) ));
          CHECK (TimeValue(0)       == case4.gridAlign(Time::MAX  ));         //.......still truncated down to frame #0
          
          // larger frames aren't possible
          Duration not_really_larger(secs(10000) + hugeFrame);
          CHECK (hugeFrame == not_really_larger);
          
          // frame sizes below the time micro grid get trapped
          long subAtomic = 2*GAVL_TIME_SCALE;                           // too small for this universe...
          VERIFY_ERROR (BOTTOM_VALUE, FixedFrameQuantiser quark(subAtomic) );
          VERIFY_ERROR (BOTTOM_VALUE, FixedFrameQuantiser quark(Duration (FSecs (1,subAtomic))) );
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (QuantiserBasics_test, "unit common");
  
  
  
}}} // namespace lib::time::test
