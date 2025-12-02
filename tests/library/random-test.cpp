/*
  Random(Test)  -  verify framework for controlled random number generation

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file random-test.cpp
 ** unit test \ref Random_test
 */



#include "lib/test/run.hpp"
#include "lib/random.hpp"
#include "lib/util.hpp"
#include "lib/test/diagnostic-output.hpp"

using util::isLimited;

namespace lib {
namespace test {
  
  /******************************************************************//**
   * @test demonstrate simple access to random number generation,
   *       as well as the setup of controlled random number sequences.
   * @see  random.hpp
   */
  class Random_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verify_distributionVariants();
          verify_reproducibleSequence();
        }
      
      
      /** @test demonstrate usage of default random number generators.
       *  @note should [draw a seed](\ref Test::seedRand()) once per Test instance
       */
      void
      simpleUsage()
        {
          seedRand();
          
          int r1 = rani();
          CHECK (0 <= r1 and r1 < RAND_MAX);
          
          int r2 = rani();
          CHECK (0 <= r2 and r2 < RAND_MAX);
          CHECK (r1 != r2);              // may fail with very low probability
        }
      
      
      /** @test properties of predefined distributions provided for convenience
       *      - the upper bound for `rani(bound)` is exclusive
       *      - uniform distributions are sufficiently uniform
       *      - spread of normal distribution is within expected scale
       */
      void
      verify_distributionVariants()
        {
          double avg{0.0};
          const uint N = 1e6;
          for (uint i=0; i < N; ++i)
            avg += 1.0/N * rani (1000);
          
          auto expect = 500;
          auto error = fabs(avg/expect - 1);
          CHECK (error < 0.005);
          
          for (uint i=0; i < N; ++i)
            CHECK (isLimited(0, rani(5), 4));
          
          for (uint i=0; i < N; ++i)
            CHECK (0 != ranHash());
          
          auto sqr = [](double v){ return v*v; };
          
          double spread{0.0};
          for (uint i=0; i < N; ++i)
            spread += sqr (ranNormal() - 0.5);
          spread = sqrt (spread/N);
          CHECK (spread < 1.12);
        }
      
      
      
      /** @test demonstrate that random number sequences can be reproduced
       *      - use a rigged SeedNucleus, always returning a fixed sees
       *      - build two distinct random sequence generators, yet seeded
       *        from the same source; they will produce the same sequence
       *      - sequences can be re-shuffled by a seed value, so that
       *        the following random numbers will start to differ
       *      - but even this re-shuffling is deterministic
       */
      void
      verify_reproducibleSequence()
        {
          class : public SeedNucleus
            {
              uint64_t getSeed()  override  { return 55; }
            }
            coreOfEvil;
          
          Random src1{coreOfEvil};
          
          int      r1 = src1.i32();
          uint64_t r2 = src1.u64();
          double   r3 = src1.uni();
          
          Random src2{coreOfEvil};
          CHECK (r1 == src2.i32());
          CHECK (r2 == src2.u64());
          CHECK (r3 == src2.uni());
          
          src1.reseed (coreOfEvil);
          CHECK (src1.u64() != src2.u64());
          
          src2.reseed (coreOfEvil);
          CHECK (src1.u64() != src2.u64());
          (void) src2.u64();
          CHECK (src1.u64() == src2.u64());
          CHECK (src1.i32() == src2.i32());
          CHECK (src1.uni() == src2.uni());
        }
    };
  
  LAUNCHER (Random_test, "unit common");
  
  
}} // namespace lib::test
