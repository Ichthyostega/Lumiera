/*
  TimeValue(Test)  -  working with time values and time intervals in C++...

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file time-value-test.cpp
 ** unit test \ref TimeValue_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <string>

using boost::lexical_cast;
using util::isnil;
using std::string;

using LERR_(BOTTOM_VALUE);

namespace lib {
namespace time{
namespace test{
  
  
  /****************************************************//**
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
            {// use random time value for all tests
              seedRand();
              return 1 + rani(10000);
            }
          else
            return lexical_cast<gavl_time_t> (arg[1]);
        }
      
      
      virtual void
      run (Arg arg) 
        {
          TimeValue ref (random_or_get(arg));
          
          checkBasicTimeValues (ref);
          checkMutableTime (ref);
          checkTimeHash (ref);
          checkTimeConvenience (ref);
          verify_invalidFramerateProtection();
          createOffsets (ref);
          buildDuration (ref);
          buildTimeSpan (ref);
          compareTimeSpan (Time(ref));
          relateTimeIntervals (ref);
          verify_extremeValues();
          verify_fractionalOffset();
        } 
      
      
      /** @test creating some time values and performing trivial comparisons.
       *  @note you can't do much beyond that, because TimeValues as such
       *        are a "dead end": they are opaque and can't be altered.
       */
      void
      checkBasicTimeValues (TimeValue org)
        {
          TimeValue zero(0);
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
          
          // unary minus will flip around origin
          CHECK (zero == -var + var);
          CHECK (zero != -var);
          CHECK (var  == org);  // unaltered
        }
      
      
      /** @test additional convenience shortcuts supported
       *        especially by the canonical Time values.
       */
      void
      checkTimeConvenience (TimeValue org)
        {
          Time o1(org);
          TimeVar v(org);
          Time o2(v);
          CHECK (o1 == o2);
          CHECK (o1 == org);
          
          // time in seconds
          Time t1(FSecs(1));
          CHECK (t1 == TimeValue(TimeValue::SCALE));
          
          // create from fractional seconds
          FSecs halve(1,2);
          CHECK (0.5 == boost::rational_cast<double> (halve));
          Time th(halve);
          CHECK (th == TimeValue(TimeValue::SCALE/2));
          
          Time tx1(500,0);
          CHECK (tx1 == th);
          Time tx2(1,2);
          CHECK (tx2 == TimeValue(2.001*TimeValue::SCALE));
          Time tx3(1,1,1,1);
          CHECK (tx3 == TimeValue(TimeValue::SCALE*(0.001 + 1 + 60 + 60*60)));
          
          CHECK ("1:01:01.001" == string(tx3));
          
          // create time variable on the fly....
          CHECK (th+th == t1);
          CHECK (t1-th == th);
          CHECK (((t1-th)*=2) == t1);
          CHECK (th-th == TimeValue(0));
          
          // that was indeed a temporary and didn't affect the originals
          CHECK (t1 == TimeValue(TimeValue::SCALE));
          CHECK (th == TimeValue(TimeValue::SCALE/2));
        }
      
      
      /** @test calculate a generic hash value from a time spec*/
      void
      checkTimeHash (TimeValue org)
        {
          std::hash<TimeValue> hashFunc;
          CHECK (0 == hashFunc (Time::ZERO));
          size_t hh = sizeof(size_t)*CHAR_BIT/2;
          CHECK (size_t(1)<<hh == hashFunc (TimeValue{1}));
          CHECK (size_t(1)     == hashFunc (TimeValue(size_t(1)<<hh)));
          
          size_t h1 = hashFunc (org);
          size_t h2 = hashFunc (Time{org} + TimeValue{1});
          size_t h3 = hashFunc (TimeValue(h1));
          CHECK (h1 > 0 || org == Time::ZERO);
          CHECK (h2 - h1 == size_t(1)<<hh);
          CHECK (h3 == size_t(_raw(org)));
        }
      
      
      void
      verify_invalidFramerateProtection()
        {
          VERIFY_ERROR (BOTTOM_VALUE, FrameRate infinite(0) );
          VERIFY_ERROR (BOTTOM_VALUE, FrameRate infinite(0,123) );
          
          CHECK (isnil (Duration (0, FrameRate::PAL)));
          CHECK (isnil (Duration (0, FrameRate(123))));
          
          CHECK (FrameRate::approx(2000) ==       "2000FPS"_expect);
          CHECK (FrameRate::approx(1e05) ==     "100000FPS"_expect);
          CHECK (FrameRate::approx(1e06) ==    "1000000FPS"_expect);      // exact
          CHECK (FrameRate::approx(1e12) ==    "4194303FPS"_expect);      // limited (≈4.2e+6)
          CHECK (FrameRate::approx(1e14) ==    "4194303FPS"_expect);      // limited   + numeric overflow prevented
          CHECK (FrameRate::approx(1e-5) == "14/1398101FPS"_expect);      // quantised ≈ 1.00135827e-5
          CHECK (FrameRate::approx(1e-6) ==  "4/4194303FPS"_expect);      // quantised ≈ 0.95367454e-6
          CHECK (FrameRate::approx(1e-7) ==  "1/4194303FPS"_expect);      // limited   ≈ 2.38418636e-7
          CHECK (FrameRate::approx(1e-9) ==  "1/4194303FPS"_expect);      // limited   ≈ 2.38418636e-7
          
          CHECK (FrameRate(      20'000, Duration{Time{0,10}}) ==           "2000FPS"_expect);  // exact
          CHECK (FrameRate(      20'000, Duration{Time::MAX }) ==      "1/4194303FPS"_expect);  // limited
          
          CHECK (FrameRate(size_t(2e10), Duration{Time::MAX }) == "272848/4194303FPS"_expect);  // quantised ≈ 6.5052048e-2
          CHECK (FrameRate(size_t(2e14), Duration{Time::MAX }) ==   "3552496/5461FPS"_expect);  // quantised ≈ 650.52115   exact:650.521
          CHECK (FrameRate(size_t(2e15), Duration{Time::MAX }) ==    "3324163/511FPS"_expect);  // quantised ≈ 6505.2114   exact:6505.21
          CHECK (FrameRate(size_t(2e16), Duration{Time::MAX }) ==     "4098284/63FPS"_expect);  // quantised ≈ 65052,127   exact:65052.1
          CHECK (FrameRate(size_t(2e17), Duration{Time::MAX }) ==         "650521FPS"_expect);  //                         exact:650521
          CHECK (FrameRate(size_t(2e18), Duration{Time::MAX }) ==        "4194303FPS"_expect);  // limited (≈4.2e+6)       exact:6.50521e+06
          CHECK (FrameRate(size_t(2e20), Duration{Time::MAX }) ==        "4194303FPS"_expect);  // limited                 exact:6.50521e+08
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
          // simple linear combinations
          CHECK (7 == -2*off9 + off5*5);
          
          // build offset by number of frames
          Offset byFrames(-125, FrameRate::PAL);
          CHECK (Time(FSecs(-5)) == byFrames);
          
          CHECK (Offset(-5, FrameRate(5,4)) == -Offset(5, FrameRate(5,4)));
          CHECK (Offset(3, FrameRate(3)) == Offset(12345, FrameRate(24690,2)));
        }                                // precise rational number calculations
      
      
      void
      buildDuration (TimeValue org)
        {
          TimeValue zero(0);
          TimeVar point(org);
          point += TimeValue(5);
          CHECK (org < point);
          
          Offset backwards(point,org);
          CHECK (backwards < zero);
          
          Duration distance(backwards);
          CHECK (distance > zero);
          CHECK (distance == backwards.abs());
          
          Duration len1(Time(23,4,5,6));
          CHECK (len1 == Time(FSecs(23,1000)) + Time(0, 4 + 5*60 + 6*3600));
          
          Duration len2(Time(FSecs(-10))); // negative specs...
          CHECK (len2 == Time(FSecs(10)));//
          CHECK (len2 > zero);           //   will be taken absolute
          
          Duration unit(50, FrameRate::PAL);
          CHECK (Time(0,2,0,0) == unit);       // duration of 50 frames at 25fps is... (guess what)
          
          CHECK (FrameRate::PAL.duration() == Time(FSecs(1,25)));
          CHECK (FrameRate::NTSC.duration() == Time(FSecs(1001,30000)));
          cout << "NTSC-Framerate = " << FrameRate::NTSC.asDouble() << endl;
          
          CHECK (zero == Duration::NIL);
          CHECK (isnil (Duration::NIL));
          
          // assign to variable for calculations
          point = backwards;
          point *= 2;
          CHECK (point < zero);
          CHECK (point < backwards);
          
          CHECK (distance + point < zero);      // using the duration as offset
          CHECK (distance == backwards.abs()); //  while this didn't alter the duration as such
        }
      
      
      void
      verify_extremeValues()
        {
          CHECK (Time::MIN < Time::MAX);
          CHECK (_raw(Time::MAX) < std::numeric_limits<int64_t>::max());
          CHECK (_raw(Time::MIN) > std::numeric_limits<int64_t>::min());
          
          // Values are limited at construction, but not in calculations
          CHECK (Time::MAX - Time(0,1) < Time::MAX);
          CHECK (Time::MAX - Time(0,1) + Time(0,3) > Time::MAX);
          CHECK (TimeValue{_raw(Time::MAX-Time(0,1)+Time(0,3))} == Time::MAX);  // clipped at max
          CHECK (TimeValue{_raw(Time::MIN+Time(0,5)-Time(0,9))} == Time::MIN);  // clipped at min
          
          TimeValue outlier{Time::MIN - TimeValue(1)};
          CHECK (outlier < Time::MIN);
          
          CHECK (Duration::MAX > Time::MAX);
          CHECK (_raw(Duration::MAX) < std::numeric_limits<int64_t>::max());
          CHECK (Duration::MAX == Time::MAX - Time::MIN);
          CHECK (-Duration::MAX == Offset{Time::MIN - Time::MAX});
          CHECK (Duration{3*Offset{Time::MAX}} == Duration::MAX);
          
          CHECK (                Time::MAX + Duration::MAX    >  Duration::MAX);
          CHECK (                Time::MIN - Duration::MAX    < -Duration::MAX);
          CHECK (         Offset{Time::MAX + Duration::MAX}  ==  Duration::MAX); // clipped at max
          CHECK (         Offset{Time::MIN - Duration::MAX}  == -Duration::MAX); // clipped at min
          CHECK (Duration{Offset{Time::MIN - Duration::MAX}} ==  Duration::MAX); // duration is absolute
          
          CHECK (TimeSpan(Time::MIN, Time::MAX) == TimeSpan(Time::MAX, Time::MIN));
          CHECK (TimeSpan(Time::MAX, Duration::MAX).start()    == Time::MAX);
          CHECK (TimeSpan(Time::MAX, Duration::MAX).end()      == Time::MAX + Duration::MAX); // note: end() can yield value beyond [Time::MIN...Time::MAX]
          CHECK (TimeSpan(Time::MAX, Duration::MAX).duration() == Duration::MAX);
          CHECK (TimeSpan(Time::MAX, Duration::MAX).conform()  == TimeSpan(Time::MIN,Duration::MAX));
          CHECK (TimeSpan(outlier,   Duration::MAX).conform()  == TimeSpan(Time::MIN,Duration::MAX));
          CHECK (TimeSpan(Time::MAX, Offset(FSecs(-1)))        == TimeSpan(Time::MAX-Offset(FSecs(1)), FSecs(1)));
          CHECK (TimeSpan(Time::MAX, FSecs(5)).start()         == Time::MAX);
          CHECK (TimeSpan(Time::MAX, FSecs(5)).duration()      == Duration(FSecs(5)));
          CHECK (TimeSpan(Time::MAX, FSecs(5)).conform()       == TimeSpan(Time::MAX-Offset(FSecs(5)), FSecs(5)));
        }
      
      
      void
      verify_fractionalOffset()
        {
          typedef boost::rational<FrameCnt> Frac;
          
          Duration three (TimeValue(3));       // three micro seconds
          
          Offset o1 = Frac(1,2) * three;
          CHECK (o1 > Time::ZERO);
          CHECK (o1 == TimeValue(1));          // bias towards the next lower micro grid position
          
          Offset o2 = -Frac(1,2) * three;
          CHECK (o2 < Time::ZERO);
          CHECK (o2 == TimeValue(-2));
          
          CHECK (three * Frac(1,2) * 2 != three);
          CHECK (three *(Frac(1,2) * 2) == three);
          // integral arithmetics is precise,
          // but not necessarily exact!
        }
      
      
      void
      buildTimeSpan (TimeValue org)
        {
          TimeValue five(5);
          
          TimeSpan interval (Time(org), Duration(Offset (org,five)));
          
          // the time span behaves like a time
          CHECK (org == interval);
          
          // can get the length by direct conversion
          Duration theLength(interval);
          CHECK (theLength == Offset(org,five).abs());
          
          Time endpoint = interval.end();
          TimeSpan successor (endpoint, FSecs(2));
          
          CHECK (Offset(interval,endpoint) == Offset(org,five).abs());
          CHECK (Offset(endpoint,successor.end()) == Duration(successor));
          
          cout <<   "Interval-1: " << interval
               << "  Interval-2: " << successor
               << "  End point: "  << successor.end()
               << endl;
        }          
      
      
      void
      compareTimeSpan (Time const& org)
        {
          TimeSpan span1 (org, org+org);                  // using the distance between start and end point
          TimeSpan span2 (org+org, org);                  // note: TimeSpan is oriented automatically
          TimeSpan span3 (org, FSecs(5,2));               // Duration given explicitly, in seconds
          TimeSpan span4 (org, FSecs(5,-2));              // note: fractional seconds taken absolute, as Duration
          
          CHECK (span1 == span2);
          CHECK (span2 == span1);
          CHECK (span3 == span4);
          CHECK (span4 == span3);
          
          CHECK (span1 != span3);
          CHECK (span3 != span1);
          CHECK (span1 != span4);
          CHECK (span4 != span1);
          CHECK (span2 != span3);
          CHECK (span3 != span2);
          CHECK (span2 != span4);
          CHECK (span4 != span2);
          
          // note that TimeSpan is oriented at creation
          CHECK (span1.end() == span2.end());
          CHECK (span3.end() == span4.end());
          
          // Verify the extended order on time intervals
          TimeSpan span1x (org+org, Duration(org));       // starting later than span1
          TimeSpan span3y (org,     FSecs(2));            // shorter than span3
          TimeSpan span3z (org+org, FSecs(2));            // starting later and shorter than span3
          
          CHECK (span1 != span1x);
          CHECK (span3 != span3y);
          CHECK (span3 != span3z);
          
          CHECK (  span1 <  span1x);
          CHECK (  span1 <= span1x);
          CHECK (!(span1 >  span1x));
          CHECK (!(span1 >= span1x));
          
          CHECK (  span3 >  span3y);
          CHECK (  span3 >= span3y);
          CHECK (!(span3 <  span3y));
          CHECK (!(span3 <= span3y));
          
          CHECK (  span3 < span3z);    // Note: the start point takes precedence on comparison
          CHECK ( span3y < span3z);
          
          // Verify this order is really different
          // than the basic ordering of time values
          CHECK (span1 < span1x);
          CHECK (span1.duration() == span1x.duration());
          CHECK (span1.start() < span1x.start());
          CHECK (span1.end()   < span1x.end());
          
          CHECK (span3 > span3y);
          CHECK (span3.duration() > span3y.duration());
          CHECK (span3.start() == span3y.start());
          CHECK (span3.end()    > span3y.end());
          CHECK (Time(span3)   == Time(span3y));
          
          CHECK (span3 < span3z);
          CHECK (span3.duration() > span3z.duration());
          CHECK (span3.start() < span3z.start());
          CHECK (span3.end()  != span3z.end());        // it's shorter, and org can be random, so that's all we know
          CHECK (Time(span3)   < Time(span3z));
          
          CHECK (span3y < span3z);
          CHECK (span3y.duration() == span3z.duration());
          CHECK (span3y.start() < span3z.start());
          CHECK (span3y.end()  < span3z.end());
          CHECK (Time(span3)  < Time(span3z));
        }
      
      
      void
      relateTimeIntervals (TimeValue org)
        {
          Time oneSec(FSecs(1));
          
          TimeSpan span1 (org,          FSecs(2));
          TimeSpan span2 (oneSec + org, FSecs(2));
          
          TimeVar probe(org);
          CHECK ( span1.contains(probe));
          CHECK (!span2.contains(probe));
          
          probe = span2;
          CHECK ( span1.contains(probe));
          CHECK ( span2.contains(probe));
          
          probe = span1.end();
          CHECK (!span1.contains(probe));              // Note: end is always exclusive
          CHECK ( span2.contains(probe));
          
          probe = span2.end();
          CHECK (!span1.contains(probe));
          CHECK (!span2.contains(probe));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TimeValue_test, "unit common");
  
  
  
}}} // namespace lib::time::test
