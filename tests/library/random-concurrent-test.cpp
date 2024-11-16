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
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/test/diagnostic-output.hpp"

#include <deque>
#include <tuple>
//#include <array>
//using util::isLimited;
//using std::array;
using std::tuple;
using std::deque;
using util::_Fmt;

namespace lib {
namespace test {
  
  namespace {
    const uint NUM_THREADS = 8;           ///< for concurrent probes
    const uint NUM_SAMPLES = 80;          ///< overall number measurement runs
    const uint NUM_INVOKES = 1'000'000;   ///< invocations of the target per measurment
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
      
      
      template<typename GEN, uint threads>
      struct Experiment
        : Sync<>
        {
          deque<tuple<double,uint>> results;
          
          void
          recordRun (double err, uint fails)
            {
              Lock sync(this);
              results.emplace_back (err, fails);
            }
          
          
          GEN generator;
          
          Experiment(GEN&& fun)
            : generator{move (fun)}
            { }
          
          const uint N = NUM_INVOKES;
          const uint REPEATS = NUM_SAMPLES / threads;
          using ResVal = typename GEN::result_type;
          ResVal expect = (GEN::max() - GEN::min()) / 2;
          
          /* === Measurement Results === */
          double percentGlitches{0.0};
          double percentTilted  {0.0};
          bool   isFailure      {false};

            
          void
          perform()
            {
              auto drawRandom = [&]()
                                  {
                                    uint fail{0};
                                    double avg{0.0};
                                    for (uint i=0; i<N; ++i)
                                      {
                                        auto r = generator();
                                        if (r < GEN::min() or r > GEN::max())
                                          ++fail;
                                        avg += 1.0/N * r;//(r % Engine::max());
                                      }
                                    auto error = avg/expect - 1;
                                    recordRun (error, fail);
                                  };
              
              threadBenchmark<threads> (drawRandom, REPEATS);
              
              uint cases{0}, lows{0}, glitches{0};
              _Fmt resultLine{"%6.3f ‰ : %d %s"};
              for (auto [err,fails] : results)
                {
                  bool isGlitch = fails or fabs(err) >0.003;
                  cout << resultLine % (err*1000)
                                     % fails
                                     % (fails? "FAIL": isGlitch? " !! ":"") << endl;
                  ++cases;
                  if (err < 0)  ++lows;
                  if (isGlitch) ++glitches;
                }
               // assess overall results......
              percentGlitches = 100.0 * glitches/cases;
              percentTilted   = 100.0 * fabs(double(lows)/cases - 0.5)*2;
              isFailure = glitches or percentTilted > 30;
              cout << _Fmt{"++-------------++  %s\n"
                           "  Glitches: %5.1f %%\n"
                           "    Tilted: %5.1f %%\n"
                           "++-------------++\n"}
                          % (isFailure? "FAIL": "(ok)")
                          % percentGlitches
                          % percentTilted
                   << endl;
            }
        };
        
        
      /** @test examine behaviour of PRNG under concurrency stress */
      void
      investigate_concurrentAccess()
        {
          using Mersenne32 = std::mt19937;
          using Mersenne64 = std::mt19937_64;
          
          Experiment<Mersenne32,1>           single32{Mersenne32(defaultGen.uni())};
          Experiment<Mersenne32,NUM_THREADS> concurr32{Mersenne32(defaultGen.uni())};
          Experiment<Mersenne64,NUM_THREADS> concurr64{Mersenne64(defaultGen.uni())};
          
          single32.perform();
          concurr32.perform();
          concurr64.perform();
          
          CHECK (not single32.isFailure, "ALARM : single-threaded Mersenne-Twister 32bit produces skewed distribution");
          CHECK (    single32.isFailure, "SURPRISE : Mersenne-Twister 32bit encountered NO glitches under concurrent pressure");
          CHECK (    single64.isFailure, "SURPRISE : Mersenne-Twister 64bit encountered NO glitches under concurrent pressure");
        }
    };
  
  LAUNCHER (RandomConcurrent_test, "unit common");
  
  
}} // namespace lib::test
