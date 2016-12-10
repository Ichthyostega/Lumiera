/*
  DummySessionConnection(Test)  -  verify scaffolding to drive the GUI-Session connection

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file dummy-session-connection-test.cpp
 ** 
 */

#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "proc/mobject/session/dummy-session-connection.hpp"

//#include <boost/lexical_cast.hpp>
//#include <iostream>
#include <string>
#include <map>

//using boost::lexical_cast;
using util::contains;
using std::string;
//using std::cout;
//using std::endl;

namespace proc    {
namespace mobject {
namespace session {
namespace test    {
  
  
  namespace { // test fixture...
  
  } //(End) test fixture
  
  
  /***********************************************************************//**
   * @test verify scaffolding used to develop the actual connection between
   *       the UI, the UI-Bus, the core services in the UI and the Session.
   *       - weakness of WIP-WIP-WIP
   *       
   * @see UiBus
   * @see DummySessionConnection
   */
  class DummySessionConnection_test
    : public Test
    {
      
      virtual void
      run (Arg)
        {
          demonstrate_weakness ();
        }
      
      /** @test demonstrate a serious weakness of
       * When...
       * 
       * This problem is especially dangerous when...
       */
      void demonstrate_weakness ( )
        {
          UNIMPLEMENTED("stop making sense...");
        }
      
    };
  
  /** Register this test class... */
  LAUNCHER(DummySessionConnection_test, "unit session");
  
}}}} // namespace proc::mobject::session::test
