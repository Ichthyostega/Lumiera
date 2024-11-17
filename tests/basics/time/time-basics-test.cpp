/*
  TimeBasics(Test)  -  working with Lumiera's internal Time values

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file time-basics-test.cpp
 ** unit test \ref TimeBasics_test
 */


#include "lib/test/run.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/diagnostics.hpp"
#include "lib/format-cout.hpp"
#include "lib/random.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using util::isnil;


namespace lib {
namespace test{
  
  using time::Time;
  using time::TimeVar;
  using time::FSecs;
  
  
  /****************************************//**
   * @test sanity check of basic Time handling.
   */
  class TimeBasics_test : public Test
    {
      virtual void
      run (Arg arg) 
        {
          FSecs refval = isnil(arg)?  1 : lexical_cast<long> (arg[1]);
          
          Time org(refval);
          
          checkBasics (org);
          checkComparisons (org);
          checkComponentDiagnostics();
        } 
      
      
      void
      checkBasics (Time const& ref)
        {
          Time zero;
          Time two (FSecs(2));
          Time max (Time::MAX);
          Time min (Time::MIN);
          
          TimeVar var (ref);
          
          var += two;
          var *= 2;
          CHECK (zero == (var - 2*(ref + two)) );
          
          var = ref;
          CHECK (zero == (var - ref));
        }
      
      
      void
      checkComparisons (Time const& ref)
        {
          Time zero;
          Time max (Time::MAX);
          Time min (Time::MIN);
          
          CHECK (zero == Time());
          CHECK (min < zero);
          CHECK (max > zero);
          
          TimeVar var (ref);
          CHECK ( (var == ref) );
          CHECK (!(var != ref) );
          CHECK ( (var >= ref) );
          CHECK ( (var <= ref) );
          CHECK (!(var <  ref) );
          CHECK (!(var >  ref) );
          
          var += Time(FSecs(2));
          CHECK (!(var == ref) );
          CHECK ( (var != ref) );
          CHECK ( (var >= ref) );
          CHECK (!(var <= ref) );
          CHECK (!(var <  ref) );
          CHECK ( (var >  ref) );
          
          gavl_time_t gat(var);
          CHECK (!(gat == ref) );
          CHECK ( (gat != ref) );
          CHECK ( (gat >= ref) );
          CHECK (!(gat <= ref) );
          CHECK (!(gat <  ref) );
          CHECK ( (gat >  ref) );
          
          CHECK ( (var == gat) );
          CHECK (!(var != gat) );
          CHECK ( (var >= gat) );
          CHECK ( (var <= gat) );
          CHECK (!(var <  gat) );
          CHECK (!(var >  gat) );
        }
      
      
      void
      checkComponentDiagnostics()
        {
          seedRand();
          int millis = rani(1000);
          int secs   = rani  (60);
          int mins   = rani  (60);
          int hours  = rani (100);
          
          Time time(millis,secs,mins,hours);
          CHECK (Time()  < time);
          CHECK (millis == getMillis(time));
          CHECK (secs   == getSecs  (time));
          CHECK (mins   == getMins  (time));
          CHECK (hours  == getHours (time));
          cout << time << endl;
          
          Time t2(2008,0);
          cout << t2 << endl;
          CHECK ( 8 == getMillis(t2));
          CHECK ( 2 == getSecs  (t2));
          CHECK ( 0 == getMins  (t2));
          CHECK ( 0 == getHours (t2));
          
          Time t3(2008,88);
          cout << t3 << endl;
          CHECK ( 8 == getMillis(t3));
          CHECK (30 == getSecs  (t3));
          CHECK ( 1 == getMins  (t3));
          CHECK ( 0 == getHours (t3));
          
          Time t4(2008,118,58);
          cout << t4 << endl;
          CHECK ( 8 == getMillis(t4));
          CHECK ( 0 == getSecs  (t4));
          CHECK ( 0 == getMins  (t4));
          CHECK ( 1 == getHours (t4));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeBasics_test, "unit common");
  
  
  
}} // namespace lib::test
