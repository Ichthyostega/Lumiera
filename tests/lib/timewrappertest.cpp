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



namespace lumiera {
  namespace test {
    
    
    /********************************************
     * @test sanity of the C++ time wrapper.
     */
    class TimeWrapper_test : public Test
      {
        virtual void
        run (Arg arg) 
          {
            int64_t refval= isnil(arg)?  1 : lexical_cast<int64_t> (arg[1]);
            
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
            ASSERT (zero == (val - 2*(ref + Time(2))) );
            
            val = ref;
            ASSERT (zero == (val - ref));
          }
        
        
        void
        checkComparisons (Time const& ref)
          {
            Time zero;
            Time max (Time::MAX);
            Time min (Time::MIN);
            
            ASSERT (zero == Time(0));
            ASSERT (min < zero);
            ASSERT (max > zero);
            
            Time val (ref);
            ASSERT ( (val == ref) );
            ASSERT (!(val != ref) );
            ASSERT ( (val >= ref) );
            ASSERT ( (val <= ref) );
            ASSERT (!(val <  ref) );
            ASSERT (!(val >  ref) );
            
            val += Time(2);
            ASSERT (!(val == ref) );
            ASSERT ( (val != ref) );
            ASSERT ( (val >= ref) );
            ASSERT (!(val <= ref) );
            ASSERT (!(val <  ref) );
            ASSERT ( (val >  ref) );
            
            gavl_time_t gat(val);
            ASSERT (!(gat == ref) );
            ASSERT ( (gat != ref) );
            ASSERT ( (gat >= ref) );
            ASSERT (!(gat <= ref) );
            ASSERT (!(gat <  ref) );
            ASSERT ( (gat >  ref) );
            
            ASSERT ( (val == gat) );
            ASSERT (!(val != gat) );
            ASSERT ( (val >= gat) );
            ASSERT ( (val <= gat) );
            ASSERT (!(val <  gat) );
            ASSERT (!(val >  gat) );
          }
        
        
        void
        checkComponentAccess()
          {
            int millis = rand() % 1000;
            int secs   = rand() % 60;
            int mins   = rand() % 60;
            int hours  = rand() % 100;
            
            Time time(millis,secs,mins,hours);
            ASSERT (millis == time.getMillis());
            ASSERT (secs   == time.getSecs());
            ASSERT (mins   == time.getMins());
            ASSERT (hours  == time.getHours());
            cout << time << endl;
            
            Time t2(2008,0);
            cout << t2 << endl;
            ASSERT ( 8 == t2.getMillis());
            ASSERT ( 2 == t2.getSecs());
            ASSERT ( 0 == t2.getMins());
            ASSERT ( 0 == t2.getHours());
            
            Time t3(2008,88);
            cout << t3 << endl;
            ASSERT ( 8 == t3.getMillis());
            ASSERT (30 == t3.getSecs());
            ASSERT ( 1 == t3.getMins());
            ASSERT ( 0 == t3.getHours());
            
            Time t4(2008,118,58);
            cout << t4 << endl;
            ASSERT ( 8 == t4.getMillis());
            ASSERT ( 0 == t4.getSecs());
            ASSERT ( 0 == t4.getMins());
            ASSERT ( 1 == t4.getHours());
          }

      };
    
    
    /** Register this test class... */
    LAUNCHER (TimeWrapper_test, "unit common");
    
    
    
  } // namespace test

} // namespace lumiera
