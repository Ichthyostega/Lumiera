/*
  Rational(Test)  -  verify support for rational arithmetics

   Copyright (C)
     2022,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file rational-test.cpp
 ** unit test \ref Rational_test
 */



#include "lib/error.hpp"
#include "lib/test/run.hpp"
#include "lib/integral.hpp"
#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"

#include "lib/rational.hpp"

#include <chrono>
#include <array>

using std::array;


namespace util {
namespace test {
  
  
  /***************************************************************************//**
   * @test cover some aspects of working with fractional numbers.
   *       - demonstrate some basics, as provided by `boost::rational`
   *       - check for possibly dangerous values
   *       - re-quantise a rational number
   * @see  rational.hpp
   * @see  stage::model::test::ZoomWindow_test
   */
  class Rational_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          demonstrate_basics();
          verify_intLog2();
          verify_limits();
          verify_requant();
        }
      
      
      /**
       * @test demonstrate fundamental properties of rational arithmetics
       *       as provided by boost::rational
       *       - represent rational fractions precisely
       *       - convert to other types and then perform the division
       *       - our typedef `Rat = boost::rational<int64_t>`
       *       - our user-defined literal "_r" to simplify notation
       *       - string conversion to reveal numerator and denominator
       *       - automatic normalisation and reduction
       *       - some typical fractional calculation examples.
       */
      void
      demonstrate_basics()
        {
          CHECK (Rat(10,3) == 10_r/3);                                           // user-defined literal to construct a fraction
          CHECK (Rat(10,3) == boost::rational<int64_t>(10,3));                   // using Rat = boost::rational<int64_t>
          CHECK (rational_cast<float> (10_r/3) == 3.3333333f);                   // rational_cast calculates division after type conversion
          
          CHECK (2_r/3 + 3_r/4 == 17_r/12);
          CHECK (2_r/3 * 3_r/4 == 1_r/2);
          CHECK (2_r/3 /(3_r/4)== 8_r/9);
          CHECK (2_r/3 / 3  /4 == 1_r/18);                                       // usual precedence and brace rules apply, yielding 2/36 here
          
          CHECK (util::toString(23_r/55) == "23/55sec"_expect);  ///////////////////TICKET #1259 and #1261 : FSecs should really be a distinct (wrapper) type,
                                                                 ///////////////////TICKET #1259 and #1261 : ...then this custom conversion with the suffix "sec" would not kick in here
          CHECK (util::toString(24_r/56) == "3/7sec"_expect  );                  // rational numbers are normalised and reduced immediately
          
          CHECK (Rat(10,3).numerator() == int64_t(10));
          CHECK (Rat(10,3).denominator() == int64_t(3));
          CHECK (boost::rational<uint>(10,3).numerator() == uint(10));
          CHECK (boost::rational<uint>(10,3).denominator() == uint(3));
          CHECK (boost::rational<uint>(10,3) == rational_cast<boost::rational<uint>> (Rat(10,3)));
          CHECK (boost::rational<uint>(11,3) != rational_cast<boost::rational<uint>> (Rat(10,3)));
        }
      
      
      /**
       * @test demonstrate the limits and perils of rational fractions
       *       - largest and smallest number representable
       *       - numeric overflow due to normalisation
       *       - predicates to check for possible trouble
       */
      void
      verify_limits()
        {
          const Rat MAXI = Rat{std::numeric_limits<int64_t>::max()};
          const Rat MINI = Rat{1, std::numeric_limits<int64_t>::max()};
          
          CHECK (rational_cast<int64_t>(MAXI) == std::numeric_limits<int64_t>::max());
          CHECK (rational_cast<double> (MAXI) == 9.2233720368547758e+18);
          
          CHECK (MAXI > 0);                                                      // so this one still works
          CHECK (MAXI+1 < 0);                                                    // but one more and we get a wrap-around
          CHECK (MAXI+1 < -MAXI);
          CHECK (util::toString(MAXI)   ==  "9223372036854775807sec"_expect);     /////////TICKET #1259 should be "9223372036854775807/1  -- get rid of the "sec" suffix
          CHECK (util::toString(MAXI+1) == "-9223372036854775808sec"_expect);     /////////TICKET #1259 should be "-9223372036854775808/1"
          CHECK (util::toString(-MAXI)  == "-9223372036854775807sec"_expect);     /////////TICKET #1259 should be "-9223372036854775807/1"

          CHECK (MINI > 0);                                                      // smallest representable number above zero
          CHECK (1-MINI < 1);
          CHECK (0 < 1-MINI);                                                    // can be used below 1 just fine
          CHECK (0 > 1+MINI);                                                    // but above we get a wrap-around in normalised numerator
          CHECK (util::toString(MINI)   ==  "1/9223372036854775807sec"_expect);
          CHECK (util::toString(-MINI)  == "-1/9223372036854775807sec"_expect);
          CHECK (util::toString(1-MINI) ==  "9223372036854775806/9223372036854775807sec"_expect);
          CHECK (util::toString(1+MINI) == "-9223372036854775808/9223372036854775807sec"_expect);
          
          CHECK ((MAXI-1)/MAXI == 1-MINI);
          CHECK (MAXI/(MAXI-1) >  1);                                            // as workaround we have to use a slightly larger ULP
          CHECK (MAXI/(MAXI-1) - 1 > MINI);                                      // ...this slightly larger one works without wrap-around
          CHECK (1 - MAXI/(MAXI-1) < -MINI);
          CHECK (util::toString(MAXI/(MAXI-1))      ==  "9223372036854775807/9223372036854775806sec"_expect);
          CHECK (util::toString(MAXI/(MAXI-1) - 1)  ==  "1/9223372036854775806sec"_expect);
          CHECK (util::toString(1 - MAXI/(MAXI-1))  == "-1/9223372036854775806sec"_expect);
          
          // Now entering absolute danger territory....
          const Rat MIMI = -MAXI-1;                                              // this is the most extreme negative representable value
          CHECK (MIMI < 0);
          CHECK (util::toString(MIMI)   ==  "-9223372036854775808sec"_expect);    /////////TICKET #1259 should be "-9223372036854775808/1"
          CHECK (util::toString(1/MIMI) ==  "-1/-9223372036854775808sec"_expect);
          try
            {
              -1-1/MIMI;                                                         // ...but it can't be used for any calculation without blowing up
              NOTREACHED("expected boost::rational to flounder");
            }
          catch (std::exception& tilt)
            {
              CHECK (tilt.what() == string{"bad rational: non-zero singular denominator"});
            }
          
          
          // yet seemingly harmless values can be poisonous...
          Rat poison = MAXI/49 / (MAXI/49-1);
          Rat decoy = poison + 5;
          CHECK (poison > 0);
          CHECK (decoy  > 6);
          CHECK (rational_cast<double>(decoy)     ==   6);                       // looks innocuous...
          CHECK (rational_cast<double>(decoy+5)   ==  11);                       // ...aaaand...
          CHECK (rational_cast<double>(decoy+50)  == -42);                       // ..ultimate answer..
          CHECK (rational_cast<double>(decoy+500) ==  15.999999999999996);       // .dead in the water.
          
          // Heuristics to detect danger zone
          CHECK (    can_represent_Sum(decoy,5));
          CHECK (not can_represent_Sum(decoy,50));
          
          // alarm is given a bit too early
          CHECK (    can_represent_Sum(decoy,15));                               // ...because the check is based on bit positions
          CHECK (not can_represent_Sum(decoy,16));                               // ...and here the highest bit of one partner moved into danger zone
          CHECK (decoy+16 > 0);
          CHECK (decoy+43 > 0);
          CHECK (decoy+44 < 0);
          
          // similar when increasing the denominator...
          CHECK (poison + 1_r/10  > 0);
          CHECK (poison + 1_r/90  > 0);
          CHECK (poison + 1_r/98  < 0);                                          // actually the flip already occurs at 1/91 but also causes an assertion failure
          CHECK (    can_represent_Sum(poison,1_r/10));
          CHECK (    can_represent_Sum(poison,1_r/15));
          CHECK (not can_represent_Sum(poison,1_r/16));
          CHECK (not can_represent_Sum(poison,1_r/91));
          CHECK (not can_represent_Sum(poison,1_r/100));
        }
      
      
      /**
       * @test an optimised implementation of integer binary logarithm
       *       - basically finds the highest bit which is set
       *       - can be used with various integral types
       *       - performs better than using the floating-point solution
       * @todo this test (and the implementation) belongs into some basic util header.
       */
      void
      verify_intLog2()
        {
          CHECK ( 5 == ilog2( int64_t(0b101010)));
          CHECK ( 5 == ilog2(uint64_t(0b101010)));
          CHECK ( 5 == ilog2( int32_t(0b101010)));
          CHECK ( 5 == ilog2(uint32_t(0b101010)));
          CHECK ( 5 == ilog2( int16_t(0b101010)));
          CHECK ( 5 == ilog2(uint16_t(0b101010)));
          CHECK ( 5 == ilog2(  int8_t(0b101010)));
          CHECK ( 5 == ilog2( uint8_t(0b101010)));
          CHECK ( 5 == ilog2(    int (0b101010)));
          CHECK ( 5 == ilog2(   uint (0b101010)));
          CHECK ( 5 == ilog2(   char (0b101010)));
          CHECK ( 5 == ilog2(  uchar (0b101010)));
          CHECK ( 5 == ilog2(   long (0b101010)));
          CHECK ( 5 == ilog2(  ulong (0b101010)));
          CHECK ( 5 == ilog2(  short (0b101010)));
          CHECK ( 5 == ilog2( ushort (0b101010)));
          
          CHECK (63 == ilog2(std::numeric_limits<uint64_t>::max()));
          CHECK (62 == ilog2(std::numeric_limits< int64_t>::max()));
          CHECK (31 == ilog2(std::numeric_limits<uint32_t>::max()));
          CHECK (30 == ilog2(std::numeric_limits< int32_t>::max()));
          CHECK (15 == ilog2(std::numeric_limits<uint16_t>::max()));
          CHECK (14 == ilog2(std::numeric_limits< int16_t>::max()));
          CHECK ( 7 == ilog2(std::numeric_limits< uint8_t>::max()));
          CHECK ( 6 == ilog2(std::numeric_limits<  int8_t>::max()));
          
          CHECK ( 5 == ilog2(0b111111));
          CHECK ( 5 == ilog2(0b101110));
          CHECK ( 5 == ilog2(0b100100));
          CHECK ( 5 == ilog2(0b100000));
          
          CHECK ( 2 == ilog2(4));
          CHECK ( 1 == ilog2(2));
          CHECK ( 0 == ilog2(1));
          CHECK (-1 == ilog2(0));
          CHECK (-1 == ilog2(-1));
          
          CHECK (-1 == ilog2(std::numeric_limits<uint64_t>::min()));
          CHECK (-1 == ilog2(std::numeric_limits< int64_t>::min()));
          CHECK (-1 == ilog2(std::numeric_limits<uint32_t>::min()));
          CHECK (-1 == ilog2(std::numeric_limits< int32_t>::min()));
          CHECK (-1 == ilog2(std::numeric_limits<uint16_t>::min()));
          CHECK (-1 == ilog2(std::numeric_limits< int16_t>::min()));
          CHECK (-1 == ilog2(std::numeric_limits< uint8_t>::min()));
          CHECK (-1 == ilog2(std::numeric_limits<  int8_t>::min()));
          
          
          /* ==== compare with naive implementation ==== */
          
          auto floatLog = [](auto n)
                            {
                              return n <=0? -1 : ilogb(n);
                            };
          auto bitshift = [](auto n)
                            {
                              if (n <= 0) return -1;
                              int logB = 0;
                              while (n >>= 1)
                                ++logB;
                              return logB;
                            };
          auto do_nothing = [](auto n){ return n; };
          
          array<uint64_t, 1000> numz;
          for (auto& n : numz)
            {
              n = rani() * uint64_t(1 << 31);
              CHECK (ilog2(n) == floatLog(n));
              CHECK (ilog2(n) == bitshift(n));
            }
          
          int64_t dump{0}; // throw-away result to prevent optimisation
          auto microbenchmark = [&numz,&dump](auto algo)
                                  {
                                    using std::chrono::system_clock;
                                    using Dur = std::chrono::duration<double>;
                                    const double SCALE = 1e9; // Results are in ns
                                    
                                    auto start = system_clock::now();
                                    for (uint i=0; i<1000; ++i)
                                      for (auto const& n : numz)
                                        dump += algo(n);
                                    Dur duration = system_clock::now () - start;
                                    return duration.count()/(1000*1000) * SCALE;
                                  };
          


          auto time_ilog2 = microbenchmark(ilog2<int64_t>);
          auto time_float = microbenchmark(floatLog);
          auto time_shift = microbenchmark(bitshift);
          auto time_ident = microbenchmark(do_nothing);
          
          cout << "Microbenchmark integer-log2"    <<endl
               << "util::ilog2 :"<<time_ilog2<<"ns"<<endl
               << "std::ilogb  :"<<time_float<<"ns"<<endl
               << "bit-shift   :"<<time_shift<<"ns"<<endl
               << "identity    :"<<time_ident<<"ns"<<endl
               << "(checksum="<<dump<<")"          <<endl; // Warning: without outputting `dump`,
                                                          //  the optimiser would eliminate most calls
          // the following holds both with -O0 and -O3
          CHECK (time_ilog2 < time_shift);
          CHECK (time_ident < time_ilog2);
          
          /****  some numbers ****
           *
           * GCC-8, -O3, Debian-Buster, AMD FX83
           *
           * with uint64_t...
           * - ilog2 : 5.6ns
           * - ilogb : 5.0ns
           * - shift : 44ns
           * - ident : 0.6ns
           *
           * with uint8_t
           * - ilog2 : 5.2ns
           * - ilogb : 5.8ns
           * - shift : 8.2ns
           * - ident : 0.3ns
           */
        }
      
      
      /**
       * @test helper to re-quantise a rational fraction
       *       - recast a number in terms of another denominator
       *       - this introduces an error of known limited size
       *       - and is an option to work around "poisonous" fractions
       */
      void
      verify_requant()
        {
          const int64_t MAX{std::numeric_limits<int64_t>::max()};
          const Rat MAXI = Rat{MAX};
          
          Rat poison = (MAXI-88)/(MAXI/7);
          
          auto approx = [](Rat rat){ return rational_cast<float> (rat); };
          CHECK (poison   > 0);
          CHECK (poison+1 < 0);                                                  // wrap around!
          CHECK (approx (poison  ) ==  6.99999952f);                             // wildly wrong results...
          CHECK (approx (poison+1) == -6);
          CHECK (approx (poison+7) == -6.83047369e-17f);
          CHECK (approx (poison+9_r/5) == 0.400000006f);
          
          Rat sleazy = reQuant (poison,  1 << 24);                               // recast into multiples of an arbitrary other divisor,
          CHECK (sleazy.denominator() == 1 << 24);                               // (here using a power of two as example)
          // and now we can do all the slick stuff...
          CHECK (sleazy   > 0);
          CHECK (sleazy+1 > 0);
          CHECK (sleazy+7 > 0);
          CHECK (approx (sleazy)   == 7);
          CHECK (approx (sleazy+1) == 8);
          CHECK (approx (sleazy+7) == 14);
          CHECK (approx (sleazy+9_r/5) == 8.80000019f);
          
          CHECK (util::toString (poison)   == "9223372036854775719/1317624576693539401sec"_expect);
          CHECK (util::toString (poison+1) =="-7905747460161236496/1317624576693539401sec"_expect);
          CHECK (util::toString (sleazy)   == "117440511/16777216sec"_expect);
          CHECK (util::toString (sleazy+1) == "134217727/16777216sec"_expect);
          
          // also works towards larger denominator, or with negative numbers...
          CHECK (reQuant (1/poison, MAX) == 1317624576693539413_r/9223372036854775807);
          CHECK (reQuant (-poison, 7777) == -54438_r/ 7777);
          CHECK (reQuant (poison, -7777) == -54438_r/-7777);
          
          CHECK (approx (         1/poison      ) == 0.142857149f);
          CHECK (approx (reQuant (1/poison, MAX)) == 0.142857149f);
          CHECK (approx (reQuant (poison,  7777)) == 6.99987125f);
        }
    };
  
  LAUNCHER (Rational_test, "unit common");
  
  
}} // namespace util::test
