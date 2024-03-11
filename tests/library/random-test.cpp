/*
  Random(Test)  -  verify framework for controlled random number generation

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file random-test.cpp
 ** unit test \ref Random_test
 */



#include "lib/test/run.hpp"
#include "lib/rational.hpp"
#include "lib/test/diagnostic-output.hpp"/////////////////////////TODO


#include <chrono>
#include <array>

using std::array;


namespace util {
namespace test {
  
  
  /***************************************************************************//**
   * @test demonstrate simple access to random number generation, as well as
   *       the setup of controlled random number sequences.
   * @see  random.hpp
   */
  class Random_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
        }
      
      
      /**
       * @test demonstrate usage of default random number generators
       */
      void
      simpleUsage()
        {
        }
    };
  
  LAUNCHER (Random_test, "unit common");
  
  
}} // namespace util::test
