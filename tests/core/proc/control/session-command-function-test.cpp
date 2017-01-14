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
#include "gui/interact/invocation-trail.hpp"
#include "backend/thread-wrapper.hpp"
#include "lib/typed-counter.hpp"
//#include "lib/format-cout.hpp" //////////TODO
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <vector>
#include <string>


namespace proc {
namespace control {
namespace test    {
  
  
//  using std::function;
//  using std::rand;
  using boost::lexical_cast;
  using lib::test::randTime;
  using gui::interact::InvocationTrail;
  using gui::ctrl::CommandHandler;
  using lib::diff::GenNode;
  using lib::diff::Rec;
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::Duration;
  using lib::time::Offset;
  using lib::time::FSecs;
  using lib::FamilyMember;
  using lib::Symbol;
  using util::isnil;
  using std::string;
  using std::vector;
  
  
  namespace { // test fixture...
    
    /* === parameters for multi-threaded stress test === */
    
    uint NUM_THREADS_DEFAULT = 20;   ///< @note _not_ const, can be overridden by command line argument
    uint NUM_INVOC_PER_THRED = 10;
    uint MAX_RAND_DELAY_ms   = 10;
    
    void
    maybeOverride (uint& configSetting, Arg cmdline, uint paramNr)
    {
      if (paramNr < cmdline.size())
        configSetting = lexical_cast<uint>(cmdline[paramNr]);
    }
    
    
    /* === mock operation to be dispatched as command === */
    
    const Symbol COMMAND_ID{"test.dispatch.function.command"};
    const Symbol COMMAND_I1{"test.dispatch.function.command.instance-1"};
    const Symbol COMMAND_I2{"test.dispatch.function.command.instance-2"};
    
    TimeVar testCommandState = randTime();
    
    void
    operate (Duration dur, Offset offset, int factor)
    {
      testCommandState += Offset(dur) + offset*factor;
    }
    
    Time
    capture (Duration, Offset, int)
    {
      return testCommandState;
    }
    
    void
    undoIt (Duration, Offset, int, Time oldState)
    {
      testCommandState = oldState;
    }
    
    
  }//(End) test fixture
  
  
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
          Command(COMMAND_ID).storeDef(COMMAND_I1);
          Command(COMMAND_ID).storeDef(COMMAND_I2);
        }
     ~SessionCommandFunction_test()
        {
          Command::remove (COMMAND_ID);
          Command::remove (COMMAND_I1);
          Command::remove (COMMAND_I2);
        }
      //-------------(End)FIXTURE
      
     
      virtual void
      run (Arg args_for_stresstest)
        {
          lumiera_interfaceregistry_init();
          lumiera::throwOnError();
          
          startDispatcher();
          perform_simpleInvocation();
          perform_messageInvocation();
          perform_massivelyParallel(args_for_stresstest);
          stopDispatcher();
          
          lumiera_interfaceregistry_destroy();
        }
      
      
      /** @test start the session loop thread,
       *        similar to what the »session subsystem« does
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
          string cmdID {COMMAND_I1};
          Rec arguments {Duration(15,10), Time(500,0), -1};
          
          CHECK (not Command(COMMAND_I1).canExec());
          SessionCommand::facade().bindArg (cmdID, arguments);
          CHECK (Command(COMMAND_I1).canExec());
          
          
          Time prevState = testCommandState;
          SessionCommand::facade().invoke(cmdID);
          
          __DELAY__
          CHECK (testCommandState - prevState == Time(0, 1));           // execution added 1500ms -1*500ms == 1sec
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
          // this happens "somewhere" in the UI interaction control framework
          InvocationTrail invoTrail{Command(COMMAND_I2)};
          
          // this happens within some tangible UI element (widget / controller)
          GenNode argumentBindingMessage = invoTrail.bindMsg (Rec {Duration(25,10), Time(500,0), -2});
          GenNode commandTriggerMessage  = invoTrail.bangMsg ();
          CHECK (argumentBindingMessage.idi.getSym() == string{COMMAND_I2});
          CHECK (commandTriggerMessage.idi.getSym() == string{COMMAND_I2});
          CHECK (not Command::canExec(COMMAND_I2));
          
          // this happens, when CoreService receives command messages from UI-Bus
          CommandHandler handler1{argumentBindingMessage};
          argumentBindingMessage.data.accept(handler1);                 // handler is a visitor for the message payload
          
          CHECK (Command::canExec(COMMAND_I2));
          CHECK (not Command::canUndo(COMMAND_I2));
          Time prevState = testCommandState;
          
          // now handling the message to trigger execution
          CommandHandler handler2{commandTriggerMessage};
          commandTriggerMessage.data.accept(handler2);
          
          __DELAY__
          CHECK (Command::canUndo(COMMAND_I2));
          CHECK (testCommandState - prevState == Time(500, 1));         // execution added 2500ms -2*500ms == 1.5sec
        }
      
      
      /** @test verify that commands are properly enqueued
       *        and executed one by one
       *        - create several threads to send random command messages
       *        - verify that, after executing all commands, the internal
       *          state variable reflects the result of a proper
       *          sequential calculation and summation
       */
      void
      perform_massivelyParallel(Arg args_for_stresstest)
        {
          maybeOverride(NUM_THREADS_DEFAULT, args_for_stresstest, 1);
          maybeOverride(NUM_INVOC_PER_THRED, args_for_stresstest, 2);
          maybeOverride(MAX_RAND_DELAY_ms,   args_for_stresstest, 3);
          
          class InvocationProducer
            : backend::ThreadJoinable
            {
              FamilyMember<InvocationProducer> id_;
              string id_buffer_{COMMAND_ID + ".thread-"+id_};
              Symbol cmdID_{cStr(id_buffer_)};
              
              void
              fabricateCommands()
                {
                  syncPoint();
                  InvocationTrail invoTrail{Command(cmdID_)};
                  
                  for (uint i=0; i<NUM_INVOC_PER_THRED; ++i)
                    {
                      __randomDelay();
                      sendCommandMessage (invoTrail.bindMsg (Rec {Duration(7*id_, 2), Time(500,0), int(-i)}));
                      
                      __randomDelay();
                      sendCommandMessage (invoTrail.bangMsg());
                    }
                }
              
              static void
              sendCommandMessage(GenNode msg)
                {
                  CommandHandler handler{msg};
                  msg.data.accept(handler);
                }
              
              static void
              __randomDelay()
                {
                  usleep (1000L * (1 + rand() % MAX_RAND_DELAY_ms));
                }
              
            public:
              InvocationProducer()
                : ThreadJoinable("test command producer", [&](){ fabricateCommands(); })
                {
                  Command(COMMAND_ID).storeDef(cmdID_);
                  this->sync();
                }
              
             ~InvocationProducer()
                {
                  this->join().maybeThrow();
                  Command::remove (cmdID_);
                }
            };
            
          Time prevState = testCommandState;
          
          // fire up several threads to issue commands in parallel...
          vector<InvocationProducer> producerThreads{NUM_THREADS_DEFAULT};
          
          
          FSecs expectedOffset{0};
          for (uint i=0; i<NUM_THREADS_DEFAULT; ++i)
            for (uint j=0; j<NUM_INVOC_PER_THRED; ++j)
              expectedOffset += FSecs(i*7,2) - FSecs(j,2);
          
          while (not ProcDispatcher::instance().empty());
          
          CHECK (testCommandState - prevState == Time(expectedOffset));
          
        }// Note: leaving this scope blocks for joining all producer threads
    };
  
  
  /** Register this test class... */
  LAUNCHER (SessionCommandFunction_test, "function controller");
  
  
}}} // namespace proc::control::test
