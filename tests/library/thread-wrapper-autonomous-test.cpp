/*
  ThreadWrapperAutonomous(Test)  -  launching a self-contained and completely detached thread

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

/** @file thread-wrapper-autonomous-test.cpp
 ** unit test \ref ThreadWrapperAutonomous_test
 */


#include "lib/test/run.hpp"
#include "lib/test/testdummy.hpp"
#include "lib/thread.hpp"
#include "lib/test/diagnostic-output.hpp"///////////////////TODO

#include <atomic>
#include <chrono>

using test::Test;
using std::atomic_uint;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using namespace std::chrono_literals;


namespace lib {
namespace test{
  
    namespace { // test parameters
      
      const uint NUM_THREADS = 200;
      const uint REPETITIONS = 10;
    }
    
    
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
            atomic_uint i{0};
            launchDetached ("anarchy", [&]{ ++i; });
            
            sleep_for(1ms);
            CHECK (i == 1);                            // verify the effect has taken place
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
SHOW_EXPR(extra)
                      sleep_for (5ms);
SHOW_EXPR("bye")
                    }
                };
            //
            CHECK (0 == Dummy::checksum());
            //
            launchDetached<TestThread> (&TestThread::doIt, 55);
            
            CHECK (0 < Dummy::checksum());
            sleep_for (10ms);
            CHECK (0 == Dummy::checksum());
          }
      };
    
    
    
    /** Register this test class... */
    LAUNCHER (ThreadWrapperAutonomous_test, "function common");
    
    
    
}} // namespace lib::test
