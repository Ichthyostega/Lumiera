/*
  CmdContext(Test)  -  verify access to command invocation state

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

/** @file cmd-context-test.cpp
 ** unit test \ref CmdContext_test
 ** 
 ** @todo created 3/2017 as a marker for planned concepts, postponed for now... 
 */


#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"
#include "stage/interact/cmd-context.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/diff/gen-node.hpp"
//#include "lib/util.hpp"

//#include <string>


//using std::string;
//using lib::idi::EntryID;
//using lib::diff::GenNode; 
//using util::isSameObject;
//using util::isnil;



namespace stage  {
namespace interact {
namespace test {
  
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  
  
  /******************************************************************************//**
   * @test verify the front-end for context bound commands, command argument binding
   *       and invocation state from within the UI structures.
   * 
   * @see CmdAccess
   * @see CommandSetup_test
   * @see SessionCommandFunction_test
   */
  class CmdContext_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          UNIMPLEMENTED ("context bound commands");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (CmdContext_test, "unit stage");
  
  
}}} // namespace stage::interact::test
