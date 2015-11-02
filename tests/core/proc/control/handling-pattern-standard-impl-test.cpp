/*
  HandlingPatternStandardImpl(Test)  -  cover the provided standard command handling patterns

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
//#include "lib/test/test-helper.hpp"
//#include "proc/asset/media.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/edl.hpp"
//#include "proc/mobject/session/testclip.hpp"
//#include "proc/mobject/test-dummy-mobject.hpp"
//#include "lib/p.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/session/placement-index.hpp"
//#include "proc/mobject/explicitplacement.hpp"
#include "proc/control/command.hpp"
#include "proc/control/command-impl.hpp"
#include "proc/control/command-registry.hpp"
//#include "proc/control/command-def.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

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
  using std::function;
//  using std::bind;
//  using std::string;
  //using std::rand;
  //using std::cout;
  //using std::endl;
//  using lib::test::showSizeof;
//  using util::isSameObject;
//  using util::contains;

//  using session::test::TestClip;
//  using lib::Symbol;
//  using lumiera::P;
  
  
  //using lib::meta::BuildTupleAccessor;
//  using lumiera::error::LUMIERA_ERROR_EXTERNAL;
  
  namespace { // test data and helpers...
  
//  Symbol TEST_CMD  = "test.command1.1";
//  Symbol TEST_CMD2 = "test.command1.2";
  }
  
  
  
  /*******************************************************************//**
   * @test verify correct behaviour of all the command handling patterns
   *       provided by the default configuration of a lumiera session.
   *       - executing quasi synchronous
   *       - the throw-on-error variant
   *       - background execution
   *       - ...?
   * @todo define and code those handling patterns; Ticket #210
   * 
   * @see HandlingPattern
   * @see CommandRegistry
   * @see command.hpp
   * @see command-basic-test.cpp
   */
  class HandlingPatternStandardImpl_test : public Test
    {
      
      uint cnt_inst;
      
      
      virtual void
      run (Arg) 
        {
          CommandRegistry& registry = CommandRegistry::instance();
          CHECK (&registry);
          
          cnt_inst = registry.instance_count();
          
          UNIMPLEMENTED ("unit test to cover the standard command handling patterns");
          
          CHECK (cnt_inst == registry.instance_count());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (HandlingPatternStandardImpl_test, "function controller");
  
  
}}} // namespace proc::control::test
