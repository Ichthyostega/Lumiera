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


#include "common/test/run.hpp"

#include "common/util.hpp"
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using util::isnil;


#include "common/lumitime.hpp"



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
      };
    
    
    /** Register this test class... */
    LAUNCHER (TimeWrapper_test, "unit common");
    
    
    
  } // namespace test

} // namespace lumiera
