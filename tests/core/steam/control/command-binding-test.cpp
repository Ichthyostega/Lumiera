/*
  CommandBinding(Test)  -  special cases of binding command arguments

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

/** @file command-binding-test.cpp
 ** unit test \ref CommandBinding_test
 */


#include "lib/test/run.hpp"
#include "steam/control/command-def.hpp"
#include "steam/control/test-dummy-commands.hpp"
#include "lib/format-cout.hpp"


namespace steam {
namespace control {
namespace test    {
  
  
  
  
  
  
  
  /*************************************************************************//**
   * @test coverage for some specific situations when binding command arguments.
   * 
   * @todo add more test cases...
   * 
   * @see Command
   * @see command-basic-test.cpp (simple usage example)
   * @see command-use1-test.cpp  (various aspects of command use)
   */
  class CommandBinding_test : public Test
    {
      
      
      virtual void
      run (Arg) 
        {
          uint cnt_defs = Command::definition_count();
          uint cnt_inst = Command::instance_count();
          
          zeroArgumentCommand();
          
          Command::remove("test.command3.1");
          Command::remove("test.command3.2");
          
          CHECK (cnt_defs == Command::definition_count());
          CHECK (cnt_inst == Command::instance_count());
        }
      
      
      void
      zeroArgumentCommand()
        {
          command3::check_ = 0;
          
          CommandDef ("test.command3.1")
              .operation (command3::operate)
              .captureUndo (command3::capture)
              .undoOperation (command3::undoIt)
              .bind()                       // spurious bind doesn't hurt
              .execSync()
              ;
          
          CHECK ( 1 == command3::check_);
          
          CommandDef ("test.command3.2")
              .operation (command3::operate)
              .captureUndo (command3::capture)
              .undoOperation (command3::undoIt)
              ;
          Command com ("test.command3.2");
          CHECK (com.canExec());
          cout << com << endl;
          
          com();
          CHECK ( 2 == command3::check_);
          com.undo();
          CHECK ( 1 == command3::check_);
          
          Command commi = com.newInstance();
          com();
          com();
          com();
          CHECK ( 4 == command3::check_);
          
          commi.undo();  // it uses the inherited UNDO state
          CHECK ( 1 == command3::check_);
          
          com.undo();
          CHECK ( 3 == command3::check_);
          
          Command::get("test.command3.1").undo();
          CHECK ( 0 == command3::check_);
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (CommandBinding_test, "function controller");
  
  
}}} // namespace steam::control::test
