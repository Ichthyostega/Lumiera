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
#include "lib/thread-wrapper.hpp"
#include "lib/error.hpp"
#include "lib/query.hpp"
#include "lib/util.hpp"
#include "lib/sync.hpp"

#include <tr1/functional>
#include <iostream>

using std::cout;
using std::tr1::bind;
using std::tr1::placeholders::_1;
using util::isnil;
using test::Test;
using lib::Sync;
using lib::RecursiveLock_Waitable;


namespace lumiera {
  namespace test  {
  
    namespace { // private test classes and data...
    
      using query::extractID;
      
      /** limit for the randomly selected duration of
       *  subsystem's running phase (milliseconds) */
      const uint MAX_RUNNING_TIME_ms = 50;
      
      /** the "running" subsystem checks for a
       *  shutdown request every XX milliseconds */ 
      const uint TICK_DURATION_ms = 5;
      
      /** dummy options just to be ignored */
      util::Cmdline dummyArgs ("");
      lumiera::Option dummyOpt (dummyArgs);
      
      
      
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
          
          bool isUp_;
          bool didRun_;
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
              Lock guard (this);
              if (isUp_) return false;  // already started
              
              Literal startSpec (extractID ("start",spec_));
              ASSERT ("false"!=startSpec);
              ASSERT (!isnil (startSpec));
              
              if ("true"==startSpec) //----simulate successful subsystem start
                { 
                  Thread (id_, bind (&MockSys::run, this, termination));            /////TODO: the thread description should be rather "Test: "+string(*this), but this requires to implement the class Literal as planned
                  isUp_ = true;
                }
              else
              if ("throw"==startSpec) //---starting flounders
                throw error::Fatal("simulated failure to start the subsystem");
              
              return isUp_;
            }
          
          void
          triggerShutdown ()  throw()
            {
              termRequest_ = true;
              cout << *this << ": triggerShutdown()\n";
            }
          
          bool 
          checkRunningState ()  throw()
            {
              Lock guard (this);
              return isUp_;
            }
          
          
          void
          run (Subsys::SigTerm termination)  ///< simulates a "running" subsystem
            {
              cout << *this << ": start running...\n";
              
              Literal runSpec (extractID ("run",spec_));
              ASSERT (!isnil (runSpec));
              
              if ("false"!=runSpec) //----actually hold running state for some time
                {
                  didRun_ = true;
                  running_duration_ = (rand() % MAX_RUNNING_TIME_ms);
                  
                  Lock wait_blocking (this, &MockSys::tick);
                }
              
              Error problemIndicator("simulated Problem killing a subsystem");
              lumiera_error();   //  reset error state....
                                //   Note: in real life this actually
                               //    would be an catched exception! 
              
              Lock guard (this);
              isUp_ = false;
              
              termination ("true"==runSpec? 0 : &problemIndicator);
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
          
          ~MockSys() { cout << *this << " dead.\n"; }
          
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
          }
        
        
        void
        singleSubsys_complete_cycle()
          {
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
      };
    
      
    
    /** Register this test class... */
    LAUNCHER (SubsystemRunner_test, "function common");
    
    
    
  } // namespace test

} // namespace lumiera
