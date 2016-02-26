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
                  TYPE_X("type", "X"),                     // a "magic" type attribute "X"
                  TYPE_Y("type", "Y"),                     // 
                  CHILD_A("a"),                            // unnamed string child node
                  CHILD_B('b'),                            // unnamed char child node
                  CHILD_T(Time(12,34,56,78)),              // unnamed time value child
                  SUB_NODE = MakeRec().genNode(),          // empty anonymous node used to open a sub scope
                  ATTRIB_NODE = MakeRec().genNode("δ"),    // empty named node to be attached as attribute δ
                  CHILD_NODE = SUB_NODE;                   // yet another child node, same ID as SUB_NODE (!)
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /***********************************************************************//**
   * @test Demonstration: apply a stuctural change to unspecified private
   *       data structures, with the help of an [dynamic adapter](\ref TreeMutator)
   *       - we use private data classes, defined here in the test fixture
   *         to represent "just some" pre-existing data structure.
   *       - we re-assign some attribute values
   *       - we add, re-order and delete child "elements", without knowing
   *         what these elements actually are and how they are to be handled.
   *       - we recurse into mutating such an _"unspecified"_ child element.
   * @see DiffTreeApplication_test generic variant of tree diff application
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
      attributeDiff()
        {
          // prepare for direkt attribute assignement
          GenNode attrib1_mut(ATTRIB1.idi.getSym(), 11);
          
          return snapshot({ins(TYPE_X)
                         , pick(ATTRIB1)
                         , del(ATTRIB2)
                         , ins(ATTRIB3)
                         , set(attrib1_mut)
                         });
        }
      
      DiffSeq
      populationDiff()
        {
          return snapshot({ins(ATTRIB2)
                         , ins(CHILD_A)
                         , ins(CHILD_A)
                         , ins(CHILD_T)
                         , ins(CHILD_T)
                         });
        }
        
        
      DiffSeq
      mutationDiff()
        {
          // prepare for direkt assignement of new value
          GenNode childT_later(CHILD_T.idi.getSym()
                              ,Time(CHILD_T.data.get<Time>() +  Time(0,1)));
          
          return snapshot({after(Ref::ATTRIBS)      // fast forward to the first child
                         , pick(CHILD_A)            // rearrange childern of mixed types...
                         , find(CHILD_T)
                         , set(childT_later)        // immediately assign to the child just picked
                         , find(CHILD_T)            // fetch the other Time child
                         , del(CHILD_A)             // delete a child of type Y
                         , skip(CHILD_T)
                         , skip(CHILD_T)
                         , mut(CHILD_A)             // mutate a child of type Y referred by ID
                           , ins(ATTRIB3)
                           , ins(CHILD_T)
                         , emu(CHILD_A)
                         });
        }
      
      
      virtual void
      run (Arg)
        {
          Rec::Mutator target;
          Rec& subject = target;
          DiffApplicator<Rec::Mutator> application(target);
          
          // Part I : apply attribute changes
          application.consume(populationDiff());
          
          // Part II : apply child population
          application.consume(mutationDiff());
          
          // Part II : apply child mutations
          application.consume(mutationDiff());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffVirtualisedApplication_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
