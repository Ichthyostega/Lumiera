/*
  FormatHelper(Test)  -  validate formatting and diagnostics helpers

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


#include "lib/test/run.hpp"
#include "lib/format.hpp"
#include "lib/error.hpp"

#include <iostream>

using std::cout;
using std::endl;


namespace util {
namespace test {
  
  class Reticent 
    {
    };
  
  class UnReticent
    : public Reticent 
    {
    public:
      operator string()  const { return "hey Joe!"; }
    };
  
  
  
  /*************************************************
   * verifies the proper working of helper functions
   * frequently used within the Lumiera testsuite.
   * @see test-helper.hpp
   */
  class FormatHelper_test : public Test
    {
      void
      run (Arg)
        {
          check2String();
        }
      
      
      /** @test verify the maybe-to-string conversion. */
      void
      check2String ()
        {
          std::cout << "Displaying some types....\n";
          
          Reticent closeLipped;
          UnReticent chatterer;
          
          cout << str (closeLipped) << endl;
          cout << str (closeLipped, "he says: ", "<no comment>") << endl;
          
          cout << str (chatterer) << endl;
          cout << str (chatterer, "he says: ", "<no comment>") << endl;
          
          cout << str (false, "the truth: ") << endl;
          cout << str (12.34e55, "just a number: ") << endl;
          cout << str (short(12)) << str (345L) << str ('X') << endl;
        }
      
      
      
    };
  
  LAUNCHER (FormatHelper_test, "unit common");
  
  
}} // namespace util::test

