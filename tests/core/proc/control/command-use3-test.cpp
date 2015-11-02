/*
  CommandUse3(Test)  -  usage aspects III

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
#include "proc/control/command-def.hpp"
#include "lib/util.hpp"

#include "proc/control/test-dummy-commands.hpp"

//#include <functional>
//#include <boost/ref.hpp>
//#include <boost/format.hpp>
//#include <iostream>
//#include <cstdlib>
//#include <string>


namespace proc {
namespace control {
namespace test    {


//  using boost::format;
//  using boost::str;
  //using util::contains;
//  using std::function;
//  using std::bind;
//  using std::string;
  //using std::rand;
  //using std::cout;
  //using std::endl;
//  using lib::test::showSizeof;
//  using util::isSameObject;
//  using util::contains;
  
  
  
  
  
  
  
  /***********************************************************************//**
   * @test command usage aspects III: elaborate handling patterns, like e.g.
   *       asynchronous or repeated invocation and command sequence bundles.
   *
   * @todo planned but not implemented as of 7/09
   *        
   * @see HandlingPattern
   */
  class CommandUse3_test : public Test
    {
    
      
      virtual void
      run (Arg) 
        {
          command1::check_ = 0;
          uint cnt_defs = Command::definition_count();
          uint cnt_inst = Command::instance_count();
          
          // prepare a command definition (prototype)
          CommandDef ("test.command1.1")
              .operation (command1::operate)
              .captureUndo (command1::capture)
              .undoOperation (command1::undoIt);

          UNIMPLEMENTED ("more elaborate command handling patterns");
          ////////////////////////////////////////////////////////////////////////////////TODO: devise tests for async, repeated and compound sequences
          
          CHECK (cnt_inst == Command::instance_count());
          
          Command::remove ("test.command1.1");
          CHECK (cnt_defs == Command::definition_count());
        }
      
      
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (CommandUse3_test, "function controller");
      
      
}}} // namespace proc::control::test
