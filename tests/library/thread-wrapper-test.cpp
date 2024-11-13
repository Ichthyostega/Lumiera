/*
  ThreadWrapper(Test)  -  starting threads and passing context

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file thread-wrapper-test.cpp
 ** unit test \ref ThreadWrapper_test
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
     * @test use the lib::Thread wrapper for simplified definition of the
     *       thread-function, argument binding and starting of threads.
     * @see thread.hpp
     * @see ThreadWrapperJoin_test
     * @see SyncLocking_test
     */
    class ThreadWrapper_test : public Test
      {
        
        virtual void
        run (Arg)
          {
            seedRand();
            demonstrateSimpleUsage();
            verifyConcurrentExecution();
          }
        
        
        /** @test demonstrate simple usage of the thread-wrapper a λ-binding */
        void
        demonstrateSimpleUsage()
          {
            atomic_uint i{0};
            Thread thread("counter", [&]{ ++i; });     // bind a λ and launch thread
            while (thread) yield();                    // ensure thread has finished and detached
            
            CHECK (i == 1);                            // verify the effect has taken place
          }
        
        
        /**
         * @test verify the thread function is actually performed concurrently
         *       - use a derived Thread object, also holding a local data field
         *       - the thread function sleeps, and then stores the sum of two numbers
         *       - demonstrate that each instance can have a different argument binding
         *       - verify each thread function has actually been invoked once per thread,
         *         by comparing a local sum with values collected from the Thread objects,
         *       - moreover measure the overall time required for launching the threads
         *         and then waiting for all threads to have terminated and detached;
         *         this time must be _shorter_ than all the _average_ sleep times
         *         compounded (as if the function was invoked sequentially).
         */
        void
        verifyConcurrentExecution()
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
                                        uint x = rani(1000);
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
    LAUNCHER (ThreadWrapper_test, "function common");
    
    
    
}} // namespace lib::test
