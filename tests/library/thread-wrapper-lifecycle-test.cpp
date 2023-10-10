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
#include "lib/test/diagnostic-output.hpp"

#include <atomic>
#include <chrono>

using test::Test;
using lib::explore;
using std::atomic_uint;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using namespace std::chrono_literals;
using std::chrono::system_clock;


namespace lib {
namespace test{
  
    namespace { // test parameters
      
      const uint NUM_THREADS = 200;
      const uint REPETITIONS = 10;
      
      using CLOCK_SCALE = std::micro; // Results are in µ-sec
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
            verifyThreadLifecycleHooks();
            demonstrateExplicitThreadLifecycle();
          }
        
        
        /** @test demonstrate terms of lifecycle for the default case */
        void
        defaultWrapperLifecycle()
          {
            using Dur = std::chrono::duration<double, CLOCK_SCALE>;
            using Point = system_clock::time_point;
            Point threadStart;
            Point afterCtor;
            
            // the new thread starts immediately from ctor-call...
            Thread thread("lifecycle", [&]{
                                            threadStart = system_clock::now();
                                          });
            afterCtor = system_clock::now();
            CHECK (thread);               // thread marked as running
            
            while (thread) yield();
            CHECK (not thread);           // thread now marked as detached/dead
            
            double offset = Dur{threadStart - afterCtor}.count();
            CHECK (offset > 0);
          }    //  Note: in practice we see here values > 100µs
              //         but in theory the thread might even overtake the launcher 
        
        
        /**
         * @test attach user provided callback hooks to the thread lifecycle.
         */
        void
        verifyThreadLifecycleHooks()
          {
            atomic_uint stage{0};              // flexible launch-builder syntax:
            ThreadHookable thread{ThreadHookable::Launch([]{ sleep_for (5ms); })
                                                .atStart([&]{ stage = 1; })
                                                .atExit ([&]{ stage = 2; })
                                                .threadID("hooked thread")};
            CHECK (thread);
            CHECK (0 == stage);
            
            sleep_for (1ms);
            CHECK (thread);
            CHECK (1 == stage);
            
            while (thread) yield();
            CHECK (not thread);
            CHECK (2 == stage);
          }
        
        
        /**
         * @test verify a special setup to start a thread explicitly and to track
         *       the thread's lifecycle state.
         */
        void
        demonstrateExplicitThreadLifecycle()
          {
            UNIMPLEMENTED ("demonstrate state change");
              struct TestThread
                : Thread
                {
                  using Thread::Thread;
                  
                  uint local{0};
                  
                  void
                  doIt (uint a, uint b) ///< the actual operation running in a separate thread
                    {
                      uint sum = a + b;
//                      sleep_for (microseconds{sum});  // Note: explicit random delay before local store
                      local = sum;
                    }
                };
          }
      };
    
    
    
    /** Register this test class... */
    LAUNCHER (ThreadWrapperLifecycle_test, "function common");
    
    
    
}} // namespace lib::test
