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
#include "test/diagnostic-output.hpp"//////////////////TODO
//#include "lib/format-cout.hpp"
//#include "lib/format-string.hpp"

using ::Test;
//using util::isnil;
//using util::_Fmt;
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
   * @todo add coverage for floating-point numbers
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
          
          verifyFloatTypes();
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
          
          I dist = LOWER - _MIN<I>;  // guaranteed to be positive and representable in-domain
          I step = (dist / 5) * 5;   // the next applicable grid point
          I excess = dist - step;
          I offset = 5 - excess;     // flip orientation (since the input val lies below LOWER)
          
          CHECK (LOWER + offset == cyc(_MIN<I>) );
        }
      
      void
      verifyFloatTypes()
        {
          double maxDom = _MAX<double>;
          double super = _MAX<double> - _MIN<double>;
SHOW_EXPR(maxDom)
SHOW_EXPR(super)
SHOW_EXPR(_EPS<double>)
SHOW_EXPR(limit_cyclicWrap<double>(1))
SHOW_EXPR(limit_cyclicWrap<double>(1024))
SHOW_EXPR(limit_cyclicWrap<double>(1.0/1024))
SHOW_EXPR(limit_cyclicWrap<double>(_EPS<double>))
SHOW_EXPR(limit_cyclicWrap<double>(maxDom))
          
          double limit5 = limit_cyclicWrap<double>(5);
          double atLimit = limit5 * (1-_EPS<double>);
          double stepLim = std::floor (limit5 / 5) * 5;
SHOW_EXPR(limit5*_EPS<double>)
SHOW_EXPR(limit5)
SHOW_EXPR(stepLim)
SHOW_EXPR(atLimit)
          
SHOW_EXPR(cyclicWrap (10.0 ,0.0, 5.0))
SHOW_EXPR(cyclicWrap (limit5    ,0.0, 5.0))
SHOW_EXPR(cyclicWrap (atLimit   ,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim   ,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim-5 ,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim+1 ,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim+2 ,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim+3 ,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim+4 ,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim+5 ,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim+6 ,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim+10,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim+20,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim+30,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim+40,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim+50,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim+100,0.0, 5.0))
SHOW_EXPR(cyclicWrap (stepLim+101,0.0, 5.0))

SHOW_EXPL(cyclicWrap (limit5    ,10.0, 15.0))
SHOW_EXPL(cyclicWrap (limit5+1  ,10.0, 15.0))
          double epsStep = _MAX<double>*_EPS<double>;
          double toxicMin = _MIN<double> + 2*epsStep;
          double toxicMax = epsStep;
          double toxicPeriod = toxicMax -  toxicMin;
SHOW_EXPR(epsStep)
SHOW_EXPR(toxicMin)
SHOW_EXPR(toxicMax)
SHOW_EXPR(toxicPeriod)
SHOW_EXPR(cyclicWrap<double>(0, toxicMin,toxicMax) )
SHOW_EXPR(cyclicWrap<double>(1, toxicMin,toxicMax) )
SHOW_EXPR(cyclicWrap<double>(1*epsStep, toxicMin,toxicMax) )
SHOW_EXPR(cyclicWrap<double>(2*epsStep, toxicMin,toxicMax) )
SHOW_EXPR(cyclicWrap<double>(3*epsStep, toxicMin,toxicMax) )
SHOW_EXPR(cyclicWrap<double>(0, 1, _MAX<double>) )
SHOW_EXPR(cyclicWrap<double>(1, 1, _MAX<double>) )
SHOW_EXPR(cyclicWrap<double>(2, 1, _MAX<double>) )
SHOW_EXPR(cyclicWrap<double>(-1,1, _MAX<double>) )
SHOW_EXPR(cyclicWrap<double>(-epsStep,1, _MAX<double>) )
SHOW_EXPR(1 == cyclicWrap<double>(1, 1, _MAX<double>))
SHOW_EXPR(2 == cyclicWrap<double>(2, 1, _MAX<double>))
SHOW_EXPR(1 == cyclicWrap<double>(-1,1, _MAX<double>) )
          double wrapped = cyclicWrap (-epsStep,double(1), _MAX<double>);
SHOW_EXPR(1 < wrapped)
SHOW_EXPR(wrapped < _MAX<double>)
          
          auto cyc = [&](double val){ return cyclicWrap (val, double(-10),double(-5)); };

          CHECK (almostEqual (double(-10), cyc(0)));
          CHECK (almostEqual (double( -9), cyc(1)));
          CHECK (almostEqual (double( -8), cyc(2)));
          CHECK (almostEqual (double( -7), cyc(3)));
          CHECK (almostEqual (double( -6), cyc(4)));
          CHECK (almostEqual (double(-10), cyc(5)));
          CHECK (almostEqual (double( -9), cyc(6)));
          CHECK (almostEqual (double( -8), cyc(7)));
          CHECK (almostEqual (double( -7), cyc(8)));
          CHECK (almostEqual (double( -6), cyc(9)));
          CHECK (almostEqual (double(-10), cyc(105)));
          
          CHECK (almostEqual (double(5)/2, limit5 * _EPS<double>));
          CHECK (limit5 + 1 == limit5);
          
          CHECK (1 == cyclicWrap<double>( 1, 1,_MAX<double>) );
          CHECK (2 == cyclicWrap<double>( 2, 1,_MAX<double>) );
          CHECK (1 == cyclicWrap<double>(-1, 1,_MAX<double>) );
          CHECK (      1 < wrapped     );
          CHECK (wrapped < _MAX<double>);
        }
    };
  
  
  
  LAUNCHER (CyclicWrap_test, "unit common");
  
  
}} // namespace util::test
