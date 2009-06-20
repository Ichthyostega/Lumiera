/*
  TestHelper(Test)  -  validate the unittest support functions
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
#include "lib/error.hpp"

#include <iostream>

using std::cout;
using std::endl;
using lumiera::Error;
using lumiera::LUMIERA_ERROR_EXCEPTION;
using lumiera::error::LUMIERA_ERROR_ASSERTION;


namespace lib {
namespace test{
namespace test{
  
  template<class T>
  class Wrmrmpft 
    {
      T tt_;
    };
  
  struct Murpf { };
  
  
  void doThrow() { throw Error("because I feel like it"); }
  int dontThrow() { return 2+2; }
  
  
  /*************************************************
   * verifies the proper working of helper functions
   * frequently used within the Lumiera testsuite.
   * @see test-helper.hpp
   */
  class TestHelper_test : public Test
    {
      void
      run (Arg)
        {
          checkTypeDisplay();
          checkThrowChecker();
        }
      
      
      /** @test prints "sizeof()" including some type name. */
      void
      checkTypeDisplay ()
        {
          std::cout << "Displaying types and sizes....\n";
          
          typedef Wrmrmpft<Murpf> Wrmpf1;
          typedef Wrmrmpft<char[2]> Wrmpf2;
          typedef Wrmrmpft<char[3]> Wrmpf3;
          
          Wrmpf1 rmpf1;
          Wrmpf2 rmpf2;
          Wrmpf3 rmpf3;
          Murpf murpf;
          
          ASSERT (1 == sizeof (rmpf1));
          ASSERT (2 == sizeof (rmpf2));
          ASSERT (3 == sizeof (rmpf3));
          
          cout << showSizeof((size_t)42, "theUniverse") << endl;
          cout << showSizeof<char>("just a char")       << endl;
          cout << showSizeof(murpf)                     << endl;
          cout << showSizeof(rmpf1)                     << endl;
          cout << showSizeof(rmpf2)                     << endl;
          cout << showSizeof<Wrmpf3>()                  << endl;
          
          Wrmpf1 *p1 = &rmpf1;
          Wrmpf1 *p2 = 0;
          cout << showSizeof(p1)  << endl;
          cout << showSizeof(p2)  << endl;
        }
      
      
      /** @test check the VERIFY_ERROR macro, 
       *        which ensures a given error is raised.
       */
      void
      checkThrowChecker()
        {
          // verify the exception is indeed raised
          VERIFY_ERROR (EXCEPTION, doThrow() );
          
#if false ////////////////////////////////////////////////////////TODO: restore throwing ASSERT
          // and when actually no exception is raised, this is an ASSERTION failure
          VERIFY_ERROR (ASSERTION, VERIFY_ERROR (EXCEPTION, dontThrow() ));
#endif    ////////////////////////////////////////////////////////
        }
      
    };
  
  LAUNCHER (TestHelper_test, "unit common");
  
  
}}} // namespace lib::test::test

