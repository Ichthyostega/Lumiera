/*
  WorkForce(Test)  -  verify worker thread service

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file work-force-test.cpp
 ** unit test \ref WorkForce_test
 */


#include "lib/test/run.hpp"
#include "vault/gear/work-force.hpp"
#include "lib/thread.hpp"
#include "lib/sync.hpp"

#include <functional>
#include <thread>
#include <chrono>
#include <set>

using test::Test;


namespace vault{
namespace gear {
namespace test {
  
  using std::this_thread::sleep_for;
  using namespace std::chrono_literals;
  using std::chrono::milliseconds;
  using lib::Thread;
  
  
  namespace {
    using WorkFun = std::function<work::SIG_WorkFun>;
    using FinalFun = std::function<work::SIG_FinalHook>;
    
    /**
     * Helper: setup a Worker-Pool configuration for the test.
     * Derived from the default configuration, it allows to bind
     * a lambda as work-functor and to tweak other parameters.
     */
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
   * @warning this test relies on empirical timings and can be brittle.
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
          verify_workerRetard();
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
                           //  ^^^ this is the doWork-λ
          CHECK (0 == check);
          
          wof.activate();
          sleep_for(20ms);
          
          CHECK (0 < check); // λ invoked in the worker threads
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
      
      
      
      /** @test a worker can be sent to sleep, throttling the poll frequency.
       */
      void
      verify_workerSleep()
        {
          atomic<uint> check{0};
          WorkForce wof{setup ([&]{ ++check; return activity::WAIT; })
                          .withSleepPeriod (10ms)};
          
          wof.incScale();
          sleep_for(1ms);
          
          CHECK (1 == check);
          
          sleep_for(10us);
          CHECK (1 == check);
          
          sleep_for(12ms);     // after waiting one sleep-period...
          CHECK (2 == check);  // ...functor invoked again
        }
      
      
      
      /** @test a worker can be retarded and throttled in case of contention.
       */
      void
      verify_workerRetard()
        {
          atomic<uint> check{0};
          {                                                  // ▽▽▽▽ regular work-cycles without delay
            WorkForce wof{setup ([&]{ ++check; return activity::PASS; })};
            wof.incScale();
            sleep_for(5ms);
          }
          uint cyclesPASS{check};
          check = 0;
          {                                                  // ▽▽▽▽ signals »contention«
            WorkForce wof{setup ([&]{ ++check; return activity::KICK; })};
            wof.incScale();
            sleep_for(5ms);
          }
          uint cyclesKICK{check};
          CHECK (cyclesKICK < cyclesPASS);
          CHECK (cyclesKICK < 50);
        }
      
      
      
      /** @test when a worker is sent into sleep-cycles for an extended time,
       *        the worker terminates itself.
       */
      void
      verify_workerDismiss()
        {
          atomic<uint> check{0};
          WorkForce wof{setup ([&]{ ++check; return activity::WAIT; })
                          .withSleepPeriod (10ms)
                          .dismissAfter(5)};
          
          wof.incScale();
          sleep_for(1ms);
          
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
          atomic<uint> exited{0};
          atomic<activity::Proc> control{activity::PASS};
          WorkForce wof{setup([&]{ return activity::Proc(control); })
                          .withFinalHook([&](bool){ ++exited; })};
          
          CHECK (0 == exited);
          
          wof.activate();
          sleep_for(10ms);
          CHECK (wof.size() == work::Config::COMPUTATION_CAPACITY);
          CHECK (0 == exited);
          
          control = activity::HALT;
          sleep_for(10ms);
          CHECK (0 == wof.size());
          CHECK (exited == work::Config::COMPUTATION_CAPACITY);
        }
      
      
      
      /** @test exceptions emanating from within the worker are catched
       *        and reported by setting the isFailure argument flag of
       *        the `finalHook` functor invoked at worker termination.
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
          CHECK (0  < check);
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
      
      
      
      /** @test the number of (separate) workers can be scaled up,
       *        both stepwise and as fraction of full hardware concurrency
       */
      void
      verify_scalePool()
        {
          /** helper to count distinct thread-IDs */
          class UniqueCnt
            : public std::set<std::thread::id>
            , public lib::Sync<>
            {
            public:
              void
              mark (std::thread::id const& tID)
                {
                  Lock guard{this};
                  this->insert(tID);
                }
              
              operator size_t()  const
                {
                  Lock guard{this};
                  return this->size();
                }
            }
            uniqueCnt;
          
          WorkForce wof{setup ([&]{
                                    uniqueCnt.mark(std::this_thread::get_id());
                                    return activity::PASS;
                                  })};
          
          CHECK (0 == uniqueCnt);
          CHECK (0 == wof.size());
          
          wof.incScale();
          sleep_for(1ms);
          CHECK (1 == uniqueCnt);
          CHECK (1 == wof.size());
          
          wof.incScale();
          sleep_for(1ms);
          CHECK (2 == uniqueCnt);
          CHECK (2 == wof.size());

          
          auto fullCnt = work::Config::COMPUTATION_CAPACITY;
          
          wof.activate (1.0);
          sleep_for(5ms);
          CHECK (fullCnt == uniqueCnt);
          CHECK (fullCnt == wof.size());
          
          wof.activate (2.0);
          sleep_for(10ms);
          CHECK (2*fullCnt == uniqueCnt);
          CHECK (2*fullCnt == wof.size());
          
          wof.awaitShutdown();
          CHECK (0 == wof.size());
          
          uniqueCnt.clear();
          sleep_for(5ms);
          CHECK (0 == uniqueCnt);
          
          wof.activate (0.5);
          sleep_for(5ms);
          CHECK (fullCnt/2 == uniqueCnt);
          CHECK (fullCnt/2 == wof.size());
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
          
          while (check < 6'000)
            sleep_for(15ms);  // .....sufficiently long to count way beyond 10'000
          CHECK (check > 6'000);
          CHECK (1 == wof.size());
        }
      
      
      
      /** @test verify that the WorkForce dtor waits for all active threads to disappear
       *        - use a work-functor which keeps all workers blocked
       *        - start the WorkForce within a separate thread
       *        - in this separate thread, cause the WorkForce destructor to be called
       *        - in the test main thread release the work-functor blocking
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
          
          Thread operate{"controller"
                        ,[&] {
                               {// nested scope...
                                 WorkForce wof{setup (blockingWork)};
                                 
                                 wof.activate();
                                 sleep_for (10ms);
                                 CHECK (wof.size() == work::Config::COMPUTATION_CAPACITY);
                                 pool_scaled_up = true;
                               } // WorkForce goes out of scope => dtor called
                               
                               // when reaching this point, dtor has terminated
                               shutdown_done = true;
                             }};
          
          CHECK (operate);                  // operate-thread is in running state
          sleep_for(100ms);
          
          CHECK (pool_scaled_up);
          CHECK (not shutdown_done);        // all workers are trapped in the work-functor
                                            // thus the destructor can't dismantle the pool
          trapped = false;
          sleep_for(20ms);
          CHECK (shutdown_done);
          CHECK (not operate);              // operate-thread has detached and terminated
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (WorkForce_test, "unit engine");
  
  
}}} // namespace vault::gear::test
