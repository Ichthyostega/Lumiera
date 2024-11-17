/*
  CommandSetup(Test)  -  verify helper for setup of actual command definitions

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file command-setup-test.cpp
 ** unit test \ref CommandSetup_test
 */


#include "lib/test/run.hpp"
#include "steam/cmd.hpp"
#include "steam/control/command-def.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <string>
#include <regex>


namespace steam {
namespace cmd  {
namespace test {
  
  using lib::Literal;
  using std::string;
  using std::regex;
  using std::regex_replace;
  using util::isnil;
  using util::_Fmt;
  
  
  
  
  
  namespace  { // Test fixture....
    
    /** will be manipulated by the commands we define */
    string testString;
    
    
    void
    do_something_pointless (CommandDef&)
    {
      testString = "Ichthyostega wuz here";
    }
    
    
    void
    operate (string search, string replace)
    {
      testString = regex_replace (testString, regex(search), replace);
    }
    
    string
    capture (string, string)
    {
      return testString;
    }
    
    void
    undoIt (string, string, string oldVal)
    {
      testString = oldVal;
    }
    
    
    /* ==== prepare a dummy command definition ==== */
    
    COMMAND_DEFINITION (test_CommandSetup_test)
      {
        def.operation(operate)
           .captureUndo(capture)
           .undoOperation(undoIt);
      };
  }
  
  
  
  /***********************************************************************//**
   * @test cover the behaviour of the CommandSetup helper intended for
   *       inserting actual command definitions into the Session.
   * 
   * @see cmd.hpp
   * @see session-cmd.cpp actual usage example
   * @see steam::control::CommandSetup
   */
  class CommandSetup_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          verify_DefinitionSetup();
          verify_DefinitionRegistration();
          verify_standardUsage();
        }
      
      
      /** @test create some command definitions inline and verify their properties. */
      void
      verify_DefinitionSetup()
        {
          // can be created from arbitrary character literal
          CommandSetup def_empty{"to be or not to be"};
          
          // at runtime it is nothing but a dressed-up C-string
          Literal empty_text = def_empty;
          CHECK (empty_text == "to be or not to be");
          CHECK (sizeof(def_empty) == sizeof(Literal));
          CHECK (sizeof(def_empty) == sizeof(char*));
          
          const char* actualContent = reinterpret_cast<char*&>(def_empty);
          CHECK (actualContent == empty_text);
          
          // for convenience a string conversion is provided...
          CHECK (string(def_empty) == string(empty_text));
          
          // can be equality compared based on sting (ID) content
          CHECK (def_empty == CommandSetup("to be or not to be"));
          CHECK (def_empty != CommandSetup("to pee or not to pee"));
          
//////////// does not compile -- copy assignment prohibited...
//        def_empty = CommandSetup{"to peel whatever"};
          
          
          // add actual definition closures... 
          CommandSetup def_0{"test.CommandSetup.def_0"};
          CHECK (CommandSetup::pendingCnt() == 0);
          
          def_0 = do_something_pointless;
          CHECK (CommandSetup::pendingCnt() == 1);
          
          
          CommandSetup def_1 = CommandSetup{"test.CommandSetup.def_1"}
                               = [](CommandDef& def)
                                   {
                                     def.operation (operate)
                                        .captureUndo (capture)
                                        .undoOperation (undoIt);
                                   };
          
          CommandSetup def_2 = CommandSetup{"test.CommandSetup.def_2"}
                               = [&](CommandDef& def)                                           // NOTE: we capture context by reference
                                    {
                                      def.operation ([&](uint cnt)
                                                        {
                                                          testString += pattern % cnt;          // NOTE: capture the field 'pattern' of the enclosing class
                                                        })
                                         .captureUndo ([](uint) -> string
                                                        {
                                                          return testString;
                                                        })
                                         .undoOperation ([](uint, string oldVal)
                                                        {
                                                          testString = oldVal;
                                                        });
                                    };
        }
      _Fmt pattern{" %d times."};
      
      
      
      /** @test actually issue the definitions captured as side-effect of the preceding test. */
      void
      verify_DefinitionRegistration()
        {
          CHECK (isnil (testString));
          
          CommandSetup::invokeDefinitionClosures();
          CHECK (CommandSetup::pendingCnt() == 0);
          CHECK (testString == "Ichthyostega wuz here");
          
          // the closure for the first entry did "something pointless",
          // but it actually did not define a command entry, thus...
          CHECK (not Command::defined("test.CommandSetup.def_0"));

          // but the other two entries did indeed define commands
          CHECK (Command::defined("test.CommandSetup.def_1"));
          CHECK (Command::defined("test.CommandSetup.def_2"));
          
          // ...and they defined the commands as specified
          Command com1{"test.CommandSetup.def_1"};
          Command com2{"test.CommandSetup.def_2"};
          
          com1.bind (string{"^(\\w+)"}, string{"No $1"});
          com2.bind (uint(42));
          
          CHECK (testString == "Ichthyostega wuz here");
          com1();
          CHECK (testString == "No Ichthyostega wuz here");
          
          com2();
          CHECK (testString == "No Ichthyostega wuz here 42 times.");
          
          com1.undo();
          CHECK (testString == "Ichthyostega wuz here");
        }
      
      
      /** @test demonstrate the standard usage pattern of command definition setup
       *        - static CommandSetup instances are created "somewhere"
       *        - when the ON_GLOBAL_INIT [lifecycle event](\ref lifecycle.h) is
       *          issued, all command definitions collected thus far are invoked
       *          and thus inserted into the steam::control::CommandRegistry
       *        - from this point on, we may access a control::Command handle
       *          corresponding to these definitions, and we may create derived
       *          instances to bind parameters and invoke the operation.
       *  @remark this test assumes that the static initialisation of the test fixture
       *          indeed created CommandSetup instances, which -- as a side-effect --
       *          also scheduled the corresponding functors for invocation. Moreover
       *          the [test-suite main()](\ref testrunner.cpp) in fact triggers the
       *          lifecycle event `ON_GLOBAL_INIT`, causing these test command
       *          definitions to be issued alongside with all the real session
       *          commands defined in the application.
       */
      void
      verify_standardUsage()
        {
          Command{test_CommandSetup_test}
            .storeDef("c1")
            .storeDef("c2");
          
          Command c1{"c1"}, c2{"c2"};
          CHECK (not c1.canExec());
          CHECK (not c2.canExec());
          
          c1.bind (string{"wuz.*"}, string{"the Devonian"});
          c2.bind (string{"\\s*\\w+$"}, string{""});
          CHECK (c1.canExec());
          CHECK (c2.canExec());
          CHECK (not Command::canExec(test_CommandSetup_test));
          
          CHECK (testString == "Ichthyostega wuz here");
          
          c1();
          CHECK (testString == "Ichthyostega the Devonian");
          
          c2();
          CHECK (testString == "Ichthyostega the");
          
          c2();
          CHECK (testString == "Ichthyostega");
          
          c2();
          CHECK (testString == "");
          
          c1.undo();
          CHECK (testString == "Ichthyostega wuz here");
          
          Command::remove("c1");
          Command::remove("c2");
          
          CHECK (not Command::defined("c1"));
          CHECK (not Command::defined("c2"));
          CHECK (Command::defined(test_CommandSetup_test));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandSetup_test, "unit controller");
  
  
}}} // namespace steam::cmd::test
