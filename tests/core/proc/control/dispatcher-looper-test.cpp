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
//#include "proc/control/command.hpp"
//#include "proc/control/command-registry.hpp"
#include "lib/format-cout.hpp"
//#include "lib/test/event-log.hpp"

//#include "proc/control/test-dummy-commands.hpp"

//#include <cstdlib>


namespace proc {
namespace control {
namespace test    {
  
  
//  using std::function;
//  using std::rand;
  
  
  namespace { // test fixture...
    
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
  
//  typedef shared_ptr<CommandImpl> PCommandImpl;
//  typedef HandlingPattern const& HaPatt;
  
  
  
  
  
  
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
          verifyBuidlerStart();
          verifyCheckpoint();
        }
      
      
      void
      verifyBasics()
        {
          Setup setup;
          Looper looper = setup.install();
          
          CHECK (not looper.isDying());
          CHECK (looper.shallLoop());
          CHECK (not looper.needBuild());
          
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
      verifyBuidlerStart()
        {
          UNIMPLEMENTED("verify the logic when builder gets triggered");
          Setup setup;
          Looper looper = setup.install();
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.needBuild());
          CHECK (    looper.isIdle());
          
          setup.has_commands_in_queue = true;      // regular command processing
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (    looper.needBuild());
          CHECK (not looper.isIdle());
          
          setup.has_commands_in_queue = false;     // done with the commands thus far
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (    looper.needBuild());          // ...note: still needs build
          CHECK (    looper.isIdle());
          ///////////////////////////////TODO verify the timeout is the short timeout
          
          ///////////////////////////////TODO how to communicate build dirty state??  // set NOT DIRTY
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.needBuild());          // ...note: now done with building
          CHECK (    looper.isIdle());
          ///////////////////////////////TODO verify the timeout is now the *extended* timeout
          
          setup.has_commands_in_queue = true;      // next command pending
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (    looper.needBuild());
          CHECK (not looper.isIdle());
          ///////////////////////////////TODO verify the timeout is back to the short period
          
          looper.enableProcessing(false);          // disable processing
          
          CHECK (not looper.requireAction());
          CHECK (    looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.needBuild());          // ...note: dirty state hidden by disabled state
          CHECK (not looper.isIdle());
          ///////////////////////////////TODO verify the timeout is *completely disabled*
          
          looper.enableProcessing(true);           // enable back
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (    looper.needBuild());          // ...note: dirty state revealed again
          CHECK (not looper.isIdle());
          ///////////////////////////////TODO verify the timeout is back to the short period
          
          setup.has_commands_in_queue = false;     // done with the commands
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (    looper.needBuild());          // ...note: still needs build
          CHECK (    looper.isIdle());
          
          looper.enableProcessing(false);          // disable processing
          
          CHECK (not looper.requireAction());
          CHECK (    looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.needBuild());          // ...note: dirty state hidden
          CHECK (not looper.isIdle());
          ///////////////////////////////TODO verify the timeout is *completely disabled*
          
          looper.enableProcessing(true);           // enable back
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (    looper.needBuild());          // ...note: dirty state revealed
          CHECK (    looper.isIdle());
          ///////////////////////////////TODO verify the timeout is back to the short period
          
          looper.enableProcessing(false);          // disable processing
          ///////////////////////////////TODO how to communicate build dirty state??  // set NOT DIRTY
                                                   // let's assume builder was running and is now finished
          
          CHECK (not looper.requireAction());
          CHECK (    looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.needBuild());          // ...note: dirty state not obvious
          CHECK (not looper.isIdle());
          ///////////////////////////////TODO verify the timeout is *completely disabled*
          
          looper.enableProcessing(true);           // enable back
          
          CHECK (not looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.needBuild());          // ...note: but now it becomes clear builder is not dirty
          CHECK (    looper.isIdle());
          ///////////////////////////////TODO verify the timeout is now the *extended* timeout
          
          setup.has_commands_in_queue = true;      // more commands again
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (    looper.isWorking());
          CHECK (    looper.needBuild());
          CHECK (not looper.isIdle());
          
          looper.triggerShutdown();                // request shutdown...
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.needBuild());          // ...note: no need to care for builder anymore
          CHECK (not looper.isIdle());
          ///////////////////////////////TODO verify the timeout *remains* at the short period (due to shutdown)
          
          setup.has_commands_in_queue = false;     // and even when done all commands
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.needBuild());          // ...note: still no need for builder run, since in shutdown
          CHECK (not looper.isIdle());
          ///////////////////////////////TODO verify the timeout *remains* at the short period (due to shutdown)
          
          ///////////////////////////////TODO how to communicate build dirty state??  // set NOT DIRTY
          
          CHECK (    looper.requireAction());
          CHECK (not looper.isDisabled());
          CHECK (not looper.isWorking());
          CHECK (not looper.needBuild());
          CHECK (not looper.isIdle());
          ///////////////////////////////TODO verify the timeout *remains* at the short period (due to shutdown)
        }
      
      
      void
      verifyCheckpoint()
        {
          UNIMPLEMENTED("verify the semantics of reaching a checkpoint");
                                                        //////////////////////////TODO a checkpoint is reached when no state change is pending
                                                        //////////////////////////TODO It is not clear how this relates to triggering the Builder,
                                                        //////////////////////////TODO since the initial idea was to trigger the Builder at checkpoints,
                                                        /// but now it looks like we don't need this distinction anymore
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DispatcherLooper_test, "unit controller");
  
  
}}} // namespace proc::control::test
