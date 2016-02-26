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
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

//#include <utility>
#include <string>
//#include <vector>

using util::isnil;
using std::string;
//using std::vector;
//using std::swap;

using util::typeStr;


namespace lib {
namespace diff{
namespace test{
  
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  namespace {//Test fixture....
    
    
    
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
