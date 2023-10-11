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
#include "lib/test/testdummy.hpp"

#include <atomic>
#include <chrono>
#include <memory>

using test::Test;
using lib::explore;
using lib::test::Dummy;
using std::atomic_uint;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using namespace std::chrono_literals;
using std::chrono::system_clock;
using std::unique_ptr;


namespace lib {
namespace test{
  
    namespace {
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
        
        
        
        /** @test attach user provided callback hooks to the thread lifecycle.
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
         * @test verify a special setup to start a thread explicitly
         *       and to track the thread's lifecycle state.
         *     - use a component to encapsulate the thread
         *     - this TestThread component is managed in a `unique_ptr`
         *     - thus it is explicitly possible to be _not_ in _running state_
         *     - when starting the TestThread, a lifecycle callback is bound
         *     - at termination this callback will clear the unique_ptr
         *     - thus allocation and _running state_ is tied to the lifecycle
         */
        void
        demonstrateExplicitThreadLifecycle()
          {
              struct TestThread
                : ThreadHookable
                {
                  using ThreadHookable::ThreadHookable;
                  
                  atomic_uint processVal{23};
                  
                  void
                  doIt (uint haveFun)
                    {
                      sleep_for (100us);
                      processVal = haveFun;
                      sleep_for (5ms);
                    }
                };
            // Note the Dummy member allows to watch instance lifecycle
            CHECK (0 == Dummy::checksum());
            
            // the frontEnd allows to access the TestThread component
            // and also represents the running state
            unique_ptr<TestThread> frontEnd;
            CHECK (not frontEnd);  // obviously not running yet
            
            // start the thread and wire lifecycle callbacks
            frontEnd.reset (new TestThread{
                                  TestThread::Launch{&TestThread::doIt, 55u}
                                            .atExit([&]{ frontEnd.reset(); })
                                            .onOrphan([](thread::ThreadWrapper& wrapper)
                                                       { wrapper.detach_thread_from_wrapper(); })
                                          });

            CHECK (frontEnd);                          // thread now marked as running
            
            CHECK (23 == frontEnd->processVal);        // this value was set by the ctor in this thread
            sleep_for (1ms);                           // wait for the thread function to become active
            CHECK (55 == frontEnd->processVal);        // changed by thread function
            sleep_for (10ms);
            
            CHECK (not frontEnd);                      // meanwhile thread has finished
          }                                            // and also cleared the front-end from the `atExit`-hook
      };
    
    
    
    /** Register this test class... */
    LAUNCHER (ThreadWrapperLifecycle_test, "function common");
    
    
    
}} // namespace lib::test
