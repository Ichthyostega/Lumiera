/*
  TimeValue(Test)  -  working with time values and time intervals in C++...

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/time/timevalue.hpp"
#include "lib/util.hpp"

//#include <boost/lexical_cast.hpp>
//#include <iostream>
//#include <cstdlib>

//using boost::lexical_cast;
//using util::isnil;
//using std::rand;
//using std::cout;
//using std::endl;


namespace lib {
namespace time{
namespace test{
  
  
  /********************************************************
   * @test verify handling of time values, time intervals.
   *       - creating times and time intervals
   *       - comparisons
   *       - time arithmetics
   */
  class TimeValue_test : public Test
    {
      virtual void
      run (Arg arg) 
        {
          long refval= isnil(arg)?  1 : lexical_cast<long> (arg[1]);
          
          Time ref (refval);
          
          checkBasics (ref);
          checkComparisons (ref);
          checkComponentAccess();
        } 
      
      
      void
      checkBasics (Time const& ref)
        {
        }
      
      
      void
      checkComparisons (Time const& ref)
        {
        }
      
      
      void
      checkComponentAccess()
        {
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeValue_test, "unit common");
  
  
  
}}} // namespace lib::time::test
