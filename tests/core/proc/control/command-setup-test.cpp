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
//#include "lib/time/timevalue.hpp"
//#include "lib/p.hpp"

//#include <cstdlib>

//using std::rand;


namespace proc {
namespace cmd  {
namespace test {
  
//using lib::time::Time;
//using lib::time::TimeVar;
//using lib::time::TimeValue;
//using lib::time::Offset;
  
  
  
  
  
  namespace  { // Test fixture....
    
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
          UNIMPLEMENTED("create CommandSetup instances");
        }
      
      
      /** @test actually issue the definitions captured as side-effect of the preceding test. */
      void
      verify_DefinitionRegistration()
        {
          UNIMPLEMENTED("trigger the collected lambdas");
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
