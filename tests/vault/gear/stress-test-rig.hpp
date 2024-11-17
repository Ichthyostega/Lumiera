/*
  STRESS-TEST-RIG.hpp  -  setup for stress and performance investigation

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
 ** Another, complimentary observation method is to inject a defined and homogeneous
 ** load peak into the scheduler and then watch the time it takes to process, the
 ** processing overhead and achieved degree of concurrency. The actual observation
 ** using this measurement setup attempts to establish a single _control parameter_
 ** as free variable, allowing to look for correlations and to build a linear
 ** regression model to characterise a supposed functional dependency. Simply put,
 ** given a number of fixed sizes jobs (not further correlated) as input, this
 ** approach yields a »number of jobs per time unit« and »socked overhead« —
 ** thereby distilling a _behaviour model_ to describe the actual stochastic data.
 ** 
 ** ## Setup
 ** To perform this test scheme, an operational Scheduler is required, and an instance
 ** of the TestChainLoad must be provided, configured with desired load properties.
 ** Moreover, the actual measurement setup requires to perform several test executions,
 ** controlling some parameters in accordance to the observation scheme. The control
 ** parameters and the specifics of the actual setup should be clearly visible, while
 ** hiding the complexities of measurement execution.
 ** 
 ** This can be achieved by a »Toolbench«, which is a framework with building blocks,
 ** providing a pre-arranged _measurement rig_ for the various kinds of measurement setup.
 ** The implementation code is arranged as a »sandwich« structure...
 ** - StressTestRig, which is also the framework class, acts as _bottom layer_ to
 **   provide an anchor point, some common definitions implying an invocation scheme
 **   + first a TestChainLoad topology is constructed, based on test parameters
 **   + this is used to create a TestChainLoad::SchedulerCtx, which is then
 **     outfitted specifically for each test run
 ** - the _middle layer_ is a custom `Setup` class, which inherits from the bottom
 **   layer and fills in the actual topology and configuration for the desired test
 ** - the test performance is then initiated by layering a specific _test tool_ on
 **   top of the compound, which in turn picks up the parametrisation from the Setup
 **   and base configuration, visible as base class (template param) \a CONF
 ** Together, this leads to the following code scheme, which aims to simplify experimentation:
 ** \code
 ** using StressRig = StressTestRig<16>;
 ** 
 ** struct Setup : StressRig
 **   {
 **     uint CONCURRENCY = 4;
 **     //// more definitions
 **     
 **     auto testLoad()
 **       {....define a Test-Chain-Load topology....}
 **     
 **     auto testSetup (TestLoad& testLoad)
 **       { return StressRig::testSetup(testLoad)
 **                          .withLoadTimeBase(500us)
 **                       // ....more customisation here
 **       }
 **   };
 ** 
 ** auto result = StressRig::with<Setup>()
 **                         .perform<bench::SpecialToolClass>();
 ** \endcode
 ** 
 ** ## Breaking Point search
 ** The bench::BreakingPoint tool typically uses a complex interwoven job plan, which is
 ** tightened until the timing breaks. The _stressFactor_ of the generated schedule will be
 ** the active parameter of this test, performing a _binary search_ for the _breaking point._
 ** The Measurement attempts to narrow down to the point of massive failure, when the ability
 ** to somehow cope with the schedule completely break down. Based on watching the Scheduler
 ** in operation, the detection was linked to three conditions, which typically will be
 ** triggered together, and within a narrow and reproducible parameter range:
 ** - an individual run counts as _accidentally failed_ when the execution slips
 **   away by more than 2ms with respect to the defined overall schedule. When more
 **   than 55% of all observed runs are considered as failed, the first condition is met
 ** - moreover, the observed ''standard derivation'' must also surpass the same limit
 **   of > 2ms, which indicates that the Scheduling mechanism  is under substantial
 **   strain; in regular operation, the slip is rather ~ 200µs.
 ** - the third condition is that the ''averaged delta'' has surpassed 4ms,
 **   which is 2 times the basic failure indicator.
 ** 
 ** ## Parameter Correlation
 ** As a complement, the bench::ParameterRange tool is provided to run a specific Scheduler setup
 ** while varying a single control parameter within defined limits. This produces a set of (x,y) data,
 ** which can be used to search for correlations or build a linear regression model to describe the
 ** Scheduler's behaviour as function of the control parameter. The typical use case would be to use
 ** the input length (number of Jobs) as control parameter, leading to a model for Scheduling expense.
 ** 
 ** ## Observation tools
 ** The TestChainLoad, together with its helpers and framework, already offers some tools to visualise
 ** the generated topology and to calculate statistics, and to watch an performance with instrumentation.
 ** In addition, the individual tools provide some debugging output to watch the measurement scheme.
 ** Result data is either a tuple of values (in case of bench::BreakingPoint), or a table of result
 ** data as function of the control parameter (for bench::ParameterRange). Result data, when converted
 ** to CSV, can be visualised as Gnuplot diagram.
 ** @see TestChainLoad_test
 ** @see SchedulerStress_test
 ** @see binary-search.hpp
 ** @see gnuplot-gen.hpp
 */


#ifndef VAULT_GEAR_TEST_STRESS_TEST_RIG_H
#define VAULT_GEAR_TEST_STRESS_TEST_RIG_H


#include "test-chain-load.hpp"
#include "lib/binary-search.hpp"
#include "lib/test/transiently.hpp"

#include "vault/gear/scheduler.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/meta/function.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/gnuplot-gen.hpp"
#include "lib/stat/statistic.hpp"
#include "lib/stat/data.hpp"
#include "lib/random.hpp"
#include "lib/util.hpp"

#include <algorithm>
#include <utility>
#include <vector>
#include <tuple>
#include <array>


namespace vault{
namespace gear {
namespace test {
  
  using std::make_tuple;
  using std::forward;
  
  
  /**
   * Configurable template framework for running Scheduler Stress tests
   * Use to build a custom setup class, which is then [injected](\ref StressTestRig::with)
   * to [perform](\ref StressTestRig::Launcher::perform) a _specific measurement tool._
   * Several tools and detailed customisations are available in `namespace bench`
   * - bench::BreakingPoint conducts a binary search to _break a schedule_
   * - bench::ParameterRange performs a randomised series of parametrised test runs
   */
  template<size_t maxFan =DEFAULT_FAN>
  class StressTestRig
    : util::NonCopyable
    {
    public:
      using TestLoad  = TestChainLoad<maxFan>;
      using TestSetup = typename TestLoad::ScheduleCtx;
      
      
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
      
      uint CONCURRENCY = work::Config::getDefaultComputationCapacity();
      bool INSTRUMENTATION = true;
      double EPSILON      = 0.01;          ///< error bound to abort binary search
      double UPPER_STRESS = 1.7;           ///< starting point for the upper limit, likely to fail
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
      testLoad(size_t nodes =64)
        {
          return TestLoad{nodes};
        }
      
      /** (optional) extension point: base configuration of the test ScheduleCtx
       * @warning the actual setup \a CONF is layered, beware of shadowing. */
      auto
      testSetup (TestLoad& testLoad)
        {
          return testLoad.setupSchedule(scheduler)
                         .withLevelDuration(200us)
                         .withJobDeadline(500ms)
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
    
    using util::_Fmt;
    using util::min;
    using util::max;
    using std::vector;
    using std::declval;
    
    
    /**************************************************//**
     * Specific stress test scheme to determine the
     * »breaking point« where the Scheduler starts to slip
     */
    template<class CONF>
    class BreakingPoint
      : public CONF
      {
        using TestLoad  = typename CONF::TestLoad;
        using TestSetup = typename TestLoad::ScheduleCtx;
        
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
            testSetup.withInstrumentation(CONF::INSTRUMENTATION)          // side-effect: clear existing statistics
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
                maybeAdaptScaleEmpirically (testSetup, stressFac);
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
            return res.percentOff > 0.99
               or( res.percentOff > CONF::TRIGGER_FAIL
               and res.stdDev     > CONF::TRIGGER_SDEV
               and res.avgDelta   > CONF::TRIGGER_DELTA);
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
        
        /** adaptive scale correction based on observed behaviour */
        double adjustmentFac{1.0};
        size_t gaugeProbes = 3 * CONF::REPETITIONS;
        
        /**
         * Attempt to factor out some observable properties, which are considered circumstantial
         * and not a direct result of scheduling overheads. The artificial computational load is
         * known to drift towards larger values than calibrated; moreover the actual concurrency
         * achieved can deviate from the heuristic assumptions built into the testing schedule.
         * The latter is problematic to some degree however, since the Scheduler is bound to
         * scale down capacity when idle. To strike a reasonable balance, this adjustment of
         * the measurement scale is done only initially, and when the stress factor is high
         * and some degree of pressure on the scheduler can thus be assumed.
         */
        void
        maybeAdaptScaleEmpirically (TestSetup& testSetup, double stressFac)
          {
            if (not gaugeProbes) return;
            double gain = util::limited (0, pow(stressFac, 9), 1);
            if (gain < 0.2) return;
            //
            double formFac = testSetup.determineEmpiricFormFactor (CONF::CONCURRENCY);
            adjustmentFac = gain*formFac + (1-gain)*adjustmentFac;
            testSetup.withAdaptedSchedule(stressFac, CONF::CONCURRENCY, adjustmentFac);
            --gaugeProbes;
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
        showRef(TestSetup& testSetup)
          {
            if (CONF::showRef)
              cout << fmtResVal_ % "refTime"
                                 % (testSetup.calcRuntimeReference() /1000)
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
            showRef (testSetup);
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
        using TestLoad  = typename CONF::TestLoad;
        using TestSetup = typename TestLoad::ScheduleCtx;
        
        // Type binding for data evaluation
        using Param = typename CONF::Param;
        using Table = typename CONF::Table;
        
        
        void
        runTest (Param param, Table& data)
          {
            TestLoad testLoad = CONF::testLoad(param).buildTopology();
            TestSetup testSetup = CONF::testSetup (testLoad)
                                       .withInstrumentation();    // Note: by default Schedule with CONF::LEVEL_STEP
            double millis = testSetup.launch_and_wait() / 1000;
            auto stat = testSetup.getInvocationStatistic();
            CONF::collectResult (data, param, millis, stat);
          }
        
      public:
        /**
         * Launch a measurement sequence running the Scheduler with a
         * varying parameter value to investigate (x,y) correlations.
         * @return ////TODO a tuple `[stress-factor, ∅delta, ∅run-time]`
         */
        Table
        perform (Param lower, Param upper)
          {
            TRANSIENTLY(work::Config::COMPUTATION_CAPACITY) = CONF::CONCURRENCY;
            
            Param dist = upper - lower;
            uint cnt = CONF::REPETITIONS;
            vector<Param> points;
            points.reserve (cnt);
            Param minP{upper}, maxP{lower};
            for (uint i=0; i<cnt; ++i)
              {
                auto random = lib::defaultGen.uni(); // [0 .. 1.0[
                Param pos = lower + Param(floor (random*dist + 0.5));
                points.push_back(pos);
                minP = min (pos, minP);
                maxP = max (pos, maxP);
              }
            // ensure the bounds participate in test
            if (maxP < upper) points[cnt-2] = upper;
            if (minP > lower) points[cnt-1] = lower;
            
            Table results;
            for (Param point : points)
              runTest (point, results);
            return results;
          }
      };
    
    
    
    /* ====== Preconfigured ParamRange-Evaluations ====== */
    
    using lib::stat::Column;
    using lib::stat::DataTable;
    using lib::stat::DataSpan;
    using lib::stat::CSVData;
    using IncidenceStat = lib::IncidenceCount::Statistic;
    
    /**
     * Calculate a linear regression model for two table columns
     * @return a tuple `(socket,gradient,Vector(predicted),Vector(deltas),correlation,maxDelta,stdev)`
     */
    template<typename F, typename G>
    inline auto
    linearRegression (Column<F> const& x, Column<G> const& y)
    {
      lib::stat::RegressionData points;
      size_t cnt = min (x.data.size(), y.data.size());
      points.reserve (cnt);
      for (size_t i=0; i < cnt; ++i)
        points.emplace_back (x.data[i], y.data[i]);
      return lib::stat::computeLinearRegression (points);
    }
    
    /**
     * Mix-in for setup of a #ParameterRange evaluation to watch
     * the processing of a single load peak, using the number of
     * added job as independent parameter.
     * @remark inject this definition (by inheritance) into the
     *   Setup, which should then also define a TestChainLoad
     *   graph with an overall size controlled by the #Param
     * @see SchedulerStress_test#watch_expenseFunction()
     */
    struct LoadPeak_ParamRange_Evaluation
      {
        using Param = size_t;
        
        struct DataRow
          {
            Column<Param>  param   {"load size"};    // independent variable / control parameter
            Column<double> time    {"result time"};
            Column<double> conc    {"concurrency"};
            Column<double> jobtime {"avg jobtime"};
            Column<double> impeded {"avg impeded"};
            
            auto allColumns()
            { return std::tie(param
                             ,time
                             ,conc
                             ,jobtime
                             ,impeded
                             );
            }
          };
        
        using Table = DataTable<DataRow>;
        
        void
        collectResult(Table& data, Param param, double millis, bench::IncidenceStat const& stat)
          {
            (void)millis;
            data.newRow();
            data.param = param;
            data.time  = stat.coveredTime / 1000;
            data.conc  = stat.avgConcurrency;
            data.jobtime = stat.activeTime / stat.activationCnt;
            data.impeded = (stat.timeAtConc(1) + stat.timeAtConc(0))/stat.activationCnt;
          }
        
        
        static double
        avgConcurrency (Table const& results)
          {
            return lib::stat::average (DataSpan<double> (results.conc.data));
          }
        
        static string
        renderGnuplot (Table const& results)
          {
            using namespace lib::gnuplot_gen;
            string csv = results.renderCSV();
            Param maxParam = * std::max_element (results.param.data.begin(), results.param.data.end());
            Param xtics = maxParam > 500? 50
                        : maxParam > 200? 20
                        : maxParam > 100? 10
                        :                  5;
            return scatterRegression(
                    ParamRecord().set (KEY_CSVData,  csv)
                                 .set (KEY_TermSize, "600,600")
                                 .set (KEY_Xtics,    int64_t(xtics))
                                 .set (KEY_Xlabel,  "load size ⟶ number of jobs")
                                 .set (KEY_Ylabel,  "active time ⟶ ms")
                                 .set (KEY_Y2label, "concurrent threads ⟶")
                                 .set (KEY_Y3label, "avg job time ⟶ µs")
                                );
          }
      };
    //
  }// namespace bench
}}}// namespace vault::gear::test
#endif /*VAULT_GEAR_TEST_STRESS_TEST_RIG_H*/
