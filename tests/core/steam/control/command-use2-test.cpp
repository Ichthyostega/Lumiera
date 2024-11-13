/*
  CommandUse2(Test)  -  usage aspects II

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file command-use2-test.cpp
 ** unit test \ref CommandUse2_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/control/command.hpp"
#include "steam/control/command-def.hpp"
#include "steam/control/handling-pattern.hpp"
#include "lib/format-string.hpp"
#include "lib/format-obj.hpp"
#include "lib/util.hpp"

#include "steam/control/test-dummy-commands.hpp"

extern "C" {
#include "common/interfaceregistry.h"
}
#include "steam/control/steam-dispatcher.hpp"
#include "include/session-command-facade.h"

#include <functional>
#include <boost/ref.hpp>
#include <boost/lexical_cast.hpp>
#include <string>


namespace steam {
namespace control {
namespace test    {

  using util::_Fmt;
  using std::string;
  using std::function;
  using std::bind;
  using std::ref;
  using boost::lexical_cast;
  using util::contains;
  
  using LERR_(EXTERNAL);
  
  
  /** diagnostics: checks if the given value has been written
   *  to the test protocol (string stream) of command2 
   *  Explanation: command2 accepts a function, invokes
   *  it and writes the result to the protocol stream. 
   */
  template<typename TY>
  inline bool
  protocolled (TY val2check)
    {
      return contains ( command2::check_.str()
                      , util::toString (val2check)
                      );
    }
  
  
  
  
  /***********************************************************************//**
   * @test command usage aspects II: patterns of command invocation.
   * 
   * @todo this test is still on hold, as the non-trivial patterns aren't implemented as of 10/09  ////////////////TICKET #211
   * 
   * @see Command
   * @see command-basic-test.cpp (simple usage example)
   */
  class CommandUse2_test : public Test
    {
    
      int randVal_;
      
      string randomTxt()
        {
          _Fmt fmt ("invoked( %2d )");
          
          randVal_ = rani (100);
          return fmt % randVal_;
        }
      
      bool blowUp_;
      
      
      virtual void
      run (Arg) 
        {
          seedRand();
          command2::check_.seekp(0);
          uint cnt_defs = Command::definition_count();
          uint cnt_inst = Command::instance_count();
          
          function<string()> randFun = bind (&CommandUse2_test::randomTxt, this);
          
          // prepare a command definition (prototype)
          CommandDef ("test.command2")
              .operation (command2::operate)
              .captureUndo (command2::capture)
              .undoOperation (command2::undoIt)
              .bind (randFun, &blowUp_);
          
           //note : blowUp_ is bound via pointer,
          //        thus we can provoke an exception at will.
          blowUp_ = false;
          
          
          check_defaultHandlingPattern();
//        check_ThrowOnError();              //////////////////////////////////////////////////////////////////////TICKET #211
          check_DispatcherInvocation();
          
          
          Command::remove ("test.command2");
          Command::remove ("test.command2.1");
          CHECK (cnt_defs == Command::definition_count());
          CHECK (cnt_inst == Command::instance_count());
        }
      
      
      
      void
      check_defaultHandlingPattern()
        {
          Command com = Command::get("test.command2");
          
          CHECK (!protocolled("invoked"));
          
          bool success = com();
          
          CHECK (success);
          CHECK (protocolled("invoked"));
          CHECK (protocolled(randVal_));
          
          success = com.undo();
          CHECK (success);           // UNDO invoked successfully
          CHECK (!protocolled(randVal_));
          CHECK (protocolled("UNDO"));
          
          blowUp_ = true;
          string current = command2::check_.str();
          
          success = com();
          CHECK (not success);       // NOT executed successfully (exception thrown and caught)
          CHECK (command2::check_.str() == current);
          CHECK (!lumiera_error_peek());   // already absorbed
          
          success = com.undo();
          CHECK (not success);       // UNDO failed (exception thrown and caught)
          CHECK (command2::check_.str() == current);
          
          blowUp_ = false;
        }
      
      
      
      void
      check_ThrowOnError()
        {
          Command com = Command::get("test.command2");
          
          blowUp_ = false;
          com.exec(HandlingPattern::SYNC_THROW);
          CHECK (protocolled(randVal_));
          
          blowUp_ = true;
          string current = command2::check_.str();
          HandlingPattern const& doThrow = HandlingPattern::get(HandlingPattern::SYNC_THROW);
          
          VERIFY_ERROR( EXTERNAL, com.exec (doThrow) );
          CHECK (command2::check_.str() == current);
          
          // we can achieve the same effect,
          // after changing the default HandlingPatern for this command instance
          com.setHandlingPattern(HandlingPattern::SYNC_THROW);
          com.storeDef ("test.command2.1");
          
          Command com2 = Command::get("test.command2.1");
          VERIFY_ERROR( EXTERNAL, com2() );
          CHECK (command2::check_.str() == current);

          blowUp_ = false;
          com2();
          CHECK (command2::check_.str() > current);
          CHECK (protocolled(randVal_));
          
          com2.undo();
          CHECK (!protocolled(randVal_));
        }
      
      
      
      /** @test simplified integration test of command dispatch
       *        - performs the minimal actions necessary to start the session loop thread
       *        - then issues a test command, which will be queued and dispatched
       *          by the SteamDispatcher. Like in the real application, the command
       *          executions happens in the dedicated session loop thread, and thus
       *          we have to wait a moment, after which execution can be verified.
       *        - finally the SteamDispatcher is signalled to shut down.
       *  @see SessionCommandFunction_test for much more in-depth coverage of this aspect
       */
      void
      check_DispatcherInvocation()
        {
          CHECK (not SteamDispatcher::instance().isRunning());
          lumiera_interfaceregistry_init();
          lumiera::throwOnError();
#define __DELAY__ usleep(10000);
          
          bool thread_has_ended{false};
          SteamDispatcher::instance().start ([&] (string*) { thread_has_ended = true; });
          
          CHECK (SteamDispatcher::instance().isRunning());
          CHECK (not thread_has_ended);
          
          //----Session-Loop-Thread-is-running------------------------
          
          string cmdID {"test.command2"};
          string prevExecLog = command2::check_.str();
          
          // previous test cases prepared the arguments
          // so that we can just trigger command execution.
          // In the real application, this call is issued
          // from CoreService when receiving a command
          // invocation message over the UI-Bus
          SessionCommand::facade().invoke(cmdID);
          
          __DELAY__  // wait a moment for the other thread to dispatch the command...
          CHECK (prevExecLog != command2::check_.str());
          
          //----Session-Loop-Thread-is-running------------------------
          
          // shut down the SteamDispatcher...
          CHECK (SteamDispatcher::instance().isRunning());
          SteamDispatcher::instance().requestStop();
          
          __DELAY__  // wait a moment for the other thread to terminate...
          CHECK (not SteamDispatcher::instance().isRunning());
          CHECK (thread_has_ended);
          
          lumiera_interfaceregistry_destroy();
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandUse2_test, "function controller");
      
      
}}} // namespace steam::control::test
