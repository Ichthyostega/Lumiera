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
        }
      
      
      void
      verifyCheckpoint()
        {
          UNIMPLEMENTED("verify the semantics of reaching a checkpoint");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DispatcherLooper_test, "unit controller");
  
  
}}} // namespace proc::control::test
