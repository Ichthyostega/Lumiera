/*
  TestGui(Test)  -  how to use this test framework...

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

/** @file test-gui-test.cpp
 ** unit test \ref TestGui_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"
#include "stage/model/session-facade.hpp"
#include "lib/format-cout.hpp"


namespace stage {
namespace test{
  
  /************************************************************************************//**
   * @test demo of writing a unit-test to execute code _implemented within the UI layer_.
   * @note the point in question here is the build system and how library dependencies
   *       are handled. Typically such unit-tests will rather cover framework aspects
   *       of the UI, not perform a remote controlled execution of the UI. Thus we
   *       still create a commandline executable, but for this code to work, it
   *       needs to be _linked against the GUI plugin_ (which is in fact a shared library)
   * @todo as of 11/2022 this is still a mere placeholder; we need a test setup for
   *       proper UI integration testing (without actually launching the UI) ////////////////////////////////TICKET #1257 : provide test-session for GUI
   */
  class TestGui_test : public Test
    {
      virtual void
      run (Arg)
        {
          cout << model::SessionFacade::beCreative() << "\n";
        } 
    };
  
  
  
  /** Register this test class to be invoked in some test groups */
  LAUNCHER (TestGui_test, "unit stage");
  
  
  
}} // namespace stage::test
