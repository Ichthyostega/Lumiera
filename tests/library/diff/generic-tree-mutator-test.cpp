/*
  GenericTreeMutator(Test)  -  customisable intermediary to abstract tree changing operations

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
#include "lib/diff/tree-mutator.hpp"
#include "lib/util.hpp"

//#include <utility>
#include <string>
//#include <vector>
#include <iostream>

using util::isnil;
using std::string;
//using std::vector;
//using std::swap;
using std::cout;
using std::endl;

using lib::test::showType;
using lib::test::demangleCxx;


namespace lib {
namespace diff{
namespace test{
  
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  namespace {//Test fixture....
    
    
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /*****************************************************************************//**
   * @test Demonstrate a customisable component for flexible bindings
   *       to enable generic tree changing and mutating operations to
   *       arbitrary hierarchical data structures.
   *       
   * @see TreeMutator
   * @see GenNodeBasic_test
   * @see GenNodeBasic_test
   * @see GenericTreeRepresentation_test
   */
  class GenericTreeMutator_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleAttributeBinding();
          verifySnapshot();
          sequenceIteration();
          duplicateDetection();
          copy_and_move();
        }
      
      
      void
      simpleAttributeBinding()
        {
          string localData;
          auto mutator =
          TreeMutator::build()
            .change<string>("data", [&](string val)
              {
                cout << "\"data\" closure received something "<<val<<endl;
                localData = val;
              });
          
          cout << "concrete TreeMutator size=" << sizeof(mutator)
               << " type="<< demangleCxx (showType (mutator))
               << endl;
          
          CHECK (isnil (localData));
          Attribute testAttribute(string ("that would be acceptable"));
          mutator.setAttribute ("lore", testAttribute);
          CHECK ( isnil (localData)); // nothing happens, nothing changed
          mutator.setAttribute ("data", testAttribute);
          CHECK (!isnil (localData));
          cout << "localData changed to: "<<localData<<endl;
          CHECK (localData == "that would be acceptable");
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
  LAUNCHER (GenericTreeMutator_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
