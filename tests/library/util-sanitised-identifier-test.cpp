/*
  UtilSanitizedIdentifier(Test)  -  remove non-standard-chars and punctuation

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

/** @file util-sanitised-identifier-test.cpp
 ** unit test \ref UtilSanitizedIdentifier_test
 */


#include "lib/test/run.hpp"
#include "lib/util.hpp"


#include <iostream>
using std::cout;



namespace util {
namespace test {
  
  
  class UtilSanitizedIdentifier_test : public Test
    {
      virtual void run (Arg)
        {
          print_clean ("Word");
          print_clean ("a Sentence");
          print_clean ("trailing Withespace\n       \t");
          print_clean ("with    a  \t lot\n  of Whitespace");
          print_clean ("@with\".\'much ($punctuation)[]!");
          print_clean ("§&Ω%€  leading garbage");
          print_clean ("mixed    Ω   garbage");
          print_clean ("Bääääh!!");
          print_clean ("§&Ω%€");
        }
      
      /** @test print the original and the sanitised string */
      void print_clean (const string org)
      {
        cout << "'" << org << "' --> '" << sanitise(org) << "'\n";
      }
    };
  
  LAUNCHER (UtilSanitizedIdentifier_test, "unit common");
  
  
}} // namespace util::test

