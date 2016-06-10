/*
  DiffVirtualisedApplication(Test)  -  apply structural changes to unspecific private data structures

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
#include "lib/diff/tree-diff-application.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"  //////////TODO necessary?
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <string>
#include <vector>

using lib::iter_stl::snapshot;
using util::isnil;
using util::join;
using std::string;
using std::vector;
using lib::time::Time;


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
                  GAMMA_PI("γ", 3.14159265);               // happens to have the same identity (ID) as ATTRIB3AS
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /***********************************************************************//**
   * @test Demonstration: apply a structural change to unspecified private
   *       data structures, with the help of an [dynamic adapter](\ref TreeMutator)
   *       - we use private data classes, defined here in the test fixture
   *         to represent "just some" pre-existing data structure.
   *       - we re-assign some attribute values
   *       - we add, re-order and delete child "elements", without knowing
   *         what these elements actually are and how they are to be handled.
   *       - we recurse into mutating such an _"unspecified"_ child element.
   * 
   * @todo this test defines a goal!! What has to be done, is to invent some test data structure and then run the exiting diffs against it. This requires the TreeMutator implementation to be finished!!!
   * @note this test uses the same verb sequence as is assumed for the
   *       coverage of diff building blocks in TreeMutatorBinding_test
   * 
   * @see DiffTreeApplication_test generic variant of tree diff application
   * @see TreeMutatorBinding_test coverage of the "building blocks"
   * @see TreeMutator_test base operations of the adapter
   * @see tree-diff-mutator-binding.hpp
   * @see diff-tree-application.hpp
   * @see tree-diff.hpp
   */
  class DiffVirtualisedApplication_test
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
        }                // ==> ATTRIB1, ATTRIB3, ATTRIB3, CHILD_B, CHILD_B, CHILD_T
      
      DiffSeq
      reorderingDiff()
        {
          return snapshot({find(ATTRIB3)
                         , pick(ATTRIB1)
                         , skip(ATTRIB3)
                         , ins(ATTRIB2)
                         , pick(ATTRIB3)
                         , del(CHILD_B)
                         , ins(SUB_NODE)
                         , pick(CHILD_B)
                         , pick(CHILD_T)
                         });
        }                // ==> ATTRIB3, ATTRIB1, ATTRIB2, ATTRIB3, SUB_NODE, CHILD_B, CHILD_T
        
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
        }                // ==> ATTRIB3 := π, ATTRIB1, ATTRIB2, ATTRIB3,
                         //     ATTRIB_NODE{ type ζ, CHILD_A, CHILD_A, CHILD_A }
                         //     SUB_NODE{ type ξ, ATTRIB2, CHILD_B, CHILD_A },
                         //     CHILD_B, CHILD_T,
      
      
      
      
      virtual void
      run (Arg)
        {
          /////////////////////////////TODO we need a suitable test datastructure. What follows is just placeholder code. As of 4/2016, this test waits for the completion of the TreeMutator
          Rec::Mutator target;
          Rec& subject = target;
          DiffApplicator<Rec::Mutator> application(target);
          //
          // TODO verify results
          cout << "before..."<<endl << subject<<endl;
          
          // Part I : apply attribute changes
          application.consume(populationDiff());
          //
          // TODO verify results
          cout << "after...I"<<endl << subject<<endl;
          
          // Part II : apply child population
          application.consume(reorderingDiff());
          //
          // TODO verify results
          cout << "after...II"<<endl << subject<<endl;
          
          // Part II : apply child mutations
          application.consume(mutationDiff());
          //
          // TODO verify results
          cout << "after...III"<<endl << subject<<endl;
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffVirtualisedApplication_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
