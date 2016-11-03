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

/** @file §§§
 ** unit test TODO §§§
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"
#include "gui/model/session-facade.hpp"
#include "lib/format-cout.hpp"


namespace gui {
namespace test{
  
  /**************************************//**
   * Hellooooooo the world is just a test
   * @test demo of using the test framework
   */
  class TestGui_test : public Test
    {
      virtual void
      run (Arg)
        {
          cout << model::SessionFacade::beCreative() << "\n";
        } 
    };
  
  
  
  
  
  /** Register this test class to be invoked in some test groups (suites) 
   * @remarks this macro \c LUNCHER is defined in run.hpp to simplify
   *          the registration of test classes. It expands to the
   *          following static variable definition
   *          \code
   *          Launch<HelloWorld_test> run_HelloWorld_test("HelloWorld_test","unit common");
   *          \endcode
   */
  LAUNCHER (TestGui_test, "unit gui");
  
  
  
}} // namespace gui::test
