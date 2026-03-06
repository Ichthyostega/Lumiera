/*
  QuantiserBasics(Test)  -  a demo quantiser to cover the basic quantiser API

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file quantiser-basics-test.cpp
 ** unit test \ref QuantiserBasics_test
 */


#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "lib/time/quantiser.hpp"
#include "lib/random.hpp"
#include "lib/util.hpp"

using lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE;
using util::isnil;
using lib::rani;



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
   * the interval (Time::MIN, Time::MAX)
   * 
   * @see TimeQuantisation_test
   */
  class QuantiserBasics_test : public Test
    {
    
      virtual void
      run (Arg) 
        {
          seedRand();
          checkSimpleQuantisation();
          coverQuantisationStandardCases();
          coverQuantisationCornerCases();
        }
      
      
      void
      checkSimpleQuantisation ()
        {
          FixedFrameQuantiser fixQ(25);
          
          int frames = rani(MAX_FRAMES);
          FSecs dirt  = (F25 / (2 + rani(DIRT_GRAIN)));
          
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
              return _raw(quantised);
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
          // For this test we exploit the limits of the time system
          Time SUB_MIN{-Duration::MAX};
          Time SUP_MAX{ Duration::MAX};
          
          // origin at lower end of the time range
          FixedFrameQuantiser case1 (1, SUB_MIN);
          CHECK (secs(0)          == case1.gridLocal(SUB_MIN  ));
          CHECK (secs(0)          == case1.gridLocal(SUB_MIN +TimeValue(1) ));
          CHECK (secs(1)          == case1.gridLocal(SUB_MIN +secs(1) ));
          CHECK (SUP_MAX -secs(1) >  case1.gridLocal( secs(-1)  ));
          CHECK (SUP_MAX -secs(1) <= case1.gridLocal( secs (0)  ));
          CHECK (SUP_MAX          >  case1.gridLocal( secs (0)  ));
          CHECK (SUP_MAX          >  case1.gridLocal( secs(+1)  ));
          CHECK (SUP_MAX          >  case1.gridLocal( secs(+2)  ));
          
          TimeValue largestPoint = case1.gridLocal(secs (0));
          CHECK (largestPoint     == case1.gridLocal( secs(+1)  ));
          CHECK (largestPoint     == case1.gridLocal( secs(+2)  ));
          CHECK (largestPoint     <  SUP_MAX);
          CHECK (largestPoint     == Offset{secs(1)} * case1.gridPoint(secs(0)));
          
          // origin at upper end of the time range
          FixedFrameQuantiser case2 (1, SUP_MAX);
          CHECK (secs( 0)         == case2.gridLocal(SUP_MAX  ));
          CHECK (secs(-1)         == case2.gridLocal(SUP_MAX -TimeValue(1) ));  // note: next lower frame
          CHECK (secs(-1)         == case2.gridLocal(SUP_MAX -secs(1) ));      //        i.e. the same as a whole frame down
          CHECK (SUB_MIN +secs(1) <  case2.gridLocal( secs(+2)  ));
          CHECK (SUB_MIN +secs(1) >= case2.gridLocal( secs(+1)  ));
          CHECK (SUB_MIN          <  case2.gridLocal( secs(+1)  ));
          CHECK (SUB_MIN          == case2.gridLocal( secs( 0)  ));        //      note: because of downward truncating,
          CHECK (SUB_MIN          == case2.gridLocal( secs(-1)  ));       //             resulting values will already exceed
          CHECK (SUB_MIN          == case2.gridLocal( secs(-2)  ));      //              allowed range and thus will be clipped
          
          // use very large frame with size of half the time range
          Duration hugeFrame(SUP_MAX);
          FixedFrameQuantiser case3 (hugeFrame);
          CHECK (SUB_MIN          == case3.gridLocal(SUB_MIN  ));
          CHECK (SUB_MIN          == case3.gridLocal(SUB_MIN +TimeValue(1) ));
          CHECK (SUB_MIN          == case3.gridLocal( secs(-1)  ));
          CHECK (TimeValue(0)     == case3.gridLocal( secs( 0)  ));
          CHECK (TimeValue(0)     == case3.gridLocal( secs(+1)  ));
          CHECK (TimeValue(0)     == case3.gridLocal(SUP_MAX -TimeValue(1) ));
          CHECK (SUP_MAX          == case3.gridLocal(SUP_MAX  ));
          
          // now displacing this grid by +1sec....
          FixedFrameQuantiser case4 (hugeFrame, secs(1));
          CHECK (SUB_MIN          == case4.gridLocal(SUB_MIN  ));
          CHECK (SUB_MIN          == case4.gridLocal(SUB_MIN +TimeValue(1) ));  // clipped...
          CHECK (SUB_MIN          == case4.gridLocal(SUB_MIN +secs(1) ));      //  but now exact (unclipped)
          CHECK (SUB_MIN          == case4.gridLocal( secs(-1)  ));
          CHECK (SUB_MIN          == case4.gridLocal( secs( 0)  ));
          CHECK (TimeValue(0)     == case4.gridLocal( secs(+1)  ));         //.....now exactly the frame number zero
          CHECK (TimeValue(0)     == case4.gridLocal(SUP_MAX -TimeValue(1) ));
          
          
          // think big...
          TimeVar excess{SUP_MAX +secs(1)};                     // this is a *loophole* to slide by the limitation of Time values
          CHECK (SUP_MAX          <  excess);
          CHECK (Duration{excess} <  excess);                   // ...yet as soon as we construct another entity, the limitation applies
          CHECK (Duration{excess} == SUP_MAX);
          
          CHECK (SUP_MAX          == case4.gridLocal(excess )); // Thus, more by accident, the next higher grid point can be computed

          CHECK (secs(1)          == case4.timeOf(0));
          CHECK (excess           == case4.timeOf(1));          // The same loophole also allows to generate this next higher grid point
          CHECK (excess           == case4.timeOf(2));          // ...while the next after next will limited in computation
          
          FixedFrameQuantiser broken (Duration::MAX, SUP_MAX);  // Can drive this loophole to the extreme...
          CHECK (secs(0)          == broken.timeOf(-1));        // since there is leeway by one order of magnitude
          CHECK (SUP_MAX          == broken.timeOf(0));
          CHECK (SUP_MAX+SUP_MAX  >  Duration::MAX);
          CHECK (SUP_MAX+SUP_MAX  == broken.timeOf(1));
          CHECK (SUP_MAX+SUP_MAX  == broken.timeOf(2));

          // frame sizes below the time micro grid get trapped
          long subAtomic = 2*TimeValue::SCALE;                          // too small for this universe...
          VERIFY_ERROR (BOTTOM_VALUE, FixedFrameQuantiser quark(subAtomic) );
          VERIFY_ERROR (BOTTOM_VALUE, FixedFrameQuantiser quark(Duration (FSecs (1,subAtomic))) );
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (QuantiserBasics_test, "unit common");
  
  
  
}}} // namespace lib::time::test
