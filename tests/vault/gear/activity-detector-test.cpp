/*
  ActivityDetector(Test)  -  verify diagnostic setup to watch scheduler activities

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

/** @file activity-detector-test.cpp
 ** unit test \ref ActivityDetector_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "activity-detector.hpp"
#include "vault/real-clock.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"


using lib::time::Time;
using lib::time::FSecs;

using util::isSameObject;
using lib::test::randStr;
using lib::test::randTime;


namespace vault{
namespace gear {
namespace test {
  
  
  /*****************************************************************//**
   * @test verify instrumentation setup to watch scheduler Activities.
   * @see SchedulerActivity_test
   * @see SchedulerUsage_test
   */
  class ActivityDetector_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          
          verifyMockInvocation();
          verifyFakeInvocation();
          verifyMockJobFunctor();
          verifyFakeExeContext();
          watch_activation();
          watch_ActivationTap();
          insert_ActivationTap();
          watch_notification();
          watch_gate();
        }
      
      
      /** @test demonstrate a simple usage scenario of this test support facility
       */
      void
      simpleUsage()
        {
          ActivityDetector detector("spectre");
          
          auto trap = detector.buildDiagnosticFun<int(double,Time)>("trap")
                              .returning(55);
          
          CHECK (55 == trap (1.23, Time{FSecs{3,2}}));
          
          CHECK (detector == "Rec(EventLogHeader| this = ActivityDetector(spectre) ), "
                             "Rec(call| fun = trap, this = ActivityDetector(spectre), Seq = 0 |{1.23, 0:00:01.500})"_expect);
        }
      
      
      
      /** @test verify the setup and detection of instrumented invocations
       *        - a _sequence number_ is embedded into the ActivityDetector
       *        - this sequence number is recorded into an attribute at each invocation
       *        - a DSL for verification is provided (based on the EventLog)
       *        - arguments and sequence numbers can be explicitly checked
       */
      void
      verifyMockInvocation()
        {
          ActivityDetector detector;
          auto fun = detector.buildDiagnosticFun<void(uint)> ("funny");
          uint rnd = rand() % 10000;
          
          detector.incrementSeq();
          CHECK (1 == detector.currSeq());
          CHECK (detector.ensureNoInvocation ("funny"));
          
          detector.incrementSeq();
          CHECK (2 == detector.currSeq());
          CHECK (detector.verifySeqIncrement(2));
          
          fun (rnd);
          CHECK (detector.verifyInvocation ("funny"));
          CHECK (detector.verifyInvocation ("funny").arg(rnd));
          CHECK (detector.verifyInvocation ("funny").seq(2));
          CHECK (detector.verifyInvocation ("funny").arg(rnd).seq(2));
          CHECK (detector.verifyInvocation ("funny").seq(2).arg(rnd));
          CHECK (detector.ensureNoInvocation ("bunny"));                    // wrong name
          CHECK (detector.ensureNoInvocation ("funny").arg());              // fails since empty argument list expected
          CHECK (detector.ensureNoInvocation ("funny").arg(rnd+5));         // expecting wrong argument
          CHECK (detector.ensureNoInvocation ("funny").seq(5));             // expecting wrong sequence number
          CHECK (detector.ensureNoInvocation ("funny").arg(rnd).seq(1));    // expecting correct argument, but wrong sequence
          
          detector.incrementSeq();
          fun (rnd+1);
          CHECK (detector.verifyInvocation ("funny").seq(2)
                         .beforeSeqIncrement(3)
                         .beforeInvocation ("funny").seq(3).arg(rnd+1));
          
          CHECK (detector == "Rec(EventLogHeader| this = ActivityDetector )"
                           ", Rec(event| ID = IncSeq |{1})"
                           ", Rec(event| ID = IncSeq |{2})"
                           ", Rec(call| fun = funny, this = ActivityDetector, Seq = 2 |{"+util::toString(rnd)+"})"
                           ", Rec(event| ID = IncSeq |{3})"
                           ", Rec(call| fun = funny, this = ActivityDetector, Seq = 3 |{"+util::toString(rnd+1)+"})"_expect);
        }
      
      
      
      /** @test verify a variation of the instrumented functor
       *        to call into a custom provided _fake implementation._
       */
      void
      verifyFakeInvocation()
        {
          ActivityDetector detector;
          auto fun = detector.buildDiagnosticFun<int(uint)> ("fakeFun");
          uint rnd = rand() % 10000;
          
          CHECK (0 == fun (rnd));
          
          fun.returning(42);
          detector.incrementSeq();
          CHECK (42 == fun (rnd));

          fun.implementedAs ([](uint i){ return -i; });
          detector.incrementSeq();
          CHECK (-int(rnd) == fun (rnd));
          
          CHECK (detector.verifyInvocation("fakeFun").seq(0)
                         .beforeInvocation("fakeFun").seq(1)
                         .beforeInvocation("fakeFun").seq(2));
        }
      
      
      
      /** @test diagnostic setup to detect a JobFunctor activation
       *        - the ActivityDetector provides specifically rigged JobFunctor instances
       *        - these capture all invocations, based on generic invocation logging
       *        - special match qualifier to verify the job's nominal invocation time parameter
       *        - event verification can be combined with other verifications to cover
       *          complex invocation sequences
       */
      void
      verifyMockJobFunctor()
        {
          ActivityDetector detector;
          InvocationInstanceID invoKey;
          Time nominal{FSecs{5,2}};
          invoKey.part.a = 55;
          
          Job dummyJob{detector.buildMockJobFunctor ("mockJob")
                      ,invoKey
                      ,nominal};
          
          CHECK (detector.ensureNoInvocation ("mockJob"));
          dummyJob.triggerJob();
          CHECK (detector.verifyInvocation ("mockJob"));
          CHECK (detector.verifyInvocation ("mockJob").arg(nominal, invoKey.part.a));
          CHECK (detector.verifyInvocation ("mockJob").timeArg(nominal));
          
          detector.incrementSeq();                                                              // note: sequence number incremented between invocations
          dummyJob.parameter.nominalTime += 5 * Time::SCALE;                                    // different job parameter (later nominal time point)
          dummyJob.triggerJob();
          
          CHECK (detector.verifyInvocation ("mockJob").timeArg(nominal).seq(0)
                         .beforeInvocation ("mockJob").timeArg(nominal + Time{FSecs{5}})        // matching first invocation and then second...
                         .afterSeqIncrement(1)                                                  // note: searching backwards from the 2nd invocation
                         );
//        cout << detector.showLog()<<endl; // HINT: use this for investigation...
        }
      
      
      
      /** @test faked execution context to perform Activity activation
       *        - wired internally to report each invocation into the EventLog
       *        - by default response of `post` and `tick` is `PASS`, but can be reconfigured
       *        - invocation sequence can be verified by the usual scheme
       */
      void
      verifyFakeExeContext()
        {
          ActivityDetector detector;
          auto& ctx = detector.executionCtx;
          // an otherwise opaque object fulfilling the "Concept"
          activity::_verify_usable_as_ExecutionContext<decltype(detector.executionCtx)>();
          
          Time t = randTime();
          size_t x = rand();
          Activity a;
          
          CHECK (detector.ensureNoInvocation(CTX_WORK));
          CHECK (detector.ensureNoInvocation(CTX_POST));
          CHECK (detector.ensureNoInvocation(CTX_DONE));
          CHECK (detector.ensureNoInvocation(CTX_TICK));
          
          ctx.work (t,x);
          CHECK (detector.verifyInvocation(CTX_WORK).arg(t,x));
          
          ctx.done (t,x);
          CHECK (detector.verifyInvocation(CTX_DONE).arg(t,x));
          
          CHECK (activity::PASS == ctx.post (t, a, ctx));
          CHECK (detector.verifyInvocation(CTX_POST).arg(t,a,ctx));
          
          CHECK (activity::PASS == ctx.tick(t));
          CHECK (detector.verifyInvocation(CTX_TICK).arg(t));
          
          detector.incrementSeq();
          ctx.tick.returning(activity::KILL);
          CHECK (activity::KILL == ctx.tick(t));
          CHECK (detector.verifyInvocation(CTX_TICK).timeArg(t));
          
          CHECK (detector.verifyInvocation(CTX_WORK).timeArg(t)
                         .beforeInvocation(CTX_DONE).timeArg(t)
                         .beforeInvocation(CTX_POST).timeArg(t)
                         .beforeInvocation(CTX_TICK).timeArg(t).seq(0)
                         .beforeInvocation(CTX_TICK).timeArg(t).seq(1));
        }
      
      
      
      /** @test diagnostic setup to detect Activity activation
       * @todo WIP 8/23 ✔ define ✔ implement
       */
      void
      watch_activation()
        {
          ActivityDetector detector;
          auto someID = "trap-" + randStr(4);
          Activity& probe = detector.buildActivationProbe (someID);
          CHECK (probe.is (Activity::HOOK));
          
          Time realTime = RealClock::now();
          probe.activate (realTime, detector.executionCtx);
          
          CHECK (detector.verifyInvocation(someID).timeArg(realTime));
        }
      
      
      
      /** @test diagnostic adaptor to detect and pass-through Activity activation
       * @todo WIP 8/23 ✔ define ✔ implement
       */
      void
      watch_ActivationTap()
        {
          ActivityDetector detector;
          
          Time nomTime{99,11};
          Activity feed{size_t{12},size_t{34}};
          Activity feed2{size_t{56},size_t{78}};
          feed.next = &feed2;
          string jobID = "job-" + randStr(4);
          Activity invoke{detector.buildMockJobFunctor(jobID), nomTime, feed};
          
          Time t1{0,1,1};
          CHECK (activity::PASS == invoke.activate (t1, detector.executionCtx));
          CHECK (detector.verifyInvocation (jobID).arg(nomTime, 12));
          
          // decorate the INVOKE-Activity with an ActivationTap
          Activity& tap = detector.buildActivationTap (invoke);
          CHECK (tap.next == invoke.next);
          
          detector.incrementSeq();
          Time t2{0,2,2};
          // now activate through the Tap....
          tap.activate(t2, detector.executionCtx);
          CHECK (detector.verifySeqIncrement(1)       // ==> the ActivationTap "tap-INVOKE" reports and passes activation
                         .beforeInvocation("tap-INVOKE").seq(1).arg("JobFun-ActivityDetector."+jobID)
                         .beforeInvocation(jobID).seq(1).arg(nomTime,12));
          
          // WARNING: can still activate the watched subject directly...
          detector.incrementSeq();
          Time t3{0,3,3};
          invoke.activate (t3, detector.executionCtx);
          CHECK (detector.verifyInvocation(jobID).seq(2));            // subject invoked
          CHECK (detector.ensureNoInvocation("tap-INVOKE").seq(2)     // but invocation not detected by ActivationTap
                         .beforeInvocation(jobID).seq(2));
        }
      
      
      
      /** @test inject (prepend) an ActivationTap into existing wiring
       * @todo WIP 8/23 ✔ define ✔ implement
       */
      void
      insert_ActivationTap()
        {
          ActivityDetector detector;
          
          Activity subject;
          Activity followUp{size_t(1), size_t(2)};
          subject.next = &followUp;
          Activity* wiring = &subject;
          CHECK (isSameObject (*wiring, subject));
          CHECK (wiring->verb_ == Activity::TICK);
          
          detector.insertActivationTap (wiring);
          CHECK (not isSameObject (*wiring, subject));
          CHECK (wiring->verb_ == Activity::HOOK);
          CHECK (wiring->data_.callback.arg == size_t(&subject));
          CHECK (wiring->next == subject.next);
          
          Time tt{1,1,1};
          // now activate through the wiring....
          wiring->activate(tt, detector.executionCtx);
          CHECK (detector.verifyInvocation("tap-TICK").arg("⧐ Act(TICK")
                         .beforeInvocation("CTX-tick").timeArg(tt));
        }
      
      
      
      /** @test diagnostic setup to detect passing a notification
       *        - setup a chain-Activity (here: a `TICK`) protected by a `GATE`
       *        - configure the `GATE` to require one notification
       *        - connect a `NOTIFY`-Activity to trigger the `GATE`
       *        - inject a diagnostics Tap into the notification-connection
       *        - dispatch of the notification can be verified
       *        - notification has been passed through the Tap to the `GATE`
       *        - `GATE` has been decremented to zero and triggers chain
       *        - finally the chained `TICK`-Activity calls into the `executionCtx` 
       * @todo WIP 8/23 ✔ define 🔁 implement
       */
      void
      watch_notification()
        {
          ActivityDetector detector;
          
          Activity chain;
          Activity gate{1};
          gate.next = &chain;
          Activity notification{&gate};
          CHECK (gate.data_.condition.rest == 1);
          
          detector.insertActivationTap (notification.data_.notification.target);
          
          Time tt{11,11};
          notification.dispatch (tt, detector.executionCtx);
          
          CHECK (detector.verifyInvocation("tap-GATE").arg("11.011 --notify-↯> Act(GATE")
                         .beforeInvocation("CTX-post").arg("11.011", "Act(TICK", "≺test::CTX≻"));
          CHECK (gate.data_.condition.rest == 0);
        }
      
      
      
      /** @test diagnostic setup to watch Activity::GATE activation
       *        - when applied, Tap will be inserted before and after the
       *          instrumented GATE-Activity
       *        - it can thus be traced when the Gate is activated,
       *          but also when the Gate condition is met and the `next`
       *          Activity after the Gate is activated
       *        - for this unit-test, a Gate and a follow-up Activity
       *          is invoked directly, to verify the generated log entries
       * @todo WIP 7/23 ✔ define ✔ implement
       */
      void
      watch_gate()
        {
          ActivityDetector detector;
          
          Activity gate{0};
          Activity followUp;
          gate.next = &followUp;
          
          Activity* wiring = &gate;
          detector.watchGate (wiring);
          
          Time tt{5,5};
          wiring->activate(tt, detector.executionCtx);
          detector.incrementSeq();
          wiring->next->activate(tt, detector.executionCtx);
          
          CHECK (detector.verifyInvocation("tap-GATE").seq(0).timeArg(tt)
                         .beforeSeqIncrement(1)
                         .beforeInvocation("after-GATE").seq(1).timeArg(tt)
                         .beforeInvocation("CTX-tick").seq(1).timeArg(tt));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ActivityDetector_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
