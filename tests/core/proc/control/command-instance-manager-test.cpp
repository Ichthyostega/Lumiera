/*
  CommandInstanceManager(Test)  -  verify helper for setup of actual command definitions

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

/** @file command-instance-manager-test.cpp
 ** unit test \ref CommandInstanceManager_test
 */


#include "lib/test/run.hpp"
#include "proc/control/test-dummy-commands.hpp"
#include "proc/control/command-instance-manager.hpp"
//#include "proc/control/command-def.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <string>
#include <regex>


namespace proc {
namespace control {
namespace test {
  
  using lib::Literal;
  using std::string;
  using std::regex;
  using std::regex_replace;
  using util::isnil;
  using util::_Fmt;
  
  
  
  
  
  namespace  { // Test fixture....
    
    const Symbol COMMAND_PROTOTYPE  = test_Dummy_command1;
    const string INVOCATION_ID = "CommandInstanceManager_test";
    
    class Fixture
      : public CommandDispatch
      {
        bool has_commands_in_queue = false;
        
        
        /* == Interface: CommandDispatch == */
        
        void
        clear()  override
          {
            UNIMPLEMENTED("DummyDispatch-Interface: clear");
          }
        
        void
        enqueue (Command)  override
          {
            UNIMPLEMENTED("DummyDispatch-Interface: enqueue");
          }
        
      public:
        bool
        contains (Symbol instanceID)
          {
            UNIMPLEMENTED("contains");
          }
      };
    
  }
  
  
  
  /***********************************************************************//**
   * @test CommandInstanceManager is responsible for providing individual
   *       clone copies from a basic command definition, to be bound with
   *       actual arguments and finally handed over to the ProcDispatcher
   *       for invocation.
   * 
   * @see proc::control::CommandInstanceManager
   */
  class CommandInstanceManager_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          verify_standardUsage();
        }
      
      
      /** @test demonstrate the standard usage pattern of...
       */
      void
      verify_standardUsage()
        {
          Fixture fixture;
          CommandInstanceManager iManager{fixture};
          Symbol instanceID = iManager.newInstance(COMMAND_PROTOTYPE, INVOCATION_ID);
          CHECK (Command::defined(instanceID));
          CHECK (not Command::canExec(instanceID));
          
          Command cmd{instanceID};
          ///TODO bind arguments
          CHECK (Command::canExec(instanceID));
          
          iManager.dispatch (instanceID);
          CHECK (not iManager.contains (instanceID));
          CHECK (fixture.contains (instanceID));
          CHECK (Command::canExec(instanceID));
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1089
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1089
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandInstanceManager_test, "unit controller");
  
  
}}} // namespace proc::control::test
