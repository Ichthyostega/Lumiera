/*
  CommandUse1(Test)  -  usage aspects I

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file command-use1-test.cpp
 ** unit test \ref CommandUse1_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/control/command-invocation.hpp"
#include "steam/control/command-def.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include "steam/control/test-dummy-commands.hpp"


namespace steam {
namespace control {
namespace test    {
  
  using util::isSameObject;
  using util::contains;
  using LERR_(INVALID_COMMAND);
  using LERR_(DUPLICATE_COMMAND);
  using LERR_(UNBOUND_ARGUMENTS);
  using LERR_(INVALID_ARGUMENTS);
  
  
  
  
  /***********************************************************************//**
   * @test command usage aspects I: defining commands in various ways,
   *       then re-accessing those definitions, create instances,
   *       invoke them and undo the effect. Clean up finally.
   * @see Command
   * @see command-basic-test.cpp (simple usage example)
   */
  class CommandUse1_test : public Test
    {
    
      int randVal;
      int random() { return randVal = 10 + rani(40); }
      
      
      
      virtual void
      run (Arg)
        {
          seedRand();
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
          
          CHECK (0 == command1::check_);
          CHECK (cnt_defs == Command::definition_count());
          CHECK (cnt_inst == Command::instance_count());
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
          
          CHECK (randVal == command1::check_);
          
          Command::get("test.command1.1").undo();
          CHECK ( 0 == command1::check_);
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
          CHECK (CommandDef("test.command1.2"));
          
          Command com ("test.command1.2");
          CHECK (com);
          CHECK (com == Command::get("test.command1.2"));
          CHECK (contains (string(com), "test.command1.2"));
          CHECK (contains (string(com), "{def}"));
          CHECK (!com.canExec());
          VERIFY_ERROR (UNBOUND_ARGUMENTS, com() );
          CHECK ( 0 == command1::check_);
          
          VERIFY_ERROR (INVALID_ARGUMENTS, com.bind ("foo") );
          com.bind (random());              // note: run-time type check only
          CHECK ( com.canExec());
          CHECK (!com.canUndo());
          com();
          CHECK (randVal == command1::check_);
          com.undo();
          CHECK ( 0 == command1::check_);
          
          // the following shortcut does the same:
          invoke ("test.command1.2") (1234);
          CHECK ( 1234 == command1::check_);
          
          com.undo();
          CHECK ( 0 == command1::check_);
        }
      
      
      void
      statePredicates()
        {
          Command::remove("test.command1.2");
          VERIFY_ERROR (INVALID_COMMAND, Command::get("test.command1.2") );
          
          CommandDef def ("test.command1.2");
          CHECK (!def);
          
          def.operation (command1::operate)
             .captureUndo (command1::capture);
          CHECK (!def);                        // undo functor still missing
          VERIFY_ERROR (INVALID_COMMAND, Command::get("test.command1.2") );
          
          def.operation (command1::operate)
             .captureUndo (command1::capture)
             .undoOperation (command1::undoIt);
          CHECK (def);
          CHECK (CommandDef("test.command1.2"));
          CHECK (Command::get("test.command1.2"));
          
          CHECK ( Command::defined("test.command1.2"));
          CHECK (!Command::canExec("test.command1.2"));
          CHECK (!Command::canUndo("test.command1.2"));
          
          Command com = Command::get("test.command1.2");
          CHECK (com);
          CHECK (!com.canExec());
          CHECK (!com.canUndo());
          
          com.bind (11111);
          CHECK ( Command::defined("test.command1.2"));
          CHECK ( Command::canExec("test.command1.2"));
          CHECK (!Command::canUndo("test.command1.2"));
          
          com();
          CHECK ( Command::defined("test.command1.2"));
          CHECK ( Command::canExec("test.command1.2"));
          CHECK ( Command::canUndo("test.command1.2"));
          
          com.undo();
          CHECK ( Command::defined("test.command1.2"));
          CHECK ( Command::canExec("test.command1.2"));
          CHECK ( Command::canUndo("test.command1.2"));
          
          com.unbind(); // revert to pristine state
          CHECK ( Command::defined("test.command1.2"));
          CHECK (!Command::canExec("test.command1.2"));
          CHECK (!Command::canUndo("test.command1.2"));
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
          
          CHECK (Command::get("test.command1.3").canExec());
        }
      
      
      void
      usePrototype()
        {
          Command c1 = Command::get("test.command1.3");
          CHECK (c1);
          CHECK (c1.canExec());
          CHECK (!c1.canUndo());
          
          Command c2 = c1.newInstance();
          CHECK (c2);
          CHECK (c2.canExec());
          CHECK (!c2.canUndo());
          CHECK (c2.isAnonymous());
          
          CHECK (c1 != c2);
          CHECK (!isSameObject(c1, c2));
          
          CHECK (0 == command1::check_);
          
          c1();
          
          CHECK (randVal == command1::check_);
          CHECK ( c1.canUndo());
          CHECK (!c2.canUndo());
          
          c2();
          CHECK (randVal + randVal == command1::check_);
          CHECK (c2.canUndo());
          CHECK (c1 != c2);
          
          c1.undo();
          CHECK (0 == command1::check_);
          c2.undo();
          CHECK (randVal == command1::check_);
          
          c2.bind(23);
          c2();
          CHECK (randVal + 23 == command1::check_);
          
          // you should not use a command more than once (but it works...)
          c1();
          CHECK (randVal + 23 + randVal == command1::check_);
          c1.undo();
          CHECK (randVal + 23 == command1::check_);
          // note we've overwritten the previous undo state
          // and get the sate captured on the second invocation
          
          c2.undo();
          CHECK (randVal == command1::check_);
          c1.undo();
          CHECK (randVal + 23 == command1::check_);
          
          // use the current sate of c2 as Prototype for new command definition
          c2.storeDef ("test.command1.4");
          Command c4 = Command::get("test.command1.4");
          CHECK (c4);
          CHECK (c4.canExec());
          CHECK (c4.canUndo());
          CHECK (not c4.isAnonymous());
          CHECK (    c2.isAnonymous());
          CHECK (c4 != c2); // note: it was stored as independent clone copy
          CHECK (c4 != c1);
          c4();
          CHECK (c4 != c2); // now lives independently from the original
          CHECK (randVal + 2*23 == command1::check_);
          
          c4.bind(int(-command1::check_)); // new command argument binding
          c4();
          CHECK (0 == command1::check_);
          c2();
          CHECK (23 == command1::check_);
          c2.undo();
          CHECK (0 == command1::check_);
          
          // remove argument bindings per instance and return to pristine state
          c4.unbind();
          CHECK (c2.canExec());
          CHECK (c2.canUndo());
          CHECK (not c4.canExec());
          CHECK (not c4.canUndo());
        }
      
      
      void
      preventDuplicates()
        {
          #define BUILD_NEW_COMMAND_DEF(_ID_) \
          CommandDef (_ID_)                    \
              .operation (command1::operate)    \
              .captureUndo (command1::capture)   \
              .undoOperation (command1::undoIt)
          
          CHECK (CommandDef ("test.command1.1"));
          VERIFY_ERROR (DUPLICATE_COMMAND, BUILD_NEW_COMMAND_DEF ("test.command1.1") );
          CHECK (CommandDef ("test.command1.2"));
          VERIFY_ERROR (DUPLICATE_COMMAND, BUILD_NEW_COMMAND_DEF ("test.command1.2") );
          CHECK (CommandDef ("test.command1.3"));
          VERIFY_ERROR (DUPLICATE_COMMAND, BUILD_NEW_COMMAND_DEF ("test.command1.3") );
          CHECK (CommandDef ("test.command1.4"));
          VERIFY_ERROR (DUPLICATE_COMMAND, BUILD_NEW_COMMAND_DEF ("test.command1.4") );
        }
      
      
      void
      stringRepresentation()
        {
          cout << Command::get("test.command1.1") << endl;
          cout << Command::get("test.command1.2") << endl;
          cout << Command::get("test.command1.3") << endl;
          cout << Command::get("test.command1.4") << endl;
          cout << Command()                       << endl;
          
          Command com
            = CommandDef ("test.command1.5")
                  .operation (command1::operate)
                  .captureUndo (command1::capture)
                  .undoOperation (command1::undoIt);
          
          cout << com << endl;
          com.bind(123);
          cout << com << endl;
          com();
          cout << com << endl;
          com.undo();
          cout << com << endl;
        }
      
      
      void
      undef()
        {
          CHECK (CommandDef ("test.command1.1"));
          CHECK (CommandDef ("test.command1.2"));
          CHECK (CommandDef ("test.command1.3"));
          CHECK (CommandDef ("test.command1.4"));
          
          CHECK (Command::get("test.command1.1"));
          CHECK (Command::get("test.command1.2"));
          CHECK (Command::get("test.command1.3"));
          CHECK (Command::get("test.command1.4"));
          
          VERIFY_ERROR (INVALID_COMMAND, Command::get("miracle"));
          VERIFY_ERROR (INVALID_COMMAND, invoke ("miracle") (1,2,3));
          
          CommandDef unbelievable ("miracle");
          CHECK (!unbelievable);
          
          Command miracle;
          // but because the miracle isn't yet defined, any use throws
          VERIFY_ERROR (INVALID_COMMAND, miracle.bind("abracadabra"));
          VERIFY_ERROR (INVALID_COMMAND, miracle.execSync());
          VERIFY_ERROR (INVALID_COMMAND, miracle.undo());
          VERIFY_ERROR (INVALID_COMMAND, miracle());
          CHECK (!miracle.canExec());
          CHECK (!miracle.canUndo());
          CHECK (!miracle);
          
          Command c5 (Command::get("test.command1.5"));
          
          CHECK (Command::remove("test.command1.1"));
          CHECK (Command::remove("test.command1.2"));
          CHECK (Command::remove("test.command1.3"));
          CHECK (Command::remove("test.command1.4"));
          CHECK (Command::remove("test.command1.5"));
          
          CHECK (!Command::remove("miracle")); // there is no such thing...
          
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.1"));
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.2"));
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.3"));
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.4"));
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.5"));
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("miracle"));
          
          
          // note, removed the registered definitions,
          // but existing instances remain valid...
          // thus we're free to create new instances...
          CHECK (c5.isValid());
          CHECK (c5.canExec());
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (CommandUse1_test, "function controller");
  
  
}}} // namespace steam::control::test
