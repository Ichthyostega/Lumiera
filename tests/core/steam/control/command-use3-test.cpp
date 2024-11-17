/*
  CommandUse3(Test)  -  usage aspects III

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file command-use3-test.cpp
 ** unit test \ref CommandUse3_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/control/command-def.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/util.hpp"

#include "steam/control/test-dummy-commands.hpp"

//#include <functional>
//#include <boost/ref.hpp>
//#include <cstdlib>
//#include <string>


namespace steam {
namespace control {
namespace test    {


  //using util::contains;
//  using std::function;
//  using std::bind;
//  using std::string;
  //using std::rand;
//  using lib::test::showSizeof;
//  using util::isSameObject;
//  using util::contains;
  
  
  
  
  
  
  
  /***********************************************************************//**
   * @test command usage aspects III: elaborate handling patterns, like e.g.
   *       asynchronous or repeated invocation and command sequence bundles.
   *
   * @todo planned but not implemented as of 7/09
   * @todo as of 12/2016 I doubt we'll get asynchronous invocation
   *       but command sequence bundles still seem a reasonable future idea
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
      
      
}}} // namespace steam::control::test
