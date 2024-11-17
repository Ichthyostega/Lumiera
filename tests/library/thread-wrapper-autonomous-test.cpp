/*
  ThreadWrapperAutonomous(Test)  -  launching a self-contained and completely detached thread

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file thread-wrapper-autonomous-test.cpp
 ** unit test \ref ThreadWrapperAutonomous_test
 */


#include "lib/test/run.hpp"
#include "lib/test/tracking-dummy.hpp"
#include "lib/thread.hpp"

#include <atomic>
#include <chrono>

using test::Test;
using std::atomic_bool;
using std::this_thread::sleep_for;
using namespace std::chrono_literals;


namespace lib {
namespace test{
    
    
    /*******************************************************************//**
     * @test a variation of the Thread wrapper to launch a detached thread,
     *       with automatic memory management for the _thread-object._.
     * @see thread.hpp
     * @see ThreadWrapper_test
     */
    class ThreadWrapperAutonomous_test : public Test
      {
        
        virtual void
        run (Arg)
          {
            demonstrateSimpleUsage();
            verifyMemoryManagement();
          }
        
        
        /** @test demonstrate simply launching a λ-function into background */
        void
        demonstrateSimpleUsage()
          {
            atomic_bool didRun{false};
            launchDetached ("anarchy", [&]{ didRun = true; });
            
            sleep_for(1ms);
            CHECK (didRun);  // verify the effect has taken place
          }
        
        
        /**
         * @test verify the detached thread autonomously manages its memory.
         */
        void
        verifyMemoryManagement()
          {
              struct TestThread
                : ThreadHookable
                {
                  using ThreadHookable::ThreadHookable;
                  
                  Dummy watcher;
                  
                  void
                  doIt (int extra)
                    {
                      watcher.setVal (extra);
                      sleep_for (5ms);
                    }
                };
            // Note the Dummy member allows to watch instance lifecycle
            CHECK (0 == Dummy::checksum());
            
            launchDetached<TestThread> (&TestThread::doIt, 55);
            
            CHECK (0 < Dummy::checksum());
            sleep_for (1ms);
            CHECK (55 == Dummy::checksum());
            sleep_for (10ms);
            CHECK (0 == Dummy::checksum());
          }
      };
    
    
    
    /** Register this test class... */
    LAUNCHER (ThreadWrapperAutonomous_test, "function common");
    
    
    
}} // namespace lib::test
