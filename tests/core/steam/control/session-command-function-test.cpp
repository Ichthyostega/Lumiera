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


/** @file session-command-function-test.cpp
 ** Function(integration) test of command dispatch into session thread.
 ** This is a test combining several components to operate similar as in the real application,
 ** while still relying upon an unit-test like setup. The goal is to cover how _session commands_
 ** are issued from an access point (CoreService) in the UI backbone, passed on through an
 ** abstraction interface (the SessionCommand facade), handed over to the SteamDispatcher,
 ** which, running within a dedicated thread (the »session loop thread«), enqueues all
 ** these commands and dispatches them one by one.
 ** 
 ** # the test operation
 ** This test setup defines a specifically rigged _test command,_ which does not actually
 ** operate on the session. Instead, it performs some time calculations and adds the resulting
 ** time offset to a global variable, which can be observed from the test methods. The generated
 ** values are controlled by the command arguments and thus predictable, which allows to verify
 ** the expected number of invocations happened, using the right arguments.
 ** 
 ** # massively multithreaded stress test
 ** The last test case performs a massively multithreaded _torture test_ to scrutinise the sanity
 ** of locking and state management. It creates several threads, each of which produces several
 ** instances of the common _test command_ used in this test, and binds each instance with different
 ** execution arguments. All these operations are sent as command messages, interspersed with short
 ** random pauses, which causes them to arrive in arbitrary order within the dispatcher queue.
 ** Moreover, while the "command producer threads" are running, the main thread temporarily
 ** disables command dispatch, which causes the command queue to build up. After re-enabling
 ** dispatch, the main thread spins to wait for the queue to become empty. The important
 ** point to note is that the test command function itself _contains no locking._ But since
 ** all command operations are triggered in a single dedicated thread, albeit in arbitrary
 ** order, at the end the checksum must add up to the expected value.
 ** 
 ** ## parametrisation
 ** It is possible to change the actual setup with the following positional commandline arguments
 ** - the number of threads to start
 ** - the number of consecutive command instances produced in each thread
 ** - the maximum delay (in µs) between each step in each thread
 ** Astute readers might have noticed, that the test fixture is sloppy with respect to proper
 ** locking and synchronisation. Rather, some explicit sleep commands are interspersed in a way
 ** tuned to work satisfactory in practice. This whole approach can only work, because each
 ** POSIX locking call actually requires the runtime system to issue a read/write barrier,
 ** which are known to have global effects on the relevant platforms (x86 and x86_64).
 ** And because the production relevant code in SteamDispatcher uses sufficient (in fact
 ** even excessive) locking, the state variables of the test fixture are properly synced
 ** by sideeffect.
 ** 
 ** This test case can fail when, by bad coincidence, the command queue is temporarily emptied,
 ** while some producer threads are still alive -- because in this case the main thread might
 ** verify the checksum before all command instances have been triggered. To avoid this
 ** situation, make sure the delay between actions in the threads is not too long and
 ** start a sufficiently high number of producer threads.
 **
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
extern "C" {
#include "common/interfaceregistry.h"
}

#include "steam/control/steam-dispatcher.hpp"
#include "steam/control/command-def.hpp"
#include "include/session-command-facade.h"
#include "lib/typed-counter.hpp"
#include "lib/format-string.hpp"
#include "lib/sync-barrier.hpp"
#include "lib/thread.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <chrono>
#include <string>
#include <vector>
#include <deque>


namespace steam {
namespace control {
namespace test    {
  
  
  using boost::lexical_cast;
  using std::this_thread::sleep_for;
  using std::chrono::microseconds;
  using namespace std::chrono_literals;
  using steam::control::SessionCommand;
  using lib::test::randTime;
  using lib::diff::GenNode;
  using lib::diff::Rec;
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::Duration;
  using lib::time::Offset;
  using lib::time::FSecs;
  using lib::FamilyMember;
  using lib::SyncBarrier;
  using lib::Symbol;
  using util::_Fmt;
  using util::isnil;
  using std::string;
  using std::vector;
  using std::deque;
  using std::rand;
  
  
  namespace { // test fixture...
    
    /* === parameters for multi-threaded stress test === */
    
    uint NUM_THREADS_DEFAULT = 50;   ///< @note _not_ const, can be overridden by command line argument
    uint NUM_INVOC_PER_THRED = 10;
    uint MAX_RAND_DELAY_us   = 50;   ///< @warning be sure to keep this way shorter than the delay in the main thread
    
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
  
  
#define __DELAY__ sleep_for (20ms);
  
  
  
  
  
  /******************************************************************************************//**
   * @test verify integrated functionality of command dispatch through the SessionCommand facade.
   *       - operate lifecycle of the supporting components,
   *         similar to activating the »session subsystem«
   *       - generate command messages similar to what is received from the UI-Bus
   *       - us the handler mechanism from stage::ctrl::CoreService to talk to the facade
   *       - have a specially rigged command function to observe invocation
   *       - wait for the session loop thread to dispatch this command
   *       - verify that commands are really executed single-threaded
   * 
   * @see steam::SessionSubsystem
   * @see SteamDispatcher
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
       *  @note we are _not_ actually starting the subsystem itself,
       *        but we indeed start the _»session loop thread«_
       *  @see facade.cpp
       */
      void
      startDispatcher()
        {
          CHECK (not SteamDispatcher::instance().isRunning());
          
          SteamDispatcher::instance().start ([&] (string* problemMessage)
                                                 {
                                                   CHECK (isnil (*problemMessage));
                                                   thread_has_ended = true;
                                                 });
          
          CHECK (SteamDispatcher::instance().isRunning());
          CHECK (not thread_has_ended);
        }
      bool thread_has_ended{false};
      
      
      /** @test verify the »session loop thread« has finished properly */
      void
      stopDispatcher()
        {
          CHECK (SteamDispatcher::instance().isRunning());
          SteamDispatcher::instance().requestStop();
          
          __DELAY__
          CHECK (not SteamDispatcher::instance().isRunning());
          CHECK (thread_has_ended);
        }
      
      
      /** @test demonstrate a simple direct invocation */
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
       *        - build a command message, similar to what the
       *          [generic UI element](\ref stage::model::Tangible) does
       *        - use the contents of this message at the SessionCommand
       *          facade, similar to what CoreService does
       */
      void
      perform_messageInvocation()
        {
          // this happens within some tangible UI element (widget / controller)
          GenNode commandMsg{string(COMMAND_I2), Rec{Duration(25,10), Time(500,0), -2}};
          CHECK (commandMsg.idi.getSym() == string{COMMAND_I2});
          CHECK (not Command::canExec(COMMAND_I2));
          Time prevState = testCommandState;
          
          // this happens, when CoreService receives command messages from UI-Bus
          SessionCommand::facade().trigger (commandMsg.idi.getSym(), commandMsg.data.get<Rec>());
          
          __DELAY__
          CHECK (testCommandState - prevState == Time(FSecs(3,2)));     // execution added 2500ms -2*500ms == 1.5sec
        }
      
      
      
      /** @test massively multithreaded _torture test_ to verify
       *        that commands are properly enqueued and executed one by one
       *        - create several threads to send random command messages
       *        - verify that, after executing all commands, the internal
       *          state variable reflects the result of a proper
       *          sequential calculation and summation
       */
      void
      perform_massivelyParallel (Arg args_for_stresstest)
        {
          seedRand();
          maybeOverride (NUM_THREADS_DEFAULT, args_for_stresstest, 1);
          maybeOverride (NUM_INVOC_PER_THRED, args_for_stresstest, 2);
          maybeOverride (MAX_RAND_DELAY_us,   args_for_stresstest, 3);
          
          
          // we'll run several instances of the following thread....
          class InvocationProducer
            : util::NonCopyable
            {
              SyncBarrier& barrier_;
              FamilyMember<InvocationProducer> id_;
              vector<string> cmdIDs_;
              lib::Random random_;
              
              lib::ThreadJoinable<void> thread_;
              
              Symbol
              cmdID (uint j)
                {
                  cmdIDs_.push_back (_Fmt("%s.thread-%02d.%d") % COMMAND_ID % id_ % j);
                  return cStr(cmdIDs_.back());
                }
              
              
            public:
              InvocationProducer (SyncBarrier& trigger)
                : barrier_{trigger}
                , random_{defaultGen}
                , thread_{"producer", [&]{ fabricateCommands(); }}
                { }
              
             ~InvocationProducer()
                {
                  thread_.join().maybeThrow();
                  for (auto& id : cmdIDs_)
                    Command::remove (cStr(id));
                }
             
            private:
              void
              fabricateCommands()
                {
                  barrier_.sync(); // barrier to unleash all threads together
                  
                  for (uint j=0; j<NUM_INVOC_PER_THRED; ++j)
                    {
                      auto cmd = Command(COMMAND_ID).storeDef(cmdID(j));
                      
                      __randomDelay();
                      sendCommandMessage (GenNode{string{cmd.getID()}, Rec{Duration(7*id_, 2), Time(500,0), -int(j)}});
                    }
                }
              
              static void
              sendCommandMessage (GenNode msg)
                {
                  SessionCommand::facade().trigger (msg.idi.getSym(), msg.data.get<Rec>());
                }
              
              void
              __randomDelay()
                {
                  if (not MAX_RAND_DELAY_us) return;
                  sleep_for (microseconds (1 + random_.i(MAX_RAND_DELAY_us)));   // random delay varying in steps of 1µs
                }
            };
          
          /* == controlling code in main thread == */
          Time prevState = testCommandState;
          
          FSecs expectedOffset{0};
          for (uint i=0; i<NUM_THREADS_DEFAULT; ++i)
            for (uint j=0; j<NUM_INVOC_PER_THRED; ++j)
              expectedOffset += FSecs(i*7,2) - FSecs(j,2);
          
          // fire up several threads to issue commands in parallel...
          SyncBarrier trigger{NUM_THREADS_DEFAULT + 1};
          deque<InvocationProducer> producerThreads;
          for (uint i=0; i<NUM_THREADS_DEFAULT; ++i)
            producerThreads.emplace_back (trigger);
          
          // start concurrent execution
          trigger.sync();
          
          // give the producer threads some head start...
          sleep_for (microseconds (MAX_RAND_DELAY_us * NUM_INVOC_PER_THRED / 2));
          __DELAY__
          
          // stop the dispatching to cause the queue to build up...
          SteamDispatcher::instance().deactivate();
          SteamDispatcher::instance().awaitDeactivation();
          
          __DELAY__
          SteamDispatcher::instance().activate();
          
          __DELAY__
          while (not SteamDispatcher::instance().empty());
          
          __DELAY__
          CHECK (testCommandState - prevState == Time(expectedOffset));
        }// Note: leaving this scope blocks for joining all producer threads
    };
  
  
  /** Register this test class... */
  LAUNCHER (SessionCommandFunction_test, "function controller");
  
  
}}} // namespace steam::control::test
