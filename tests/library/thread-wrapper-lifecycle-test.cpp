/*
  ThreadWrapperLifecycle(Test)  -  verify lifecycle aspects of the thread wrapper

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

/** @file thread-wrapper-lifecycle-test.cpp
 ** unit test \ref ThreadWrapperLifecycle_test
 */


#include "lib/test/run.hpp"
#include "lib/thread.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/scoped-collection.hpp"
#include "lib/test/microbenchmark.hpp"

#include <atomic>
#include <chrono>

using test::Test;
using lib::explore;
using std::atomic_uint;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using std::chrono::microseconds;


namespace lib {
namespace test{
  
    namespace { // test parameters
      
      const uint NUM_THREADS = 200;
      const uint REPETITIONS = 10;
    }
    
    
    /*******************************************************************//**
     * @test verify lifecycle behaviour of threads managed by thread-wrapper.
     * @see thread.hpp
     * @see ThreadWrapperBackground_test
     * @see ThreadWrapperJoin_test
     */
    class ThreadWrapperLifecycle_test : public Test
      {
        
        virtual void
        run (Arg)
          {
            defaultWrapperLifecycle();
            verifyExplicitLifecycleState();
          }
        
        
        /** @test demonstrate terms of lifecycle for the default case */
        void
        defaultWrapperLifecycle()
          {
            atomic_uint i{0};
            Thread thread("counter", [&]{ ++i; });     // bind a λ and launch thread
            while (thread) yield();                    // ensure thread has finished and detached
            
            CHECK (i == 1);                            // verify the effect has taken place
            UNIMPLEMENTED ("demonstrate state change");
          }
        
        
        /**
         * @test verify a special setup to start a thread explicitly and to track
         *       the thread's lifecycle state.
         */
        void
        verifyExplicitLifecycleState()
          {
              struct TestThread
                : Thread
                {
                  using Thread::Thread;
                  
                  uint local{0};
                  
                  void
                  doIt (uint a, uint b) ///< the actual operation running in a separate thread
                    {
                      uint sum = a + b;
                      sleep_for (microseconds{sum});  // Note: explicit random delay before local store
                      local = sum;
                    }
                };
            
            // prepare Storage for these objects (not created yet)
            lib::ScopedCollection<TestThread> threads{NUM_THREADS};
            
            size_t checkSum = 0;
            size_t globalSum = 0;
            auto launchThreads = [&]
                                  {
                                    for (uint i=1; i<=NUM_THREADS; ++i)
                                      {
                                        uint x = rand() % 1000;
                                        globalSum += (i + x);
                                        threads.emplace (&TestThread::doIt, i, x);
                                      }                            // Note: bind to member function, copying arguments
                                    
                                    while (explore(threads).has_any())
                                      yield();                  // wait for all threads to have detached
                                    
                                    for (auto& t : threads)
                                      {
                                        CHECK (0 < t.local);
                                        checkSum += t.local;
                                      }
                                  };
            
            double runTime = benchmarkTime (launchThreads, REPETITIONS);
            
            CHECK (checkSum == globalSum);           // sum of precomputed random numbers matches sum from threads
            CHECK (runTime < NUM_THREADS * 1000/2);  // random sleep time should be > 500ms on average
          }
      };
    
    
    
    /** Register this test class... */
    LAUNCHER (ThreadWrapperLifecycle_test, "function common");
    
    
    
}} // namespace lib::test
