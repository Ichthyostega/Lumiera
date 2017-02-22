/*
  OptionalRef(Test)  -  verify an optional and switchable object link

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file optional-ref-test.cpp
 ** unit test \ref OptionalRef_test
 */



#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"

#include "lib/optional-ref.hpp"



namespace lib {
namespace test{
  
  using ::Test;
  using lib::test::randStr;
  
  using std::string;
  
  
  
  /***************************************************************************//**
   * @test verify a reference-like wrapper class,
   *       used to provide a switchable link to an already existing object.
   *       - bottom ref can be detected by bool check
   *       - access to bottom ref raises exception
   *       - refs can be compared
   *       - refs can be changed and copied
   *       - access to refs is secure even after destructor invocation
   *       
   * @see  lib::OptionalRef
   * @see  lib::AutoRegistered usage example
   */
  class OptionalRef_test : public Test
    {
      
      
      virtual void
      run (Arg)
        {
          string s1 (randStr(50));
          string s2 (randStr(50));
          
          typedef OptionalRef<string> SRef;
          
          SRef r1;
          CHECK (!r1);
          VERIFY_ERROR (BOTTOM_VALUE, r1() );
          VERIFY_ERROR (BOTTOM_VALUE, s1 == r1 );
          VERIFY_ERROR (BOTTOM_VALUE, r1 == s1 );
          CHECK (!r1.points_to (s1));
          
          r1.link_to (s1);
          CHECK (r1);
          CHECK (r1 == s1);
          CHECK (s1 == r1);
          CHECK (r1.points_to (s1));
          
          SRef r2(s2);
          CHECK (r2);
          CHECK (r2 == s2);
          CHECK (r2.points_to (s2));
          CHECK (!r2.points_to (s1));
          CHECK (!r1.points_to (s2));
          CHECK (r2 != r1);
          CHECK (r1 != r2);
          
          r2.link_to (s1);
          CHECK (r2);
          CHECK (r2 == s1);
          CHECK (r2 == r1);
          CHECK (r1 == r2);
          CHECK (r2.points_to (s1));
          CHECK (!r2.points_to (s2));
          
          r2.clear();
          CHECK (!r2);
          VERIFY_ERROR (BOTTOM_VALUE, r2() );
          VERIFY_ERROR (BOTTOM_VALUE, s1 == r2 );
          VERIFY_ERROR (BOTTOM_VALUE, r2 == s1 );
          VERIFY_ERROR (BOTTOM_VALUE, r2 == s2 );
          
          CHECK (r1 != r2); // comparison with bottom ref allowed
          CHECK (r2 != r1);
          
          //OptionalRef objects are copyable values
          r2 = r1;
          CHECK (r2);
          CHECK (r2 == r1);
          CHECK (r1 == r2);
          CHECK (r2 == s1);
          
          r1.link_to (s2);
          CHECK (r2 != r1); // but they are indeed independent instances
          CHECK (r1 != r2);
          CHECK (r2 == s1);
          CHECK (r2 != s2);
          CHECK (r1 == s2);
          
          SRef r3(r2);
          CHECK (r3);
          CHECK (r3 == r2);
          CHECK (r2 == r3);
          CHECK (r3 == s1);
          
          CHECK (r3 != r1);
          CHECK (r1 != r3);
          CHECK (r3 != s2);
          
          // access is secured even after destruction
          CHECK (r3);
          r3.~SRef();       // don't try this at home!
          CHECK (!r3);
          VERIFY_ERROR (BOTTOM_VALUE, r3 == s1 );
          CHECK (r3 != r2);
          
          r2.clear();
          CHECK (!r2);
          CHECK (r3 == r2);
          CHECK (r2 == r3);
        }
      
    };
  
  LAUNCHER (OptionalRef_test, "unit common");
  
  
}} // namespace lib::test
