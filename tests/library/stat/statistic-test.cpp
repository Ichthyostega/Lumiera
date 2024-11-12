/*
  Statistic(Test)  -  validate simple statistic calculations

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file statistic-test.cpp
 ** unit test \ref Statistic_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/stat/statistic.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/format-util.hpp"
#include "lib/random.hpp"
#include "lib/util.hpp"
#include "lib/format-cout.hpp"            ///////////////////////TODO
#include "lib/test/diagnostic-output.hpp" ///////////////////////TODO



namespace lib {
namespace stat{
namespace test{
  
  namespace {
    const size_t NUM_POINTS = 1'000;
  }

  using lib::test::roughEQ;
  using util::isnil;
  using error::LUMIERA_ERROR_INVALID;
  
  
  /**************************************************************//**
   * @test verifies the proper working of statistic helper functions.
   *     - calculate mean and standard derivation
   *     - one-dimensional linear regression
   * @see DataCSV_test.hpp
   * @see statistic.hpp
   */
  class Statistic_test : public Test
    {
      void
      run (Arg)
        {
          demonstrate_DataSpan();
          check_baseStatistics();
          check_wightedLinearRegression();
          check_TimeSeriesLinearRegression();
        }
      
      
      
      /** @test a simplified preview on C++20 ranges */
      void
      demonstrate_DataSpan()
        {
          auto dat = VecD{0,1,2,3,4,5};
          
          DataSpan all{dat};
          CHECK (not isnil (all));
          CHECK (dat.size() == all.size());
          
          auto i = all.begin();
          CHECK (i != all.end());
          CHECK (0 == *i);
          ++i;
          CHECK (1 == *i);
          
          DataSpan innr{*i, dat.back()};
          CHECK (util::join(innr) == "1, 2, 3, 4"_expect);
          CHECK (2 == innr.at(1));
          CHECK (2 == innr[1]);
          CHECK (4 == innr[3]);
          CHECK (5 == innr[4]); // »undefined behaviour«
          
          VERIFY_ERROR (INVALID, innr.at(4) )
          
          CHECK (1+2+3+4 == lib::explore(innr).resultSum());
        }
      
      
      /** @test helpers to calculate mean and standard derivation */
      void
      check_baseStatistics ()
        {
          auto dat = VecD{4,2,5,8,6};
          DataSpan all = lastN(dat, dat.size());
          DataSpan rst = lastN(dat, 4);
          CHECK (2 == *rst.begin());
          CHECK (4 == rst.size());
          CHECK (5 == all.size());
          
          CHECK (5.0 == average (all));
          CHECK (5.25 == average(rst));
          
          // Surprise : divide by N-1 since it is a guess for the real standard derivation
          CHECK (sdev (all, 5.0) == sqrt(20/(5-1)));
          
          CHECK (5.0  == averageLastN (dat,20));
          CHECK (5.0  == averageLastN (dat, 5));
          CHECK (5.25 == averageLastN (dat, 4));
          CHECK (7.0  == averageLastN (dat, 2));
          CHECK (6.0  == averageLastN (dat, 1));
          CHECK (0.0  == averageLastN (dat, 0));
        }
      
      
      /** @test attribute a weight to each data point going into linear regression
       *      - using a simple scenario with three points
       *      - a line with gradients would run through the end points (1,1) ⟶ (5,5)
       *      - but we have a middle point, offset by -2 and with double weight
       *      - thus the regression line is overall shifted by -1
       *      - standard derivation is √3 and correlation 81%
       *        (both plausible and manually checked
       */
      void
      check_wightedLinearRegression()
        {
          RegressionData points{{1,1, 1}
                               ,{5,5, 1}
                               ,{3,1, 2}
                               };
          
          auto [socket,gradient
               ,predicted,deltas
               ,correlation
               ,maxDelta
               ,sdev]          = computeLinearRegression (points);
          
          CHECK (socket ==  -1);
          CHECK (gradient == 1);
          CHECK (util::join (predicted) == "0, 4, 2"_expect );
          CHECK (util::join (deltas)    == "1, 1, -1"_expect );
          CHECK (maxDelta               == 1);
          CHECK (correlation            == "0.81649658"_expect );
          CHECK (sdev                   == "1.7320508"_expect );
        }
      
      
      
      /** @test regression over a series of measurement data
       *      - use greater mount of data generated with randomness
       *      - actually a power function is  _hidden in the data_
       */
      void
      check_TimeSeriesLinearRegression()
        {
          auto dirt = []        { return ranRange(-0.5,+0.5); };
          auto fun = [&](uint i){ auto x = double(i)/NUM_POINTS;
                                  return x*x;
                                };
          VecD data;
          data.reserve (NUM_POINTS);
          for (uint i=0; i<NUM_POINTS; ++i)
            data.push_back (fun(i) + dirt());
          
          auto [socket,gradient,correlation] = computeTimeSeriesLinearRegression (data);
          
          // regression line should roughly connect 0 to 1,
          // yet slightly shifted downwards, cutting through the parabolic curve
          CHECK (roughEQ (gradient*NUM_POINTS, 1,    0.08));
          CHECK (roughEQ (socket,             -0.16, 0.3 ));
          CHECK (correlation > 0.65);
        }
    };
  
  LAUNCHER (Statistic_test, "unit calculation");
  
  
}}} // namespace lib::stat::test

