/*
  StreamTypeLifecycle(Test)  -  check lifecycle of the stream type registration

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file stream-type-lifecycle-test.cpp
 ** unit test \ref StreamTypeLifecycle_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "proc/control/stypemanager.hpp"
#include "proc/mobject/session.hpp"
//#include "teststreamtypes.hpp"
#include "include/lifecycle.h"




namespace proc {
namespace test_format {
  
  using mobject::Session;
  using control::STypeManager;
  using control::ON_STREAMTYPES_RESET;
  using lumiera::LifecycleHook;
  
  
  //////////TODO define a dummy-type-info here
  //////////TODO
  
  void 
  setup_basicDummyTypeInfo () 
    { 
      UNIMPLEMENTED ("setup basic dummy-type-info"); 
    }
  
  namespace // enrol this basic setup to be triggered when the type system is reset 
    {
      LifecycleHook _schedule_at_reset (ON_STREAMTYPES_RESET, &setup_basicDummyTypeInfo);         
    }
  
  
  
  /***************************************************************//**
   * @test check the stream type registration lifecycle. 
   *       Any internal or external component (plugin) can extend
   *       the Steam Layer's registry of media stream types.
   *       There is a basic pristine set of type information, which is
   *       restored automatically every time the STypeManager is reset,
   *       which in turn happens before loading a (new) Session.
   */
  class StreamTypeLifecycle_test : public Test
    {
      virtual void
      run (Arg arg)
        {
          check_pristineState ();
          register_additional_TypeInfo ();
          check_pristineState ();
        }
      
      
      /** @test this test defines a new (dummy) type info
       *  and schedules it for setup in the pristine state;
       *  check this info is actually present after resetting
       *  the stream type manager, while other additional info
       *  \em not scheduled in this manner is not present 
       *  in this state
       */
      void
      check_pristineState ()
        {
          Session::current.reset();
          TODO ("verify the test-dummy basic type info is present now...");
          TODO ("verify the additional type info is *not* present");
        }
      
      /** @test use the stream type manager to register additional
       *  type info and verify it is used in type resolution.
       */
      void
      register_additional_TypeInfo ()
        {
          TODO ("verify the additional type info is *not* present");
          
          STypeManager& typeManager = STypeManager::instance();
          TODO ("use the registration facility to add additional type info");
          TODO ("verify the additional type info to be present now...");
        }
    };
  
  LAUNCHER (StreamTypeLifecycle_test, "unit common");
  
  
}} // namespace proc::test_format

