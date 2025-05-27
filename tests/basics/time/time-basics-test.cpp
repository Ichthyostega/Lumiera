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
#include "lib/test/test-helper.hpp"
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
  using time::raw_time_64;
  
  
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
          checkComponentBreakdown();
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
          
          raw_time_64 rat(var);
          CHECK (!(rat == ref) );
          CHECK ( (rat != ref) );
          CHECK ( (rat >= ref) );
          CHECK (!(rat <= ref) );
          CHECK (!(rat <  ref) );
          CHECK ( (rat >  ref) );
          
          CHECK ( (var == rat) );
          CHECK (!(var != rat) );
          CHECK ( (var >= rat) );
          CHECK ( (var <= rat) );
          CHECK (!(var <  rat) );
          CHECK (!(var >  rat) );
        }
      
      
      void
      checkComponentBreakdown()
        {
          Time t1(2008,0);
          CHECK (t1 == "0:00:02.008"_expect);
          
          Time t2(2008,88);
          CHECK (t2 == "0:01:30.008"_expect);
          
          Time t3(2008,118,58);
          CHECK (t3 == "1:00:00.008"_expect);
          
          Time t4(t2 - t3);
          CHECK (t4 == "-0:58:30.000"_expect);
          
          CHECK (Time::ZERO ==         "0:00:00.000"_expect);
          CHECK (Time::MAX  ==  "85401592:56:01.825"_expect);
          CHECK (Time::MIN  == "-85401592:56:01.825"_expect);
          
          
          seedRand();
          int millis = 1 + rani (999);
          int secs   =     rani  (60);
          int mins   =     rani  (60);
          int hours  =     rani (100);
          
          Time randTime(millis,secs,mins,hours);
          CHECK (Time()  < randTime);
          
          const auto TIME_SCALE_sec{lib::time::TimeValue::SCALE       };
          const auto TIME_SCALE_ms {lib::time::TimeValue::SCALE / 1000};
          
          CHECK (millis == (_raw(randTime) / TIME_SCALE_ms )      % 1000);
          CHECK (secs   == (_raw(randTime) / TIME_SCALE_sec)      % 60);
          CHECK (mins   == (_raw(randTime) / TIME_SCALE_sec / 60) % 60);
          CHECK (hours  ==  _raw(randTime) / TIME_SCALE_sec / 60 / 60);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeBasics_test, "unit common");
  
  
  
}} // namespace lib::test
