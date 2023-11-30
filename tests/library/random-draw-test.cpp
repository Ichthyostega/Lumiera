/*
  RandomDraw(Test)  -  verify the component builder for random selected values

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file random-draw-test.cpp
 ** unit test \ref RandomDraw_test
 */



#include "lib/test/run.hpp"
#include "lib/random-draw.hpp"
#include "lib/format-string.hpp"
#include "lib/test/test-helper.hpp"

#include <array>



namespace lib {
namespace test{
  
  using util::_Fmt;
  using lib::meta::_FunRet;
  using err::LUMIERA_ERROR_LIFECYCLE;
  
  
  
  namespace { // policy and configuration for test...

    double ctxParameter = 1.0;

    /**
     * @note the test uses a rather elaborate result value setting
     *     - produces five distinct values
     *     - value range is symmetrical to origin
     *     - zero is defined as the _neutral value_
     *     - accepts a `size_t` hash value as basic input
     */
    struct SymmetricFive
      : function<Limited<int, 2,-2, 0>(size_t)>
      {
        /** by default use the hash directly as source of randomness */
        static size_t defaultSrc (size_t hash) { return hash; }
        
        /** Adaptor to handle further mapping functions */
        template<class SIG>
        struct Adaptor
          {
            static_assert (not sizeof(SIG), "Unable to adapt given functor.");
          };
        
        /** allow a mapping function rely on quantisation cycles */
        template<typename RES>
        struct Adaptor<RES(uint,uint)>
          {
            template<typename FUN>
            static auto
            build (FUN&& fun)
              {
                return [functor=std::forward<FUN>(fun)]
                       (size_t hash) -> _FunRet<FUN>
                          {
                            return functor(uint(hash/64), uint(hash%64));
                          };
              }
          };
        
        /** inject external contextual state into a mapping function */
        template<typename RES>
        struct Adaptor<RES(size_t, double)>
          {
            template<typename FUN>
            static auto
            build (FUN&& fun)
              {
                return [functor=std::forward<FUN>(fun)]
                       (size_t hash) -> _FunRet<FUN>
                          {
                            return functor(hash, ctxParameter);
                          };
              }
          };
      };
    //
  }//(End) Test config


  using Draw = RandomDraw<SymmetricFive>;
  
  
  
  /***********************************************************************************//**
   * @test Verify a flexible builder for random-value generators; using a config template,
   *       these can be outfitted to use a suitable source of randomness and to produce
   *       values from a desired target type and limited range.
   *       - for this test, generated result values are ∈ [-2 .. 0 .. +2]
   *       - no actual randomness is used; rather a `size_t` challenge is
   *         sent in to verify precisely deterministic numeric results.
   * @see random-draw.hpp
   * @see vault::gear::TestChainLoad as usage example
   * @see SchedulerStress_test
   */
  class RandomDraw_test
    : public Test
    {
      
      void
      run (Arg)
        {
          simpleUse();
          
          verify_policy();
          verify_numerics();
          verify_adaptMapping();
          verify_dynamicChange();
        }
      
      
      
      /** @test demonstrate a basic usage scenario
       */
      void
      simpleUse()
        {
          auto draw = Draw().probability(0.5);
          CHECK (draw(  0) ==  0);
          CHECK (draw( 16) ==  0);
          CHECK (draw( 32) ==  1);
          CHECK (draw( 40) ==  2);
          CHECK (draw( 48) == -2);
          CHECK (draw( 56) == -1);
          CHECK (draw( 64) ==  0); // values repeat after 64 steps
          CHECK (draw( 95) ==  0); // ~ half of each cycle yields the »neutral value«
          CHECK (draw( 96) ==  1);
          CHECK (draw(127) == -1);
          CHECK (draw(128) ==  0);
          CHECK (draw(168) ==  2);
          CHECK (draw(256) ==  0);
        }
      
      
      
      /** @test verify configuration through policy template
       *      - use the default policy, which takes no input values,
       *        but rather directly generates a random number; in this
       *        case here, input values are ∈ [0 .. 5]
       *      - define another policy template, to produce char values,
       *        while always requiring two input data values `(char,uint)`;
       *        moreover, define the `defaultSrc()` directly to produce the
       *        raw mapping values (double) using a custom formula; the
       *        resulting RandomDraw instance is now a function with
       *        two input arguments, producing char values.
       */
      void
      verify_policy()
        {
          auto d1 = RandomDraw<random_draw::LimitedRandomGenerate<5>>().probability(1.0);
          uint v1 = d1();
          CHECK (0 < v1 and v1 <=5);
          
          struct SpecialPolicy
            : function<Limited<char, 'Z','A'>(char,uint)>
            {
              static double defaultSrc (char b, uint off) { return fmod ((b-'A'+off)/double('Z'-'A'), 1.0); }
            };
          
          auto d2 = RandomDraw<SpecialPolicy>().probability(1.0);
          CHECK (d2('A', 2) == 'D');
          CHECK (d2('M',10) == 'X');
          CHECK (d2('Y', 0) == 'Z');
          CHECK (d2('Y',15) == 'P');
        }
      
      
      
      /** @test verify random number transformations.
       *      - use a Draw instance with result values `[-2..0..+2]`
       *      - values are evenly distributed within limits of quantisation
       *      - the probability parameter controls the amount of neutral results
       *      - maximum and minimum value settings will be respected
       *      - the interval [min..max] is independent from neutral value
       *      - probability defines the cases within [min..max] \ neutral
       *      - all other cases `q = 1 - p` will yield the neutral value
       *      - implausible max/min settings will be corrected automatically
       */
      void
      verify_numerics()
        {
          auto distribution = [](Draw const& draw)
                  { // investigate value distribution
                    using Arr = std::array<int,5>;
                    Arr step{-1,-1,-1,-1,-1};
                    Arr freq{0};
                    for (uint i=0; i<128; ++i)
                      {
                        int res = draw(i);
                        CHECK (-2 <= res and res <= +2);
                        int idx = res+2;
                        freq[idx] += 1;
                        if (step[idx] < 0)
                          step[idx] = i;
                      }
                    _Fmt line{"val:%+d (%02d|%5.2f%%)\n"};
                    string report;
                    for (int idx=0; idx<5; ++idx)
                      {
                        report += line % (idx-2) % step[idx] % (100.0*freq[idx]/128);
                      }
                    return report;
                 };
          
          auto draw = Draw();
          string report{"+++| --empty--    \n"};

          CHECK (draw(  0) ==  0);
          CHECK (draw( 32) ==  0);
          CHECK (draw( 96) ==  0);

          report += distribution(draw);
          CHECK (report ==
                "+++| --empty--    \n"
                "val:-2 (-1| 0.00%)\n"
                "val:-1 (-1| 0.00%)\n"
                "val:+0 (00|100.00%)\n"
                "val:+1 (-1| 0.00%)\n"
                "val:+2 (-1| 0.00%)\n"_expect);
          
          
          draw.probability(1.0);
          CHECK (draw(  0) == +1);
          CHECK (draw( 15) == +1);
          CHECK (draw( 16) == +2);
          CHECK (draw( 31) == +2);
          CHECK (draw( 32) == -2);
          CHECK (draw( 47) == -2);
          CHECK (draw( 48) == -1);
          CHECK (draw( 63) == -1);
          CHECK (draw( 64) == +1);
          CHECK (draw( 96) == -2);

          report = "+++| p ≔ 1.0      \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 1.0      \n"
                "val:-2 (32|25.00%)\n"
                "val:-1 (48|25.00%)\n"
                "val:+0 (-1| 0.00%)\n"
                "val:+1 (00|25.00%)\n"
                "val:+2 (16|25.00%)\n"_expect);
          
          
          draw.probability(0.99);
          CHECK (draw(  0) ==  0);
          CHECK (draw(  1) == +1);
          CHECK (draw( 16) == +1);
          CHECK (draw( 17) == +2);
          CHECK (draw( 32) == +2);
          CHECK (draw( 33) == -2);
          CHECK (draw( 48) == -2);
          CHECK (draw( 49) == -1);
          CHECK (draw( 63) == -1);
          CHECK (draw( 64) ==  0);
          CHECK (draw( 65) == +1);
          CHECK (draw( 80) == +1); // 64+16
          CHECK (draw( 82) == +2); // 64+17
          CHECK (draw( 97) == -2); // 64+33
          CHECK (draw(352) == +2); // 64+32+256
          CHECK (draw(353) == -2); // 64+33+256
          
          report = "+++| p ≔ 0.99     \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 0.99     \n"
                "val:-2 (33|25.00%)\n"
                "val:-1 (49|23.44%)\n"
                "val:+0 (00| 1.56%)\n"
                "val:+1 (01|25.00%)\n"
                "val:+2 (17|25.00%)\n"_expect);
          
          
          draw.probability(0.98);
          CHECK (draw(  0) ==  0);
          CHECK (draw(  1) ==  0);
          CHECK (draw(  2) == +1);
          CHECK (draw( 63) == -1);
          CHECK (draw( 64) ==  0);
          CHECK (draw( 65) ==  0);
          CHECK (draw( 66) == +1);
          
          report = "+++| p ≔ 0.98     \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 0.98     \n"
                "val:-2 (33|25.00%)\n"
                "val:-1 (49|23.44%)\n"
                "val:+0 (00| 3.12%)\n"
                "val:+1 (02|23.44%)\n"
                "val:+2 (17|25.00%)\n"_expect);
          
          
          draw.probability(0.97);
          report = "+++| p ≔ 0.97     \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 0.97     \n"
                "val:-2 (33|25.00%)\n"
                "val:-1 (49|23.44%)\n"
                "val:+0 (00| 3.12%)\n"
                "val:+1 (02|25.00%)\n"
                "val:+2 (18|23.44%)\n"_expect);
          
          
          draw.probability(0.75);
          report = "+++| p ≔ 0.75     \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 0.75     \n"
                "val:-2 (40|18.75%)\n"
                "val:-1 (52|18.75%)\n"
                "val:+0 (00|25.00%)\n"
                "val:+1 (16|18.75%)\n"
                "val:+2 (28|18.75%)\n"_expect);
          
          
          draw.probability(0.5);
          report = "+++| p ≔ 0.50     \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 0.50     \n"
                "val:-2 (48|12.50%)\n"
                "val:-1 (56|12.50%)\n"
                "val:+0 (00|50.00%)\n"
                "val:+1 (32|12.50%)\n"
                "val:+2 (40|12.50%)\n"_expect);
          
          
          draw.probability(0.2);
          report = "+++| p ≔ 0.20     \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 0.20     \n"
                "val:-2 (58| 4.69%)\n"
                "val:-1 (61| 4.69%)\n"
                "val:+0 (00|81.25%)\n"
                "val:+1 (52| 4.69%)\n"
                "val:+2 (55| 4.69%)\n"_expect);
          
          
          draw.probability(0.1);
          report = "+++| p ≔ 0.10     \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 0.10     \n"
                "val:-2 (61| 3.12%)\n"
                "val:-1 (63| 1.56%)\n"
                "val:+0 (00|90.62%)\n"
                "val:+1 (58| 3.12%)\n"
                "val:+2 (60| 1.56%)\n"_expect);
          
          
                             // ══════════
          draw.probability(1.0).shuffle(1);
          CHECK (draw( 6) == +1); //  6*1
          CHECK (draw( 6) == +1); //  6*2
          CHECK (draw( 6) == +2); //  6*3
          CHECK (draw( 6) == +2); //  6*4
          CHECK (draw( 6) == +2); //  6*5
          CHECK (draw( 6) == -2); //  6*6
          CHECK (draw(16) == -1); // 16*7 %64 = 48
          CHECK (draw(16) == +1); // 16*8 %64 =  0
          
          report = "+++| p ≔ 1.0 +shuffle \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 1.0 +shuffle \n"
                "val:-2 (03|25.00%)\n"
                "val:-1 (04|25.00%)\n"
                "val:+0 (-1| 0.00%)\n"
                "val:+1 (00|25.00%)\n"
                "val:+2 (02|25.00%)\n"_expect);
          draw.shuffle(0);
          CHECK (draw(16) == +2); // shuffling disabled
          CHECK (draw(16) == +2); // values reproducible
          CHECK (draw(32) == -2);
          CHECK (draw(32) == -2);
          CHECK (draw(16) == +2);
          CHECK (draw(16) == +2);
          
          
          
                             // ═════════
          draw.probability(0.5).maxVal(1);
          CHECK (draw(  0) ==  0);
          CHECK (draw( 16) ==  0);
          CHECK (draw( 31) ==  0);
          CHECK (draw( 32) == +1);
          CHECK (draw( 42) == +1);
          CHECK (draw( 43) == -2);
          CHECK (draw( 53) == -2);
          CHECK (draw( 54) == -1);
          CHECK (draw( 63) == -1);
          CHECK (draw( 64) ==  0);
          CHECK (draw( 95) ==  0);
          CHECK (draw( 96) == +1);
          
          report = "+++| p ≔ 0.50 max ≔ 1 \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 0.50 max ≔ 1 \n"
                "val:-2 (43|17.19%)\n"
                "val:-1 (54|15.62%)\n"
                "val:+0 (00|50.00%)\n"
                "val:+1 (32|17.19%)\n"
                "val:+2 (-1| 0.00%)\n"_expect);
          
          
          draw.probability(1.0).maxVal(1);
          CHECK (draw(  0) == +1);
          CHECK (draw( 16) == +1);
          CHECK (draw( 21) == +1);
          CHECK (draw( 22) == -2);
          CHECK (draw( 42) == -2);
          CHECK (draw( 43) == -1);
          CHECK (draw( 63) == -1);
          CHECK (draw( 64) == +1);
          CHECK (draw( 85) == +1);
          CHECK (draw( 86) == -2);
          CHECK (draw( 96) == -2);
          
          report = "+++| p ≔ 1.0  max ≔ 1 \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 1.0  max ≔ 1 \n"
                "val:-2 (22|32.81%)\n"
                "val:-1 (43|32.81%)\n"
                "val:+0 (-1| 0.00%)\n"
                "val:+1 (00|34.38%)\n"
                "val:+2 (-1| 0.00%)\n"_expect);
          
          
                             // ═════════
          draw.probability(0.5).maxVal(0);
          CHECK (draw(  0) ==  0);
          CHECK (draw( 31) ==  0);
          CHECK (draw( 32) == -2);
          CHECK (draw( 47) == -2);
          CHECK (draw( 48) == -1);
          CHECK (draw( 63) == -1);
          CHECK (draw( 64) ==  0);
          CHECK (draw( 95) ==  0);
          CHECK (draw( 96) == -2);
          
          report = "+++| p ≔ 0.50 max ≔ 0 \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 0.50 max ≔ 0 \n"
                "val:-2 (32|25.00%)\n"
                "val:-1 (48|25.00%)\n"
                "val:+0 (00|50.00%)\n"
                "val:+1 (-1| 0.00%)\n"
                "val:+2 (-1| 0.00%)\n"_expect);
          
          
          draw.probability(1.0).maxVal(0);
          CHECK (draw(  0) == -2);
          CHECK (draw( 31) == -2);
          CHECK (draw( 32) == -1);
          CHECK (draw( 63) == -1);
          CHECK (draw( 64) == -2);
          CHECK (draw( 96) == -1);
          
          report = "+++| p ≔ 1.0  max ≔ 0 \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 1.0  max ≔ 0 \n"
                "val:-2 (00|50.00%)\n"
                "val:-1 (32|50.00%)\n"
                "val:+0 (-1| 0.00%)\n"
                "val:+1 (-1| 0.00%)\n"
                "val:+2 (-1| 0.00%)\n"_expect);
          
          
                             // ═════════
          draw.probability(0.5).maxVal(-1);
          CHECK (draw( 32) == -2);
          CHECK (draw( 47) == -2);
          CHECK (draw( 48) == -1);
          CHECK (draw( 63) == -1);
          CHECK (draw( 64) ==  0);
          CHECK (draw( 95) ==  0);
          CHECK (draw( 96) == -2);
          
          report = "+++| p ≔ 0.50 max ≔ -1 \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 0.50 max ≔ -1 \n"
                "val:-2 (32|25.00%)\n"
                "val:-1 (48|25.00%)\n"
                "val:+0 (00|50.00%)\n"
                "val:+1 (-1| 0.00%)\n"
                "val:+2 (-1| 0.00%)\n"_expect);
          
          
          draw.probability(1.0).maxVal(-1);
          CHECK (draw(  0) == -2);
          CHECK (draw( 31) == -2);
          CHECK (draw( 32) == -1);
          CHECK (draw( 63) == -1);
          CHECK (draw( 64) == -2);
          
          report = "+++| p ≔ 1.0  max ≔ -1 \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 1.0  max ≔ -1 \n"
                "val:-2 (00|50.00%)\n"
                "val:-1 (32|50.00%)\n"
                "val:+0 (-1| 0.00%)\n"
                "val:+1 (-1| 0.00%)\n"
                "val:+2 (-1| 0.00%)\n"_expect);
          
          
                             // ═════════
          draw.probability(0.5).maxVal(2).minVal(1);
          CHECK (draw( 32) == +1);
          CHECK (draw( 48) == +2);
          CHECK (draw( 63) == +2);
          CHECK (draw( 64) ==  0);
          
          report = "+++| p ≔ 0.50 min ≔ 1 max ≔ 2 \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 0.50 min ≔ 1 max ≔ 2 \n"
                "val:-2 (-1| 0.00%)\n"
                "val:-1 (-1| 0.00%)\n"
                "val:+0 (00|50.00%)\n"
                "val:+1 (32|25.00%)\n"
                "val:+2 (48|25.00%)\n"_expect);
          
          
          draw.probability(1.0).maxVal(2).minVal(1);
          CHECK (draw(  0) == +1);
          CHECK (draw( 32) == +2);
          CHECK (draw( 63) == +2);
          CHECK (draw( 64) == +1);
          
          report = "+++| p ≔ 1.0  min ≔ 1 max ≔ 2 \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 1.0  min ≔ 1 max ≔ 2 \n"
                "val:-2 (-1| 0.00%)\n"
                "val:-1 (-1| 0.00%)\n"
                "val:+0 (-1| 0.00%)\n"
                "val:+1 (00|50.00%)\n"
                "val:+2 (32|50.00%)\n"_expect);
          
          
                             // ═════════
          draw.probability(0.5).maxVal(0);
          CHECK (draw( 32) == -1);
          CHECK (draw( 63) == -1);
          CHECK (draw( 64) ==  0);
          
          report = "+++| p ≔ 0.50 max ≔ 0 (-> min ≔ -1) \n";
          report += distribution(draw);
          CHECK (report ==
                "+++| p ≔ 0.50 max ≔ 0 (-> min ≔ -1) \n"
                "val:-2 (-1| 0.00%)\n"
                "val:-1 (32|50.00%)\n"
                "val:+0 (00|50.00%)\n"
                "val:+1 (-1| 0.00%)\n"
                "val:+2 (-1| 0.00%)\n"_expect);
        }
      
      
      
      /** @test bind custom mapping transformation functions.
       *      - use different translation into positional values
       *        as input for the actual result value mapping;
       *      - use a mapping function with different arguments,
       *        which is wired by the appropriate Adapter from the Policy;
       *      - moreover, the concrete Policy may tap into the context, which is
       *        demonstrated here by accessing a global variable. In practice,
       *        this capability allows to accept custom types as data source.
       */
      void
      verify_adaptMapping()
        {
          // Note: no special Adapter required for the following function,
          //       since it takes the same arguments as our RandomDraw (size_t);
          //       moreover, since the function yields a double, the adapter scheme
          //       concludes that this function wants to feed directly into the
          //       primary mapping function RandomDraw::limited(double)
          auto d1 = Draw([](size_t hash) -> double { return hash / 10.0; });
          CHECK (d1( 0) == +1);
          CHECK (d1( 1) == +1);
          CHECK (d1( 2) == +1);
          CHECK (d1( 3) == +2);
          CHECK (d1( 4) == +2);
          CHECK (d1( 5) == -2);
          CHECK (d1( 6) == -2);
          CHECK (d1( 7) == -2);
          CHECK (d1( 8) == -1);
          CHECK (d1( 9) == -1);
          CHECK (d1(10) ==  0);
          CHECK (d1(11) ==  0);
          CHECK (d1(12) ==  0);
          CHECK (d1(13) ==  0);
          
          d1.probability(0.4);
          CHECK (d1( 0) ==  0);
          CHECK (d1( 1) ==  0);
          CHECK (d1( 2) ==  0);
          CHECK (d1( 3) ==  0);
          CHECK (d1( 4) ==  0);
          CHECK (d1( 5) ==  0);
          CHECK (d1( 6) == +1); // probability 0.4
          CHECK (d1( 7) == +2);
          CHECK (d1( 8) == -2);
          CHECK (d1( 9) == -1);
          CHECK (d1(10) ==  0);
          
          d1.minVal(-1).probability(0.7);
          CHECK (d1( 0) ==  0);
          CHECK (d1( 1) ==  0);
          CHECK (d1( 2) ==  0);
          CHECK (d1( 3) ==  0);
          CHECK (d1( 4) == +1); // probability 0.7
          CHECK (d1( 5) == +1);
          CHECK (d1( 6) == +2);
          CHECK (d1( 7) == +2);
          CHECK (d1( 8) == -1);
          CHECK (d1( 9) == -1);
          CHECK (d1(10) ==  0);
          
          // The next example demonstrates accepting special input arguments;
          // as defined in the policy, this function will get the `(div, mod)`
          // of the hash with modulus 64
          auto d2 = Draw([](uint cycle, uint rem){ return double(rem) / ((cycle+1)*5); });
          CHECK (d2( 0) == +1);
          CHECK (d2( 1) == +1);
          CHECK (d2( 2) == +2);
          CHECK (d2( 3) == -2);
          CHECK (d2( 4) == -1); // the first cycle is only 5 steps long (0+1)*5
          CHECK (d2( 5) ==  0);
          CHECK (d2( 6) ==  0);
          CHECK (d2( 7) ==  0);
          CHECK (d2( 8) ==  0);
          CHECK (d2( 9) ==  0);
          CHECK (d2(10) ==  0);
          CHECK (d2(63) ==  0);
          CHECK (d2(64) == +1); // the second cycle starts here...
          CHECK (d2(65) == +1);
          CHECK (d2(66) == +1);
          CHECK (d2(67) == +2);
          CHECK (d2(68) == +2);
          CHECK (d2(69) == -2);
          CHECK (d2(70) == -2);
          CHECK (d2(71) == -2);
          CHECK (d2(72) == -1);
          CHECK (d2(73) == -1);
          CHECK (d2(74) ==  0); // and is 10 steps long (same pattern as in the first example above)
          CHECK (d2(75) ==  0);
          
          // The next example uses the other Adapter variant, which „sneaks in“ a context value
          // Moreover, we can change the mapping function of an existing RandomDraw, as demonstrated here
          d2.mapping([](size_t hash, double ctx){ return hash / ctx; });
          
          ctxParameter = 4.0;
          CHECK (d2( 0) == +1);
          CHECK (d2( 1) == +2);
          CHECK (d2( 2) == -2);
          CHECK (d2( 3) == -1); // cycle-length: 4
          CHECK (d2( 4) ==  0);
          CHECK (d2( 5) ==  0);
          CHECK (d2( 6) ==  0);
          CHECK (d2( 7) ==  0);
          CHECK (d2( 8) ==  0);
          CHECK (d2( 9) ==  0);
          CHECK (d2(10) ==  0);
          
          ctxParameter = 8.0;
          CHECK (d2( 0) == +1);
          CHECK (d2( 1) == +1);
          CHECK (d2( 2) == +2);
          CHECK (d2( 3) == +2);
          CHECK (d2( 4) == -2);
          CHECK (d2( 5) == -2);
          CHECK (d2( 6) == -1);
          CHECK (d2( 7) == -1); // cycle-length: 8
          CHECK (d2( 8) ==  0);
          CHECK (d2( 9) ==  0);
          CHECK (d2(10) ==  0);
          
          // and can of course dynamically tweak the mapping profile...
          d2.maxVal(0).probability(0.5);
          CHECK (d2( 0) ==  0);
          CHECK (d2( 1) ==  0);
          CHECK (d2( 2) ==  0);
          CHECK (d2( 3) ==  0);
          CHECK (d2( 4) == -2); // start here due to probability 0.5
          CHECK (d2( 5) == -2);
          CHECK (d2( 6) == -1);
          CHECK (d2( 7) == -1); // cycle-length: 8
          CHECK (d2( 8) ==  0);
          CHECK (d2( 9) ==  0);
          CHECK (d2(10) ==  0);
          
          // NOTE: once a custom mapping function has been installed,
          //       the object can no longer be moved, due to reference binding.
          VERIFY_ERROR (LIFECYCLE, Draw dx{move(d2)} );
        }
      
      
      
      
      /** @test change the generation profile dynamically, based on current input;
       *        in the example here, the probability is manipulated in each cycle.
       */
      void
      verify_dynamicChange()
        {
          auto d1 = Draw([](uint cycle, uint)
                            {  // dynamically control probability
                              return Draw().probability((cycle+1)*0.25);
                            });
          
          CHECK (d1(        0) ==  0);
          CHECK (d1(        8) ==  0);
          CHECK (d1(       16) ==  0);
          CHECK (d1(       24) ==  0);
          CHECK (d1(       32) ==  0);
          CHECK (d1(       40) ==  0);
          CHECK (d1(       48) ==  1); // 1st cycle: 25% probability
          CHECK (d1(       56) == -2);
          CHECK (d1(       63) == -1);
          CHECK (d1(    64 +0) ==  0);
          CHECK (d1(    64 +8) ==  0);
          CHECK (d1(    64+16) ==  0);
          CHECK (d1(    64+24) ==  0);
          CHECK (d1(    64+32) ==  1); // 2nd cycle: 50% probability
          CHECK (d1(    64+40) ==  2);
          CHECK (d1(    64+48) == -2);
          CHECK (d1(    64+56) == -1);
          CHECK (d1(    64+63) == -1);
          CHECK (d1(128    +0) ==  0);
          CHECK (d1(128    +8) ==  0);
          CHECK (d1(128   +16) ==  1); // 3rd cycle: 75% probability
          CHECK (d1(128   +24) ==  1);
          CHECK (d1(128   +32) ==  2);
          CHECK (d1(128   +40) == -2);
          CHECK (d1(128   +48) == -2);
          CHECK (d1(128   +56) == -1);
          CHECK (d1(128   +63) == -1);
          CHECK (d1(128+64 +0) ==  1); // 4rth cycle: 100% probability
          CHECK (d1(128+64 +8) ==  1);
          CHECK (d1(128+64+16) ==  2);
          CHECK (d1(128+64+24) ==  2);
          CHECK (d1(128+64+32) == -2);
          CHECK (d1(128+64+40) == -2);
          CHECK (d1(128+64+48) == -1);
          CHECK (d1(128+64+56) == -1);
          CHECK (d1(128+64+63) == -1);
          CHECK (d1(128+64+64) ==  1);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (RandomDraw_test, "unit common");
  
  
}} // namespace lib::test
