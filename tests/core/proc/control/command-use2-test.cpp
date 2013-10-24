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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "proc/control/command.hpp"
#include "proc/control/command-def.hpp"
#include "proc/control/handling-pattern.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

#include "proc/control/test-dummy-commands.hpp"

#include <tr1/functional>
#include <boost/ref.hpp>
#include <boost/lexical_cast.hpp>
#include <string>


namespace proc {
namespace control {
namespace test    {


  using util::_Fmt;
  using std::string;
  using std::tr1::function;
  using std::tr1::bind;
  using std::tr1::ref;
  using boost::lexical_cast;
  using util::contains;
  
  
  using lumiera::error::LUMIERA_ERROR_EXTERNAL;
  
  
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
                      , lexical_cast<string> (val2check)
                      );
    }
  
  
  
  
  /***********************************************************************//**
   * @test command usage aspects II: patterns of invoking commands.
   * 
   * @todo this test is still on hold, as the non-trivial patterns aren't implemented as of 10/09
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
          
          randVal_ = rand() % 100;
          return str (fmt % randVal_);
        }
      
      bool blowUp_;
      
      
      virtual void
      run (Arg) 
        {
          command2::check_.seekp(0);
          uint cnt_defs = Command::definition_count();
          uint cnt_inst = Command::instance_count();
          
          function<string()> randFun = bind (&CommandUse2_test::randomTxt, this);
          
          // prepare a command definition (prototype)
          CommandDef ("test.command2")
              .operation (command2::operate)
              .captureUndo (command2::capture)
              .undoOperation (command2::undoIt)
              .bind (randFun, ref(blowUp_));
          
           //note : blowUp_ is bound via reference_wrapper,
          //        thus we can provoke an exception at will.
          blowUp_ = false;
          
          
          check_defaultHandlingPattern();
          check_ThrowOnError();
          
          
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
          
          bool res = com();
          
          CHECK (res);
          CHECK (protocolled("invoked"));
          CHECK (protocolled(randVal_));
          
          res = com.undo();
          CHECK (res);        // UNDO invoked successfully
          CHECK (!protocolled(randVal_));
          CHECK (protocolled("UNDO"));
          
          blowUp_ = true;
          string current = command2::check_.str();
          
          res = com();
          CHECK (!res);       // not executed successfully (exception thrown)
          CHECK (command2::check_.str() == current);
          CHECK (LUMIERA_ERROR_EXTERNAL == lumiera_error());
          
          res = com.undo();
          CHECK (!res);       // UNDO failed (exception thrown)
          CHECK (command2::check_.str() == current);
          CHECK (LUMIERA_ERROR_EXTERNAL == lumiera_error());
          
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
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandUse2_test, "function controller");
      
      
}}} // namespace proc::control::test
