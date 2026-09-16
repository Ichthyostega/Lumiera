/*
  ParameterScale(Test)  -  verify parameter value scales

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file parameter-test.cpp
 ** unit test \ref ParameterScale_test
 */


#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "lib/par/scale.hpp"
#include "test/diagnostic-output.hpp"/////////////TODO

//#include <utility>
//#include <string>
//#include <vector>

//using std::string;
//using std::vector;
//using std::swap;
using test::roughEQ;


namespace lib {
namespace par {
namespace test{
  
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  namespace {//Test fixture....
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /**************************************************************************//**
   * @test cover properties of generic parameter containers.
   */
  class ParameterScale_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verify_Conforming();
          verify_cyclicScale();
        }
      
      
      void
      simpleUsage()
        {
          Scale<int> scale = {.minVal = -5, .maxVal = 23};
          CHECK (scale.isValid());
          
          CHECK (-5 == scale.conform (-55));
          CHECK (23 == scale.conform (+55));
          
          CHECK ( 5 == scale.join (2,3)   );
          CHECK (-5 == scale.join (23,-55));
        }
      
      
      /** @test accommodate values to comply with the scale
       *      - optionally a maximum and minimum can be enforced
       *      - optionally the value can be cyclically wrapped
       * @todo WIP 9/26 ✔ define ⟶ ✔ implement
       */
      void
      verify_Conforming()
        {
          Scale<float> scale;
          CHECK (1e5f == scale.conform (1e5f));
          
          scale.maxVal = -5.0f;
          CHECK (-5.0f == scale.conform (1e5f));
          
          CHECK (scale.isValid());
          scale.minVal = 5.0f;
          CHECK (not scale.isValid());
          scale.maxVal = 8.0f;
          CHECK (scale.isValid());
          CHECK (5.0f == scale.conform (-5.0f));
          
          CHECK (not scale.isCyclic());
          scale.cyclicLim = 100;
          CHECK (scale.isCyclic());
          CHECK (7.0f == scale.conform (-5.0f));
          CHECK (5.0f == scale.conform (-4.0f));
          CHECK (6.0f == scale.conform (-3.0f));
          CHECK (7.0f == scale.conform (-2.0f));
          
          VERIFY_FAIL ("beyond supported numeric precision"
                      , scale.conform (-1e5f));
        }
      
      
      /** @test Scales can be configured to wrap cyclically, e.g. for angles.
       *      - demonstrate an unsigned angle scale with short precision
       *      - demonstrate a high-resolution scale symmetric to zero
       * @todo WIP 9/26 ✔define ⟶ 🔁 implement
       */
      void
      verify_cyclicScale()
        {
          Scale<ushort> angleScale = {.minVal=0, .maxVal=360, .cyclicLim=0xFFFF};
          CHECK (angleScale.isValid());
          CHECK (angleScale.isCyclic());
          
          CHECK (  0 == angleScale.conform(0));
          CHECK (180 == angleScale.conform(180));
          CHECK (240 == angleScale.conform(240));
          CHECK (  0 == angleScale.conform(360));
          CHECK (359 == angleScale.conform(719));
          CHECK (  0 == angleScale.conform(65520));
          CHECK ( 15 == angleScale.conform(65535));

          
          Scale<lflp> tinyScale = {.minVal=-lflp(1)/100, .maxVal=lflp(1)/100};
          tinyScale.cyclicLim = util::limit_cyclicWrap(*tinyScale.maxVal - *tinyScale.minVal);
          CHECK (1e16 < tinyScale.cyclicLim);
          CHECK (tinyScale.isValid());
          CHECK (tinyScale.isCyclic());
          
          CHECK (         tinyScale.conform(0    ) == 0  );
          CHECK (roughEQ (tinyScale.conform(0.001)  , 0.001));
          CHECK (roughEQ (tinyScale.conform(0.005)  , 0.005));
          CHECK (roughEQ (tinyScale.conform(0.00999), 0.00999));
          CHECK (roughEQ (tinyScale.conform(0.01   ),-0.0099999));  // surprise: 1/100 not exactly representable in floating-point
          
          // since the domain is symmetric and 2/100 long, the ten-folds will fall into the middle,
          // but they typically do not land precisely on zero due to inherent "number dust"
          CHECK (1e15 > fabs(tinyScale.conform(1)));
          CHECK (0 !=        tinyScale.conform(1) );
          
          auto beyond_precision = -10 * tinyScale.cyclicLim;
          VERIFY_FAIL ("beyond supported numeric precision"
                      , tinyScale.conform (beyond_precision));
       }
      
      
      /** @test 
       * @todo WIP 9/26 🔁 define ⟶ implement
       */
      void
      verify_valueJoining()
        {
          Scale<float> scale;
       }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ParameterScale_test, "unit lib");
  
  
  
}}} // namespace lib::par::test
