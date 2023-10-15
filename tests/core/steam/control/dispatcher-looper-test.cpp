/*
  DispatcherLooper(Test)  -  verify loop control and timing functionality of SteamDispatcher

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
#include "steam/control/looper.hpp"

#include <chrono>
#include <thread>


namespace steam {
namespace control {
namespace test    {
  
  using std::this_thread::sleep_for;
  using namespace std::chrono_literals;
  
  
  namespace { // test fixture...
    
    using Dur = std::chrono::duration<double, std::milli>;
    
    /**
     * @todo this value should be retrieved from configuration                  ////////////////////////////////TICKET #1052 : access application configuration
     * @see Looper::establishWakeTimeout()
     */
    const uint EXPECTED_BUILDER_DELAY_ms = 50;
    
    bool
    isFast (milliseconds timeoutDelay)
      {
        return timeoutDelay < Dur{1.2 * EXPECTED_BUILDER_DELAY_ms}
           and 0ms < timeoutDelay;
      }
    
    bool
    isSlow (milliseconds timeoutDelay)
      {
        return timeoutDelay >= Dur{1.2 * EXPECTED_BUILDER_DELAY_ms};
      }
    
    bool
    isDisabled (milliseconds timeoutDelay)
      {
        return 0ms == timeoutDelay;
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
   * @test verify encapsulated control logic of SteamDispatcher.
   *       - fusing of conditions for the pthread waiting condition
   *       - detection and handling of work states
   *       - management of builder run triggers
   * 
   * @see steam::control::Looper
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
          CHECK (isDisabled (looper.getTimeout()));
          
          setup.has_commands_in_queue = true;
          CHECK (looper.requireAction());
          
          milliseconds timeout = looper.getTimeout();
          CHECK (10ms < timeout,  "configured idle timeout %2l to short", timeout.count());
          CHECK (timeout < 800ms, "configured idle timeout %3l to long",  timeout.count());
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
          looper.markStateProcessed();         // after command processing
          CHECK (not looper.requireAction());  // stops immediate work state
          CHECK (    looper.useTimeout());     // but still performs timeout
          CHECK (not looper.isWorking());
          CHECK (not looper.isIdle());         // still need to run the builder
          
          looper.markStateProcessed();         // second round-trip, after builder run
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isWorking());
          CHECK (    looper.isIdle());
          CHECK (looper.shallLoop());
          
          looper.triggerShutdown();
          
          CHECK (not looper.shallLoop());
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isWorking());
          CHECK (not looper.isIdle());
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
      
      
      /** @test logic to trigger the builder over a complete simulated lifecycle.
       *        - when Looper::requireAction is true, the thread does not go into wait state
       *        - in the loop body
       *          * either when `runBuild()` is true, the builder run is triggered
       *          * or when `isWorking()` is true, the next command is processed
       *        - after that, Looper::markStateProcessed proceeds the state machine
       *  @note this test actually has to sleep in order to verify triggering a timeout
       * 
       */
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
          
          CHECK (isSlow (looper.getTimeout()));
          
          
          looper.markStateProcessed();             // next processing round: further command(s) processed,
                                                   // yet still more commands pending...
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (not looper.runBuild());           // ...build still postponed
          CHECK (not looper.isIdle());
          
          sleep_for (800ms);                       // let's assume we did command processing for a long time...
          
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
          
          CHECK (isDisabled(looper.getTimeout())); // ...now Dispatcher is idle and goes to sleep
          
          
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
          CHECK (not looper.runBuild());           // ...note: dirty state hidden by disabled state
          CHECK (not looper.isIdle());
          
          CHECK (isDisabled (looper.getTimeout()));
          
          
          looper.enableProcessing(true);           // enable back
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (    looper.runBuild());           // ...note: dirty state revealed again
          CHECK (not looper.isIdle());
          
          CHECK (isFast (looper.getTimeout()));
          
          looper.enableProcessing(false);          // disable processing
          looper.markStateProcessed();             // let's assume builder was running and is now finished
          
          CHECK (not looper.requireAction());
          CHECK (    looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.runBuild());           // ...note: dirty state not obvious
          CHECK (not looper.isIdle());
          
          CHECK (isDisabled (looper.getTimeout()));
          
          
          looper.enableProcessing(true);           // enable back
                                                   // NOTE special twist: it's unclear, if builder was triggered before the disabled state...
          CHECK (isFast (looper.getTimeout()));    //                     ...and thus we remain in dirty state
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (    looper.runBuild());           // so the builder will be triggered (possibly a second time) after a short timeout
          CHECK (not looper.isIdle());
          
          looper.markStateProcessed();             // and after one round-trip the builder was running and is now finished
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.runBuild());
          CHECK (    looper.isIdle());             // ...system is in idle state now and waits until triggered externally
          
          CHECK (isDisabled (looper.getTimeout()));
          
          
          setup.has_commands_in_queue = true;      // more commands again -> wake up
          looper.markStateProcessed();             // ...and let's assume one command has already been processed
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (not looper.runBuild());
          CHECK (not looper.isIdle());
          
          looper.triggerShutdown();                // request shutdown...
          
          CHECK (    looper.requireAction());
          CHECK (    looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.runBuild());
          CHECK (not looper.isIdle());
          
          CHECK (isDisabled (looper.getTimeout()));
          
          
          setup.has_commands_in_queue = false;     // and even when done with all commands...
          looper.markStateProcessed();
          
          CHECK (isDisabled (looper.getTimeout()));
          CHECK (not looper.shallLoop());          // we remain disabled and break out of the loop
          
          CHECK (    looper.requireAction());
          CHECK (    looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.runBuild());           // ...note: still no need for builder run, since in shutdown
          CHECK (not looper.isIdle());
          
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DispatcherLooper_test, "unit controller");
  
  
}}} // namespace steam::control::test
