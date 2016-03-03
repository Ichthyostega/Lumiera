/*
  TreeManipulationBinding(Test)  -  techniques to map generic changes to concrete tree shaped data

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/format-util.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/diff/tree-mutator.hpp"
#include "lib/diff/test-mutation-target.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

//#include <utility>
#include <string>
//#include <vector>

using util::isnil;
using lib::time::Time;
using std::string;
//using std::vector;
//using std::swap;

using util::typeStr;


namespace lib {
namespace diff{
namespace test{
  
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  
  namespace {//Test fixture....
    
    // define some GenNode elements
    // to act as templates within the concrete diff
    // NOTE: everything in this diff language is by-value
    const GenNode ATTRIB1("α", 1),                         // attribute α = 1
                  ATTRIB2("β", int64_t(2)),                // attribute α = 2L   (int64_t)
                  ATTRIB3("γ", 3.45),                      // attribute γ = 3.45 (double)
                  TYPE_X("type", "ξ"),                     // a "magic" type attribute "Xi"
                  TYPE_Z("type", "ζ"),                     // 
                  CHILD_A("a"),                            // unnamed string child node
                  CHILD_B('b'),                            // unnamed char child node
                  CHILD_T(Time(12,34,56,78)),              // unnamed time value child
                  SUB_NODE = MakeRec().genNode(),          // empty anonymous node used to open a sub scope
                  ATTRIB_NODE = MakeRec().genNode("δ"),    // empty named node to be attached as attribute δ
                  CHILD_NODE = SUB_NODE;                   // yet another child node, same ID as SUB_NODE (!)
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /********************************************************************************//**
   * @test Building blocks to map generic changes to arbitrary private data structures.
   *       - use a dummy diagnostic implementation to verify the interface
   *       - integrate the standard case of tree diff application to `Rec<GenNode>`
   *       - verify an adapter to apply structure modification to a generic collection
   *       - use closures to translate mutation into manipulation of private attribues
   * 
   * @see TreeMutator
   * @see TreeMutator_test
   * @see DiffTreeApplication_test
   * @see GenNodeBasic_test
   * @see AbstractTangible_test::mutate()
   */
  class TreeManipulationBinding_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          mutateDummy();
          mutateGenNode();
          mutateCollection();
          mutateAttributeMap();
        }
      
      
      void
      mutateDummy()
        {
          TestMutationTarget target;
          auto mutator =
          TreeMutator::build()
            .attachDummy (target);
          
          CHECK (isnil (target));
          
          mutator.injectNew (ATTRIB1);
          CHECK (!isnil (target));
          CHECK (target.contains("α = 1"));
          CHECK (target.verifyEvent("insert","α = 1")
                       .after("attachMutator"));
        }
      
      
      void
      mutateGenNode()
        {
          TODO ("define how to fit GenNode tree mutation into the framework");
        }
      
      
      void
      mutateCollection()
        {
          TODO ("define how to map the mutation primitives on a generic collection");
        }
      
      
      void
      mutateAttributeMap ()
        {
          TODO ("define how to translate generic mutation into attribute manipulation");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TreeManipulationBinding_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
