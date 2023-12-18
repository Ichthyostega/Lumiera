/*
  SchedulerService(Test)  -  component integration test for the scheduler

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

/** @file scheduler-usage-test.cpp
 ** unit test \ref SchedulerService_test
 */


#include "lib/test/run.hpp"
#include "test-chain-load.hpp"
#include "activity-detector.hpp"
#include "vault/gear/scheduler.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-string.hpp"
#include "lib/test/transiently.hpp"
#include "lib/test/microbenchmark.hpp"
#include "lib/util.hpp"

#include <thread>

using test::Test;


namespace vault{
namespace gear {
namespace test {
  
  using util::max;
  using util::_Fmt;
  using lib::time::Time;
  using std::this_thread::sleep_for;
  
  namespace {                            ////////////////////////////////////////////////////////////////////TICKET #1055 want to construct lumiera Time from std::chrono literals
    Time t100us = Time{FSecs{1, 10'000}};
    Time t200us = t100us + t100us;
    Time t500us = t200us + t200us + t100us;
    Time t1ms   = Time{1,0};
    
    const uint TYPICAL_TIME_FOR_ONE_SCHEDULE_us = 3;
  }
  
  
  
  
  /*************************************************************************//**
   * @test Scheduler component integration test: use the service API for
   *       state control and to add Jobs and watch processing patterns.
   * @see SchedulerActivity_test
   * @see SchedulerInvocation_test
   * @see SchedulerCommutator_test
   * @see SchedulerLoadControl_test
   */
  class SchedulerService_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           simpleUsage();
           verify_StartStop();
           verify_LoadFactor();
           invokeWorkFunction();
           scheduleRenderJob();
           processSchedule();
        }
      
      
      /** @test demonstrate a simple usage scenario
       */
      void
      simpleUsage()
        {
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          CHECK (scheduler.empty());
          
          auto task = onetimeCrunch(4ms);
          CHECK (1 == task.remainingInvocations());
          
          Job job{ task
                 , InvocationInstanceID()
                 , Time::ANYTIME
                 };
          scheduler.defineSchedule(job)
                   .startOffset(6ms)
                   .lifeWindow(2ms)
                   .post();
          CHECK (not scheduler.empty());
          
          sleep_for (3ms); // not invoked yet
          CHECK (1 == task.remainingInvocations());
          
          sleep_for (20ms);
          CHECK (0 == task.remainingInvocations());
        }             // task has been invoked
      
      
      
      
      /**
       * @internal helper to inject a new task into the Scheduler,
       *           without also activating WorkForce and load control.
       * @remark this class is declared friend by the Scheduler to grant
       *           this kind of »implementation backdoor« access; the function
       *           defined there does essentially the same than Scheduler::postChain()
       */
      static void
      postNewTask (Scheduler& scheduler, Activity& chain, Time start)
        {
          ActivationEvent actEvent{chain, start, start + Time{50,0}};  // add dummy deadline +50ms
          Scheduler::ExecutionCtx ctx{scheduler, actEvent};
          scheduler.layer2_.postDispatch (actEvent, ctx, scheduler.layer1_);
        }
      
      
      
      /** @test get the scheduler into running state
       */
      void
      verify_StartStop()
        {
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          CHECK (scheduler.empty());

          Activity dummy{Activity::FEED};
          auto postIt = [&] { postNewTask (scheduler, dummy, RealClock::now()+t200us); };
          
          scheduler.ignite();
          CHECK (not scheduler.empty());// repeated »tick« task enlisted....
          
          postIt();
          CHECK (not scheduler.empty());
          
          scheduler.terminateProcessing();
          CHECK (scheduler.empty());
          
          postIt();
          postIt();
          scheduler.ignite();
          CHECK (not scheduler.empty());
          //... and just walk away => scheduler unwinds cleanly from destructor
        }//     Note: BlockFlow and WorkForce unwinding is covered in dedicated tests
      
      
      
      /** @test verify the scheduler processes scheduled events,
       *        indicates current load and winds down automatically
       *        when falling empty.
       *      - schedule short bursts of single FEED-Activities
       *      - these actually do nothing and can be processed typically < 5µs
       *      - placing them spaced by 1µs, so the scheduler will build up congestion
       *      - since this Activity does not drop the »grooming-token«, actually only
       *        a single worker will process all Activities in a single peak
       *      - after the peak is done, the load indicator will drop again
       *      - when reaching the scheduler »tick«, the queue should be empty
       *        and the scheduler will stop active processing
       *      - the main thread (this test) polls every 50µs to observe the load
       *      - after 2 seconds of idle-sleeping, the WorkForce is disengaged
       *      - verify the expected load pattern
       */
      void
      verify_LoadFactor()
        {
          MARK_TEST_FUN
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          CHECK (scheduler.empty());
          
          // use a single FEED as content
          Activity dummy{Activity::FEED};
          
          auto anchor = RealClock::now();
          auto offset = [&](Time when =RealClock::now()){ return _raw(when) - _raw(anchor); };
          
          auto createLoad = [&](Offset start, uint cnt)
                            { // use internal API (this test is declared as friend)
                              for (uint i=0; i<cnt; ++i) // flood the queue
                                postNewTask (scheduler, dummy, anchor + start + TimeValue{i});
                            };
          
          
          auto LOAD_PEAK_DURATION_us = 2000;
          auto fatPackage = LOAD_PEAK_DURATION_us/TYPICAL_TIME_FOR_ONE_SCHEDULE_us;
          
          createLoad (Offset{Time{ 5,0}}, fatPackage);
          createLoad (Offset{Time{15,0}}, fatPackage);
          
          scheduler.ignite();
          cout << "Timing : start-up required.."<<offset()<<" µs"<<endl;
          
          // now watch change of load and look out for two peaks....
          uint   peak1_s  =0;
          uint   peak1_dur=0;
          double peak1_max=0;
          uint   peak2_s  =0;
          uint   peak2_dur=0;
          double peak2_max=0;
          
          uint phase=0;
          _Fmt row{"%6d | Load: %5.3f  Head:%5d Lag:%6d\n"};
          
          while (not scheduler.empty()) // should fall empty at end
            {
              sleep_for(50us);
              double load = scheduler.getLoadIndicator();
              
              switch (phase) {
                case 0:
                  if (load > 1.0)
                    {
                      ++phase;
                      peak1_s = offset();
                    }
                  break;
                case 1:
                  peak1_max = max (load, peak1_max);
                  if (load < 1.0)
                    {
                      ++phase;
                      peak1_dur = offset() - peak1_s;
                    }
                  break;
                case 2:
                  if (load > 1.0)
                    {
                      ++phase;
                      peak2_s = offset();
                    }
                  break;
                case 3:
                  peak2_max = max (load, peak2_max);
                  if (load < 1.0)
                    {
                      ++phase;
                      peak2_dur = offset() - peak2_s;
                    }
                  break;
                }
                cout << row % offset() % load
                                       % offset(scheduler.layer1_.headTime())
                                       % scheduler.loadControl_.averageLag();
            }
          uint done = offset();
          
          //--------Summary-Table------------------------------
          _Fmt peak{"\nPeak %d ....... %5d +%dµs %34tmax=%3.1f"};
          cout << "-------+-------------+----------+----------"
               << "\n\n"
               << peak % 1 % peak1_s % peak1_dur % peak1_max
               << peak % 2 % peak2_s % peak2_dur % peak2_max
               << "\nTick   ....... "<<done
               <<endl;
          
          CHECK (phase == 4);
          CHECK (peak1_s > 5000);   // first peak was scheduled at 5ms
          CHECK (peak1_s < 10000);
          CHECK (peak2_s > 15000);  // second peak was scheduled at 15ms
          CHECK (peak2_s < 20000);
          CHECK (peak1_max > 2.0);
          CHECK (peak2_max > 2.0);
          
          CHECK (done > 50000);     // »Tick« period is 50ms
                                    // and this tick should determine end of timeline
          
          cout << "\nwaiting for shutdown of WorkForce";
          while (scheduler.workForce_.size() > 0)
            {
              sleep_for(10ms);
              cout << "." << std::flush;
            }
          uint shutdown = offset();
          cout << "\nShutdown after "<<shutdown / 1.0e6<<"sec"<<endl;
          CHECK (shutdown > 2.0e6);
        }
      
      
      
      /** @test verify visible behaviour of the [work-pulling function](\ref Scheduler::getWork)
       *      - use a rigged Activity probe to capture the schedule time on invocation
       *      - additionally perform a timing measurement for invoking the work-function
       *      - invoking the Activity probe itself costs 50...150µs, Scheduler internals < 50µs
       *      - this implies we can show timing-delay effects in the millisecond range
       *      - demonstrated behaviour
       *        + an Activity already due will be dispatched immediately by post()
       *        + an Activity due at the point when invoking the work-function is dispatched
       *        + while queue is empty, the work-function returns immediately, indicating sleep
       *        + invoking the work-function when there is still some time span up to the next
       *          planned Activity will enter a targeted sleep, returning shortly after the
       *          next schedule. Entering then again will cause dispatch of that activity.
       *        + if the work-function dispatches an Activity while the next entry is planned
       *          for some time ahead, the work-function will likewise go into a targeted
       *          sleep and only return at or shortly after that next planned time entry
       *        + after dispatching an Activity in a situation with no follow-up work,
       *          the work-function inserts a targeted sleep of random duration,
       *          to re-shuffle the rhythm of sleep cycles
       *        + when the next planned Activity was already »tended for« (by placing
       *          another worker into a targeted sleep), further workers entering the
       *          work-function will be re-targeted by a random sleep to focus capacity
       *          into a time zone behind the next entry.
       * @note Invoking the Activity probe itself can take 50..150µs, due to the EventLog,
       *       which is not meant to be used in performance critical paths but only for tests,
       *       because it performs lots of heap allocations and string operations. Moreover,
       *       we see additional cache effects after an extended sleep period.
       */
      void
      invokeWorkFunction()
        {
          MARK_TEST_FUN
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          
          ActivityDetector detector;
          Activity& probe = detector.buildActivationProbe ("testProbe");
          
          TimeVar start;
          int64_t delay_us;
          int64_t slip_us;
          activity::Proc res;

          auto post = [&](Time start)
                              { // this test class is declared friend to get a backdoor into Scheduler internals...
                                scheduler.layer2_.acquireGoomingToken();
                                postNewTask (scheduler, probe, start);
                              };
          
          auto pullWork = [&] {
                                delay_us = lib::test::benchmarkTime([&]{ res = scheduler.getWork(); });
                                slip_us = _raw(detector.invokeTime(probe)) - _raw(start);
                                cout << "res:"<<res<<" delay="<<delay_us<<"µs slip="<<slip_us<<"µs"<<endl;
                              };
          

          auto wasClose = [](TimeValue a, TimeValue b)
                              {                                // 500µs are considered "close"
                                return Duration{Offset{a,b}} < Duration{FSecs{1,2000}};
                              };
          auto wasInvoked = [&](Time start)
                              {
                                Time invoked = detector.invokeTime (probe);
                                return invoked >= start
                                   and wasClose (invoked, start);
                              };
          
          
          cout << "Scheduled right away..."<<endl;
          start = RealClock::now();
          post(start);                                                      // Post the testProbe to be scheduled "now"
          CHECK (wasInvoked(start));                                        // Result: invoked directly, not enqueued at all
          CHECK (scheduler.empty());
          
          
          cout << "pullWork() on empty queue..."<<endl;
          pullWork();                                                       // Call the work-Function on empty Scheduler queue
          CHECK (activity::WAIT == res);                                    // the result instructs this thread to go to sleep immediately
          CHECK (delay_us < 40);
          
          
          cout << "Due at pullWork()..."<<endl;
          TimeVar now = RealClock::now();
          start = now + t100us;                                             // Set a schedule 100ms ahead of "now"
          post (start);
          CHECK (not scheduler.empty());                                    // was enqueued
          CHECK (not wasInvoked(start));                                    // ...but not activated yet
          
          sleep_for (100us);                                                // wait beyond the planned start point (typically waits ~150µs or more)
          pullWork();
          CHECK (wasInvoked(start));
          CHECK (slip_us  < 300);                                           // Note: typically there is a slip of 100..200µs, because sleep waits longer
          CHECK (scheduler.empty());                                        // The scheduler is empty now and this thread will go to sleep,
          CHECK (delay_us < 20200);                                         // however the sleep-cycle is first re-shuffled by a wait between 0 ... 20ms
          CHECK (activity::PASS == res);                                    // this thread is instructed to check back once
          pullWork();
          CHECK (activity::WAIT == res);                                    // ...yet since the queue is still empty, it is sent immediately to sleep
          CHECK (delay_us < 40);
          
          
          cout << "next some time ahead => up-front delay"<<endl;
          now = RealClock::now();
          start = now + t500us;                                             // Set a schedule significantly into the future...
          post (start);
          CHECK (not scheduler.empty());
          
          pullWork();                                                       // ...and invoke the work-Function immediately "now"
          CHECK (activity::PASS == res);                                    // Result: this thread was kept in sleep in the work-Function
          CHECK (not wasInvoked(start));                                    // but the next dispatch did not happen yet; we are instructed to re-invoke immediately
          CHECK (delay_us > 500);                                           // this proves that there was a delay to wait for the next schedule
          CHECK (delay_us < 1000);
          pullWork();                                                       // if we now re-invoke the work-Function as instructed...
          CHECK (wasInvoked(start));                                        // then the next schedule is already slightly overdue and immediately invoked
          CHECK (scheduler.empty());                                        // the queue is empty and thus this thread will be sent to sleep
          CHECK (delay_us < 20200);                                         // but beforehand the sleep-cycle is re-shuffled by a wait between 0 ... 20ms
          CHECK (slip_us  < 300);
          CHECK (activity::PASS == res);                                    // instruction to check back once
          pullWork();
          CHECK (activity::WAIT == res);                                    // but next call will send this thread to sleep right away
          CHECK (delay_us < 40);
          
          
          cout << "follow-up with some distance => follow-up delay"<<endl;
          now = RealClock::now();
          start = now + t100us;
          post (start);                                                     // This time the schedule is set to be "soon"
          post (start+t1ms);                                                // But another schedule is placed 1ms behind
          sleep_for (100us);                                                // wait for "soon" to pass...
          pullWork();
          CHECK (wasInvoked(start));                                        // Result: the first invocation happened immediately
          CHECK (slip_us  < 300);
          CHECK (delay_us > 900);                                           // yet this thread was afterwards kept in sleep to await the next task;
          CHECK (activity::PASS == res);                                    // returns instruction to re-invoke immediately
          CHECK (not scheduler.empty());                                    // since there is still work in the queue
          
          start += t1ms;                                                    // (just re-adjust the reference point to calculate slip_us)
          pullWork();                                                       // re-invoke immediately as instructed
          CHECK (wasInvoked(start));                                        // Result: also the next Activity has been dispatched
          CHECK (slip_us < 400);                                            // not much slip
          CHECK (delay_us < 20200);                                         // ...and the post-delay is used to re-shuffle the sleep cycle as usual
          CHECK (activity::PASS == res);                                    // since queue is empty, we will call back once...
          CHECK (scheduler.empty());
          pullWork();
          CHECK (activity::WAIT == res);                                    // and then go to sleep.
          
          
          cout << "already tended-next => re-target capacity"<<endl;
          now = RealClock::now();
          start = now + t500us;                                             // Set the next schedule with some distance...
          post (start);
          
          // Access scheduler internals (as friend)
          CHECK (start == scheduler.layer1_.headTime());                    // next schedule indeed appears as next-head
          CHECK (not scheduler.loadControl_.tendedNext(start));             // but this next time was not yet marked as "tended"
          
          scheduler.loadControl_.tendNext(start);                           // manipulate scheduler to mark next-head as "tended"
          CHECK (    scheduler.loadControl_.tendedNext(start));
          
          CHECK (start == scheduler.layer1_.headTime());                    // other state still the same
          CHECK (not scheduler.empty());
          
          pullWork();
          CHECK (not wasInvoked(start));                                    // since next-head was marked as "tended"...
          CHECK (not scheduler.empty());                                    // ...this thread is not used to dispatch it
          CHECK (delay_us < 6000);                                          // rather it is re-focussed as free capacity within WORK_HORIZON
        }
      
      
      
      
      /** @test Schedule a render job through the high-level Job-builder API.
       *      - use the mock Job-Functor provided by the ActivityDetector
       *      - manipulate the WorkForce to prevent it from scaling up
       *      - this allows us to investigate the queue entry created
       *        through the public regular API for scheduling Render Jobs
       *      - after that, the test manually invokes the work-pulling function
       *        and verifies the mock Job-Functor has been invoked
       *      - note however that this time a complete Activity chain
       *        was created, including a Gate and all state transitions,
       *        since we used the high-level API of the SchedulerService
       */
      void
      scheduleRenderJob()
        {
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          
          // prevent scale-up of the Scheuler's WorkForce
          TRANSIENTLY(work::Config::COMPUTATION_CAPACITY) = 0;

          Time nominal{7,7};
          Time start{0,1};
          Time dead{0,10};
          
          ActivityDetector detector;
          Job testJob{detector.buildMockJob("testJob", nominal, 1337)};
          
          CHECK (scheduler.empty());
          
          // use the public Render-Job builder API
          scheduler.defineSchedule(testJob)
                   .startOffset(400us)
                   .lifeWindow (2ms)
                   .post();
          
          CHECK (not scheduler.empty());
          
          // cause the new entry to migrate to the priority queue...
          scheduler.layer2_.maybeFeed(scheduler.layer1_);
          
          // investigate the generated ActivationEvent at queue head
          auto entry = scheduler.layer1_.peekHead();
          auto now = RealClock::now();
          
          CHECK (entry.activity->is(Activity::POST));
          CHECK (entry.activity->next->is(Activity::GATE));
          CHECK (entry.activity->next->next->is(Activity::WORKSTART));
          CHECK (entry.activity->next->next->next->is(Activity::INVOKE));
          CHECK (entry.startTime() - now < _uTicks( 400us));
          CHECK (entry.deathTime() - now < _uTicks(2400us));
          CHECK (entry.manifestation == 0);
          CHECK (entry.isCompulsory  == false);
          
          
          sleep_for(400us);          // wait to be sure the new entry has reached maturity
          detector.incrementSeq();   // mark this point in the detector-log...
          
          // Explicitly invoke the work-Function (normally done by the workers)
          CHECK (activity::PASS == scheduler.getWork());
          
          CHECK (detector.verifySeqIncrement(1)
                         .beforeInvocation("testJob").arg("7.007", 1337));
          
//        cout << detector.showLog()<<endl; // HINT: use this for investigation...
        }
      
      
      
      
      
      /** @test schedule and process a complete work load
       *      - use a complex computation structure generated by TestChainLoad
       *      - dispatch 64 jobs, each depending more or less on its predecessors
       *      - processing proceeds first in two parallel chains, then joins
       *        and forks again into a massive overload towards the end.
       *      - each _level of jobs_ is scheduled 1ms apart
       *      - some jobs use a _computation weight_ ranging form 500µs to 1.5ms
       *      - at the end, both a single threaded computation and the computation
       *        processed by the scheduler must yield the same result hash, which
       *        is computed for each job by combining its predecessor hashes.
       */
      void
      processSchedule()
        {
          MARK_TEST_FUN
          TestChainLoad<16> testLoad{64};
          testLoad.configureShape_chain_loadBursts()
                  .buildToplolgy();
          
          auto stats = testLoad.computeGraphStatistics();
          cout << _Fmt{"Test-Load: Nodes: %d  Levels: %d  ∅Node/Level: %3.1f  Forks: %d  Joins: %d"}
                      % stats.nodes
                      % stats.levels
                      % stats.indicators[STAT_NODE].pL
                      % stats.indicators[STAT_FORK].cnt
                      % stats.indicators[STAT_JOIN].cnt
               << endl;
          
          // while building the calculation-plan graph
          // node hashes were computed, observing dependencies
          size_t expectedHash = testLoad.getHash();
          
          // some jobs/nodes are marked with a weight-step
          // these can be instructed to spend some CPU time
          auto LOAD_BASE = 500us;
          testLoad.performGraphSynchronously(LOAD_BASE);
          CHECK (testLoad.getHash() == expectedHash);
          
          double referenceTime = testLoad.calcRuntimeReference(LOAD_BASE);
          cout << "refTime(singleThr): "<<referenceTime/1000<<"ms"<<endl;
          
          
          // Perform through Scheduler----------
          BlockFlowAlloc bFlow;
          EngineObserver watch;
          Scheduler scheduler{bFlow, watch};
          
          double performanceTime =
            testLoad.setupSchedule(scheduler)
                    .withLoadTimeBase(LOAD_BASE)
                    .withJobDeadline(30ms)
                    .launch_and_wait();
          
          cout << "runTime(Scheduler): "<<performanceTime/1000<<"ms"<<endl;

          // invocation through Scheduler has reproduced all node hashes
          CHECK (testLoad.getHash() == expectedHash);
          
          // due to the massive load burst at end, Scheduler falls behind plan
          CHECK (performanceTime < 2*referenceTime);
          // typical values: refTime ≡ ~35ms, runTime ≡ ~45ms
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerService_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
