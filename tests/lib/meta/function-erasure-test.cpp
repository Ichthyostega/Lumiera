/*
  FunctionErasure(Test)  -  verify the wrapping of functor object with type erasure
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file function-erasure-test.cpp
 ** bla
 **
 ** @see control::FunErasure
 ** @see command-mutation.hpp real world usage example
 **
 */


#include "lib/test/run.hpp"
#include "lib/meta/function-erasure.hpp"
#include "lib/util.hpp"

//#include <boost/format.hpp>
#include <iostream>

using ::test::Test;
using std::string;
using std::cout;


namespace lumiera {
namespace typelist{
namespace test {
  
  
  namespace { // test data
    
  } // (End) test data
  
  
  
  
  
  
  /*************************************************************************
   * @test //////////////////////////////////////////
   *       - building combinations and permutations
   */
  class FunctionErasure_test : public Test
    {
      virtual void run(Arg arg) 
        {
          check_distribute();
          check_combine();
        }
      
      
      void check_distribute()
        {
        }
      
      
      void check_combine()
        {
        }

      
    };
  
  
  /** Register this test class... */
  LAUNCHER (FunctionErasure_test, "unit common");
  
  
  
}}} // namespace lumiera::typelist::test
