/*
  UtilFloordiv(Test)  -  verify integer rounding function

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

/** @file util-floordiv-test.cpp
 ** unit test \ref UtilFloordiv_test
 */


#include "lib/test/run.hpp"
#include "lib/util-quant.hpp"
#include "lib/util.hpp"

#include "lib/format-cout.hpp"
#include "lib/format-string.hpp"

#include <cmath>
#include <time.h>
#include <vector>

using ::Test;
using util::isnil;
using util::_Fmt;


namespace util {
namespace test {
  
  
  
  namespace{ // Test data and operations
    
    const uint NUM_ELMS_PERFORMANCE_TEST = 50000000;
    const uint NUMBER_LIMIT              =  1 << 30;
    
    typedef std::vector<int> VecI;
    
    VecI
    buildTestNumberz (uint cnt)
      {
        VecI data;
        for (uint i=0; i<cnt; ++i)
          {
            int someNumber = -int(NUMBER_LIMIT)+rani(2*NUMBER_LIMIT);
            if (!someNumber) someNumber -= 1 +  rani(NUMBER_LIMIT);
            
            data.push_back (someNumber);
          }
        return data;
      }
    
    
    /** the built-in integer division operator,
     *  packaged as inline function for timing comparison
     */
    inline long
    integerDiv (long num, long den)
      {
        return num / den;
      }
    
    
    /** an alternate formulation,
     *  which turned out to perform slightly worse
     */
    inline long
    floordiv_alternate (long num, long den)
      {
        ldiv_t res = ldiv(num,den);
        return (0 >= res.quot && res.rem)? res.quot-1
                                         : res.quot;
      }
    
  } // (End) test data and operations
  
  
  
  /******************************************************************//**
   * @test Evaluate a custom built integer floor function.
   *       Also known as Knuth's floor division.
   *       This function is crucial for Lumiera's rule of quantisation
   *       of time values into frame intervals. This rule requires time
   *       points to be rounded towards the next lower frame border always,
   *       irrespective of the relation to the actual time origin.
   *       Contrast this to the built-in integer division operator, which
   *       truncates towards zero.
   * 
   * @note if invoked with an non empty parameter, this test performs
   *       some interesting timing comparisons, which initially were
   *       used to tweak the implementation a bit.
   * @see lib/util.hpp
   * @see QuantiserBasics_test
   */
  class UtilFloordiv_test : public Test
    {
      
      virtual void
      run (Arg arg)
        {
          seedRand();
          
          verifyBehaviour ();
          
          verifyIntegerTypes<int>();
          verifyIntegerTypes<long>();
          verifyIntegerTypes<short>();
          verifyIntegerTypes<int64_t>();
          verifyIntegerTypes<long long int>();
          
          if (!isnil (arg))
            runPerformanceTest();
        }
      
      
      void
      verifyBehaviour ()
        {
          CHECK ( 3 == floordiv ( 12,4));
          CHECK ( 2 == floordiv ( 11,4));
          CHECK ( 2 == floordiv ( 10,4));
          CHECK ( 2 == floordiv (  9,4));
          CHECK ( 2 == floordiv (  8,4));
          CHECK ( 1 == floordiv (  7,4));
          CHECK ( 1 == floordiv (  6,4));
          CHECK ( 1 == floordiv (  5,4));
          CHECK ( 1 == floordiv (  4,4));
          CHECK ( 0 == floordiv (  3,4));
          CHECK ( 0 == floordiv (  2,4));
          CHECK ( 0 == floordiv (  1,4));
          CHECK ( 0 == floordiv (  0,4));
          CHECK (-1 == floordiv (- 1,4));
          CHECK (-1 == floordiv (- 2,4));
          CHECK (-1 == floordiv (- 3,4));
          CHECK (-1 == floordiv (- 4,4));
          CHECK (-2 == floordiv (- 5,4));
          CHECK (-2 == floordiv (- 6,4));
          CHECK (-2 == floordiv (- 7,4));
          CHECK (-2 == floordiv (- 8,4));
          CHECK (-3 == floordiv (- 9,4));
          CHECK (-3 == floordiv (-10,4));
          CHECK (-3 == floordiv (-11,4));
          CHECK (-3 == floordiv (-12,4));
        }
      
      
      template<typename I>
      void
      verifyIntegerTypes ()
        {
          I n,d,expectedRes;
          
          for (int i=-12; i <= 12; ++i)
            {
              n = i;
              d = 4;
              expectedRes = floordiv (i,4);
              CHECK (floordiv(n,d) == expectedRes);
            }
        }
      
      
      
      /** @test timing measurements to compare implementation details.
       *  This test uses a sequence of random integers, where the values
       *  used as denominator are ensured not to be zero.
       *  
       * \par measurement results
       * My experiments (AMD Athlon-64 4200 X2) gave me
       * the following timing measurements in nanoseconds:
       * 
       * Verification..........  127.7
       * Integer_div...........  111.7
       * double_floor..........   74.8
       * floordiv_int..........  112.7
       * floordiv_long.........  119.8
       * floordiv_int64_t......  121.4
       * floordiv_long_alt.....  122.7
       * 
       * These figures are the average of 6 runs with 50 million
       * iterations each (as produced by this function)
       * 
       * \par conclusions
       * The most significant result is the striking performance of the
       * fpu based calculation. Consequently, integer arithmetics should
       * only be used when necessary due to resolution requirements, as
       * is the case for int64_t based Lumiera Time values, which require
       * a precision beyond the 16 digits provided by double.
       * Besides that, we can conclude that the additional tests and
       * adjustment of the custom floordiv only creates a slight overhead
       * compared to the built-in integer div function. An oddity to note
       * is the slightly better performance of long over int64_t. Also,
       * the alternative formulation of the function, which uses the
       * \c fdiv() function also to divide the positive results,
       * performs only slightly worse. So this implementation
       * was chosen mainly because it seems to state its
       * intent more clearly in code.  
       */
      void
      runPerformanceTest ()
        {
          VecI testdata = buildTestNumberz (2*NUM_ELMS_PERFORMANCE_TEST);
          typedef VecI::const_iterator I;
          
          clock_t start(0), stop(0);
          _Fmt resultDisplay{"timings(%s)%|30T.|%5.3fsec\n"};
          
#define   START_TIMINGS start=clock();          
#define   DISPLAY_TIMINGS(ID) \
          stop = clock();      \
          cout << resultDisplay % STRINGIFY (ID) % (double(stop-start)/CLOCKS_PER_SEC) ;
          
          START_TIMINGS
          for (I ii =testdata.begin(); ii!=testdata.end(); )
            {
              int num = *ii;
              ++ii;
              int den = *ii;
              ++ii;
              CHECK (floor(double(num)/den) == floordiv(num,den));
            }
          DISPLAY_TIMINGS (Verification)
          
          START_TIMINGS
          for (I ii =testdata.begin(); ii!=testdata.end(); )
            {
              integerDiv (*ii++, *ii++);
            }
          DISPLAY_TIMINGS (Integer_div)
          
          START_TIMINGS
          for (I ii =testdata.begin(); ii!=testdata.end(); )
            {
              floor (double(*ii++) / *ii++);
            }
          DISPLAY_TIMINGS (double_floor)
          
          START_TIMINGS
          for (I ii =testdata.begin(); ii!=testdata.end(); )
            {
              floordiv (*ii++, *ii++);
            }
          DISPLAY_TIMINGS (floordiv_int)
          
          START_TIMINGS
          for (I ii =testdata.begin(); ii!=testdata.end(); )
            {
              floordiv (long(*ii++), long(*ii++));
            }
          DISPLAY_TIMINGS (floordiv_long)
          
          START_TIMINGS
          for (I ii =testdata.begin(); ii!=testdata.end(); )
            {
              floordiv (int64_t(*ii++), int64_t(*ii++));
            }
          DISPLAY_TIMINGS (floordiv_int64_t)
          
          START_TIMINGS
          for (I ii =testdata.begin(); ii!=testdata.end(); )
            {
              floordiv_alternate (*ii++, *ii++);
            }
          DISPLAY_TIMINGS (floordiv_long_alt)
        }
    };
  
  
  
  
  LAUNCHER (UtilFloordiv_test, "unit common");
  
  
}} // namespace util::test
