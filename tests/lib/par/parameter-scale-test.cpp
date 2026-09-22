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

using test::roughEQ;


namespace lib {
namespace par {
namespace test{
  
  namespace {//Test helpers....
    
    template<typename NUM>
    constexpr NUM _MAX = std::numeric_limits<NUM>::max();
    
    template<typename NUM>
    constexpr NUM _MIN = std::numeric_limits<NUM>::lowest();
    
    template<floating_point FLO>
    constexpr FLO _EPSILON = std::numeric_limits<FLO>::epsilon();
    
  }//(End)Test helpers
  
  
  
  
  
  
  
  
  
  /**************************************************************************//**
   * @test demonstrate features and verify behaviour of value Scale definitions.
   *     - a par::Scale is a descriptor record with some feature configuration settings
   *     - some combination of setting marks specific scale features to be present
   *     - values can be _conformed_ to comply with the scale constraints
   *     - a Scale can be configured to maintain cyclic wrapping values
   *     - values defined on different scales can be _joined_ (numerically combined)
   *     - scales can be defined to be logarithmic, and work together with linear scales. 
   */
  class ParameterScale_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verify_Predicate();
          verify_Conforming();
          verify_cyclicScale();
          verify_valueJoining();
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
      
      
      /** @test verify the feature detection and descriptor predicates */
      void
      verify_Predicate()
        {
          Scale<double> scale;
          
          CHECK (scale.isValid());
          CHECK (not scale.isCyclic());
          CHECK (not scale.isLimited());
          CHECK (not scale.isLogarithmic());
          
          scale.minVal = -5;
          CHECK (scale.isValid());
          CHECK (not scale.isCyclic());
          CHECK (    scale.isLimited());
          
          scale.cyclicLim = 10;
          CHECK (not scale.isValid());
          CHECK (    scale.isCyclic());
          CHECK (not scale.isLimited());
          
          scale.maxVal = +5;
          CHECK (    scale.isValid());
          CHECK (    scale.isCyclic());
          CHECK (not scale.isLimited());
          
          CHECK (not scale.isFactor());
          scale.neutral = 2;
          CHECK (not scale.isFactor());
          scale.neutral = 1;
          CHECK (    scale.isFactor());
          
          CHECK (scale.metric == LIN);
          CHECK (not scale.isLogarithmic());
          scale.metric = NOM;
          CHECK (not scale.isLogarithmic());
          scale.metric = DEC;
          CHECK (    scale.isLogarithmic());
          scale.metric = BIN;
          CHECK (    scale.isLogarithmic());
          scale.metric = NAT;
          CHECK (    scale.isLogarithmic());
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
       * @todo WIP 9/26 ✔define ⟶ ✔ implement
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
      
      
      /** @test Base value and data feeds from differing Scales can be joined together
       *      - when the target scale defines limits (or is cyclic),
       *        this conforming is applied after all conversions
       *      - a feed can be marked as "factor", in which case
       *        the feed is multiplied to the target rather than added
       *      - even scales with integral type can carry log meaning
       *      - suitable intermediary types used for good precision,
       *        and especially to avoid wrapping signed into unsigned
       *      - the supported logarithmic flavours are accommodated,
       *        possibly converting the base of the logarithm accordingly
       *      - the factor 10 inherent to decibels is applied / removed
       *        before combining decibel values with other logarithms
       *      - applying a linear factor to a logarithmic scale is
       *        transformed into an addition in the logarithmic domain
       *      - adding a linear offset to a logarithmic scale is rejected
       *      - extended precision scales handled using `lflp` (≙`long double`)
       *      - this allows to retain full precision of 64bit integrals
       * @todo WIP 9/26 ✔ define ⟶ ✔ implement
       */
      void
      verify_valueJoining()
        {
          Scale<ushort> scale = {.minVal = 2, .maxVal = 10};
          Scale<int64_t> lscale;
          
          CHECK ( 5 == scale.join (2, int64_t(3), lscale));   // 2+3 ≡ 5     (all computations done as double)
          CHECK (10 == scale.join (5, int64_t(8), lscale));   // 5+8 ≡ 13 ⟼ capped to 10
          
          lscale.neutral = 1;
          CHECK (lscale.isFactor());
          CHECK ( 6 == scale.join (2, int64_t(3), lscale));   // 2*3 ≡ 6
          CHECK ( 2 == scale.join (2, int64_t(-3),lscale));   // 2*-3 ≡ -6 ⟼ first conditioned to ushort domain, then capped to 2
          
          lscale.metric = DEC; // in dB
          CHECK ( 7 == scale.join (4, int64_t(3), lscale));   // 4 + 3dB ⟼ 4 * 1.995 ≡ 7.98 ⟼ truncated to 7
          
          Scale<short> sscale;
          Scale<uint64_t> uscale = {.neutral = 1};
          
          // Note: number promotion rules of C++ would backfire nastily here...
          CHECK (short(-3) * uint64_t(2) > numeric_limits<int64_t>::max());
          CHECK (short(-6) == sscale.join (-3, uint64_t(2), uscale));
          
           //---floating-point logarithmic scale conversions---
          //
          Scale<float> fscale  = {.metric = NAT};
          Scale<float> feed_dB = {.metric = DEC};
          CHECK (roughEQ (fscale.join (0, float(20), feed_dB) , log(100.0f) ));       // dB ⟼ natural logarithm; note 20dB ≙ factor 10 * lg(100); input 20 divided by 10
          fscale.metric = BIN;
          CHECK (roughEQ (fscale.join (0, float(20), feed_dB) , log2(100.0f)));       // dB ⟼ binary logarithm; (factor 1/10 on input is folded into the log factor)
          fscale.metric = DEC;
          CHECK (roughEQ (fscale.join (0, float(20), feed_dB) , 20.0f ));             // dB ⟼ dB just passed-through and then added to value ≔ 0.0
          
          fscale.metric = DEC;
          lscale.metric = BIN;
          CHECK (roughEQ (fscale.join (0, int64_t(-3), lscale), log10(1.0f/8) *10));  // binary log (given as int value)  ⟼  dB
          lscale.metric = NAT;
          CHECK (roughEQ (fscale.join (0, int64_t(-3), lscale), log10(exp(-3))*10));  // natural log (input as int)       ⟼  dB
          lscale.metric = DEC;
          CHECK (roughEQ (fscale.join (0, int64_t(-3), lscale),           -3     ));  // dB ⟼ dB just passed-through
          
          fscale.metric = BIN;
          lscale.metric = BIN;
          CHECK (roughEQ (fscale.join (0, int64_t(-3), lscale),           -3     ));  // lb ⟼ lb just passed-through
          lscale.metric = NAT;
          CHECK (roughEQ (fscale.join (0, int64_t(-3), lscale), log2 (exp(-3))   ));  // natural log  ⟼  binary log
          lscale.metric = DEC;
          CHECK (roughEQ (fscale.join (0, int64_t(-30),lscale), log2 (1.0/1000)  ));  // conversion factor: dB/10 ⟼ binary log
          
          fscale.metric = LIN;
          lscale.metric = BIN;
          CHECK (roughEQ (fscale.join (1, int64_t(-3), lscale),       1.0f/8    ));   // binary log applied as factor to a linear scale
          lscale.metric = NAT;
          CHECK (roughEQ (fscale.join (1, int64_t(-1), lscale),       1.0f/exp(1)));  // natural log applied as factor 1/e
          lscale.metric = DEC;
          CHECK (roughEQ (fscale.join (1, int64_t(-30),lscale),       1.0f/1000 ));   // decibels applied as factor : -30 dB ≙ 10^-3
          
          fscale.metric = BIN;
          lscale.metric = LIN;
          lscale.neutral = 1;
          CHECK (lscale.isFactor());
          CHECK (fscale.isLogarithmic());
          CHECK (         fscale.join (1, int64_t(8),  lscale) == 1 + 3);             // feed factor 8  ⟼  to binary logarithmic fscale (8 ≡ 2^3) and added there (exact)
          fscale.metric = NAT;
          CHECK (roughEQ (fscale.join (1, int64_t(8),  lscale),   1 + logf(8)));      // feed factor 8  ⟼  now applied to a natural log scale (and added to one, in float)
          fscale.metric = DEC;
          CHECK (roughEQ (fscale.join (1, int64_t(100),lscale),   1 + 20));           // feed factor 8  ⟼  now applied as offset in decibels
          
          lscale.neutral = 0;
          CHECK (not lscale.isFactor());
          VERIFY_FAIL ("adding a non-logarithmic data feed on top of a logarithmic base value"
                      , fscale.join (1, int64_t(8), lscale));
          
          
          //---verify a long-double target scale
          //
          Scale<lflp> megaScale = {.minVal = -10, .maxVal = +10};    // setup a cyclic scale using the long double domain
          megaScale.cyclicLim = util::limit_cyclicWrap<lflp>(20);    // (cyclic wrap is handled by conform(); just another little twist)
          CHECK (megaScale.isCyclic());
          megaScale.metric = DEC;
          fscale.metric = LIN;
          fscale.neutral = 1;
          CHECK (fscale.isFactor());
          CHECK ( 5 == megaScale.join ( 5, 100.0f,  fscale));        // input ⟼ decibels, then added to the base value, and finally wrapped ∈ [-10 ... 10[
          CHECK (-5 == megaScale.join (-5, 100.0f,  fscale));
          CHECK ( 5 == megaScale.join (-5, 1000.0f, fscale));        //          ...so here we get -5dB + 30dB
          
          // now push that to the limits....
          CHECK (logb (1e18)          < logb (_MAX<int64_t>));       // use the largest power of 10 representable in int64_t
          CHECK (logb (_MAX<int64_t>) < logb (megaScale.cyclicLim)); // ...which the (cyclic) target scale can still handle precisely!
          
          lflp res1 = megaScale.join (-5, 1e18f, fscale);            // log10 ⟼ 18, so we add -5dB + 180dB;
          CHECK (roughEQ (res1, -5));                                //          the cyclicWrap then computes 175 - (-10) ≡ 185 % 20 (≙5) + (minVal ≙ -10)
          
          // but the computation is not exact...
          CHECK (0 != std::fmod (res1, 1));
          // yet the cause lies in the input already:
          lflp srcErr = lflp(1e18f) - pow (lflp(10), 18);
          CHECK (1e10 < fabs (srcErr));                              // ... so the float input incurs a gigantic error margin
          
          // however: use an exact input, and the result will be exact
          lscale.neutral = 1;
          CHECK (lscale.isFactor());
          CHECK (-5 == megaScale.join (-5, int64_t(1e18),   lscale));
          lflp res2 =  megaScale.join (-5, int64_t(1e18)+1, lscale);
          CHECK (-5 != res2);                                       // even a tiny offset makes it through the computation
          CHECK (-16 > log10 (res2 -(-5)));                         // error is below the 16. decimal place (plausible, since log10 is computed)
          
          // compare to target scale with lesser precision...
          Scale<double> doubleTargetScale = {.metric = DEC};
          Scale<uint64_t> longTargetScale = {.metric = DEC};
          CHECK (180 == doubleTargetScale.join (0, int64_t(1e18),    lscale));        // uses double for internal log10 computation
          CHECK (180 == doubleTargetScale.join (0, int64_t(1e18)-10, lscale));        // offset chosen large enough to prevail in int64_t but not in double
          CHECK (180 ==   longTargetScale.join (0, int64_t(1e18),    lscale));        // 64bit integral scales use long-double for log10 computation internally
          CHECK (179 ==   longTargetScale.join (0, int64_t(1e18)-10, lscale));        // ...and thus the offset survives the log10 and is truncated down on conversion
        }
      
      
      /** @test 
       * @todo WIP 9/26 🔁 define ⟶ implement
       */
      void
      verify_NominalScale()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ParameterScale_test, "unit lib");
  
  
  
}}} // namespace lib::par::test
