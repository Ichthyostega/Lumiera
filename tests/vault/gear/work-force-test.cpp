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
#include "lib/format-cout.hpp"   ///////////////////////////////WIP
//#include "lib/util.hpp"

//#include <utility>
//#include <chrono>
#include <functional>
#include <thread>

using test::Test;
//using std::move;
//using util::isSameObject;


namespace vault{
namespace gear {
namespace test {
  
  using std::this_thread::sleep_for;
  using namespace std::chrono_literals;
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
          
          Setup (FUN&& workFun)
            : doWork{std::forward<FUN> (workFun)}
            { }
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
          verify_workerDemote();
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
      
      
      
      /** @test TODO
       * @todo WIP 9/23 ⟶ define ⟶ implement
       */
      void
      verify_workerHalt()
        {
        }
      
      
      
      /** @test TODO
       * @todo WIP 9/23 ⟶ define ⟶ implement
       */
      void
      verify_workerSleep()
        {
        }
      
      
      
      /** @test TODO
       * @todo WIP 9/23 ⟶ define ⟶ implement
       */
      void
      verify_workerDemote()
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
      
      
      
      /** @test TODO
       * @todo WIP 9/23 ⟶ define ⟶ implement
       */
      void
      verify_countActive()
        {
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
