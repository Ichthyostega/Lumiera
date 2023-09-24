/*
  SyncBarrierPerformance(Test)  -  investigate performance of yield-waiting synchronisation

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

/** @file sync-barrier-performance-test.cpp
 ** Assess the performance characteristics of lib::SyncBarrier
 ** Helpers and setup for the \ref SyncBarrierPerformance_test
 */


#include "lib/test/run.hpp"
#include "lib/sync-barrier.hpp"
//#include "lib/iter-explorer.hpp"
//#include "lib/util-foreach.hpp"
#include "lib/test/microbenchmark.hpp"
#include "lib/test/diagnostic-output.hpp"  /////////////////////TODO

//#include <chrono>
//#include <thread>
//#include <atomic>
#include <array>

using test::Test;
//using util::and_all;
//using lib::explore;
using std::array;

//using std::atomic_uint;
using std::this_thread::sleep_for;
using namespace std::chrono_literals;


namespace lib {
namespace test {
  
  namespace {// Test setup for a concurrent calculation with checksum....
    
    const uint NUM_STAGES = 1024;
    
    /**
     */
    class FakeBarrier
      {
        public:
      };
  }//(End)Test setup
  
  
  
  
  /*******************************************************************//**
   * @test investigate performance of N-fold thread synchronisation.
   *       - start a _huge number_ of TestThread
   *       - all those pick up the partial sum from stage1
   * @remark without coordinated synchronisation, some threads would see
   *         an incomplete sum and thus the stage2 checksum would be lower
   * @see lib::SyncBarrier
   * @see steam::control::DispatcherLoop
   */
  class SyncBarrierPerformance_test : public Test
    {
      template<size_t nThreads>
      double
      performanceTest()
        {
          auto testSubject = [&](size_t i) -> size_t
                                {
                                  sleep_for (1us);
                                  return 1;
                                };
          
          auto [micros, cnt] = threadBenchmark<nThreads> (testSubject, NUM_STAGES);
          CHECK (cnt == nThreads*NUM_STAGES);
          return micros;
        }
      
      
      virtual void
      run (Arg)
        {
          cout<<"\n\n■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■"<<endl;
          double time_emptySetup = performanceTest<100>();
          cout<<"\n___Microbenchmark____"
              <<"\nemptySetup  : "<<time_emptySetup
              <<"\n_____________________\n"
              <<"\nbarriers..... "<<NUM_STAGES
              <<endl;
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SyncBarrierPerformance_test, "function common");
  
  
  
}} // namespace lib::test
