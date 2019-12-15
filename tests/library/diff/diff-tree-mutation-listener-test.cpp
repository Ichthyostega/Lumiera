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
#include "lib/util.hpp"

#include <boost/algorithm/string.hpp>
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
                  
                  VAL_C_UPPER{"C"},
                  VAL_D_UPPER{"D"};
    
    string
    contents (vector<string> const& strings)
    {
      return util::join (strings);
    }
    
    string
    lowerCase (string src)
    {
      return boost::algorithm::to_lower_copy(src); //WARNING: only works for ASCII
    }
    
    bool
    caseInsensitiveEqual (string a, string b)
    {
      return lowerCase (a) == lowerCase (b);
    }
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /****************************************************************************//**
   * @test When creating a TreeMutator binding, a listener (lambda) can be attached,
   *       to be invoked on structural changes...
   *       - inserting, removing and reordering of children counts as "structural" change
   *       - whereas assignment of a new value or nested mutation does not trigger
   * @note This test binds the test class itself for diff mutation, applying changes
   *       onto a vector with test data.  The binding itself is somewhat unusual,
   *       insofar it allows to re-assign elements within the vector, which can be
   *       identified and picked by equality match. In actual code, you would not
   *       do that, since typically you'd distinguish between attributes, which
   *       are marked by an identifier and can be reassigned, and children, which
   *       are treated as sequence and maybe re-ordered. However, the diff system
   *       as such does not enforce such conventions; if you want to find a
   *       sub-element, you need to provide a _matcher_ to identify it,
   *       given a suitable "spec" in the relevant diff verbs.
   * @remark Now the special rigging for this test is that we match case-insensitively,
   *       which allows to assign a different value, while this value still counts as
   *       "equal", as far as matching is concerned. We do all this trickery in order
   *       to apply a diff, which _changes values_ while not _changing the structure_.
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
      
      /** rig the test class itself to receive a diff mutation.
       *  - bind the #subject_ data collection to be changed by diff
       *  - attach a listener, to be invoked on _structural changes
       */
      void
      buildMutator (TreeMutator::Handle buff)  override
        {
          buff.create (
            TreeMutator::build()
              .attach (collection (subject_)
                        .matchElement ([](GenNode const& spec, string const& elm) -> bool
                                          {                                    // »Matcher« : what target string "matches" a diff spec?
                                            return caseInsensitiveEqual(elm, spec.data.get<string>());
                                          })
                        .assignElement ([](string& target, GenNode const& spec) -> bool
                                          {                                    // »Setter« : how to assign the value from the spec to the target
                                            target = spec.data.get<string>();
                                            return true;
                                          }))
              .onSeqChange ([&]()
                  {
                    ++structChanges_;    // Note: this lambda is the key point for this test
                  })
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
                                             , set (VAL_C_UPPER)    // Note: the current element is tried first, which happens to match
                                             , set (VAL_D_UPPER)    //       ...while in this case, a linear search finds the "d"
                                             }});
          CHECK ("a, c, D, C" == contents(subject_));
          CHECK (1 == structChanges_);                              // Note: the listener has not fired, since this counts as value change.
          
          applicator.consume (MutationMessage{{pick(VAL_A)
                                             , ins (VAL_B)
                                             , find(VAL_D)
                                             , pick(VAL_C)
                                             , skip(VAL_D)
                                             , del (VAL_C)
                                             }});
          CHECK ("a, b, D, c" == contents(subject_));
          CHECK (2 == structChanges_);                              // Note: this obviously is a structure change, so the listener fired.
          
          applicator.consume (MutationMessage{{after(Ref::END)
                                             }});
          CHECK ("a, b, D, c" == contents(subject_));
          CHECK (2 == structChanges_);                              // Note: contents confirmed as-is, listener not invoked.
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffTreeMutationListener_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
