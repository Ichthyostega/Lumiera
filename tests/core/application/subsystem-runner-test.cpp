/*
  SubsystemRunner(Test)  -  validate starting and stopping of dependent subsystems

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file subsystem-runner-test.cpp
 ** unit test \ref SubsystemRunner_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "common/subsys.hpp"
#include "common/subsystem-runner.hpp"
#include "common/option.hpp"

#include "lib/symbol.hpp"
#include "backend/thread-wrapper.hpp"
#include "lib/query-util.hpp"
#include "lib/format-cout.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"
#include "lib/sync.hpp"

#include <functional>

using std::bind;
using util::isnil;
using util::cStr;
using test::Test;
using lib::Literal;
using lib::query::extractID;
using backend::Thread;


namespace lumiera {
namespace test  {
    
    namespace { // private test classes and data...
      
      using lib::query::extractID;
      
      /** limit for the randomly selected duration of
       *  subsystem's running phase (milliseconds) */
      const uint MAX_RUNNING_TIME_ms = 80;
      const uint MIN_RUNNING_TIME_ms = 20;
      
      /** the "running" subsystem checks for a
       *  shutdown request every XX milliseconds */
      const uint TICK_DURATION_ms = 5;
      
      /** due to a shortcoming of this test fixture,
       *  a floundering subsystem continues to run for
       *  a short time after the sync barrier.
       *  Relevant for singleSubsys_start_failure().
       */
      const uint DELAY_FOR_FLOUNDERING_THRAD_ms = 20;
      
      /** dummy options just to be ignored */
      lib::Cmdline dummyArgs ("");
      lumiera::Option dummyOpt (dummyArgs);
      
      /** marker for simulated failure exceptions */
      LUMIERA_ERROR_DEFINE( TEST, "simulated failure.");
      
      using error::LERR_(LOGIC);
      using error::LERR_(STATE);
      
      
      
      
      /** 
       * A simulated "Lumiera Subsystem".
       * It is capable of starting a separate thread, which may terminate regularly
       * after a random time, or may fail in various ways. The behaviour is controlled
       * by a number of definitions, given at construction in logic predicate notation.
       */
      class MockSys
        : public lumiera::Subsys
        {
          Literal id_;
          const string spec_;
          
          volatile bool isUp_;
          volatile bool didRun_;
          volatile bool started_;
          volatile bool termRequest_;
          int running_duration_;
          
          
          bool
          shouldStart (lumiera::Option&)  override
            {
              string startSpec (extractID ("start",spec_));
              return "true" ==startSpec 
                  || "fail" ==startSpec
                  || "throw"==startSpec;
            }
          
          
          bool
          start (lumiera::Option&, Subsys::SigTerm termination)  override
            {
              CHECK (!(isUp_|started_|didRun_), "attempt to start %s twice!", cStr(*this));
              
              string startSpec (extractID ("start",spec_));
              CHECK (!isnil (startSpec));
              
              if ("true"==startSpec) //----simulate successful subsystem start
                {
                  CHECK (!started_);
                   
                  Thread (id_, bind (&MockSys::run, this, termination))
                        .sync();     // run-status handshake
                  
                  CHECK (started_);
                }
              else
              if ("fail"==startSpec) //----not starting, incorrectly reporting success
                return true;
              else
              if ("throw"==startSpec) //---starting flounders
                throw error::Fatal("simulated failure to start the subsystem", LERR_(TEST));
              
              return started_;
            }
          
          void
          triggerShutdown ()  noexcept override
            {
              // note: *not* locking here...
              termRequest_ = true;
              
              INFO (test, "triggerShutdown() --> %s....", cStr(*this));
            }
          
          bool 
          checkRunningState ()  noexcept override
            {
              // note: *not* locking here...
              return isUp_;
            }
          
          
          /** executes in a separate thread and
           *  simulates a "running" subsystem.
           *  Behaviour determined by run(XX) spec:
           *  - run(true) : start, run, terminate normally
           *  - run(throw): start, run, signal abnormal termination
           *  - run(fail) : set didRun_, but abort, never enter running state
           *  - run(false): just handshake, but then abort without further action
           */
          void
          run (Subsys::SigTerm termination)
            {
              string runSpec (extractID ("run",spec_));
              CHECK (!isnil (runSpec));
              
              // run-status handshake
              started_ = true;
              isUp_    = ("true"==runSpec || "throw"==runSpec);
              didRun_  = ("false"!=runSpec); // includes "fail" and "throw"
              lumiera_thread_sync ();
              
              if (isUp_) //-------------actually enter running state for some time
                {
                  running_duration_  =  MIN_RUNNING_TIME_ms;
                  running_duration_ += (rand() % (MAX_RUNNING_TIME_ms - MIN_RUNNING_TIME_ms));
                  
                  INFO (test, "thread %s now running....", cStr(*this));
                  
                  while (!shouldTerminate())
                    {
                      usleep (1000*TICK_DURATION_ms);
                      running_duration_ -= TICK_DURATION_ms;
                    }
                  
                  INFO (test, "thread %s about to terminate...", cStr(*this));
                  isUp_ = false;
                }
              
              if ("fail" ==runSpec) return; // terminate without further notice
              if ("true" ==runSpec) termination(0); // signal regular termination
              if ("throw"==runSpec)
                {
                  Error problemIndicator("simulated Problem terminating subsystem",LERR_(TEST));
                  lumiera_error();    //  reset error state....
                                     //   Note: in real life this actually
                                    //    would be an catched exception!
                  string problemReport (problemIndicator.what());
                  termination (&problemReport);
                }
              
            }
          
          
          bool
          shouldTerminate ()  ///< simulates async termination, either on request or by timing
            {
              return termRequest_ || running_duration_ <= 0;
            }
          
          
          
        public:
          MockSys(Literal id, Literal spec)
            : id_(id),
              spec_(spec),
              isUp_(false),
              didRun_(false),
              started_(false),
              termRequest_(false),
              running_duration_(0)
            { }
          
          ~MockSys() { }
          
          operator string ()  const { return "MockSys(\""+id_+"\")"; }
          
          bool didRun ()  const { return didRun_; }
        };
      
      
      
    } // (End) test classes and data....
    
    
    
    
    
    
    
    
    
    
    /**********************************************************************//**
     * @test managing start and stop of several dependent "subsystems"
     *       under various conditions. Using mock-subsystems, which actually
     *       spawn a thread and finish by themselves and generally behave sane.
     *       For each such MockSys, we can define a behaviour pattern, e.g.
     *       weather the start succeeds and if the run terminates with error.
     * 
     * @see lumiera::Subsys
     * @see lumiera::SubsystemRunner
     * @see lumiera::AppState
     * @see main.cpp
     */
    class SubsystemRunner_test : public Test
      {
        
        virtual void
        run (Arg)
          {
            singleSubsys_complete_cycle();
            singleSubsys_start_failure();
            singleSubsys_emegency_exit();
            
            dependentSubsys_complete_cycle();
            dependentSubsys_start_failure();
          }
        
        
        void
        singleSubsys_complete_cycle()
          {
            cout << "-----singleSubsys_complete_cycle-----\n";
            
            MockSys unit ("one", "start(true), run(true).");
            SubsystemRunner runner(dummyOpt);
            CHECK (!unit.isRunning());
            CHECK (!unit.didRun());
            
            runner.maybeRun (unit);
            bool emergency = runner.wait();
            
            CHECK (!emergency);
            CHECK (!unit.isRunning());
            CHECK (unit.didRun());
          }
        
        
        /** @note as this test focuses on the SubsystemRunner, the mock subsystem
         *  is implemented rather simplistic. Especially, there is a race when a
         *  subsystem is configured to "fail" -- because in this case the starting
         *  context may go away before the remainder of the subsystem thread has
         *  executed after the sync() barrier. Especially in this case, no MockSys
         *  actually starts without failure, and thus the SubsystemRunner::wait()
         *  has no guarding effect. This can be considered a shortcoming of the
         *  test fixture; a well behaved subsystem won't just go away...
         */
        void
        singleSubsys_start_failure()
          {
            cout << "-----singleSubsys_start_failure-----\n";
            
            MockSys unit1 ("U1", "start(false), run(false).");
            MockSys unit2 ("U2", "start(throw), run(false).");
            MockSys unit3 ("U3", "start(fail),  run(false).");  // simulates incorrect behaviour
            MockSys unit4 ("U4", "start(true),  run(fail)." );  // simulates failure immediately after start
            SubsystemRunner runner(dummyOpt);
            
            runner.maybeRun (unit1);  // this one doesn't start at all, which isn't considered an error
            
            VERIFY_ERROR (TEST,  runner.maybeRun (unit2) );
            VERIFY_ERROR (LOGIC, runner.maybeRun (unit3) );     // incorrect behaviour trapped
            VERIFY_ERROR (LOGIC, runner.maybeRun (unit4) );     // detected that the subsystem didn't come up
            
            usleep (DELAY_FOR_FLOUNDERING_THRAD_ms * 1000);     // preempt to allow unit4 to go away
            runner.wait();
            
            CHECK (!unit1.isRunning());
            CHECK (!unit2.isRunning());
            CHECK (!unit3.isRunning());
            CHECK (!unit4.isRunning());
            CHECK (!unit1.didRun());
            CHECK (!unit2.didRun());
            CHECK (!unit3.didRun());
            CHECK ( unit4.didRun()); // ...but it failed immediately
          }
        
        
        void
        singleSubsys_emegency_exit()
          {
            cout << "-----singleSubsys_emegency_exit-----\n";
            
            MockSys unit ("one", "start(true), run(throw).");
            SubsystemRunner runner(dummyOpt);
            
            runner.maybeRun (unit);
            bool emergency = runner.wait();
            
            CHECK (emergency);      // emergency state got propagated
            CHECK (!unit.isRunning());
            CHECK (unit.didRun());
          }
        
        
        void
        dependentSubsys_complete_cycle()
          {
            cout << "-----dependentSubsys_complete_cycle-----\n";
            
            MockSys unit1 ("U1", "start(true), run(true).");
            MockSys unit2 ("U2", "start(true), run(true).");
            MockSys unit3 ("U3", "start(true), run(true).");
            MockSys unit4 ("U4", "start(true), run(true).");
            unit2.depends (unit1);
            unit4.depends (unit3);
            unit4.depends (unit1);
            unit3.depends (unit2);
            SubsystemRunner runner(dummyOpt);
            
            runner.maybeRun (unit4);
            CHECK (unit1.isRunning());
            CHECK (unit2.isRunning());
            CHECK (unit3.isRunning());
            CHECK (unit4.isRunning());
            
            bool emergency = runner.wait();
            
            CHECK (!emergency);
            CHECK (!unit1.isRunning());
            CHECK (!unit2.isRunning());
            CHECK (!unit3.isRunning());
            CHECK (!unit4.isRunning());
            CHECK (unit1.didRun());
            CHECK (unit2.didRun());
            CHECK (unit3.didRun());
            CHECK (unit4.didRun());
          }
        
        
        void
        dependentSubsys_start_failure()
          {
            cout << "-----dependentSubsys_start_failure-----\n";
            
            MockSys unit1 ("U1", "start(true), run(true).");
            MockSys unit2 ("U2", "start(true), run(true).");
            MockSys unit3 ("U3", "start(false),run(false)."); // note
            MockSys unit4 ("U4", "start(true), run(true).");
            unit2.depends (unit1);
            unit4.depends (unit3);
            unit4.depends (unit1);
            unit3.depends (unit2);
            SubsystemRunner runner(dummyOpt);
            
            VERIFY_ERROR (STATE, runner.maybeRun (unit4) );   // failure to bring up prerequisites is detected
            CHECK ( unit1.isRunning());
            CHECK ( unit2.isRunning());
            CHECK (!unit3.isRunning());
            // shutdown has been triggered for unit4, but may require some time
            
            bool emergency = runner.wait();
            
            CHECK (!emergency);     // no problems with the subsystems actually running...
            CHECK (!unit1.isRunning());
            CHECK (!unit2.isRunning());
            CHECK (!unit3.isRunning());
            CHECK (!unit4.isRunning());
            CHECK ( unit1.didRun());
            CHECK ( unit2.didRun());
            CHECK (!unit3.didRun());
            // can't say for sure if unit4 actually did run
          }
      };
    
    
    
    /** Register this test class... */
    LAUNCHER (SubsystemRunner_test, "function common");
    
    
    
}} // namespace lumiera::test
