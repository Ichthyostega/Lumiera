/*
  TimeWrapper(Test)  -  working with gavl_time_t values in C++...
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "lib/test/run.hpp"

#include "lib/util.hpp"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <cstdlib>

using boost::lexical_cast;
using util::isnil;
using std::rand;
using std::cout;
using std::endl;


#include "lib/lumitime.hpp"
#include "lib/lumitime-fmt.hpp"



namespace lumiera{
namespace test   {
  
  
  /********************************************
   * @test sanity of the C++ time wrapper.
   */
  class TimeWrapper_test : public Test
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
          Time zero;
          Time one (1);
          Time max (Time::MAX);
          Time min (Time::MIN);
          
          Time val (ref);
          
          val += Time(2);
          val *= 2;
          CHECK (zero == (val - 2*(ref + Time(2))) );
          
          val = ref;
          CHECK (zero == (val - ref));
        }
      
      
      void
      checkComparisons (Time const& ref)
        {
          Time zero;
          Time max (Time::MAX);
          Time min (Time::MIN);
          
          CHECK (zero == Time(0));
          CHECK (min < zero);
          CHECK (max > zero);
          
          Time val (ref);
          CHECK ( (val == ref) );
          CHECK (!(val != ref) );
          CHECK ( (val >= ref) );
          CHECK ( (val <= ref) );
          CHECK (!(val <  ref) );
          CHECK (!(val >  ref) );
          
          val += Time(2);
          CHECK (!(val == ref) );
          CHECK ( (val != ref) );
          CHECK ( (val >= ref) );
          CHECK (!(val <= ref) );
          CHECK (!(val <  ref) );
          CHECK ( (val >  ref) );
          
          gavl_time_t gat(val);
          CHECK (!(gat == ref) );
          CHECK ( (gat != ref) );
          CHECK ( (gat >= ref) );
          CHECK (!(gat <= ref) );
          CHECK (!(gat <  ref) );
          CHECK ( (gat >  ref) );
          
          CHECK ( (val == gat) );
          CHECK (!(val != gat) );
          CHECK ( (val >= gat) );
          CHECK ( (val <= gat) );
          CHECK (!(val <  gat) );
          CHECK (!(val >  gat) );
        }
      
      
      void
      checkComponentAccess()
        {
          int millis = rand() % 1000;
          int secs   = rand() % 60;
          int mins   = rand() % 60;
          int hours  = rand() % 100;
          
          Time time(millis,secs,mins,hours);
          CHECK (millis == time.getMillis());
          CHECK (secs   == time.getSecs());
          CHECK (mins   == time.getMins());
          CHECK (hours  == time.getHours());
          cout << time << endl;
          
          Time t2(2008,0);
          cout << t2 << endl;
          CHECK ( 8 == t2.getMillis());
          CHECK ( 2 == t2.getSecs());
          CHECK ( 0 == t2.getMins());
          CHECK ( 0 == t2.getHours());
          
          Time t3(2008,88);
          cout << t3 << endl;
          CHECK ( 8 == t3.getMillis());
          CHECK (30 == t3.getSecs());
          CHECK ( 1 == t3.getMins());
          CHECK ( 0 == t3.getHours());
          
          Time t4(2008,118,58);
          cout << t4 << endl;
          CHECK ( 8 == t4.getMillis());
          CHECK ( 0 == t4.getSecs());
          CHECK ( 0 == t4.getMins());
          CHECK ( 1 == t4.getHours());
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeWrapper_test, "unit common");
  
  
  
}} // namespace lumiera::test
