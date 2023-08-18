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
#include "activity-detector.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp" /////////////////////////////TODO
//#include "lib/util.hpp"

//#include <utility>

//using test::Test;
//using lib::time::Time;
//using lib::time::FSecs;
//using std::move;
//using util::isSameObject;
using lib::test::randStr;
using lib::test::randTime;


namespace vault{
namespace gear {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
  using lib::time::Time;
  using lib::time::FSecs;
  
  
  
  
  
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
          verifyMockJobFunctor();
          verifyFakeExeContext();
          detect_activation();
          detect_gate();
        }
      
      
      /** @test TODO demonstrate a simple usage scenario
       * @todo WIP 7/23 🔁 define 🔁 implement
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
          
          ++detector;
          CHECK (1 == detector.currSeq());
          CHECK (detector.ensureNoInvocation ("funny"));
          
          ++detector;
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
          
          ++detector;
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
          CHECK (detector.verifyInvocation ("mockJob").nominalTime(nominal));
          
          ++detector;                                                                           // note: sequence number incremented between invocations
          dummyJob.parameter.nominalTime += 5 * Time::SCALE;                                    // different job parameter (later nominal time point)
          dummyJob.triggerJob();
          
          CHECK (detector.verifyInvocation ("mockJob").nominalTime(nominal).seq(0)
                         .beforeInvocation ("mockJob").nominalTime(nominal + Time{FSecs{5}})    // matching first invocation and then second...
                         .afterSeqIncrement(1)                                                  // note: searching backwards from the 2nd invocation
                         );
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
          
          ++detector;
          ctx.tick.returning(activity::KILL);
          CHECK (activity::KILL == ctx.tick(t));
          CHECK (detector.verifyInvocation(CTX_TICK).nominalTime(t));
          
          CHECK (detector.verifyInvocation(CTX_WORK).nominalTime(t)
                         .beforeInvocation(CTX_DONE).nominalTime(t)
                         .beforeInvocation(CTX_POST).nominalTime(t)
                         .beforeInvocation(CTX_TICK).nominalTime(t).seq(0)
                         .beforeInvocation(CTX_TICK).nominalTime(t).seq(1));
        }
      
      
      
      /** @test TODO diagnostic setup to detect Activity activation and propagation
       * @todo WIP 8/23 🔁 define 🔁 implement
       */
      void
      detect_activation()
        {
          auto someID = "trap-" + randStr(4);
          ActivityDetector detector;
          Activity& probe = detector.buildActivationProbe (someID);
          cout << probe << endl;
        }
      
      
      
      /** @test TODO diagnostic setup to watch Activity::GATE activation
       * @todo WIP 7/23 ⟶ define ⟶ implement
       */
      void
      detect_gate()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ActivityDetector_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
