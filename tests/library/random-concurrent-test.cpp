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
#include <tuple>
//#include <array>
//using util::isLimited;
//using std::array;
using std::tuple;
using std::deque;

namespace lib {
namespace test {
  
  namespace {
    const uint NUM_THREADS = 8;
    const uint NUM_REPEATS = 10;
    const uint NUM_INVOKES = 1'000'000;
    
    
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
            : deque<tuple<double,uint>>
            , Sync<>
            {
              void
              post (double err, uint fails)
                {
                  Lock sync(this);
                  emplace_back (err, fails);
                }
            };
            
            Results results;
          
          using Engine = std::mt19937;
//        using Engine = std::mt19937_64;
          
          Engine ranGen{defaultGen.u64()};
          
          const uint N = NUM_INVOKES;
          auto expect = (Engine::max() - Engine::min()) / 2;
          
          auto drawRandom = [&]()
                              {
                                uint fail{0};
                                double avg{0.0};
                                for (uint i=0; i<N; ++i)
                                  {
                                    auto r = ranGen();
                                    if (r < Engine::min() or r > Engine::max())
                                      ++fail;
                                    avg += 1.0/N * (r % Engine::max());
                                  }
                                auto error = avg/expect - 1;
                                results.post (error, fail);
                              };
          
          auto [dur,sum] = threadBenchmark<NUM_THREADS> (drawRandom, NUM_REPEATS);
          for (auto res : results)
            SHOW_EXPR(res);
          SHOW_EXPR(sum)
          SHOW_EXPR(dur/NUM_INVOKES)
        }
    };
  
  LAUNCHER (RandomConcurrent_test, "unit common");
  
  
}} // namespace lib::test
