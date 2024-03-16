/*
  TestHelper(Test)  -  validate the unittest support functions

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

/** @file test-helper-test.cpp
 ** unit test \ref TestHelper_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/error.hpp"
#include "lib/util-foreach.hpp"
#include "lib/format-cout.hpp"

#include <boost/algorithm/string.hpp>
#include <functional>
#include <string>

using util::for_each;
using lumiera::Error;
using LERR_(EXCEPTION);
using LERR_(ASSERTION);
using lib::time::TimeVar;
using lib::time::Time;

using boost::algorithm::is_lower;
using boost::algorithm::is_digit;
using std::function;
using std::string;


namespace lib {
namespace test{
namespace test{
  
  template<class T>
  class Wrmrmpft
    {
      T tt_;
    };
  
  struct Murpf { };
  
  
  void doThrow() { throw Error("because I feel like it"); }
  int dontThrow() { return 2+2; }
  
  
  /*********************************************//**
   * verifies the proper working of helper functions
   * frequently used within the Lumiera testsuite.
   * @see test-helper.hpp
   */
  class TestHelper_test : public Test
    {
      void
      run (Arg)
        {
          checkGarbageStr();
          checkTypeDisplay();
          checkThrowChecker();
          checkLocalManipulation();
        }
      
      
      /** @test prints "sizeof()" including some type name. */
      void
      checkTypeDisplay ()
        {
          std::cout << "Displaying types and sizes....\n";
          
          typedef Wrmrmpft<Murpf>   Wrmpf1;
          typedef Wrmrmpft<char[2]> Wrmpf2;
          typedef Wrmrmpft<char[3]> Wrmpf3;
          
          Wrmpf1 rmpf1;
          Wrmpf2 rmpf2;
          Wrmpf3 rmpf3;
          Murpf murpf;
          
          CHECK (1 == sizeof (rmpf1));
          CHECK (2 == sizeof (rmpf2));
          CHECK (3 == sizeof (rmpf3));
          
          cout << showSizeof<char>("just a char") << endl;
          cout << showSizeof(murpf)               << endl;
          cout << showSizeof(rmpf1)               << endl;
          cout << showSizeof(rmpf2)               << endl;
          cout << showSizeof<Wrmpf3>()            << endl;
          cout << showSizeof(size_t(42),
                             string{"Universe"})  << endl;
          
          // all the following ways to refer to an object are equivalent...
          Wrmpf1 *p1 = &rmpf1;
          Wrmpf1 *p2 = 0;
          cout << showSizeof(p1)  << endl;
          cout << showSizeof(p2)  << endl;
          
          Wrmpf1 & r = rmpf1;
          Wrmpf1 const& cr = r;
          Wrmpf1 const* cp = &r;
          
          cout << showSizeof(r)  << endl;
          cout << showSizeof(cr)  << endl;
          cout << showSizeof(cp)  << endl;
        }
      
      
      
      
      void
      checkGarbageStr()
        {
          string garN = randStr(0);
          CHECK (0 == garN.size());
          
          typedef function<bool(string::value_type)> ChPredicate;
          ChPredicate is_OK (is_lower() || is_digit());
          
          string garM = randStr(1000000);
          for_each (garM, is_OK);
          
          cout << randStr(80) << endl;
        }
      
      
      /** @test check the VERIFY_ERROR macro,
       *        which ensures a given error is raised.
       */
      void
      checkThrowChecker()
        {
          // verify the exception is indeed raised
          VERIFY_ERROR (EXCEPTION, doThrow() );
          
#if false ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
          // and when actually no exception is raised, this is an ASSERTION failure
          VERIFY_ERROR (ASSERTION, VERIFY_ERROR (EXCEPTION, dontThrow() ));
#endif    ///////////////////////////////////////////////////////////////////////////////////////////////TICKET #537 : restore throwing ASSERT
        }
      
      
      /** @test check a local manipulations,
       *        which are undone when leaving the scope.
       */
      void
      checkLocalManipulation()
        {
          int equilibrium = 42;
          {
            // manipulate the value temporarily...
            TRANSIENTLY(equilibrium) = 49;
            
            CHECK (49 == equilibrium);
          }
          CHECK (42 == equilibrium);
          
          
          TimeVar day_of_reckoning{Time{555,5}};
          try
            {
              TRANSIENTLY(equilibrium) = 55;
              TRANSIENTLY(day_of_reckoning) = Time::ANYTIME;
              
              CHECK (55 == equilibrium);
              CHECK (Time::ANYTIME  == day_of_reckoning);
              throw "RRRrrevenge!!!!!!!!!!!!!!!!1!!11!!";
            }
          catch(...) { }
          CHECK (42 == equilibrium);
          CHECK (Time(555,5) == day_of_reckoning);
          
          
          { // can also use λ for manipulation and clean-up
            TRANSIENTLY ([&]{ day_of_reckoning *= 2; })
               .cleanUp ([&]{ equilibrium      /= 2; });
            
            CHECK (42 == equilibrium);             // not yet touched...
            CHECK (Time(110,11) == day_of_reckoning);
          }
          CHECK (Time(110,11) == day_of_reckoning);
          CHECK (21 == equilibrium);
        }
    };
  
  LAUNCHER (TestHelper_test, "unit common");
  
  
}}} // namespace lib::test::test

