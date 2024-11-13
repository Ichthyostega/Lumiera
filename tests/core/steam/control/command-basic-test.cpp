/*
  CommandBasic(Test)  -  checking simple SteamDispatcher command definition and execution

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

/** @file command-basic-test.cpp
 ** unit test \ref CommandBasic_test
 */


#include "lib/test/run.hpp"
#include "steam/control/command-def.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/p.hpp"


namespace steam {
namespace control {
namespace test    {
  
  using lib::P;
  using lib::makeP;
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::TimeValue;
  using lib::time::Offset;
  
  
  
  
  
  namespace  { // functions to be invoked through the command system
    
    void
    operate (P<TimeVar> dummyObj, int randVal)
    {
      *dummyObj += TimeValue(randVal);
    }
    
    Offset
    capture (P<TimeVar> dummyObj, int)
    {
      return Offset{*dummyObj};
    }
    
    void
    undoIt (P<TimeVar> dummyObj, int, Offset oldVal)
    {
      *dummyObj = oldVal;
    }
  
  }
  
  
  
  /***********************************************************************//**
   * @test basic usage of the Steam-Layer command dispatch system.
   *       Shows how to define a simple command inline and how to
   *       trigger execution and UNDO. Verifies the command action
   *       takes place and is reverted again by the UNDO function.
   *       
   * This is a simplified demonstration. Usually, commands would be defined
   * in bulk and without specifying parameters. Later, typically client code
   * accesses a handle by ID, binds to the concrete argument and dispatches
   * the invocation. Note in this example that by using a smart-ptr as
   * argument allows accessing an object by reference and late binding.
   * 
   * @see  control::Command
   * @see  control::CommandDef
   * @see  control::SteamDispatcher
   */
  class CommandBasic_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          seedRand();
          int randVal{rani(10) - 5};
          Time five(TimeValue(5));
          TimeValue randomTime(randVal);
          auto obj = makeP<TimeVar>(five);
          
          CommandDef ("test.command1")
              .operation (operate)
              .captureUndo (capture)
              .undoOperation (undoIt)
              .bind (obj, randVal)
              ;
          
          
          Command ourCmd = Command::get("test.command1");
          
          // invoke the command
          CHECK (*obj == five);
          ourCmd();
          CHECK (*obj == five + randomTime);
          
          // undo the effect of the command
          ourCmd.undo();
          CHECK (*obj == five);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandBasic_test, "unit controller");
  
  
}}} // namespace steam::control::test
