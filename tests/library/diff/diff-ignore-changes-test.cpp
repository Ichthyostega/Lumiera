/*
  DiffIgnoreChanges(Test)  -  apply diff to black hole

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file diff-ignore-changes-test.cpp
 ** unit test \ref DiffIgnoreChanges_test.
 ** Covers the special case of a TreeMutator configured to accept
 ** any diff without actually doing anything.
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/format-util.hpp"
#include "lib/diff/tree-diff-application.hpp"
#include "lib/diff/test-mutation-target.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <string>

using lib::iter_stl::snapshot;
using lib::time::Time;
using std::string;


namespace lib {
namespace diff{
namespace test{
  
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
                  GAMMA_PI("γ", 3.14159265);               // happens to have the same identity (ID) as ATTRIB3
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  /**************************************************************************//**
   * @test Special case: build a TreeMutator to accept any change and do nothing.
   * 
   * @note for sake of symmetry, this test uses the same verb sequence used in
   *       all the other tree diff tests, assuming this sequence covers pretty
   *       much all features supported by the tree diff language.
   * 
   * @see DiffComplexApplication_test test case which _indeed does a lot..._
   * @see TreeMutator_test base operations of the adapter
   * @see diff-tree-application.hpp
   * @see tree-diff.hpp
   */
  class DiffIgnoreChanges_test
    : public Test
    , TreeDiffLanguage
    {
      using DiffSeq = iter_stl::IterSnapshot<DiffStep>;
      
      DiffSeq
      populationDiff()
        {
          return snapshot({ins(ATTRIB1)
                         , ins(ATTRIB3)
                         , ins(ATTRIB3)
                         , ins(CHILD_B)
                         , ins(CHILD_B)
                         , ins(CHILD_T)
                         });
        }                // ==> ATTRIB1, ATTRIB3, (ATTRIB3), CHILD_B, CHILD_B, CHILD_T
      
      DiffSeq
      reorderingDiff()
        {
          return snapshot({after(Ref::ATTRIBS)
                         , ins(ATTRIB2)
                         , del(CHILD_B)
                         , ins(SUB_NODE)
                         , find(CHILD_T)
                         , pick(CHILD_B)
                         , skip(CHILD_T)
                         });
        }                // ==> ATTRIB1, ATTRIB3, (ATTRIB3), ATTRIB2, SUB_NODE, CHILD_T, CHILD_B
        
      DiffSeq
      mutationDiff()
        {
          return snapshot({after(CHILD_B)
                         , after(Ref::END)
                         , set(GAMMA_PI)
                         , mut(SUB_NODE)
                           , ins(TYPE_X)
                           , ins(ATTRIB2)
                           , ins(CHILD_B)
                           , ins(CHILD_A)
                         , emu(SUB_NODE)
                         , ins(ATTRIB_NODE)
                         , mut(ATTRIB_NODE)
                           , ins(TYPE_Z)
                           , ins(CHILD_A)
                           , ins(CHILD_A)
                           , ins(CHILD_A)
                         , emu(ATTRIB_NODE)
                         });
        }                // ==> ATTRIB1, ATTRIB3 := π, (ATTRIB3), ATTRIB2,
                         //     ATTRIB_NODE{ type ζ, CHILD_A, CHILD_A, CHILD_A }
                         //     SUB_NODE{ type ξ, ATTRIB2, CHILD_B, CHILD_A },
                         //     CHILD_T, CHILD_B
      
      
      
      
      virtual void
      run (Arg)
        {
          fail_or_ignore();
          fish_for_content();
        }
      
      
      /** @test fail or ignore, depending on toggle.
       * - the TreeMutator default implementation produces a failure,
       *   when it actually has to handle some diff verb
       * - yet a custom TreeMutator can be configured to `ignoreAllChanges()`,
       *   in which case it will consume any diff without effect.
       * This test also demonstrates that the actual TreeMutator is built anew
       * for each diff application (TreeMutator is meant to be disposable).
       * Thus we may alter the behaviour of the diff binding dynamically.
       * @remark the actual use case for this is the TimelineGui, which
       *         either forwards changes to a TimelineWidget, or silently
       *         ignores them when the corresponding timeline is not opened.
       */
      void
      fail_or_ignore()
        {
          struct HappyBlackHole
            {
              bool diligent = true;
              
              void
              buildMutator (TreeMutator::Handle buff)
                {
                  if (diligent)
                    buff.create(
                      TreeMutator());
                  else
                    buff.create(
                      TreeMutator::build()
                        .ignoreAllChanges());
                }
            };
          
          HappyBlackHole subject;
          DiffApplicator<HappyBlackHole> application(subject);
          
          VERIFY_ERROR(DIFF_CONFLICT, application.consume(populationDiff()) );
          VERIFY_ERROR(DIFF_CONFLICT, application.consume(reorderingDiff()) );
          VERIFY_ERROR(DIFF_CONFLICT, application.consume(mutationDiff()) );
          
          subject.diligent = false;
          
          application.consume(populationDiff());
          application.consume(reorderingDiff());
          application.consume(mutationDiff());
        }
      
      
      /** @test fish some content and ignore everything else.
       * While the BlackHoleMutation binding generated by `ignoreAllChanges()`
       * must be used as bottom layer of a custom TreeMutator, it is possible
       * to layer a partial diff binding on top. This test demonstrates this
       * with a single attribute binding, which just "fishes" any value
       * mentioned in the diff for that specific attribute. Any other
       * changes are silently ignored non the less...
       */
      void
      fish_for_content()
        {
          struct Scrounger
            {
              double loot = 0.0;
              
              void
              buildMutator (TreeMutator::Handle buff)
                {
                  buff.create(
                    TreeMutator::build()
                      .ignoreAllChanges()
                      .change("γ", [&](double val) { loot = val; }));
                }
            };
          
          Scrounger subject;
          DiffApplicator<Scrounger> application(subject);
          
          const auto NOTHING = 0.0;
          const auto VAL_1   = ATTRIB3.data.get<double>();
          const auto VAL_2   = GAMMA_PI.data.get<double>();
          
          CHECK (subject.loot == NOTHING);
          
          application.consume(populationDiff());
          CHECK (subject.loot == VAL_1);
          
          application.consume(reorderingDiff());
          CHECK (subject.loot == VAL_1);
          
          application.consume(mutationDiff());
          CHECK (subject.loot == VAL_2);
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffIgnoreChanges_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
