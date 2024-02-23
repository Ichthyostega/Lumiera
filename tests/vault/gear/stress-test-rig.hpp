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
 ** of this test, performing a binary search for the _breaking point._ The Measurement
 ** attempts to narrow down to the point of massive failure, when the ability to somehow
 ** cope with the schedule completely break down. Based on watching the Scheduler in
 ** operation, the detection was linked to three conditions, which typically will
 ** be triggered together, and within a narrow and reproducible parameter range:
 ** - an individual run counts as _accidentally failed_ when the execution slips
 **   away by more than 2ms with respect to the defined overall schedule. When more
 **   than 55% of all observed runs are considered as failed, the first condition is met
 ** - moreover, the observed ''standard derivation'' must also surpass the same limit
 **   of > 2ms, which indicates that the Scheduling mechanism  is under substantial
 **   strain; in regular operation, the slip is rather ~ 200µs.
 ** - the third condition is that the ''averaged delta'' has surpassed 4ms,
 **   which is 2 times the basic failure indicator.
 ** 
 ** ## Observation tools
 ** As a complement to the bench::BreakingPoint tool, another tool is provided to
 ** run a specific Scheduler setup while varying a single control parameter within
 ** defined limits. This produces a set of (x,y) data, which can be used to search
 ** for correlations or build a linear regression model to describe the Scheduler's
 ** behaviour as function of the control parameter. The typical use case would be
 ** to use the input length (number of Jobs) as control parameter, leading to a
 ** model for the Scheduler's expense.
 ** 
 ** @see TestChainLoad_test
 ** @see SchedulerStress_test
 ** @see binary-search.hpp
 */


#ifndef VAULT_GEAR_TEST_STRESS_TEST_RIG_H
#define VAULT_GEAR_TEST_STRESS_TEST_RIG_H


#include "vault/common.hpp"
#include "lib/binary-search.hpp"
//#include "test-chain-load.hpp"
//#include "lib/test/transiently.hpp"

#include "vault/gear/scheduler.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/iter-explorer.hpp"
#include "lib/meta/function.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"//////////////////////////TODO RLY?
//#include "lib/util.hpp"

//#include <functional>
#include <utility>
//#include <memory>
//#include <string>
#include <vector>
#include <tuple>
#include <array>


namespace vault{
namespace gear {
namespace test {
  
  using util::_Fmt;
  using util::min;
  using util::max;
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
  using std::make_pair;
  using std::make_tuple;
//  using std::forward;
//  using std::string;
//  using std::swap;
  using std::vector;
  using std::move;
  
  namespace err = lumiera::error;  //////////////////////////TODO RLY?
  
  namespace { // Default definitions ....
    
  }
  
  
  
  /** configurable template framework for running Scheduler Stress tests */
  class StressRig
    : util::NonCopyable
    {
      
    public:
      /***********************************************************************//**
       * Entrance Point: build a stress test measurement setup using a dedicated
       * \a TOOL class, takes the configuration \a CONF as template parameter
       * and which is assumed to inherit (indirectly) from StressRig.
       * @tparam CONF specialised subclass of StressRig with customisation
       * @return a builder to configure and then launch the actual test
       */
      template<class CONF>
      static auto
      with()
        {
          return Launcher<CONF>{};
        }
      
      
      /* ======= default configuration (inherited) ======= */
      
      using usec = std::chrono::microseconds;
      
      usec LOAD_BASE = 500us;
      usec BASE_EXPENSE = 0us;
      bool SCHED_NOTIFY  = true;
      bool SCHED_DEPENDS = false;
      uint CONCURRENCY = work::Config::getDefaultComputationCapacity();
      bool INSTRUMENTATION = true;
      double EPSILON      = 0.01;          ///< error bound to abort binary search
      double UPPER_STRESS = 0.6;           ///< starting point for the upper limit, likely to fail
      double FAIL_LIMIT   = 2.0;           ///< delta-limit when to count a run as failure
      double TRIGGER_FAIL = 0.55;          ///< %-fact: criterion-1 failures above this rate
      double TRIGGER_SDEV = FAIL_LIMIT;    ///< in ms : criterion-2 standard derivation
      double TRIGGER_DELTA = 2*FAIL_LIMIT; ///< in ms : criterion-3 average delta above this limit
      bool showRuns = false;    ///< print a line for each individual run
      bool showStep = true;     ///< print a line for each binary search step
      bool showRes  = true;     ///< print result data
      bool showRef  = true;     ///< calculate single threaded reference time
      
      static uint constexpr REPETITIONS{20};

      BlockFlowAlloc bFlow{};
      EngineObserver watch{};
      Scheduler scheduler{bFlow, watch};
      
      
      
    protected:
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
      
      template<class CONF>
      struct Launcher : CONF
        {
          template<template<class> class TOOL, typename...ARGS>
          auto
          perform (ARGS&& ...args)
            {
              return TOOL<CONF>{}.perform (std::forward<ARGS> (args)...);
            }
        };
    };
  
  
  
  
  namespace bench { ///< Specialised tools to investigate scheduler performance
    
    using std::declval;
    
    
    /**************************************************//**
     * Specific stress test scheme to determine the
     * »breaking point« where the Scheduler starts to slip
     */
    template<class CONF>
    class BreakingPoint
      : public CONF
      {
        using TestLoad  = decltype(declval<BreakingPoint>().testLoad());
        using TestSetup = decltype(declval<BreakingPoint>().testSetup (declval<TestLoad&>()));
        
        struct Res
          {
            double stressFac{0};
            double percentOff{0};
            double stdDev{0};
            double avgDelta{0};
            double avgTime{0};
            double expTime{0};
          };
        
        double adjustmentFac{1.0};
        
        /** prepare the ScheduleCtx for a specifically parametrised test series */
        void
        configureTest (TestSetup& testSetup, double stressFac)
          {
            testSetup.withLoadTimeBase(CONF::LOAD_BASE)
                     .withBaseExpense (CONF::BASE_EXPENSE)
                     .withSchedNotify (CONF::SCHED_NOTIFY)
                     .withSchedDepends(CONF::SCHED_DEPENDS)
                     .withInstrumentation(CONF::INSTRUMENTATION)          // side-effect: clear existing statistics
                     .withAdaptedSchedule(stressFac, CONF::CONCURRENCY, adjustmentFac);
          }
        
        /** perform a repetition of test runs and compute statistics */
        Res
        runProbes (TestSetup& testSetup, double stressFac)
          {
            auto sqr = [](auto n){ return n*n; };
            Res res;
            auto& [sf,pf,sdev,avgD,avgT,expT] = res;
            sf   = stressFac;
            std::array<double, CONF::REPETITIONS> runTime;
            for (uint i=0; i<CONF::REPETITIONS; ++i)
              {
                runTime[i] = testSetup.launch_and_wait() / 1000;
                avgT += runTime[i];
                testSetup.adaptEmpirically (stressFac, CONF::CONCURRENCY);
                this->adjustmentFac = 1 / (testSetup.getStressFac() / stressFac);
              }
            expT = testSetup.getExpectedEndTime() / 1000;
            avgT /= CONF::REPETITIONS;
            avgD = (avgT-expT); // can be < 0
            for (uint i=0; i<CONF::REPETITIONS; ++i)
              {
                sdev += sqr (runTime[i] - avgT);
                double delta = (runTime[i] - expT);
                bool fail = (delta > CONF::FAIL_LIMIT);
                if (fail)
                  ++ pf;
                showRun(i, delta, runTime[i], runTime[i] > avgT, fail);
              }
            pf /= CONF::REPETITIONS;
            sdev = sqrt (sdev/CONF::REPETITIONS);
            showStep(res);
            return res;
          }
        
        /** criterion to decide if this test series constitutes a slipped schedule */
        bool
        decideBreakPoint (Res& res)
          {
            return res.percentOff > CONF::TRIGGER_FAIL
               and res.stdDev     > CONF::TRIGGER_SDEV
               and res.avgDelta   > CONF::TRIGGER_DELTA;
          }
        
        /**
         * invoke a binary search to produce a sequence of test series
         * with the goal to narrow down the stressFact where the Schedule slips away.
         */
        template<class FUN>
        Res
        conductBinarySearch (FUN&& runTestCase, vector<Res> const& results)
          {
            double breakPoint = lib::binarySearch_upper (forward<FUN> (runTestCase)
                                                        , 0.0, CONF::UPPER_STRESS
                                                        , CONF::EPSILON);
            uint s = results.size();
            ENSURE (s >= 2);
            Res res;
            auto& [sf,pf,sdev,avgD,avgT,expT] = res;
            // average data over the last three steps investigated for smoothing
            uint points = min (results.size(), 3u);
            for (uint i=results.size()-points; i<results.size(); ++i)
              {
                Res const& resx = results[i];
                pf   += resx.percentOff;
                sdev += resx.stdDev;
                avgD += resx.avgDelta;
                avgT += resx.avgTime;
                expT += resx.expTime;
              }
            pf   /= points;
            sdev /= points;
            avgD /= points;
            avgT /= points;
            expT /= points;
            sf = breakPoint;
            return res;
          }
        
        
        _Fmt fmtRun_ {"....·%-2d:  Δ=%4.1f        t=%4.1f  %s %s"};                          //      i % Δ  % t % t>avg?  % fail?
        _Fmt fmtStep_{ "%4.2f|  : ∅Δ=%4.1f±%-4.2f  ∅t=%4.1f  %s %%%-3.0f -- expect:%4.1fms"};// stress % ∅Δ % σ % ∅t % fail % pecentOff % t-expect
        _Fmt fmtResSDv_{"%9s= %5.2f ±%4.2f%s"};
        _Fmt fmtResVal_{"%9s: %5.2f%s"};
        
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
              cout << fmtStep_ % res.stressFac % res.avgDelta % res.stdDev % res.avgTime
                               % (decideBreakPoint(res)? "—◆—":"—◇—")
                               % (100*res.percentOff) % res.expTime
                   << endl;
          }
        
        void
        showRes(Res& res)
          {
            if (CONF::showRes)
              {
                cout << fmtResVal_ % "stresFac" % res.stressFac             % ""  <<endl;
                cout << fmtResVal_ %     "fail" %(res.percentOff * 100)     % '%' <<endl;
                cout << fmtResSDv_ %    "delta" % res.avgDelta % res.stdDev % "ms"<<endl;
                cout << fmtResVal_ %  "runTime" % res.avgTime               % "ms"<<endl;
                cout << fmtResVal_ % "expected" % res.expTime               % "ms"<<endl;
              }
          }
        
        void
        showRef(TestLoad& testLoad)
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
        perform()
          {
            TRANSIENTLY(work::Config::COMPUTATION_CAPACITY) = CONF::CONCURRENCY;
            
            TestLoad testLoad = CONF::testLoad().buildTopology();
            TestSetup testSetup = CONF::testSetup (testLoad);
            
            vector<Res> observations;
            auto performEvaluation = [&](double stressFac)
                                        {
                                          configureTest (testSetup, stressFac);
                                          auto res = runProbes (testSetup, stressFac);
                                          observations.push_back (res);
                                          return decideBreakPoint(res);
                                        };
            
            Res res = conductBinarySearch (move(performEvaluation), observations);
            showRes (res);
            showRef (testLoad);
            return make_tuple (res.stressFac, res.avgDelta, res.avgTime);
          }
      };
    
    
    
    
    /**************************************************//**
     * Specific test scheme to perform a Scheduler setup
     * over a given control parameter range to determine
     * correlations
     */
    template<class CONF>
    class ParameterRange
      : public CONF
      {
        using TestLoad  = decltype(declval<ParameterRange>().testLoad());
        using TestSetup = decltype(declval<ParameterRange>().testSetup (declval<TestLoad&>()));
        
        
        
      public:
        /**
         * Launch a measurement sequence running the Scheduler with a
         * varying parameter value to investigate (x,y) correlations.
         * @return ////TODO a tuple `[stress-factor, ∅delta, ∅run-time]`
         */
        auto
        perform()
          {
            TRANSIENTLY(work::Config::COMPUTATION_CAPACITY) = CONF::CONCURRENCY;
            
            TestLoad testLoad = CONF::testLoad().buildTopology();
            TestSetup testSetup = CONF::testSetup (testLoad);
            
            UNIMPLEMENTED ("parametric runs");
//          return make_tuple (res.stressFac, res.avgDelta, res.avgTime);
          }
      };
    //
  }// namespace bench
}}}// namespace vault::gear::test
#endif /*VAULT_GEAR_TEST_STRESS_TEST_RIG_H*/
