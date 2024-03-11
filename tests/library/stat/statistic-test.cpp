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
//#include "lib/time/timevalue.hpp"
//#include "lib/error.hpp"
//#include "lib/util-foreach.hpp"
#include "lib/format-cout.hpp"            ///////////////////////TODO
#include "lib/test/diagnostic-output.hpp" ///////////////////////TODO

//#include <functional>
//#include <string>

//using util::for_each;
//using lumiera::Error;
//using lumiera::LUMIERA_ERROR_EXCEPTION;
//using lumiera::error::LUMIERA_ERROR_ASSERTION;
//using lib::time::TimeVar;
//using lib::time::Time;

//using boost::algorithm::is_lower;
//using boost::algorithm::is_digit;
//using std::function;
//using std::string;


namespace lib {
namespace stat{
namespace test{
  
  template<class T>
  class Wrmrmpft
    {
      T tt_;
    };
  
  struct Murpf { };
  
  
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
      
      
      /** @test prints "sizeof()" including some type name. */
      void
      check_baseStatistics ()
        {
        }
      
      
      
      
      void
      demonstrate_DataSpan()
        {
        }
      
      
      /** @test check the VERIFY_ERROR macro,
       *        which ensures a given error is raised.
       */
      void
      check_wightedLinearRegression()
        {
        }
      
      
      /** @test check a local manipulations,
       *        which are undone when leaving the scope.
       */
      void
      check_TimeSeriesLinearRegression()
        {
        }
    };
  
  LAUNCHER (Statistic_test, "unit calculation");
  
  
}}} // namespace lib::stat::test

