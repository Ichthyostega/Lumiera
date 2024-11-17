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
      run (Arg arg)
        {
          seedRand();
          benchmark_random_gen();
          if ("quick" != firstTok (arg))
            investigate_concurrentAccess();
        }
      
      
      /** @test microbenchmark of various random number generators
       * @remark typical values
       *       - `rand()` (trinomial generator) : 15ns / 10ns (O3)
       *       - Mersenne twister 64bit : 55ns / 25ns (O3)
       *       - reading /dev/urandom   : 480ns / 470 (O3)
       */
      void
      benchmark_random_gen()
        {
          auto do_nothing = []{ /* take it easy */       };
          auto mersenne64 = []{ return rani();           };
          auto legacy_gen = []{ return rand();           };
          std::random_device entropySource{"/dev/urandom"};
          auto rly_random = [&]{ return entropySource(); };
          
          _Fmt resultDisplay{"µ-bench(%s)%|45T.| %5.3f µs"};
          
          double d1 = microBenchmark (do_nothing, NUM_INVOKES).first;
          cout << resultDisplay % "(empty call)" % d1 <<endl;
          
          double d2 = microBenchmark (mersenne64, NUM_INVOKES).first;
          cout << resultDisplay % "Mersenne-64" % d2 <<endl;
          
          double d3 = microBenchmark (legacy_gen, NUM_INVOKES).first;
          cout << resultDisplay % "std::rand()" % d3 <<endl;
          
          double d4 = microBenchmark (rly_random, NUM_INVOKES).first;
          cout << resultDisplay % "/dev/urandom" % d4 <<endl;
          
          CHECK (d3 < d2 and d2 < d4);
        }
      
      
      /**
       * Research setup to investigate concurrent access to a random generator.
       * From each test thread, the shared generator instance is invoked a huge number times
       * (defined by NUM_INVOKES), thereby computing the mean value and checking for defect
       * numbers outside the generator's definition range. This probe cycle is repeated
       * several times (defined by NUM_SAMPLES) and the results are collected and evaluated
       * afterwards to detect signs of a skewed distribution.
       * @tparam GEN     a C++ compliant generator type
       * @tparam threads number of threads to run in parallel
       * @remark Pseudo random number generation as such is not threadsafe, and pressing for
       *         concurrent access (as done here) will produce a corrupted internal generator
       *         state sooner or later. Under some circumstances however, theses glitches
       *         can be ignored, if quality of generated numbers actually does not matter.
       */
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

          /** run the experiment series */
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
                                        avg += 1.0/N * r;
                                      }
                                    auto error = avg/expect - 1;
                                    recordRun (error, fail);
                                  };
              
              threadBenchmark<threads> (drawRandom, REPEATS);
              
              uint cases{0}, lows{0}, glitches{0};
              _Fmt resultLine{"%6.3f ‰ : %d %s"};
              for (auto [err,fails] : results)
                {
                  bool isGlitch = fails or fabs(err) > 3 * 1/sqrt(N);       // mean of a sound distribution will remain within bounds
                  cout << resultLine % (err*1000)
                                     % fails
                                     % (fails? "FAIL": isGlitch? " !! ":"") << endl;
                  ++cases;
                  if (err < 0)  ++lows;
                  if (isGlitch) ++glitches;
                }
               // assess overall results......
              percentGlitches = 100.0 * glitches/cases;
              percentTilted   = 100.0 * fabs(double(lows)/cases - 0.5)*2;   // degree to which mean is biased for one side
              isFailure = glitches or percentTilted > 30;                   // (empirical trigger criterion)
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
      
      
      /** @test examine behaviour of PRNG under concurrency stress
       *      - running a 32bit generator single threaded should not trigger alarms
       *      - while under concurrent pressure several defect numbers should be produced
       *      - even the 64bit generator will show uneven distribution due to corrupted state
       *      - the 32bit generator capped to its valid range exhibits skew only occasionally
       * @see lib::CappedGen
       */
      void
      investigate_concurrentAccess()
        {
          using Mersenne64 = std::mt19937_64;
          using Mersenne32 = std::mt19937;
          using CappedMs32 = CappedGen<Mersenne32>;
          
          Experiment<Mersenne32,1>           single_mers32{Mersenne32(defaultGen.uni())};
          Experiment<Mersenne32,NUM_THREADS> concurr_mers32{Mersenne32(defaultGen.uni())};
          Experiment<Mersenne64,NUM_THREADS> concurr_mers64{Mersenne64(defaultGen.uni())};
          Experiment<CappedMs32,NUM_THREADS> concCap_mers32{CappedMs32(defaultGen.uni())};
          
          single_mers32.perform();
          concurr_mers32.perform();
          concurr_mers64.perform();
          concCap_mers32.perform();
          
          CHECK (not single_mers32.isFailure,  "ALARM : single-threaded Mersenne-Twister 32bit produces skewed distribution");
          CHECK (    concurr_mers32.isFailure, "SURPRISE : Mersenne-Twister 32bit encountered NO glitches under concurrent pressure");
          CHECK (    concurr_mers64.isFailure, "SURPRISE : Mersenne-Twister 64bit encountered NO glitches under concurrent pressure");
        }
    };
  
  LAUNCHER (RandomConcurrent_test, "unit common");
  
  
}} // namespace lib::test
