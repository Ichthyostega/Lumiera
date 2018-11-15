/*
  CommandMessageBinding(Test)  -  verify argument binding via GenNode-Message, with immutable types

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

/** @file command-message-binding-test.cpp
 ** unit test \ref CommandMessageBinding_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/control/command-def.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/diff/gen-node.hpp"


namespace steam {
namespace control {
namespace test    {
  
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::TimeValue;
  using lib::time::TimeSpan;
  using lib::time::Duration;
  using lib::test::randTime;
  using lib::diff::Rec;
  
  
  
  
  
  namespace  { // functions to be invoked through the command system
    
    TimeVar implicitTestState;
    
    void
    operate (Duration dur)
    {
      implicitTestState += dur;
    }
    
    Time
    capture (Duration)
    {
      return implicitTestState;
    }
    
    void
    undoIt (Duration, Time oldVal)
    {
      implicitTestState = oldVal;
    }
  }
  
  
  
  
  /***********************************************************************//**
   * @test verify the simple standard usage pattern, where command arguments
   *       for binding are passed via UI-Bus message with GenNode elements.
   * This is a simplified demonstration, but covers some tricky aspects.
   * - commands may define arguments of arbitrary type, and at runtime
   *   these are initialised from the set of standard types allowed
   *   within GenNode elements. Here the Duration is initialised from
   *   a TimeSpan object
   * - we support immutable argument types, which means the command binding
   *   machinery works without (re)assignment, only copy construction of
   *   argument holders into an inline buffer.
   * - likewise we support to use an immutable type as captured state
   *   memento, which is demonstrated by capturing a Time value
   * 
   * @see BusTerm_test
   * @see TupleRecordInit_test
   */
  class CommandMessageBinding_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          Time five(TimeValue(5));
          implicitTestState = five;
          
          CommandDef ("test.command2")
              .operation (operate)
              .captureUndo (capture)
              .undoOperation (undoIt)
              ;
          
          
          
          TimeSpan testSpan (randTime(), randTime());
          Rec argMsg {testSpan};
          
          // bind the command argument from a GenNode message
          Command ourCmd = Command::get("test.command2");
          CHECK (not ourCmd.canExec());
          
          ourCmd.bindArg (argMsg);
          CHECK (ourCmd.canExec());
          
          // invoke the command
          ourCmd();
          CHECK (implicitTestState == five + Duration(testSpan));
          
          // undo the effect of the command
          ourCmd.undo();
          CHECK (implicitTestState == five);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandMessageBinding_test, "unit controller");
  
  
}}} // namespace steam::control::test
