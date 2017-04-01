/*
  BoolCheckable(Test)  -  verify the mixin for implicit conversion to bool

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

/** @file bool-checkable-test.cpp
 ** unit test \ref BoolCheckable_test
 */


#include "lib/test/run.hpp"
#include "lib/bool-checkable.hpp"
#include "lib/test/test-helper.hpp"

#include <iostream>

using std::cout;
using lib::test::showSizeof;


namespace lib {
namespace test{
  
  /** 
   * An example custom type,
   * which provides an implicit bool conversion
   * to express some special validity check. 
   */
  
  class ExampleCustomType123
    : public lib::BoolCheckable<ExampleCustomType123>
    {
      
      int val_;
      
    public:
      
      ExampleCustomType123 (int i)
        : val_(i)
        { }
      
      /** implements the custom logic to decide
       *  if this instance can be considered "valid"
       */
      bool
      isValid()  const
        {
          return val_ % 3;
        }
      
    };
  
  
  
  
  /******************************************************//**
   * @test verify the implementation of a validity check,
   *       to be accessed as an implicit conversion to bool.
   *       
   * @see lib::BoolCheckable
   * @todo list some usage examples here 
   */
  class BoolCheckable_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          for (int i=1; i<16; ++i)
            {
              ExampleCustomType123 test (i);
              
              if (!test)
                cout << "doIt \n";
              if (test)
                cout << i << "\n";
              
              CHECK (test.isValid() == bool(test) );
              CHECK (test.isValid() != !test);
            }
          
          cout << showSizeof<ExampleCustomType123>() <<"\n";
          CHECK (sizeof (int) == sizeof (ExampleCustomType123));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BoolCheckable_test, "unit common");
  
  
}} // namespace lib::test
