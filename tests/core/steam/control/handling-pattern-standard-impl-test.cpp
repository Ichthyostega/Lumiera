/*
  HandlingPatternStandardImpl(Test)  -  cover the provided standard command handling patterns

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file handling-pattern-standard-impl-test.cpp
 ** unit test \ref HandlingPatternStandardImpl_test
 */


#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"
//#include "steam/asset/media.hpp"
//#include "steam/mobject/session.hpp"
//#include "steam/mobject/session/edl.hpp"
//#include "steam/mobject/session/testclip.hpp"
//#include "steam/mobject/test-dummy-mobject.hpp"
//#include "lib/p.hpp"
//#include "steam/mobject/placement.hpp"
//#include "steam/mobject/session/placement-index.hpp"
//#include "steam/mobject/explicitplacement.hpp"
#include "steam/control/command.hpp"
#include "steam/control/command-impl.hpp"
#include "steam/control/command-registry.hpp"
//#include "steam/control/command-def.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/symbol.hpp"
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
  using std::function;
//  using std::bind;
//  using std::string;
  //using std::rand;
//  using lib::test::showSizeof;
//  using util::isSameObject;
//  using util::contains;

//  using session::test::TestClip;
//  using lib::Symbol;
//  using lumiera::P;
  
  
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
  
  
}}} // namespace steam::control::test
