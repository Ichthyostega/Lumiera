/*
  IosSavepoint(Test)  -  manipulate and restore output stream formatting

  Copyright (C)         Lumiera.org
    2022,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file ios-savepoint-test.cpp
 ** unit test \ref IosSavepoint_test
 */


#include "lib/test/run.hpp"
#include "lib/format-cout.hpp"
#include "lib/ios-savepoint.hpp"

#include <string>


using std::string;


namespace util {
namespace test {
  
  
  
  
  /***************************************************************************//**
   * @test verifies capturing and restoring of std::ostream formatting state.
   * @see ios-savepoint.hpp
   */
  class IosSavepoint_test
    : public Test
    {
      void
      run (Arg)
        {
          switchToHex();
          __verifySane();
          
          setFill();
          __verifySane();
          
          nested();
          __verifySane();
          
          try {
            restoreAfterException();
          } catch(...){
            cout << "Tilt" <<endl;
          }
          __verifySane();
        }
      
      
      /** verify that original state is restored */
      void
      __verifySane()
        {
          cout << std::setw(10) << 42 <<endl;
        }
      
      
      /** @test verify that hexadecimal output is cleared  */
      void
      switchToHex()
        {
          IosSavepoint save{cout};
          cout << std::hex
               << std::showbase
               << 42 <<endl;
        }
      
      
      /** @test verify that a custom fill character is cleared */
      void
      setFill()
        {
          IosSavepoint save{cout};
          cout << std::setfill('*')
               << std::setw(20)
               << 42 <<endl;
        }
      
      
      /** @test verify usage in nested scopes */
      void
      nested()
        {
          IosSavepoint save{cout};
          cout << std::hex << 42 <<endl;
          {
            IosSavepoint inner(cout);
            cout << std::oct << std::showbase << 42 <<endl;
          }
          cout << 42 <<endl;
        }
      
      
      /** @test verify clean-up happens even in case of an exception */
      void
      restoreAfterException()
        {
          auto boom = []() -> float
                      { throw 42; };
          
          IosSavepoint save{cout};
          cout << std::hexfloat
               << 1234
               << endl
               << boom()
               << endl;
        }
    };
  
  LAUNCHER (IosSavepoint_test, "unit common");
  
  
}} // namespace util::test

