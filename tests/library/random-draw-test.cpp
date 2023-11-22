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
#include "lib/time/timevalue.hpp"
#include "lib/test/diagnostic-output.hpp"////////////////////TODO
#include "lib/format-string.hpp"

#include <array>



namespace lib {
namespace test{
  
  using util::_Fmt;
  using lib::time::FSecs;
  using lib::time::TimeVar;
  
  
  
  namespace { // policy and configuration for test...
    
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
        
        template<typename RES>
        struct Adaptor<RES(size_t)>
          {
            template<typename FUN>
            static decltype(auto)
            build (FUN&& fun)
              {
                return std::forward<FUN>(fun);
              }
          };
        
        template<typename RES>
        struct Adaptor<RES(void)>
          {
            template<typename FUN>
            static auto
            build (FUN&& fun)
              {
                return [functor=std::forward<FUN>(fun)]
                       (size_t)
                          {
                            return functor();
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
          verify_buildProfile();
          verify_dynamicChange();
        }
      
      
      
      /** @test demonstrate a basic usage scenario
       * @todo WIP 11/23 ✔ define ⟶ ✔ implement
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
          CHECK (draw( 64) ==  0);
          CHECK (draw( 95) ==  0);
          CHECK (draw( 96) ==  1);
          CHECK (draw(127) == -1);
          CHECK (draw(128) ==  0);
          CHECK (draw(168) ==  2);
          CHECK (draw(256) ==  0);
        }
      
      
      
      /** @test TODO verify configuration through policy template
       * @todo WIP 11/23 🔁 define ⟶ 🔁 implement
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
      
      
      
      /** @test verify random number transformations
       *      - use a Draw instance with result values `[-2..0..+2]`
       *      - values are evenly distributed within limits of quantisation
       *      - the probability parameter controls the amount of neutral results
       *      - maximum and minimum value settings will be respected
       *      - the interval [min..max] is independent from neutral value
       *      - probability defines the cases within [min..max] \ neutral
       *      - all other cases `q = 1 - p` will yield the neutral value
       *      - implausible max/min settings will be corrected automatically
       * @todo WIP 11/23 ✔ define ⟶ ✔ implement
       */
      void
      verify_numerics()
        {
          auto distribution = [](Draw const& draw)
                  {
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
      
      
      
      /** @test TODO verify the Builder-API to define the profile of result values.
       * @todo WIP 11/23 🔁 define ⟶ implement
       */
      void
      verify_buildProfile()
        {
          UNIMPLEMENTED ("verify random number profile configuration");
        }
      
      
      
      /** @test TODO change the generation profile dynamically
       * @todo WIP 11/23 🔁 define ⟶ implement
       */
      void
      verify_dynamicChange()
        {
          UNIMPLEMENTED ("change the generation profile dynamically");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (RandomDraw_test, "unit common");
  
  
}} // namespace lib::test
