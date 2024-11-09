/*
  Random(Test)  -  verify framework for controlled random number generation

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file random-test.cpp
 ** unit test \ref Random_test
 */



#include "lib/test/run.hpp"
#include "lib/random.hpp"


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
          verify_reproducibleSequence();
        }
      
      
      /** @test demonstrate usage of default random number generators */
      void
      simpleUsage()
        {
          int r1 = rani();
          CHECK (0 <= r1 and r1 < RAND_MAX);
          
          int r2 = rani();
          CHECK (0 <= r2 and r2 < RAND_MAX);
          CHECK (r1 != r2);
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
