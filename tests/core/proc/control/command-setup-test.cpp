/*
  CommandSetup(Test)  -  verify helper for setup of actual command definitions

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

/** @file command-setup-test.cpp
 ** unit test \ref CommandSetup_test
 */


#include "lib/test/run.hpp"
#include "proc/cmd.hpp"
#include "proc/control/command-def.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/p.hpp"

//#include <cstdlib>
#include <string>
#include <regex>


//using std::rand;


namespace proc {
namespace cmd  {
namespace test {
  
//using lib::time::Time;
//using lib::time::TimeVar;
//using lib::time::TimeValue;
//using lib::time::Offset;
  using lib::Literal;
  using std::string;
  using std::regex;
  using std::regex_replace;
  using util::_Fmt;
  
  
  
  
  
  namespace  { // Test fixture....
    
    /** will be manipulated by the commands we define */
    string testString;
    
    
    void
    do_something_pointless (CommandDef&)
    {
      cout << "before-->" << testString << endl;
      testString = "Ichthyostega wuz here";
      cout << "after--->" << testString << endl;
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
  }
  
  
  
  /***********************************************************************//**
   * @test cover the behaviour of the CommandSetup helper for inserting
   *       actual command definitions into the Session.
   * 
   * @see cmd.hpp
   * @see session-cmd.cpp actual usage example
   * @see proc::control::CommandSetup
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
//        def_empty = def_1;
          
          
          // add actual definition closures... 
          CommandSetup def_0{"test.CommandSetup.def_0"};
          CHECK (CommandSetup::pendingCnt() == 0);
          
          def_0 = do_something_pointless;
          CHECK (CommandSetup::pendingCnt() == 1);
          
          
          CommandSetup def_1 = CommandSetup{"test.CommandSetup.def_1"}
                               = [](CommandDef def)
                                   {
                                     def.operation (operate)
                                        .captureUndo (capture)
                                        .undoOperation (undoIt);
                                   };
          
          CommandSetup def_2 = CommandSetup{"test.CommandSetup.def_2"}
                               = [&](CommandDef def)                                            // NOTE: we capture context by reference
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
          size_t cnt = CommandSetup::invokeDefinitionClosures();
          CHECK (CommandSetup::pendingCnt() == 0);
          CHECK (cnt == 3);
          CHECK (testString == "Ichthyostega wuz here");
          TODO ("verify the command definitions happened");
        }
      
      
      /** @test demonstrate the standard usage pattern of command definition setup
       *        - static CommandSetup instances are created "somewhere"
       *        - when the ON_GLOBAL_INIT [lifecycle event](\ref lifecycle.h) is
       *          issued, all command definitions collected thus far are invoked
       *          and thus inserted into the proc::control::CommandRegistry
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
          UNIMPLEMENTED ("cover standard usage of command definitions");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandSetup_test, "unit controller");
  
  
}}} // namespace proc::control::test
