/*
  GenNodeBasic(Test)  -  fundamental properties of a generic tree node element

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/diff/gen-node.hpp"
#include "lib/diff/record.hpp"
#include "lib/util.hpp"

//#include <utility>
//#include <string>
//#include <vector>
#include <cmath>

using util::contains;
//using std::string;
//using std::vector;
//using std::swap;
using std::fabs;


namespace lib {
namespace diff{
namespace test{
  
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  using error::LUMIERA_ERROR_WRONG_TYPE;
  
  namespace {//Test fixture....
    
    
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /*****************************************************************************//**
   * @test Verify properties of a special collection type shaped for
   *       external representation of object-like data.
   *       
   * @see IndexTable
   * @see DiffListApplication_test
   */
  class GenNodeBasic_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verifySnapshot();
          sequenceIteration();
          duplicateDetection();
          copy_and_move();
        }
      
      
      void
      simpleUsage()
        {
          // can build from the supported value types
          GenNode n1(42);
          CHECK (42 == n1.data.get<int>());
          CHECK (!n1.isNamed());
          CHECK (contains (n1.idi.getSym(), "_CHILD_"));
          
          // can optionally be named
          GenNode n2("π", 3.14159265358979323846264338328);
          CHECK (fabs (3.14159265 - n2.data.get<double>) < 1e-5 );
          CHECK (n2.isNamed());
          CHECK ("π" == n2.idi.getSym());
          
          // is a copyable value
          GenNode n11(n1);
          CHECK (n1 == n11);
          CHECK (42 == n11.data.get<int>());
          
          // is assignable with compatible payload value
          n11.data = 24;
          CHECK (n1 != n11);
          CHECK (24 == n11.data.get<int>());
          CHECK (42 == n1.data.get<int>());
          
          // is assignable within the same kind of value
          n1 = n11;
          CHECK (n1 == n11);
          
          // but assignment may not alter payload type
          VERIFY_ERROR (WRONG_TYPE, n1 = n2 );
          
          // can build recursive data structures
          GenNode n3(Rec({"spam", GenNode("ham", "eggs")}));
          CHECK ("spam" == n3.data.get<Rec>().getType());
          CHECK ("eggs" == n3.data.get<Rec>().get("ham").data.get<string>());
          CHECK ("ham"  == n3.data.get<Rec>().get("ham").idi.getSym());
          CHECK (n3.data.get<Rec>().get("ham").isNamed());
          CHECK (!n3.isNamed());
        }
      
      
      void
      verifySnapshot()
        {
        }
      
      
      void
      sequenceIteration()
        {
        }
      
      
      void
      duplicateDetection()
        {
        }
      
      
      void
      copy_and_move()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (GenNodeBasic_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
