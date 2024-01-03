/*
  STRESS-TEST-RIG.hpp  -  setup for stress and performance investigation

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

*/

/** @file stress-test-rig.hpp
 ** A test bench to conduct performance measurement series. Outfitted especially
 ** to determine runtime behaviour of the Scheduler and associated parts of the
 ** Lumiera Engine through systematic execution of load scenarios.
 ** 
 ** # Scheduler Stress Testing
 ** 
 ** The point of departure for any stress testing is to show that the subject will
 ** break in controlled ways only. For the Scheduler this can easily be achieved by
 ** overloading until job deadlines are broken. Much more challenging however is the
 ** task to find out about the boundary of regular scheduler operation. This realm
 ** can be defined by the ability of the scheduler to follow and conform to the
 ** timings set out explicitly in the schedule. Obviously, short and localised
 ** load peaks can be accommodated, yet once a persistent backlog builds up,
 ** the schedule starts to slip and the calculation process will flounder.
 ** 
 ** A method to determine such a _»breaking point«_ in a systematic way is based on
 ** building a [synthetic calculation load](\ref test-chain-load.hpp) and establish
 ** the timings of a test schedule based on a simplified model of expected computation
 ** expense. By scaling and condensing these schedule timings, a loss of control can
 ** be provoked, and determined by statistical observation: since the process of
 ** scheduling contains an essentially random component, persistent overload will be
 ** indicated by an increasing variance of the overall runtime, and a departure from
 ** the nominal runtime of the executed schedule.
 ** 
 ** ## Setup
 ** To perform this test scheme, an operational Scheduler is required, and an instance
 ** of the TestChainLoad must be provided, configured with desired load properties.
 ** The _stressFactor_ of the corresponding generated schedule will be the active parameter
 ** of this test, performing a binary search for the _breaking point._
 ** 
 ** ## Observation tools
 ** 
 ** @see TestChainLoad_test
 ** @see SchedulerStress_test
 */


#ifndef VAULT_GEAR_TEST_STRESS_TEST_RIG_H
#define VAULT_GEAR_TEST_STRESS_TEST_RIG_H


#include "vault/common.hpp"
//#include "test-chain-load.hpp"
//#include "lib/test/transiently.hpp"

#include "vault/gear/scheduler.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/iter-explorer.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"//////////////////////////TODO RLY?
//#include "lib/util.hpp"

//#include <functional>
#include <utility>
//#include <memory>
//#include <string>
//#include <vector>
#include <tuple>
#include <array>


namespace vault{
namespace gear {
namespace test {
  
  using util::_Fmt;
//  using util::min;
//  using util::max;
//  using util::isnil;
//  using util::limited;
//  using util::unConst;
//  using util::toString;
//  using util::isLimited;
//  using lib::time::Time;
//  using lib::time::TimeValue;
//  using lib::time::FrameRate;
//  using lib::time::Duration;
//  using lib::test::Transiently;
//  using lib::meta::_FunRet;

//  using std::string;
//  using std::function;
//  using std::make_pair;
  using std::make_tuple;
//  using std::forward;
//  using std::string;
//  using std::swap;
  using std::move;
  
  namespace err = lumiera::error;  //////////////////////////TODO RLY?
  
  namespace { // Default definitions ....
    
  }
  
  namespace stress_test_rig {
    
    /**
     * Specific stress test scheme to determine the
     * »breaking point« where the Scheduler starts to slip
     */
    template<class CONF>
    class BreakingPointBench
      : CONF
      {
        using TestLoad  = decltype(std::declval<CONF>().testLoad());
        using TestSetup = decltype(std::declval<CONF>().testSetup (std::declval<TestLoad&>()));
        
        struct Res
          {
            double stressFac{0};
            double percentOff{0};
            double stdDev{0};
            double avgDelta{0};
            double avgTime{0};
            double expTime{0};
          };
        
        /** prepare the ScheduleCtx for a specifically parametrised test series */
        void
        configureTest (TestSetup& testSetup, double stressFac)
          {
            testSetup.withLoadTimeBase (CONF::LOAD_BASE)
                     .withAdaptedSchedule(stressFac, CONF::CONCURRENCY);
          }
        
        /** perform a repetition of test runs and compute statistics */
        Res
        runProbes (TestSetup& testSetup, double stressFac)
          {
            auto sqr = [](auto n){ return n*n; };
            Res res;
            auto& [sf,pf,sdev,avgD,avgT,expT] = res;
            sf = stressFac;
            expT = testSetup.getExpectedEndTime() / 1000;
            std::array<double, CONF::REPETITIONS> runTime;
            for (uint i=0; i<CONF::REPETITIONS; ++i)
              {
                runTime[i] = testSetup.launch_and_wait() / 1000;
                avgT += runTime[i];
              }
            avgT /= CONF::REPETITIONS;
            avgD = fabs (avgT-expT);
            for (uint i=0; i<CONF::REPETITIONS; ++i)
              {
                sdev += sqr (runTime[i] - avgT);
                double delta = fabs (runTime[i] - expT);
                bool fail = (delta > CONF::FAIL_LIMIT);
                if (fail)
                  ++ pf;
                showRun(i, delta, runTime[i], runTime[i] > avgT, fail);
              }
            sdev = sqrt (sdev/CONF::REPETITIONS);
            showStep(res);
            return res;
          }
        
        /** criterion to decide if this test series constitutes a slipped schedule */
        bool
        decideBreakPoint (Res& res)
          {
            return true; //////TODO booooo
          }
        
        /**
         * invoke a binary search to produce a sequence of test series
         * with the goal to narrow down the stressFact where the Schedule slips away.
         */
        template<class FUN>
        Res
        conductBinarySearch (FUN&& runTestCase)
          {
            UNIMPLEMENTED ("invoke a library implementation of binary search");
          }
        
        _Fmt fmtRun_ {"....·%-2d:  Δ=%4.1f         t=%4.1f %s %s"};                      //      i % Δ % t % t>avg?  % fail?
        _Fmt fmtStep_{ "%4.2f|  : ∅Δ=%4.1f±%-4.2f  ∅t=%4.1f %%%3.1f -- expect:%4.1fms"}; // stress % ∅Δ % σ % ∅t % fail % t-expect
        _Fmt fmtResVal_{"%9s: %5.2f%s"};
        _Fmt fmtResSDv_{"%9s= %5.2f ±%4.2f%s"};
        
        void
        showRun(uint i, double delta, double t, bool over, bool fail)
          {
            if (CONF::showRuns)
              cout << fmtRun_ % i % delta % t % (over? "+":"-") % (fail? "●":"○")
                   << endl;
          }
        
        void
        showStep(Res& res)
          {
            if (CONF::showStep)
              cout << fmtStep_ % res.stressFac % res.avgDelta % res.stdDev % res.avgTime % res.percentOff % res.expTime
                   << endl;
          }
        
        void
        showRes(Res& res)
          {
            if (CONF::showRes)
              {
                cout << fmtResVal_ % "stresFac" % res.stressFac             % ""  <<endl;
                cout << fmtResVal_ %     "fail" %(res.percentOff * 100)     % '%' <<endl;
                cout << fmtResSDv_ %       "∅Δ" % res.avgDelta % res.stdDev % "ms"<<endl;
                cout << fmtResVal_ %  "runTime" % res.avgTime               % "ms"<<endl;
                cout << fmtResVal_ % "expected" % res.expTime               % "ms"<<endl;
              }
          }
        
        void
        showRef(TestLoad testLoad)
          {
            if (CONF::showRef)
              cout << fmtResVal_ % "refTime"
                                 % (testLoad.calcRuntimeReference(CONF::LOAD_BASE) /1000)
                                 % "ms" << endl;
          }
        
        
      public:
        /**
         * Launch a measurement sequence to determine the »breaking point«
         * for the configured test load and parametrisation of the Scheduler.
         * @return a tuple `[stress-factor, ∅delta, ∅run-time]`
         */
        auto
        searchBreakingPoint()
          {
            TRANSIENTLY(work::Config::COMPUTATION_CAPACITY) = CONF::CONCURRENCY;
            
            TestLoad testLoad = CONF::testLoad().buildTopology();
            TestSetup testSetup = CONF::testSetup (testLoad);
            
            auto performEvaluation = [&](double stressFac)
                                        {
                                          configureTest (testSetup, stressFac);
                                          auto res = runProbes (testSetup, stressFac);
                                          return make_tuple (decideBreakPoint(res), res);
                                        };
            
            Res res = conductBinarySearch (move (performEvaluation));
            return make_tuple (res.stressFac, res.avgDelta, res.avgTime);
          }
      };
  }//namespace stress_test_rig
  
  
  
  /** configurable template for running Scheduler Stress tests */
  class StressRig
    : util::NonCopyable
    {
      
    public:
      using usec = std::chrono::microseconds;
      
      usec LOAD_BASE = 500us;
      uint CONCURRENCY = work::Config::getDefaultComputationCapacity();
      double FAIL_LIMIT = 2.0;          ///< delta-limit when to count a run as failure
      double TRIGGER_FAIL = 0.55;       ///< %-fact: criterion-1 failures above this rate
      double TRIGGER_SDEV = FAIL_LIMIT; ///< in ms : criterion-2 standard derivation
      double TRIGGER_DELTA = 4.0;       ///< in ms : criterion-3 delta above this limit
      bool showRuns = false;    ///< print a line for each individual run
      bool showStep = true;     ///< print a line for each binary search step
      bool showRes  = true;     ///< print result data
      bool showRef  = true;     ///< calculate single threaded reference time
      
      static uint constexpr REPETITIONS{30};

      BlockFlowAlloc bFlow{};
      EngineObserver watch{};
      Scheduler scheduler{bFlow, watch};
      
      /** Extension point: build the computation topology for this test */
      auto
      testLoad()
        {
          return TestChainLoad<>{64};
        }
      
      /** (optional) extension point: base configuration of the test ScheduleCtx */
      template<class TL>
      auto
      testSetup (TL& testLoad)
        {
          return testLoad.setupSchedule(scheduler)
                         .withJobDeadline(100ms)
                         .withUpfrontPlanning();
        }
      
      /**
       * Entrance Point: build a stress test measurement setup
       * to determine the »breaking point« where the Scheduler is unable
       * to keep up with the defined schedule.
       * @tparam CONF specialised subclass of StressRig with customisation
       * @return a builder to configure and then launch the actual test
       */
      template<class CONF>
      static auto
      with()
        {
          return stress_test_rig::BreakingPointBench<CONF>{};
        }
    };
  
  
}}} // namespace vault::gear::test
#endif /*VAULT_GEAR_TEST_STRESS_TEST_RIG_H*/
