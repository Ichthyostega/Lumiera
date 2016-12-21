/*
  DispatcherLooper(Test)  -  verify loop control and timing functionality of ProcDispatcher

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "lib/test/run.hpp"
#include "proc/control/looper.hpp"
#include "lib/format-cout.hpp"

#include <chrono>
#include <thread>


namespace proc {
namespace control {
namespace test    {
  
  using std::this_thread::sleep_for;
  using namespace std::chrono_literals;
  
  
  namespace { // test fixture...
    
    /**
     * @todo this value should be retrieved from configuration                  ////////////////////////////////TICKET #1052 : access application configuration
     * @see Looper::establishWakeTimeout()
     */
    const uint EXPECTED_BUILDER_DELAY_ms = 50;
    
    bool
    isFast (uint timeoutDelay_ms)
      {
        return timeoutDelay_ms < 1.2 * EXPECTED_BUILDER_DELAY_ms
           and 0 < timeoutDelay_ms;
      }
    
    bool
    isSlow (uint timeoutDelay_ms)
      {
        return timeoutDelay_ms >= 1.2 * EXPECTED_BUILDER_DELAY_ms;
      }
    
    bool
    isDisabled (uint timeoutDelay_ms)
      {
        return 0 == timeoutDelay_ms;
      }
    
    
    /**
     * Setup for testing the Looper.
     * In operation, the Looper receives its input by invoking closures.
     * With the help of this adapter, unit tests may just set values
     * on the Setup record and verify the result on the wired Looper
     */
    struct Setup
      {
        bool has_commands_in_queue = false;
        
        Looper
        install()
          {
            return Looper([&](){ return has_commands_in_queue; });
          }
      };
    
  }//(End) test fixture
  
  
  
  
  
  
  
  /******************************************************************************//**
   * @test verify encapsulated control logic of ProcDispatcher.
   *       - fusing of conditions for the pthread waiting condition
   *       - detection and handling of work states
   *       - management of builder run triggers
   * 
   * @see proc::control::Looper
   * @see DispatcherLoop
   * @see CommandQueue_test
   */
  class DispatcherLooper_test : public Test
    {
      
      
      virtual void
      run (Arg)
        {
          verifyBasics();
          verifyShutdown();
          verifyWakeupActivity();
          verifyShutdown_stops_processing();
          verifyDisabling_stops_processing();
          verifyBuilderStart();
        }
      
      
      void
      verifyBasics()
        {
          Setup setup;
          Looper looper = setup.install();
          
          CHECK (not looper.isDying());
          CHECK (looper.shallLoop());
          CHECK (not looper.runBuild());
          
          uint timeout = looper.getTimeout();
          CHECK (10 < timeout,  "configured idle timeout %2u to short", timeout);
          CHECK (timeout < 500, "configured idle timeout %3u to long",  timeout);
        }
      
      
      void
      verifyShutdown()
        {
          Setup setup;
          Looper looper = setup.install();
          
          CHECK (not looper.isDying());
          CHECK (looper.shallLoop());
          
          looper.triggerShutdown();
          CHECK (looper.isDying());
          CHECK (not looper.shallLoop());
        }
      
      
      void
      verifyWakeupActivity()
        {
          Setup setup;
          Looper looper = setup.install();
          
          CHECK (not looper.isDying());
          CHECK (looper.shallLoop());
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isWorking());
          CHECK (    looper.isIdle());
          
          setup.has_commands_in_queue = true;
          
          CHECK (    looper.requireAction());
          CHECK (    looper.isWorking());
          CHECK (not looper.isIdle());
          CHECK (looper.shallLoop());
          
          setup.has_commands_in_queue = false;
          looper.markStateProcessed();
          looper.markStateProcessed();
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isWorking());
          CHECK (    looper.isIdle());
          CHECK (looper.shallLoop());
        }
      
      
      void
      verifyShutdown_stops_processing()
        {
          Setup setup;
          Looper looper = setup.install();
          
          CHECK (not looper.isDying());
          CHECK (looper.shallLoop());
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isWorking());
          CHECK (    looper.isIdle());
          
          setup.has_commands_in_queue = true;
          
          CHECK (    looper.requireAction());
          CHECK (    looper.isWorking());
          CHECK (not looper.isIdle());
          CHECK (    looper.shallLoop());
          CHECK (not looper.isDying());
          
          looper.triggerShutdown();
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isWorking());
          CHECK (not looper.isIdle());
          CHECK (not looper.shallLoop());
          CHECK (    looper.isDying());
          
          setup.has_commands_in_queue = false;
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isWorking());
          CHECK (not looper.isIdle());
          CHECK (not looper.shallLoop());
          CHECK (    looper.isDying());
          
          setup.has_commands_in_queue = true;
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isWorking());
          CHECK (not looper.isIdle());
          CHECK (not looper.shallLoop());
          CHECK (    looper.isDying());
        }
      
      
      void
      verifyDisabling_stops_processing()
        {
          Setup setup;
          Looper looper = setup.install();
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (    looper.isIdle());
          CHECK (    looper.shallLoop());
          CHECK (not looper.isDying());
          
          setup.has_commands_in_queue = true;      // normal operation: pending commands will be processed
          
          CHECK (    looper.requireAction());      // ..causes wake-up
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (not looper.isIdle());
          CHECK (    looper.shallLoop());
          CHECK (not looper.isDying());
          
          looper.enableProcessing(false);          // disable processing
          
          CHECK (not looper.requireAction());
          CHECK (    looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.isIdle());
          CHECK (    looper.shallLoop());
          CHECK (not looper.isDying());
          
          setup.has_commands_in_queue = false;     // while disabled, state of the command queue has no effect
          
          CHECK (not looper.requireAction());
          CHECK (    looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.isIdle());
          CHECK (    looper.shallLoop());
          CHECK (not looper.isDying());
          
          setup.has_commands_in_queue = true;
          
          CHECK (not looper.requireAction());
          CHECK (    looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.isIdle());
          CHECK (    looper.shallLoop());
          CHECK (not looper.isDying());
          
          looper.enableProcessing();               // resume normal operation
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (not looper.isIdle());
          CHECK (    looper.shallLoop());
          CHECK (not looper.isDying());
          
          looper.enableProcessing(false);          // disable again
          
          CHECK (not looper.requireAction());
          CHECK (    looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.isIdle());
          CHECK (    looper.shallLoop());
          CHECK (not looper.isDying());
          
          looper.triggerShutdown();                // wake-up for shutdown even from disabled state
          
          CHECK (    looper.requireAction());
          CHECK (    looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.isIdle());
          CHECK (not looper.shallLoop());
          CHECK (    looper.isDying());
        }
      
      
      void
      verifyBuilderStart()
        {
          Setup setup;
          Looper looper = setup.install();
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.runBuild());
          CHECK (    looper.isIdle());
          
          setup.has_commands_in_queue = true;      // regular command processing
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (not looper.runBuild());
          CHECK (not looper.isIdle());
          
          looper.markStateProcessed();             // at least one command has been handled
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (not looper.runBuild());           // ...note: build not yet triggered
          CHECK (not looper.isIdle());
          
          CHECK (isFast (looper.getTimeout()));
          
          
          looper.markStateProcessed();             // next processing round: further command(s) processed,
                                                   // yet still more commands pending...
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (not looper.runBuild());           // ...build still postponed
          CHECK (not looper.isIdle());
          
          sleep_for (1200ms);
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (    looper.runBuild());           // ...after some time of command processing, a build run is forced
          CHECK (not looper.isIdle());
          
          looper.markStateProcessed();             // and when the builder run is confirmed...
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (not looper.runBuild());           // ...we are back to normal working state (build postponed)
          CHECK (not looper.isIdle());
          
          
          setup.has_commands_in_queue = false;     // now emptied our queue
          looper.markStateProcessed();             // at least one further command has been handled
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (    looper.runBuild());           // ...note: now build will be triggered
          CHECK (not looper.isIdle());
          
          CHECK (isFast (looper.getTimeout()));    // ...but only after a short wait period,
                                                   //    since not looper.requireAction()
          
          
          looper.markStateProcessed();             // next processing round: invoked builder,
                                                   // and no more commands commands pending...
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.runBuild());           // ...note: now done with building
          CHECK (    looper.isIdle());
          
          CHECK (isSlow (looper.getTimeout()));    // ...now Dispatcher is idle and goes to sleep
          
          
          setup.has_commands_in_queue = true;      // next command pending
          
          CHECK (    looper.requireAction());      // return to work mode
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (not looper.runBuild());
          CHECK (not looper.isIdle());
          
          setup.has_commands_in_queue = false;     // now let's assume command has been processed
          looper.markStateProcessed();             // and queue is empty again
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (    looper.runBuild());
          CHECK (not looper.isIdle());
          
          CHECK (isFast (looper.getTimeout()));    // now build *would* be triggered after short timeout, but..
          
          
          looper.enableProcessing(false);          // disable processing
          
          CHECK (not looper.requireAction());
          CHECK (    looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.runBuild());          // ...note: dirty state hidden by disabled state
          CHECK (not looper.isIdle());
          
          CHECK (isDisabled (looper.getTimeout()));
          
          
          looper.enableProcessing(true);           // enable back
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (    looper.runBuild());          // ...note: dirty state revealed again
          CHECK (not looper.isIdle());
          
          CHECK (isFast (looper.getTimeout()));
          
          looper.enableProcessing(false);          // disable processing
          looper.markStateProcessed();             // let's assume builder was running and is now finished
          
          CHECK (not looper.requireAction());
          CHECK (    looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.runBuild());          // ...note: dirty state not obvious
          CHECK (not looper.isIdle());
          
          CHECK (isDisabled (looper.getTimeout()));
          
          
          looper.enableProcessing(true);           // enable back
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.runBuild());          // ...note: but now it becomes clear builder is not dirty
          CHECK (    looper.isIdle());
          
          CHECK (isSlow (looper.getTimeout()));
          
          
          setup.has_commands_in_queue = true;      // more commands again
          looper.markStateProcessed();             // ...and let's assume one command has already been processed
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (not looper.runBuild());
          CHECK (not looper.isIdle());
          
          looper.triggerShutdown();                // request shutdown...
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.runBuild());
          CHECK (not looper.isIdle());
          
          CHECK (isFast (looper.getTimeout()));
          
          
          setup.has_commands_in_queue = false;     // and even when done with all commands...
          looper.markStateProcessed();
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.runBuild());          // ...note: still no need for builder run, since in shutdown
          CHECK (not looper.isIdle());
          
          CHECK (isFast (looper.getTimeout()));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DispatcherLooper_test, "unit controller");
  
  
}}} // namespace proc::control::test
