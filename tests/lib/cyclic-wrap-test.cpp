/*
  CyclicWrap(Test)  -  verify cyclically wrapping number domain

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file util-floordiv-test.cpp
 ** unit test \ref CyclicWrap_test
 */


#include "test/run.hpp"
#include "lib/util-quant.hpp"
#include "lib/util.hpp"

using ::Test;
using util::isLimited;
using std::floating_point;

namespace util {
namespace test {
  
  namespace{ // Test helpers...
    
    template<typename NUM>
    constexpr NUM _MAX = std::numeric_limits<NUM>::max();
    
    template<typename NUM>
    constexpr NUM _MIN = std::numeric_limits<NUM>::lowest();
    
    template<floating_point FLO>
    constexpr FLO _EPS = std::numeric_limits<FLO>::epsilon();
    
  } // (End) Test helpers
  
  
  
  
  /******************************************************************//**
   * @test Demonstrate and verify a cyclic wrapping numeric domain.
   *     - show the wrapping behaviour with simple numbers
   *     - verify the behaviour pattern for all integer types (signed / unsigned)
   *     - verify the computation works at the very bounds of the domain
   *     - verify the most difficult path with a negative minVal,
   *       and the smallest number in the domain
   *     - show a similar simple wrapping behaviour with all float types
   *     - demonstrate the meaning of the precision limit
   *     - demonstrate aliasing and sanitised rounding errors
   * @see util::cyclicWrap
   * @see parameter-scale-test.cpp
   */
  class CyclicWrap_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleDemonstration();
          
          verifyIntegerTypes<int>();
          verifyIntegerTypes<uint>();
          verifyIntegerTypes<int8_t>();
          verifyIntegerTypes<uint8_t>();
          verifyIntegerTypes<int16_t> ();
          verifyIntegerTypes<uint16_t>();
          verifyIntegerTypes<int32_t> ();
          verifyIntegerTypes<uint32_t>();
          verifyIntegerTypes<int64_t> ();
          verifyIntegerTypes<uint64_t>();
          
          verifyFloatTypes<float>();
          verifyFloatTypes<double>();
          verifyFloatTypes<f128>();
        }
      
      
      void
      simpleDemonstration ()
        {
          CHECK (10 == cyclicWrap (10,10,15));
          CHECK (11 == cyclicWrap (11,10,15));
          CHECK (12 == cyclicWrap (12,10,15));
          CHECK (13 == cyclicWrap (13,10,15));
          CHECK (14 == cyclicWrap (14,10,15));
          CHECK (10 == cyclicWrap (15,10,15));
          CHECK (11 == cyclicWrap (16,10,15));
          CHECK (12 == cyclicWrap (17,10,15));
          CHECK (13 == cyclicWrap (18,10,15));
          CHECK (14 == cyclicWrap (19,10,15));
          CHECK (10 == cyclicWrap (20,10,15));
          
          CHECK (10 == cyclicWrap ( 0,10,15));
          CHECK (14 == cyclicWrap (-1,10,15));
        }
      
      
      template<typename I>
      void
      verifyIntegerTypes ()
        {
          const I LOWER(-100);   // note: expected to wrap for unsigned types
          const I UPPER(LOWER+5);
          
          auto cyc = [&](I val){ return cyclicWrap (val, LOWER,UPPER); };
          
          CHECK (LOWER   == cyc(LOWER+0) );
          CHECK (LOWER+1 == cyc(LOWER+1) );
          CHECK (LOWER+2 == cyc(LOWER+2) );
          CHECK (LOWER+3 == cyc(LOWER+3) );
          CHECK (LOWER+4 == cyc(LOWER+4) );
          CHECK (LOWER   == cyc(LOWER+5) );
          
          CHECK (LOWER   == cyc(LOWER-5) );
          CHECK (LOWER+1 == cyc(LOWER-4) );
          CHECK (LOWER+2 == cyc(LOWER-3) );
          CHECK (LOWER+3 == cyc(LOWER-2) );
          CHECK (LOWER+4 == cyc(LOWER-1) );
          
          
          CHECK (isLimited (LOWER, cyc(_MAX<I>), UPPER));
          CHECK (isLimited (LOWER, cyc(_MIN<I>), UPPER));
          
          CHECK (isLimited (LOWER, cyc(_MAX<I>-1), UPPER));
          CHECK (isLimited (LOWER, cyc(_MIN<I>+1), UPPER));
          
          //------ Demonstrate behaviour at lower domain bound
          //
          I dist = LOWER - _MIN<I>;  // guaranteed to be positive and representable in-domain
          I step = (dist / 5) * 5;   // the next applicable grid point
          I excess = dist - step;
          I offset = 5 - excess;     // flip orientation (since the input val lies below LOWER)
          
          CHECK (LOWER + offset == cyc(_MIN<I>) );
        }
      
      
      template<typename F>
      void
      verifyFloatTypes()
        {
          auto cyc = [&](F val){ return cyclicWrap (val, F(-10),F(-5)); };

          CHECK (almostEqual (F(-10), cyc(0))  );
          CHECK (almostEqual (F( -9), cyc(1))  );
          CHECK (almostEqual (F( -8), cyc(2))  );
          CHECK (almostEqual (F( -7), cyc(3))  );
          CHECK (almostEqual (F( -6), cyc(4))  );
          CHECK (almostEqual (F(-10), cyc(5))  );
          CHECK (almostEqual (F( -9), cyc(6))  );
          CHECK (almostEqual (F( -8), cyc(7))  );
          CHECK (almostEqual (F( -7), cyc(8))  );
          CHECK (almostEqual (F( -6), cyc(9))  );
          CHECK (almostEqual (F(-10), cyc(105)));
          
          // For each period-length, a precision limit can be computed;
          // whenever bounds or inputs approach that limit, the rounding errors
          // approach the order of magnitude of the period-length, and thus
          // computing of wrapped values becomes pointless...
          F period5 = F(5);
          F limit5  = limit_cyclicWrap<F> (period5);
          
          // one ULP-Step at the limit equals already half the period-length:
          CHECK (almostEqual (period5/2, limit5 * _EPS<F>));
          // so small steps will alias...
          CHECK (limit5 + 1 == limit5);
          
          // HOWEWER, very large periods can be handled (within numeric precision)
          CHECK (1 == cyclicWrap<F>( 1, 1,_MAX<F>) );
          CHECK (2 == cyclicWrap<F>( 2, 1,_MAX<F>) );   // note: these values are *exact* -- due to std::fmod()
          CHECK (3 == cyclicWrap<F>( 3, 1,_MAX<F>) );
          CHECK (4 == cyclicWrap<F>( 4, 1,_MAX<F>) );
          CHECK (5 == cyclicWrap<F>( 5, 1,_MAX<F>) );
          
          CHECK (1 == cyclicWrap<F>(-1, 1,_MAX<F>) );   // yet this one gets aliased with the minVal
          
          F epsStep =  _MAX<F>*_EPS<F>;
          // yet the following input is clearly below minVal and will wrap upwards...
          F wrapped = cyclicWrap (-epsStep, F(1),_MAX<F>);
          
          CHECK (      1 < wrapped);
          CHECK (wrapped < _MAX<F>);
        }
    };
  
  
  
  LAUNCHER (CyclicWrap_test, "unit common");
  
  
}} // namespace util::test
