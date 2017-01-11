/*
  SessionCommandFunction(Test)  -  function test of command dispatch via SessionCommand facade

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/test/test-helper.hpp"
extern "C" {
#include "common/interfaceregistry.h"
}

#include "proc/control/proc-dispatcher.hpp"
#include "proc/control/command-def.hpp"
#include "gui/ctrl/command-handler.hpp"
#include "proc/asset/meta/time-grid.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time/timecode.hpp"
#include "lib/format-obj.hpp"
#include "lib/format-cout.hpp" //////////TODO
#include "lib/symbol.hpp"
#include "lib/util.hpp"

//#include <cstdlib>
#include <string>


namespace proc {
namespace control {
namespace test    {
  
  
//  using std::function;
//  using std::rand;
  using lib::test::randTime;
  using gui::ctrl::CommandHandler;
  using lib::diff::Rec;
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::Duration;
  using lib::time::Offset;
  using lib::time::FSecs;
  using lib::Symbol;
  using util::isnil;
  using util::toString;
  using std::string;
  
  
  namespace { // test fixture...
    
    /* === mock operation to be dispatched as command === */
    
    const Symbol COMMAND_ID{"test.dispatch.function.command"};
    
    TimeVar testCommandState = randTime();
    
    void
    operate (Duration dur, Offset offset, int factor)
    {
      testCommandState += dur + offset*factor;
    }
    
    string
    capture (Duration, Offset, int)
    {
      return testCommandState;
    }
    
    void
    undoIt (Duration, Offset, int, string oldState)
    {
//    dummyState = oldState;
    }
    
    
  }//(End) test fixture
  
//  typedef shared_ptr<CommandImpl> PCommandImpl;
//  typedef HandlingPattern const& HaPatt;
#define __DELAY__ usleep(10000);
  
  
  
  
  
  /******************************************************************************************//**
   * @test verify integrated functionality of command dispatch through the SessionCommand facade.
   *       - operate lifecycle of the supporting components,
   *         similar to activating the »session subsystem«
   *       - generate command messages similar to what is received from the UI-Bus
   *       - us the handler mechanism from gui::ctrl::CoreService to talk to the facade
   *       - have a specially rigged command function to observe invocation
   *       - wait for the session loop thread to dispatch this command
   *       - verify that commands are really executed single-threaded
   * 
   * @see proc::SessionSubsystem
   * @see ProcDispatcher
   * @see CommandQueue_test
   * @see AbstractTangible_test::invokeCommand()
   */
  class SessionCommandFunction_test : public Test
    {
      
      //------------------FIXTURE
    public:
      SessionCommandFunction_test()
        {
          CommandDef (COMMAND_ID)
               .operation (operate)
               .captureUndo (capture)
               .undoOperation (undoIt)
               ;
        }
     ~SessionCommandFunction_test()
        {
          Command::remove (COMMAND_ID);
        }
      //-------------(End)FIXTURE
      
     
      virtual void
      run (Arg)
        {
          lumiera_interfaceregistry_init();
          lumiera::throwOnError();
          
          startDispatcher();
          perform_simpleInvocation();
//        perform_messageInvocation();
//        perform_massivelyParallel();
          stopDispatcher();
          
          lumiera_interfaceregistry_destroy();
        }
      
      
      /** @test start the session loop thread, similar
       *        to what the »session subsystem« does
       *  @note we are _not_ actually starting the subsystem
       *  @see facade.cpp
       */
      void
      startDispatcher()
        {
          CHECK (not ProcDispatcher::instance().isRunning());
          
          ProcDispatcher::instance().start ([&] (string* problemMessage)
                                                {
                                                  CHECK (isnil (*problemMessage));
                                                  thread_has_ended = true;
                                                });
          
          CHECK (ProcDispatcher::instance().isRunning());
          CHECK (not thread_has_ended);
        }
      bool thread_has_ended{false};
      
      
      void
      stopDispatcher()
        {
          CHECK (ProcDispatcher::instance().isRunning());
          ProcDispatcher::instance().requestStop();
          
          __DELAY__
          CHECK (not ProcDispatcher::instance().isRunning());
          CHECK (thread_has_ended);
        }
      
      
      void
      perform_simpleInvocation()
        {
          string cmdID {COMMAND_ID};
          Rec arguments {Duration(15,10), Time(500,0), -1};
          
          CHECK (not Command(COMMAND_ID).canExec());
          SessionCommand::facade().bindArg (cmdID, arguments);
          CHECK (Command(COMMAND_ID).canExec());
          
          cout << "prevState="<<testCommandState<<endl;
          
          Time prevState = testCommandState;
          SessionCommand::facade().invoke(cmdID);
          
          __DELAY__
          cout << "postState="<<testCommandState<<endl;
          CHECK (testCommandState - prevState == Time(0, 1));
        }
      
      
      /** @test invoke a command in the same way as CoreService does
       *        when handling command messages from the UI-Bus
       *        - use the help of an InvocationTrail, similar to what the
       *          [generic UI element](\ref gui::model::Tangible) does 
       *        - generate a argument binding message
       *        - generate a "bang!" message
       */
      void
      perform_messageInvocation()
        {
          UNIMPLEMENTED ("invoke via message");
        }
      
      
      /** @test verify that commands are properly enqueued
       *        and executed one by one
       *        - create several threads to send random command messages
       *        - verify that, after executing all commands, the internal
       *          state variable reflects the result of a proper
       *          sequential calculation and summation
       */
      void
      perform_massivelyParallel()
        {
          UNIMPLEMENTED ("verify sequentialisation by queue");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SessionCommandFunction_test, "function controller");
  
  
}}} // namespace proc::control::test
