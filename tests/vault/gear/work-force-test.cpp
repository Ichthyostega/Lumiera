/*
  WorkForce(Test)  -  verify worker thread service

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

/** @file work-force-test.cpp
 ** unit test \ref WorkForce_test
 */


#include "lib/test/run.hpp"
#include "vault/gear/work-force.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/format-cout.hpp"   ///////////////////////////////WIP
#include "lib/test/diagnostic-output.hpp"   ///////////////////////////////WIP
//#include "lib/util.hpp"

//#include <utility>
//#include <chrono>
#include <functional>
#include <thread>
#include <chrono>

using test::Test;
//using std::move;
//using util::isSameObject;


namespace vault{
namespace gear {
namespace test {
  
  using std::this_thread::sleep_for;
  using namespace std::chrono_literals;
  using std::chrono::milliseconds;
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
//  using lib::time::Time;
  
  namespace {
    using WorkFun = std::function<work::SIG_WorkFun>;
    using FinalFun = std::function<work::SIG_FinalHook>;
    
    template<class FUN>
    auto
    setup (FUN&& workFun)
    {
      struct Setup
        : work::Config
        {
          WorkFun doWork;
          FinalFun finalHook = [](bool){ /*NOP*/ };
          
          milliseconds IDLE_WAIT      = work::Config::IDLE_WAIT;
          size_t       DISMISS_CYCLES = work::Config::DISMISS_CYCLES;
          
          Setup (FUN&& workFun)
            : doWork{std::forward<FUN> (workFun)}
            { }
          
          Setup&&
          withFinalHook (FinalFun finalFun)
            {
              finalHook = move (finalFun);
              return move(*this);
            }
          
          Setup&&
          withSleepPeriod (std::chrono::milliseconds millis)
            {
              IDLE_WAIT = millis;
              return move(*this);
            }
          
          Setup&&
          dismissAfter (size_t cycles)
            {
              DISMISS_CYCLES = cycles;
              return move(*this);
            }
          
        };
       
      return Setup{std::forward<FUN> (workFun)};
    }
  }
  
  
  
  
  /*************************************************************************//**
   * @test WorkForce-Service: maintain a pool of active worker threads.
   * @see SchedulerUsage_test
   */
  class WorkForce_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          
          verify_pullWork();
          verify_workerHalt();
          verify_workerSleep();
          verify_workerDismiss();
          verify_finalHook();
          verify_detectError();
          verify_defaultPool();
          verify_scalePool();
          verify_countActive();
          verify_dtor_blocks();
        }
      
      
      /** @test demonstrate simple worker pool usage
       */
      void
      simpleUsage()
        {
          atomic<uint> check{0};
          WorkForce wof{setup ([&]{ ++check; return activity::PASS; })};
          
          CHECK (0 == check);
          
          wof.activate();
          sleep_for(20ms);
          
          CHECK (0 < check);
        }
      
      
      
      /** @test the given work-functor is invoked repeatedly, once activated.
       */
      void
      verify_pullWork()
        {
          atomic<uint> check{0};
          WorkForce wof{setup ([&]{ ++check; return activity::PASS; })};
          
          CHECK (0 == check);
          
          wof.incScale();
          sleep_for(20ms);
          
          uint invocations = check;
          CHECK (0 < invocations);
          
          sleep_for(2ms);
          CHECK (invocations < check);
          
          invocations = check;
          sleep_for(2ms);
          CHECK (invocations < check);
          
          wof.awaitShutdown();
          
          invocations = check;
          sleep_for(2ms);
          CHECK (invocations == check);
        }
      
      
      
      /** @test can cause a worker to terminate by return-value from the work-functor
       */
      void
      verify_workerHalt()
        {
          atomic<uint> check{0};
          atomic<activity::Proc> control{activity::PASS};
          WorkForce wof{setup ([&]{ ++check; return activity::Proc(control); })};
          
          wof.incScale();
          sleep_for(1ms);
          
          uint invocations = check;
          CHECK (0 < invocations);
          
          control = activity::HALT;
          sleep_for(1ms);
          
          invocations = check;
          sleep_for(10ms);
          CHECK (invocations == check);
        }
      
      
      
      /** @test a worker can be sent to sleep, reducing the poll frequency.
       */
      void
      verify_workerSleep()
        {
          atomic<uint> check{0};
          WorkForce wof{setup ([&]{ ++check; return activity::WAIT; })
                          .withSleepPeriod (10ms)};
          
          wof.incScale();
          sleep_for(50us);
          
          CHECK (1 == check);
          
          sleep_for(10us);
          CHECK (1 == check);
          
          sleep_for(12ms);     // after waiting one sleep-period...
          CHECK (2 == check);  // ...functor invoked again
        }
      
      
      
      /** @test when a worker is sent into sleep-cycles for an extended time,
       *        the worker terminates itself
       */
      void
      verify_workerDismiss()
        {
          atomic<uint> check{0};
          WorkForce wof{setup ([&]{ ++check; return activity::WAIT; })
                          .withSleepPeriod (10ms)
                          .dismissAfter(5)};
          
          wof.incScale();
          sleep_for(50us);
          
          CHECK (1 == check);
          
          sleep_for(12ms);
          CHECK (2 == check);      // after one wait cycle, one further invocation
          
          sleep_for(100ms);
          CHECK (5 == check);      // only 5 invocations total...
          CHECK (0 == wof.size()); // ...after that, the worker terminated
        }
      
      
      
      /** @test verify invocation of a thread-termination callback
       */
      void
      verify_finalHook()
        {
          atomic<uint> check{0};
          atomic<activity::Proc> control{activity::PASS};
          WorkForce wof{setup([&]{ return activity::Proc(control); })
                          .withFinalHook([&](bool){ ++check; })};
          
          CHECK (0 == check);
          
          wof.activate();
          sleep_for(10ms);
          CHECK (wof.size() == work::Config::COMPUTATION_CAPACITY);
          CHECK (0 == check);
          
          control = activity::HALT;
          sleep_for(10ms);
          CHECK (0 == wof.size());
          CHECK (check == work::Config::COMPUTATION_CAPACITY);
        }
      
      
      
      /** @test TODO
       * @todo WIP 9/23 ⟶ define ⟶ implement
       */
      void
      verify_detectError()
        {
          atomic<uint> check{0};
          atomic<uint> errors{0};
          WorkForce wof{setup ([&]{
                                    if (++check == 555)
                                      throw error::State("evil");
                                    return activity::PASS;
                                  })
                          .withFinalHook([&](bool isFailure)
                                           {
                                             if (isFailure)
                                               ++errors;
                                           })};
          
          CHECK (0 == check);
          CHECK (0 == errors);
          
          wof.incScale();
          wof.incScale();
          wof.incScale();
          
          sleep_for(10us);
          CHECK (3 == wof.size());
          CHECK (0 < check);
          CHECK (0 == errors);
          
          sleep_for(200ms);   // wait for the programmed disaster
          CHECK (2 == wof.size());
          CHECK (1 == errors);
        }
      
      
      
      /** @test by default, the WorkForce is initially inactive;
       *        once activated, it scales up to the number of cores
       *        reported by the runtime system.
       */
      void
      verify_defaultPool()
        {
          atomic<uint> check{0};
          WorkForce wof{setup ([&]{ ++check; return activity::PASS; })};
          
          // after construction, the WorkForce is inactive
          CHECK (0 == wof.size());
          CHECK (0 == check);
          
          wof.activate();
          sleep_for(20ms);
          
          CHECK (0 < check);
          CHECK (wof.size() == work::Config::COMPUTATION_CAPACITY);
          CHECK (work::Config::COMPUTATION_CAPACITY == std::thread::hardware_concurrency());
        }
      
      
      
      /** @test TODO
       * @todo WIP 9/23 ⟶ define ⟶ implement
       */
      void
      verify_scalePool()
        {
        }
      
      
      
      /** @test dynamically determine count of currently active workers.
       */
      void
      verify_countActive()
        {
          atomic<uint> check{0};
          WorkForce wof{setup ([&]{
                                    ++check;
                                    if (check == 5'000 or check == 5'110)
                                      return activity::HALT;
                                    else
                                      return activity::PASS;
                                  })};
          
          CHECK (0 == wof.size());
          
          wof.incScale();
          wof.incScale();
          wof.incScale();
          sleep_for(10us);         // this may be fragile; must be sufficiently short
          
          CHECK (3 == wof.size());
          
          sleep_for(500ms);        // ...sufficiently long to count way beyond 10'000
          CHECK (check > 6'000);
          CHECK (1 == wof.size());
        }
      
      
      
      /** @test verify that the WorkForce dtor waits for all active threads to disappear
       *        - use a work-functor which keeps all workers blocked
       *        - start the WorkForce within a separate thread
       *        - in this separate thread, cause the WorkForce destructor to be called
       *        - in the outer (controlling thread) release the work-functor blocking
       *        - at this point, all workers return, detect shutdown and terminate
       */
      void
      verify_dtor_blocks()
        {
          atomic<bool> trapped{true};
          auto blockingWork = [&]{
                                   while (trapped)
                                     /* spin */;
                                   return activity::PASS;
                                 };
          
          atomic<bool> pool_scaled_up{false};
          atomic<bool> shutdown_done{false};
          
          std::thread operate{[&]{
                                   {// nested scope...
                                     WorkForce wof{setup (blockingWork)};
                                     
                                     wof.activate();
                                     sleep_for(10ms);
                                     CHECK (wof.size() == work::Config::COMPUTATION_CAPACITY);
                                     pool_scaled_up = true;
                                   } // WorkForce goes out of scope => dtor called
                                   
                                   // when reaching this point, dtor has terminated
                                   shutdown_done = true;
                                   operate.detach();
                                 }};
          
          CHECK (operate.joinable());       // operate-thread is in running state
          sleep_for(100ms);
          
          CHECK (pool_scaled_up);
          CHECK (not shutdown_done);        // all workers are trapped in the work-functor
                                            // thus the destructor can't dismantle the pool
          trapped = false;
          sleep_for(20ms);
          CHECK (shutdown_done);
          CHECK (not operate.joinable());   // operate-thread has detached and terminated
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (WorkForce_test, "unit engine");
  
  
  
}}} // namespace vault::mem::test
