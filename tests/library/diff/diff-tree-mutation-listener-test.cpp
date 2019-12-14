/*
  DiffTreeMutationListener(Test)  -  verify notification on structural changes

  Copyright (C)         Lumiera.org
    2019,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file diff-tree-mutation-listener-test.cpp
 ** unit test \ref DiffTreeMutationListener_test.
 ** Verify a listener mechanism to be invoked on structural changes.
 */


#include "lib/test/run.hpp"
#include "lib/diff/mutation-message.hpp"
#include "lib/diff/tree-diff-application.hpp"
#include "lib/format-util.hpp"
#include "lib/format-cout.hpp"/////////////////////////TODO
#include "lib/util.hpp"

#include <string>
#include <vector>

using util::isnil;
using std::string;
using std::vector;


namespace lib {
namespace diff{
namespace test{
  
  namespace {//Test fixture....
    
    // some symbolic values to be used within the diff
    const GenNode VAL_A{"a"},
                  VAL_B{"b"},
                  VAL_C{"c"},
                  VAL_D{"d"},
                  
                  C_TO_B{"c", "B"};
    
    string
    contents (vector<string> const& strings)
    {
      return util::join (strings);
    }
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /****************************************************************************//**
   * @test When creating a TreeMutator binding, a listener (lambda) can be atteched,
   *       to be invoked on structural changes...
   *       - inserting, removing and reordering of children counts as "structural" change
   *       - whereas assignment of a new value or nested mutation does not trigger
   * @note This test binds the test class itself for diff mutation, applying changes
   *       onto a vector with test data.  The binding itself is somewhat unusual,
   *       insofar it allows to re-assign elements within the vector, can be
   *       identified and picked by equality match. In actual code, you would not
   *       do that, since typically you'd distinguish between attributes, which
   *       are marked by an identifier and can be reassigned, and children, which
   *       are treated as sequence and maybe re-ordered. However, the diff system
   *       as such does not enforce such conventions; if you want to find a
   *       sub-element, you need to provide a _matcher_ to identify it,
   *       given a suitable "spec" in the relevant diff verbs.
   * @see DiffTreeApplicationSimple_test introductory example demonstration
   * @see DiffTreeApplication_test extended demonstration of possible diff operations
   * @see DiffComplexApplication_test handling arbitrary data structures
   * @see GenericRecord_test
   * @see GenNode_test
   */
  class DiffTreeMutationListener_test
    : public Test
    , public DiffMutable
    , TreeDiffLanguage
    {
      std::vector<string> subject_;
      int structChanges_ = 0;
      
      void
      buildMutator (TreeMutator::Handle buff)  override
        {
          buff.create (
            TreeMutator::build()
              .attach (collection (subject_))
              .onStructuralChange ([&](){ ++structChanges_; })
            );
        }
      
      
      virtual void
      run (Arg)
        {
          CHECK (isnil (subject_));
          CHECK (0 == structChanges_);
          
          DiffApplicator<DiffTreeMutationListener_test> applicator{*this};
          
          applicator.consume (MutationMessage{{ins (VAL_A)
                                             , ins (VAL_C)
                                             , ins (VAL_D)
                                             , ins (VAL_C)
                                             }});
          CHECK ("a, c, d, c" == contents(subject_));
          CHECK (1 == structChanges_);
          
          applicator.consume (MutationMessage{{after(Ref::END)
                                             , set (C_TO_B)
                                             }});
          CHECK ("a, B, d, c" == contents(subject_));
          CHECK (1 == structChanges_);
          
          applicator.consume (MutationMessage{{pick(VAL_A)
                                             , ins (VAL_B)
                                             , find(VAL_C)
                                             , after(Ref::END)
                                             }});
          CHECK ("a, b, c, B, d" == contents(subject_));
          CHECK (2 == structChanges_);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffTreeMutationListener_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
