/*
  CommandUse1(Test)  -  usage aspects I
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
#include "proc/control/command-invocation.hpp"
#include "proc/control/command-def.hpp"
#include "lib/format.hpp"
#include "lib/util.hpp"

#include "proc/control/test-dummy-commands.hpp"

#include <iostream>


namespace control {
namespace test    {
  
  using util::isSameObject;
  using util::contains;
  using util::str;
  using std::cout;
  using std::endl;
  
  
  
  
  
  
  
  /***************************************************************************
   * @test command usage aspects I: defining commands in various ways,
   *       then re-accessing those definitions, create instances,
   *       invoke them and undo the effect. Clean up finally.
   * 
   * @see Command
   * @see command-basic-test.cpp (simple usage example)
   */
  class CommandUse1_test : public Test
    {
    
      int randVal;
      int random() { return randVal = 10 + (rand() % 40); }
      
      
      
      virtual void
      run (Arg) 
        {
          command1::check_ = 0;
          uint cnt_defs = Command::definition_count();
          uint cnt_inst = Command::instance_count();
          
          allInOneStep();
          standardUse();
          statePredicates();
          definePrototype();
          usePrototype();
          preventDuplicates();
          stringRepresentation();
          undef();
          
          ASSERT (0 == command1::check_);
          ASSERT (cnt_defs == Command::definition_count());
          ASSERT (cnt_inst == Command::instance_count());
        }
      
      
      void
      allInOneStep()
        {
          
          CommandDef ("test.command1.1")
              .operation (command1::operate)
              .captureUndo (command1::capture)
              .undoOperation (command1::undoIt)
              .bind (random())
              .execSync()
              ;
          
          ASSERT (randVal == command1::check_);
          
          Command::get("test.command1.1").undo();
          ASSERT ( 0 == command1::check_);
        }
      
      
      void
      standardUse()
        {
          {
            CommandDef ("test.command1.2")
                 .operation (command1::operate)
                 .captureUndo (command1::capture)
                 .undoOperation (command1::undoIt)
                 ;
          }
          ASSERT (CommandDef("test.command1.2"));
          
          Command com ("test.command1.2");
          ASSERT (com);
          ASSERT (com == Command::get("test.command1.2"));
          ASSERT (contains (str(com), "test.command1.2"));
          ASSERT (contains (str(com), "{def}"));
          ASSERT (!com.canExec());
          VERIFY_ERROR (UNBOUND_ARGUMENTS, com() );
          ASSERT ( 0 == command1::check_);
          
          VERIFY_ERROR (INVALID_ARGUMENTS, com.bind ("foo") );
          com.bind (random());             // note: run-time type check only
          ASSERT ( com.canExec());
          ASSERT (!com.canUndo());
          com();
          ASSERT (randVal == command1::check_);
          com.undo();
          ASSERT ( 0 == command1::check_);
          
          // the following shortcut does the same:
          invoke ("test.command1.2") (1234);
          ASSERT ( 1234 == command1::check_);
          
          // another shortcut, with static type check:
//        invoke (command1::operate) (5678);                                  //////////////////TICKET #291  : unimplemented for now (9/09)
//        ASSERT ( 1234+5678 == command1::check_);
          
          com.undo();
          ASSERT ( 0 == command1::check_);
        }
      
      
      void
      statePredicates()
        {
          Command::remove("test.command1.2");
          VERIFY_ERROR (INVALID_COMMAND, Command::get("test.command1.2") );
          
          CommandDef def ("test.command1.2");
          ASSERT (!def);
          
          def.operation (command1::operate)
             .captureUndo (command1::capture);
          ASSERT (!def);                       // undo functor still missing
          VERIFY_ERROR (INVALID_COMMAND, Command::get("test.command1.2") );
          
          def.operation (command1::operate)
             .captureUndo (command1::capture)
             .undoOperation (command1::undoIt);
          ASSERT (def);
          ASSERT (CommandDef("test.command1.2"));
          ASSERT (Command::get("test.command1.2"));
          
          ASSERT ( Command::defined("test.command1.2"));
          ASSERT (!Command::canExec("test.command1.2"));
          ASSERT (!Command::canUndo("test.command1.2"));
          
          Command com = Command::get("test.command1.2");
          ASSERT (com);
          ASSERT (!com.canExec());
          ASSERT (!com.canUndo());
          
          com.bind (11111);
          ASSERT ( Command::defined("test.command1.2"));
          ASSERT ( Command::canExec("test.command1.2"));
          ASSERT (!Command::canUndo("test.command1.2"));
          
          com();
          ASSERT ( Command::defined("test.command1.2"));
          ASSERT ( Command::canExec("test.command1.2"));
          ASSERT ( Command::canUndo("test.command1.2"));
          
          com.undo();
          ASSERT ( Command::defined("test.command1.2"));
          ASSERT ( Command::canExec("test.command1.2"));
          ASSERT ( Command::canUndo("test.command1.2"));
        }
      
      
      void
      definePrototype()
        {
          CommandDef ("test.command1.3")
               .operation (command1::operate)
               .captureUndo (command1::capture)
               .undoOperation (command1::undoIt)
               .bind (random())
               ;
          
          ASSERT (Command::get("test.command1.3").canExec());
        }
      
      
      void
      usePrototype()
        {
          Command c1 = Command::get("test.command1.3");
          ASSERT (c1);
          ASSERT (c1.canExec());
          ASSERT (!c1.canUndo());
          
          Command c2 = c1.newInstance();
          ASSERT (c2);
          ASSERT (c2.canExec());
          ASSERT (!c2.canUndo());
          
          ASSERT (c1 == c2);
          ASSERT (!isSameObject(c1, c2));
          
          ASSERT (0 == command1::check_);
          
          c1();
          
          ASSERT (randVal == command1::check_);
          ASSERT ( c1.canUndo());
          ASSERT (!c2.canUndo());
          ASSERT (c1 != c2);
          
          c2();
          ASSERT (randVal + randVal == command1::check_);
          ASSERT (c2.canUndo());
          ASSERT (c1 != c2);
          
          c1.undo();
          ASSERT (0 == command1::check_);
          c2.undo();
          ASSERT (randVal == command1::check_);
          
          c2.bind(23);
          c2();
          ASSERT (randVal + 23 == command1::check_);
          
          // you should not use a command more than once (but it works...)
          c1();
          ASSERT (randVal + 23 + randVal == command1::check_);
          c1.undo();
          ASSERT (randVal + 23 == command1::check_);
          // note we've overwritten the previous undo state
          // and get the sate captured on the second invocation
          
          c2.undo();
          ASSERT (randVal == command1::check_);
          c1.undo();
          ASSERT (randVal + 23 == command1::check_);
          
          // use the current sate of c2 as Prototype for new command definition
          c2.storeDef ("test.command1.4");
          Command c4 = Command::get("test.command1.4");
          ASSERT (c4);
          ASSERT (c4.canExec());
          ASSERT (c4.canUndo());
          ASSERT (c4 == c2);
          ASSERT (c4 != c1);
          c4();
          ASSERT (c4 != c2); // now lives independently from the original
          ASSERT (randVal + 2*23 == command1::check_);
          
          c4.bind(int(-command1::check_)); // new command argument binding
          c4();
          ASSERT (0 == command1::check_);
          c2();
          ASSERT (23 == command1::check_);
          c2.undo();
          ASSERT (0 == command1::check_);
        }
      
      
      void
      preventDuplicates()
        {
          #define BUILD_NEW_COMMAND_DEF(_ID_) \
          CommandDef (_ID_)                    \
              .operation (command1::operate)    \
              .captureUndo (command1::capture)   \
              .undoOperation (command1::undoIt)
          
          ASSERT (CommandDef ("test.command1.1"));
          VERIFY_ERROR (DUPLICATE_COMMAND, BUILD_NEW_COMMAND_DEF ("test.command1.1") );
          ASSERT (CommandDef ("test.command1.2"));
          VERIFY_ERROR (DUPLICATE_COMMAND, BUILD_NEW_COMMAND_DEF ("test.command1.2") );
          ASSERT (CommandDef ("test.command1.3"));
          VERIFY_ERROR (DUPLICATE_COMMAND, BUILD_NEW_COMMAND_DEF ("test.command1.3") );
          ASSERT (CommandDef ("test.command1.4"));
          VERIFY_ERROR (DUPLICATE_COMMAND, BUILD_NEW_COMMAND_DEF ("test.command1.4") );
        }
      
      
      void
      stringRepresentation()
        {
          cout << string (Command::get("test.command1.1")) << endl;
          cout << string (Command::get("test.command1.2")) << endl;
          cout << string (Command::get("test.command1.3")) << endl;
          cout << string (Command::get("test.command1.4")) << endl;
          cout << string (Command()                      ) << endl;
          
          CommandDef ("test.command1.5")
              .operation (command1::operate)
              .captureUndo (command1::capture)
              .undoOperation (command1::undoIt);
          
          Command com("test.command1.5");
          cout << string (com) << endl;
          com.bind(123);
          cout << string (com) << endl;
          com();
          cout << string (com) << endl;
          com.undo();
          cout << string (com) << endl;
        }
      
      
      void
      undef()
        {
          ASSERT (CommandDef ("test.command1.1"));
          ASSERT (CommandDef ("test.command1.2"));
          ASSERT (CommandDef ("test.command1.3"));
          ASSERT (CommandDef ("test.command1.4"));
          
          ASSERT (Command::get("test.command1.1"));
          ASSERT (Command::get("test.command1.2"));
          ASSERT (Command::get("test.command1.3"));
          ASSERT (Command::get("test.command1.4"));
          
          VERIFY_ERROR (INVALID_COMMAND, Command::get("miracle"));
          VERIFY_ERROR (INVALID_COMMAND, invoke ("miracle") (1,2,3));
          
          CommandDef unbelievable ("miracle");
          ASSERT (!unbelievable);
          
          Command miracle;
          // but because the miracle isn't yet defined, any use throws
          VERIFY_ERROR (INVALID_COMMAND, miracle.bind("abracadabra"));
          VERIFY_ERROR (INVALID_COMMAND, miracle.execSync());
          VERIFY_ERROR (INVALID_COMMAND, miracle.undo());
          VERIFY_ERROR (INVALID_COMMAND, miracle());
          ASSERT (!miracle.canExec());
          ASSERT (!miracle.canUndo());
          ASSERT (!miracle);
          
          Command c5 (Command::get("test.command1.5"));
          
          ASSERT (Command::remove("test.command1.1"));
          ASSERT (Command::remove("test.command1.2"));
          ASSERT (Command::remove("test.command1.3"));
          ASSERT (Command::remove("test.command1.4"));
          ASSERT (Command::remove("test.command1.5"));
          
          ASSERT (!Command::remove("miracle")); // there is no such thing...
          
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.1"));
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.2"));
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.3"));
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.4"));
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.5"));
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("miracle"));
          
          
          // note, removed the registered definitions,
          // but existing instances remain valid...
          // thus we're free to create new instances...
          ASSERT (c5.isValid());
          ASSERT (c5.canExec());
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (CommandUse1_test, "function controller");
  
  
}} // namespace control::test
