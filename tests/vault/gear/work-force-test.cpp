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
    
    template<class FUN>
    auto
    setup (FUN&& workFun)
    {
      struct Setup
        : work::Config
        {
          WorkFun doWork;
          
          milliseconds IDLE_WAIT      = work::Config::IDLE_WAIT;
          size_t       DISMISS_CYCLES = work::Config::DISMISS_CYCLES;
          
          Setup (FUN&& workFun)
            : doWork{std::forward<FUN> (workFun)}
            { }
          
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
      
      
      
      /** @test TODO
       * @todo WIP 9/23 ⟶ define ⟶ implement
       */
      void
      verify_workerSleep()
        {
          atomic<uint> check{0};
          WorkForce wof{setup ([&]{ ++check; return activity::WAIT; })
                          .withSleepPeriod (10ms)};
          
          wof.incScale();
          sleep_for(10us);
          
          CHECK (1 == check);
          
          sleep_for(10us);
          CHECK (1 == check);
          
          sleep_for(10ms);     // after waiting one sleep-period...
          CHECK (2 == check);  // ...functor invoked again
        }
      
      
      
      /** @test TODO
       * @todo WIP 9/23 ⟶ define ⟶ implement
       */
      void
      verify_workerDismiss()
        {
        }
      
      
      
      /** @test TODO
       * @todo WIP 9/23 ⟶ define ⟶ implement
       */
      void
      verify_finalHook()
        {
        }
      
      
      
      /** @test TODO
       * @todo WIP 9/23 ⟶ define ⟶ implement
       */
      void
      verify_detectError()
        {
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
                                    if (check == 10'000 or check == 10'110)
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
          
          sleep_for(200ms);        // ...sufficiently long to count way beyond 10'000
          CHECK (1 == wof.size());
        }
      
      
      
      /** @test TODO
       * @todo WIP 9/23 ⟶ define ⟶ implement
       */
      void
      verify_dtor_blocks()
        {
        }
      
      
      
      /** @test TODO
       * @todo WIP 9/23 ⟶ define ⟶ implement
       */
      void
      walkingDeadline()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (WorkForce_test, "unit engine");
  
  
  
}}} // namespace vault::mem::test
