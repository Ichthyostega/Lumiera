/*
  SyncBarrier(Test)  -  verify yield-waiting synchronisation latch

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

/** @file sync-barrier-test.cpp
 ** unit test \ref SyncBarrier_test
 */


#include "lib/test/run.hpp"
#include "lib/sync-barrier.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/thread.hpp"

#include <chrono>
#include <atomic>
#include <array>

using test::Test;
using lib::explore;
using std::array;

using std::atomic_uint;
using std::this_thread::sleep_for;
using namespace std::chrono_literals;


namespace lib {
namespace test {
  
  namespace {// Test setup for a concurrent calculation with checksum....
    
    const uint NUM_THREADS = 256;
    
    atomic_uint stage1{0};
    atomic_uint stage2{0};
    atomic_uint finish{0};
    
    SyncBarrier interThread{NUM_THREADS  };
    SyncBarrier afterThread{NUM_THREADS+1};
    
    /**
     * A test thread to perform a summation protocol including synchronisation points
     * - build a compound sum of random numbers in the first stage
     * - wait for the compound sum to build up completely
     * - book in the compound sum plus a further random number
     */
    class TestThread
      : public lib::Thread
      {
        public:
          TestThread()
            : Thread{"Load Test"
                    ,[&, ran=lib::Random{seedFromDefaultGen()}]
                     () mutable
                        {                                   //-STAGE-1------------------------------
                          localSum = ran.i(1000);           // generate local value
                          stage1.fetch_add (localSum);      // book in local value
                          interThread.sync();               // wait for all other threads to have booked in
                          
                                                            //-STAGE-2------------------------------
                          uint sync = stage1;               // pick up compounded sum from STAGE-1
                          localSum += ran.i(1000);          // add further local value for STAGE-2
                          stage2.fetch_add (localSum+sync); // book in both local values and synced sum
                          afterThread.sync();               // wait for other threads and supervisor
                          
                          finish.fetch_add(1);              // mark completion of this thread
                        }}
            { }
          
          uint localSum; // *deliberately* not initialised to avoid race
      };
    
    
    /** sum up all `localSum` fields from all TestThread instances in a container */
    template<class CON>
    uint
    sumLocals (CON const& threads)
    {
      return explore (threads)
              .reduce ([&](TestThread const& t){ return t.localSum; });
    }
  }//(End)Test setup
  
  
  
  
  /*******************************************************************//**
   * @test verify N-fold synchronisation points by multi-thread loat-test.
   *       - start a _huge number_ of TestThread
   *       - all those pick up the partial sum from stage1
   * @remark without coordinated synchronisation, some threads would see
   *         an incomplete sum and thus the stage2 checksum would be lower
   * @see lib::SyncBarrier
   * @see steam::control::DispatcherLoop
   */
  class SyncBarrier_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          // Launch several TestThread
          array<TestThread,NUM_THREADS> threads;
          
          CHECK (0 == finish);
          CHECK (explore(threads).and_all());
          
          afterThread.sync();
          sleep_for (5ms); // give the threads a chance to terminate
          
          CHECK (NUM_THREADS == finish);                                // all threads have passed out....
          CHECK (0 < stage1);
          CHECK (stage1 < stage2);
          CHECK (stage2 > sumLocals(threads));
          CHECK (stage2 == sumLocals(threads) + NUM_THREADS*stage1);    // this holds only if all threads waited to get the complete stage1 sum
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SyncBarrier_test, "function common");
  
  
  
}} // namespace lib::test
