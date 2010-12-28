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

#include <boost/lexical_cast.hpp>
#include <iostream>
//#include <cstdlib>
#include <string>

using boost::lexical_cast;
using util::isnil;
//using std::rand;
using std::cout;
using std::endl;
using std::string;


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
      gavl_time_t
      random_or_get (Arg arg)
        {
          if (isnil(arg))
            return 1 + (rand() % 10000);
          else
            return lexical_cast<gavl_time_t> (arg[1]);
        }
      
      
      virtual void
      run (Arg arg) 
        {
          TimeValue ref (random_or_get(arg));
          
          checkBasicTimeValues (ref);
          checkMutableTime (ref);
          createOffsets (ref);
          buildDuration (ref);
          buildTimeSpan (ref);
        } 
      
      
      /** @test creating some time values and performing trivial comparisons.
       *  @note you can't do much beyond that, because TimeValues as such
       *        are a "dead end": they are opaque and can't be altered.
       */
      void
      checkBasicTimeValues (TimeValue org)
        {
          TimeValue zero;
          TimeValue one (1);
          TimeValue max (Time::MAX);
          TimeValue min (Time::MIN);
          
          TimeValue val (org);
          
          CHECK (zero == zero);
          CHECK (zero <= zero);
          CHECK (zero >= zero);
          
          CHECK (zero < one);
          CHECK (min  < max);
          CHECK (min  < val);
          CHECK (val  < max);
          
          // mixed comparisons with raw numeric time
          gavl_time_t g2 (-2);
          CHECK (zero > g2);
          CHECK (one  > g2);
          CHECK (one >= g2);
          CHECK (g2  < max);
          
          CHECK (!(g2 > max));
          CHECK (!(g2 < min));
        }
      
      
      /** @test time variables can be used for the typical calculations,
       *        like summing and subtracting values, as well as multiplication
       *        with a scale factor. Additionally, the raw time value is
       *        accessible by conversion.
       */
      void
      checkMutableTime (TimeValue org)
        {
          TimeVar zero;
          TimeVar one = TimeValue(1);
          TimeVar two = TimeValue(2);
          
          TimeVar var (org);
          
          var += two;
          var *= 2;
          CHECK (zero == (var - 2*(org + two)) );
          
          // the transient vars caused no side-effects
          CHECK (var == 2*two + org + org);
          CHECK (two == TimeValue(2));
          
          var = org;   // assign new value
          CHECK (zero == (var - org));
          
          CHECK (zero < one);
          CHECK (one  < two);
          CHECK (var < Time::MAX);
          CHECK (var > Time::MIN);
          
          gavl_time_t raw (var);
          CHECK (raw == org);
          CHECK (raw >  org - two);
        }
      
      
      void
      createOffsets (TimeValue org)
        {
          TimeValue four(4);
          TimeValue five(5);
          
          Offset off5 (five);
          CHECK (0 < off5);
          
          TimeVar point(org);
          point += off5;
          CHECK (org < point);
          
          Offset reverse(point,org);
          CHECK (reverse < off5);
          CHECK (reverse.abs() == off5);
          
          CHECK (0 == off5 + reverse);
          
          // chaining and copy construction
          Offset off9 (off5 + Offset(four));
          CHECK (9 == off9);
        }
      
      
      void
      buildDuration (TimeValue org)
        {
          TimeValue zero;
          TimeVar point(org);
          point += TimeValue(5);
          CHECK (org < point);
          
          Offset backwards(point,org);
          CHECK (backwards < zero);
          
          Duration distance(backwards);
          CHECK (distance > zero);
          CHECK (distance == backwards.abs());
          
          point = backwards;
          point *= 2;
          CHECK (point < zero);
          CHECK (point < backwards);
          
          CHECK (distance + point < zero);      // using the duration as offset
          CHECK (distance == backwards.abs()); //  while this didn't alter the duration as such
        }
      
      
      void
      buildTimeSpan (TimeValue org)
        {
          TimeValue zero;
          TimeValue five(5);
          
          TimeSpan interval (Time(org), Duration(Offset (org,five)));
          
          // the time span behaves like a time
          CHECK (org == interval);
          CHECK (string(Time(org)) == string(interval));
          
          // can get the length by direct conversion
          Duration theLength(interval);
          CHECK (theLength == Offset(org,five).abs());
          
          Time endpoint = interval.getEnd();
          CHECK (Offset(interval,endpoint) == Offset(org,five).abs());
          
          cout << "Interval: " << string(interval) 
               << " Endpoint: " << string(endpoint) << endl; 
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeValue_test, "unit common");
  
  
  
}}} // namespace lib::time::test
