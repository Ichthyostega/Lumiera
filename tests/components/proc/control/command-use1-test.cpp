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
//#include "proc/asset/media.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/edl.hpp"
//#include "proc/mobject/session/testclip.hpp"
//#include "proc/mobject/test-dummy-mobject.hpp"
//#include "lib/p.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/placement-index.hpp"
//#include "proc/mobject/explicitplacement.hpp"
#include "proc/control/command-def.hpp"
//#include "lib/lumitime.hpp"
#include "lib/util.hpp"

#include "proc/control/test-dummy-commands.hpp"

//#include <boost/format.hpp>
//#include <iostream>
//#include <cstdlib>
//#include <string>

//using boost::format;
//using lumiera::Time;
//using util::contains;
//using std::string;
//using std::rand;
//using std::cout;
//using std::endl;


namespace control {
namespace test    {

//  using lib::test::showSizeof;
  using util::isSameObject;

//  using session::test::TestClip;
//  using lumiera::P;
  
  
  //using lumiera::typelist::BuildTupleAccessor;
  
  
  
  
  
  
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
          definePrototype();
          usePrototype();
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
          ASSERT ( CommandDef("test.command1.2"));
          ASSERT (!Command::get("test.command1.2"));
          
          Command com = Command::get("test.command1.2");
          ASSERT (!com);
          ASSERT (!com.canUndo());
          VERIFY_ERROR (UNBOUND_ARGUMENTS, com() );
          
          ASSERT ( 0 == command1::check_);
          VERIFY_ERROR (INVALID_ARGUMENTS, com.bind ("foo") );
          com.bind (random());             // note: run-time type check only
          com();
          ASSERT (randVal == command1::check_);
          com.undo();
          ASSERT ( 0 == command1::check_);

          // the following shortcut does the same:
          Command::invoke ("test.command1.2", 1234);
          ASSERT ( 1234 == command1::check_);
          
          // another shortcut, with static type check:
          invoke (command1::operate, 5678);
          ASSERT ( 1234+5678 == command1::check_);
          
          com.undo();
          ASSERT ( 0 == command1::check_);
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
          
          Command c2 = Command::get("test.command1.3");
          ASSERT (c1);
          ASSERT (c1.canExec());
          ASSERT (!c1.canUndo());
          
          ASSERT (c1 == c2);
          ASSERT (!isSameObject(c1, c2));
          
          ASSERT (0 == command1::check_);
          
          c1();
          
          ASSERT (randVal == command1::check_);
          ASSERT (c1.canUndo());
          ASSERT (c1 != c2);
          ASSERT (!c2.canUndo());
          
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
          
          c2.undo()
          ASSERT (randVal == command1::check_);
          c1.undo();
          ASSERT (randVal + 23 == command1::check_);
          
          // use the current sate of c2 as Prototype for new command definition
          c2.storeDef ("test.command1.4");
          Command c3 = Command::get("test.command1.4");
          ASSERT (c3);
          ASSERT (!c3.canUndo());
          c3();
          ASSERT (randVal + 2*23 == command1::check_);
          
          c3.bind(-command1::check_);
          c3();
          ASSERT (0 == command1::check_);
          c2();
          ASSERT (23 == command1::check_);
          c2.undo();
          ASSERT (0 == command1::check_);
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
          
          CommandDef unbelievable ("miracle");
          ASSERT (!unbelievable);
          
          // but because the miracle isn't yet defined, any use throws
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("miracle"));
          VERIFY_ERROR (UNBOUND_ARGUMENTS, unbelievable.execSync() );
          VERIFY_ERROR (INVALID_COMMAND,   unbelievable.bind("abracadabra"));
          
          ASSERT (Command::remove("test.command1.1"));
          ASSERT (Command::remove("test.command1.2"));
          ASSERT (Command::remove("test.command1.3"));
          
          ASSERT (!Command::remove("miracle")); // there is no such thing...
          
          // note, we didn't remove the *definitions*
          // thus we're free to create new instances...
          Command xxx = Command::get("test.command1.1");
          VERIFY_ERROR (UNBOUND_ARGUMENTS, xxx() );
          
          // but this one is still there (we didn't remove it)
          ASSERT (Command::get("test.command1.4"));
          
          // now kill the definitions too...
          ASSERT (Command::undef ("test.command1.1"));
          ASSERT (Command::undef ("test.command1.2"));
          ASSERT (Command::undef ("test.command1.3"));
          ASSERT (Command::undef ("test.command1.4"));
          ASSERT (Command::undef ("miracle"));

          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.1"));
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.2"));
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.3"));
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("miracle"));
          
          // note: removing the definition automatically killed the remaining instance:
          VERIFY_ERROR (INVALID_COMMAND,   Command::get("test.command1.4"));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandUse1_test, "function controller");
      
      
}} // namespace control::test
