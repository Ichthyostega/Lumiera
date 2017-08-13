/*
  DiffTreeApplication(Test)  -  demonstrate the basics of tree diff representation

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

/** @file diff-tree-application-test.cpp
 ** unit test \ref DiffTreeApplication_test.
 ** Demonstrates the basic concept of reshaping structured data
 ** through a tree-diff sequence.
 */


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
   * @test Demonstration/Concept: a description language for tree differences.
   *       The representation is given as a linearised sequence of verb tokens.
   *       In addition to the verbs used for list diffing, here we additionally
   *       have to deal with nested scopes, which can be entered thorough a
   *       bracketing construct \c mut(ID)...emu(ID).
   *       This test demonstrates the application of such diff sequences
   *       - in the first step, an empty root #Record<GenNode> is populated
   *         with a type-ID, three named attribute values, three child values
   *         and a nested child-Record.
   *       - the second step demonstrates various diff language constructs
   *         to alter, reshape and mutate this data structure
   *       After applying those two diff sequences, we verify the data
   *       is indeed in the expected shape.
   * @remarks to follow this test, you should be familiar both with our
   *       [generic data record](\ref diff::Record), as well as with the
   *       [variant data node](\ref diff::GenNode). The key point to note
   *       is the usage of Record elements as payload within GenNode, which
   *       allows to represent tree shaped object like data structures.
   * @note literally the same test case is repeated in MutationMessage_test,
   *       just there the diff is transported in a MutationMessage capsule,
   *       as is the case in the real application as well.
   * @see DiffComplexApplication_test handling arbitrary data structures
   * @see GenericRecordRepresentation_test
   * @see GenNodeBasic_test
   * @see DiffListApplication_test
   * @see diff-tree-application.hpp
   * @see tree-diff.hpp
   */
  class DiffTreeApplication_test
    : public Test
    , TreeDiffLanguage
    {
      using DiffSeq = iter_stl::IterSnapshot<DiffStep>;
      
      DiffSeq
      populationDiff()
        {
          return snapshot({ins(TYPE_X)
                         , ins(ATTRIB1)
                         , ins(ATTRIB2)
                         , ins(ATTRIB3)
                         , ins(CHILD_A)
                         , ins(CHILD_T)
                         , ins(CHILD_T)
                         , ins(SUB_NODE)
                         , mut(SUB_NODE)
                           , ins(CHILD_B)
                           , ins(CHILD_A)
                         , emu(SUB_NODE)
                         });
        }
        
        
      DiffSeq
      mutationDiff()
        {
          // prepare for direct assignment of new value
          // NOTE: the target ID will be reconstructed, including hash
          GenNode childA_upper(CHILD_A.idi.getSym(), "A");
          
          return snapshot({after(Ref::ATTRIBS)      // fast forward to the first child
                         , find(CHILD_T)
                         , pick(CHILD_A)
                         , skip(CHILD_T)
                         , del(CHILD_T)
                         , after(Ref::END)          // accept anything beyond as-is
                         , mut(SUB_NODE)
                           , ins(ATTRIB3)
                           , ins(ATTRIB_NODE)       // attributes can also be nested objects
                           , find(CHILD_A)
                           , del(CHILD_B)
                           , ins(CHILD_NODE)
                           , ins(CHILD_T)
                           , skip(CHILD_A)
                           , mut(CHILD_NODE)
                             , ins(TYPE_Y)
                             , ins(ATTRIB2)
                           , emu(CHILD_NODE)
                           , set(childA_upper)      // direct assignment, target found by ID (out of order)
                           , mut(ATTRIB_NODE)       // mutation can be out-of order, target found by ID
                             , ins(CHILD_A)
                             , ins(CHILD_A)
                             , ins(CHILD_A)
                           , emu(ATTRIB_NODE)
                         , emu(SUB_NODE)
                         });
        }
      
      
      virtual void
      run (Arg)
        {
          Rec::Mutator target;
          Rec& subject = target;
          DiffApplicator<Rec::Mutator> application(target);
          
          // Part I : apply diff to populate
          application.consume (populationDiff());
          
          CHECK (!isnil (subject));                                    // nonempty -- content has been added
          CHECK ("X" == subject.getType());                            // type was set to "X"
          CHECK (1 == subject.get("α").data.get<int>());               // has gotten our int attribute "α"
          CHECK (2L == subject.get("β").data.get<int64_t>());          // ... the long attribute "β"
          CHECK (3.45 == subject.get("γ").data.get<double>());         // ... and double attribute "γ"
          auto scope = subject.scope();                                // look into the scope contents...
          CHECK (  *scope == CHILD_A);                                 //   there is CHILD_A
          CHECK (*++scope == CHILD_T);                                 //   followed by a copy of CHILD_T
          CHECK (*++scope == CHILD_T);                                 //   and another copy of CHILD_T
          CHECK (*++scope == MakeRec().appendChild(CHILD_B)            //   and there is a nested Record 
                                      .appendChild(CHILD_A)            //       with CHILD_B
                              .genNode(SUB_NODE.idi.getSym()));        //       and CHILD_A
          CHECK (isnil(++scope));                                      // thats all -- no more children
          
          // Part II : apply the second diff
          application.consume (mutationDiff());
          CHECK (join (subject.keys()) == "α, β, γ");                  // the attributes weren't altered 
          scope = subject.scope();                                     // but the scope was reordered
          CHECK (  *scope == CHILD_T);                                 //   CHILD_T
          CHECK (*++scope == CHILD_A);                                 //   CHILD_A
          Rec nested = (++scope)->data.get<Rec>();                     //   and our nested Record, which too has been altered:
            CHECK (nested.get("γ").data.get<double>() == 3.45);        //       it carries now an attribute "δ", which is again
            CHECK (nested.get("δ") == MakeRec().appendChild(CHILD_A)   //           a nested Record with three children CHILD_A
                                               .appendChild(CHILD_A)   // 
                                               .appendChild(CHILD_A)   // 
                                       .genNode("δ"));                 // 
            auto subScope = nested.scope();                            //       and within the nested sub-scope we find
            CHECK (  *subScope != CHILD_A);                            //           CHILD_A has been altered by assignment
            CHECK (CHILD_A.idi == subScope->idi);                      //           ...: same ID as CHILD_A
            CHECK ("A" == subScope->data.get<string>());               //           ...: but mutated payload
            CHECK (*++subScope == MakeRec().type("Y")                  //           a yet-again nested sub-Record of type "Y"
                                           .set("β", int64_t(2))       //               with just an attribute "β" == 2L
                                   .genNode(CHILD_NODE.idi.getSym())); //               (and an empty child scope)
            CHECK (*++subScope == CHILD_T);                            //           followed by another copy of CHILD_T
            CHECK (isnil (++subScope));                                // 
          CHECK (isnil (++scope));                                     // and nothing beyond that.
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffTreeApplication_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
