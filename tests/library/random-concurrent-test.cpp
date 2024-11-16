/*
  RandomConcurrent(Test)  -  investigate concurrent random number generation

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

/** @file random-concurrent-test.cpp
 ** unit test \ref RandomConcurrent_test
 */



#include "lib/test/run.hpp"
#include "lib/sync-barrier.hpp"
#include "lib/random.hpp"
#include "lib/thread.hpp"
#include "lib/sync.hpp"
#include "lib/util.hpp"
#include "lib/scoped-collection.hpp"
#include "lib/test/microbenchmark.hpp"
#include "lib/test/diagnostic-output.hpp"

#include <deque>
//using util::isLimited;

namespace lib {
namespace test {
  
  namespace {
    const uint NUM_THREADS = 8;
    
    const uint NUM_INVOKE = 1'000'000;
    
    
  }
  
  /******************************************************************//**
   * @test demonstrate simple access to random number generation,
   *       as well as the setup of controlled random number sequences.
   * @see  random.hpp
   */
  class RandomConcurrent_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          investigate_concurrentAccess();
        }
      
      
      /** @test examine behaviour of PRNG under concurrency stress */
      void
      investigate_concurrentAccess()
        {
          struct Results
            : std::deque<double>
            , Sync<>
            {
              void
              post (double val)
                {
                  Lock sync(this);
                  push_back (val);
                }
            };
            
            Results results;
          
          const uint N = NUM_INVOKE;
          auto expect = RAND_MAX / 2;
          
          auto drawRandom = [&]()
                              {
                                double avg{0.0};
                                for (uint i=0; i<N; ++i)
                                  avg += 1.0/N * rani();
                                auto error = avg/expect - 1;
                                results.post (error);
                              };
          
          auto [dur,sum] = threadBenchmark<NUM_THREADS> (drawRandom, 1);
          for (auto res : results)
            SHOW_EXPR(res);
          SHOW_EXPR(sum)
          SHOW_EXPR(dur/NUM_INVOKE)
        }
    };
  
  LAUNCHER (RandomConcurrent_test, "unit common");
  
  
}} // namespace lib::test
