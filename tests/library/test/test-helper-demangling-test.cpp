/*
  TestHelperDemangling(Test)  -  ensure a helper for C++ demangling works as expected

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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

#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;


namespace lib {
namespace test{
namespace test{
  
  template<class T>
  struct Outer
    {
      struct Inner { };
      
      static const T*
      phantom (Inner const&&)
        {
          return nullptr;
        }
    };
  
  struct Space { };
  
  
  
  
  /**********************************************//**
   * @test verify the demangling of C++ names,
   *       as available through the GCC platform ABI.
   *       The Lumiera support library exposes this
   *       non-portable feature through a convenience
   *       helper to ease the writing of unit tests.
   *       
   * @see test-helper.hpp
   */
  class TestHelperDemangling_test : public Test
    {
      void
      run (Arg)
        {
          Outer<Space> ship;
          auto magic = &ship.phantom;
          
          cout << showType(magic) << endl;
          cout << demangleCxx(showType(magic)) << endl;
        }
    };
  
  LAUNCHER (TestHelperDemangling_test, "unit common");
  
  
}}} // namespace lib::test::test
