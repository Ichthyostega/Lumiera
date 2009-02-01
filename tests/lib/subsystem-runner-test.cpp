/*
  SubsystemRunner(Test)  -  validate starting and stopping of dependent subsystems
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "lib/test/run.hpp"
#include "common/subsys.hpp"
#include "common/subsystem-runner.hpp"
#include "common/option.hpp"

#include "include/symbol.hpp"
#include "backend/thread-wrapper.hpp"
#include "lib/error.hpp"
#include "lib/query.hpp"
#include "lib/util.hpp"
#include "lib/sync.hpp"

#include <tr1/functional>
#include <iostream>

using std::cout;
using std::tr1::bind;
using util::isnil;
using util::cStr;
using test::Test;
using lib::Sync;
using lib::RecursiveLock_Waitable;
using backend::Thread;


namespace lumiera {
  namespace test  {
    
    namespace { // private test classes and data...
      
      using query::extractID;
      
      /** limit for the randomly selected duration of
       *  subsystem's running phase (milliseconds) */
      const uint MAX_RUNNING_TIME_ms = 80;
      const uint MIN_RUNNING_TIME_ms = 10;
      
      /** the "running" subsystem checks for a
       *  shutdown request every XX milliseconds */
      const uint TICK_DURATION_ms = 5;
      
      /** dummy options just to be ignored */
      util::Cmdline dummyArgs ("");
      lumiera::Option dummyOpt (dummyArgs);
      
      /** marker for simulated failure exceptions */
      LUMIERA_ERROR_DEFINE( TEST, "simulated failure.");
      
      
      
      
      /** 
       * A simulated "Lumiera Subsystem".
       * It is capable of starting a separate thread,
       * which may terminate regularly after a random
       * time, or may fail in various ways. 
       * The behaviour is controlled by a number of
       * definitions, given at construction in
       * logic predicate notation.
       */
      class MockSys
        : public lumiera::Subsys,
          public Sync<RecursiveLock_Waitable>
        {
          Literal id_;
          Literal spec_;
          
          volatile bool isUp_;
          volatile bool didRun_;
          volatile bool termRequest_;
          int running_duration_;
          
          
          bool
          shouldStart (lumiera::Option&)
            {
              Literal startSpec (extractID ("start",spec_));
              return "true" ==startSpec 
                  || "fail" ==startSpec
                  || "throw"==startSpec;
            }
          
          
          bool
          start (lumiera::Option&, Subsys::SigTerm termination)
            {
              REQUIRE (!isUp_, "attempt to start %s twice!", cStr(*this));
              
              Lock guard (this);
              Literal startSpec (extractID ("start",spec_));
              ASSERT (!isnil (startSpec));
              
              if ("true"==startSpec) //----simulate successful subsystem start
                { 
                  Thread (id_, bind (&MockSys::run, this, termination));            /////TODO: the thread description should be rather "Test: "+string(*this), but this requires to implement the class Literal as planned
                  isUp_ = true;
                }
              else
              if ("fail"==startSpec) //----not starting, incorrectly reporting success
                return true;
              else
              if ("throw"==startSpec) //---starting flounders
                throw error::Fatal("simulated failure to start the subsystem", LUMIERA_ERROR_TEST);
              
              return isUp_;
            }
          
          void
          triggerShutdown ()  throw()
            {
              // note: *not* locking here...
              termRequest_ = true;
              
              INFO (test, "triggerShutdown() --> %s....", cStr(*this));
            }
          
          bool 
          checkRunningState ()  throw()
            {
              // note: *not* locking here...
              return isUp_;
            }
          
          
          void
          run (Subsys::SigTerm termination)  ///< simulates a "running" subsystem
            {
              Literal runSpec (extractID ("run",spec_));
              ASSERT (!isnil (runSpec));
              
              if ("false"!=runSpec) //----actually enter running state for some time
                {
                  didRun_ = true;
                  running_duration_  =  MIN_RUNNING_TIME_ms;
                  running_duration_ += (rand() % (MAX_RUNNING_TIME_ms - MIN_RUNNING_TIME_ms));
                  
                  INFO (test, "thread %s now running....", cStr(*this));
                  
                  Lock wait_blocking (this, &MockSys::tick);
                }
              
              Error problemIndicator("simulated Problem killing a subsystem",LUMIERA_ERROR_TEST);
              lumiera_error();   //  reset error state....
                                //   Note: in real life this actually
                               //    would be an catched exception!
              string problemReport (problemIndicator.what());
              
              {
                Lock guard (this);
                isUp_ = false;
              }
              INFO (test, "thread %s terminates.", cStr(*this));
              termination ("true"==runSpec? 0 : &problemReport);
            }
          
          
          bool
          tick ()   ///< simulates async termination, either on request or by timing
            {
              Lock sync(this);
              if (!sync.isTimedWait())
                {
                  sync.setTimeout(TICK_DURATION_ms);
                  running_duration_ += TICK_DURATION_ms;
                }
              
              running_duration_ -= TICK_DURATION_ms;
              return termRequest_ || running_duration_ <= 0;
            }
          
          
          
        public:
          MockSys(Literal id, Literal spec)
            : id_(id),
              spec_(spec),
              isUp_(false),
              didRun_(false),
              termRequest_(false),
              running_duration_(0)
            { }
          
          ~MockSys() { }
          
          operator string ()  const { return "MockSys(\""+id_+"\")"; }
          
          friend inline ostream&
          operator<< (ostream& os, MockSys const& mosi) { return os << string(mosi); }
          
          bool didRun ()  const { return didRun_; }
        };
      
      
      
    } // (End) test classes and data....
    
    
    
    
    
    
    
    
    
    
    /**************************************************************************
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
            REQUIRE (!unit.isRunning());
            REQUIRE (!unit.didRun());
            
            runner.maybeRun (unit);
            bool emergency = runner.wait();
            
            ASSERT (!emergency);
            ASSERT (!unit.isRunning());
            ASSERT (unit.didRun());
          }
        
        
        void
        singleSubsys_start_failure()
          {
            cout << "-----singleSubsys_start_failure-----\n";
            
            MockSys unit1 ("U1", "start(false), run(false).");
            MockSys unit2 ("U2", "start(throw), run(false).");
            MockSys unit3 ("U3", "start(fail),  run(false).");  // simulates incorrect behaviour
            MockSys unit4 ("U4", "start(true),  run(false).");
            SubsystemRunner runner(dummyOpt);
            
            runner.maybeRun (unit1);  // this one doesn't start at all, which isn't considered an error
            
            try 
              { 
                runner.maybeRun (unit2);
                NOTREACHED;
              }
            catch (lumiera::Error&)
              {
                ASSERT (lumiera_error() == LUMIERA_ERROR_TEST);
              }
            try 
              { 
                runner.maybeRun (unit3);
                NOTREACHED;
              }
            catch (lumiera::Error&)
              {
                ASSERT (lumiera_error() == error::LUMIERA_ERROR_LOGIC); // incorrect behaviour trapped
              }
            try 
              { 
                runner.maybeRun (unit4);
              }
            catch (lumiera::Error&)
              {
                ASSERT (lumiera_error() == error::LUMIERA_ERROR_LOGIC); // detected that the subsystem didn't come up
              }                                                         //   (due to the way the test subsystem is written,
                                                                        //    this may not always be detected, because there
                                                                        //    is a short time window where isUp_==true )
            
            
            runner.wait();
            
            ASSERT (!unit1.isRunning());
            ASSERT (!unit2.isRunning());
            ASSERT (!unit3.isRunning());
            ASSERT (!unit4.isRunning());
            ASSERT (!unit1.didRun());
            ASSERT (!unit2.didRun());
            ASSERT (!unit3.didRun());
            ASSERT (!unit4.didRun());
          }
        
        
        void
        singleSubsys_emegency_exit()
          {
            cout << "-----singleSubsys_emegency_exit-----\n";
            
            MockSys unit ("one", "start(true), run(fail).");
            SubsystemRunner runner(dummyOpt);
            
            runner.maybeRun (unit);
            bool emergency = runner.wait();
            
            ASSERT (emergency);      // emergency state got propagated
            ASSERT (!unit.isRunning());
            ASSERT (unit.didRun());
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
            ASSERT (unit1.isRunning());
            ASSERT (unit2.isRunning());
            ASSERT (unit3.isRunning());
            ASSERT (unit4.isRunning());
            
            bool emergency = runner.wait();
            
            ASSERT (!emergency);
            ASSERT (!unit1.isRunning());
            ASSERT (!unit2.isRunning());
            ASSERT (!unit3.isRunning());
            ASSERT (!unit4.isRunning());
            ASSERT (unit1.didRun());
            ASSERT (unit2.didRun());
            ASSERT (unit3.didRun());
            ASSERT (unit4.didRun());
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
            
            try 
              { 
                runner.maybeRun (unit4);
                NOTREACHED;
              }
            catch (lumiera::Error&)
              {
                ASSERT (lumiera_error() == error::LUMIERA_ERROR_LOGIC); // failure to bring up prerequisites is detected
              }
            ASSERT ( unit1.isRunning());
            ASSERT ( unit2.isRunning());
            ASSERT (!unit3.isRunning());
            // shutdown has been triggered for unit4, but may require some time
            
            bool emergency = runner.wait();
            
            ASSERT (!emergency);     // no problems with the subsystems actually running...
            ASSERT (!unit1.isRunning());
            ASSERT (!unit2.isRunning());
            ASSERT (!unit3.isRunning());
            ASSERT (!unit4.isRunning());
            ASSERT ( unit1.didRun());
            ASSERT ( unit2.didRun());
            ASSERT (!unit3.didRun());
            // can't say for sure if unit4 actually did run
          }
      };
    
    
    
    /** Register this test class... */
    LAUNCHER (SubsystemRunner_test, "function common");
    
    
    
  } // namespace test

} // namespace lumiera
