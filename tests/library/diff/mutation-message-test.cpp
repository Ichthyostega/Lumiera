/*
  MutationMessage(Test)  -  demonstrate the basics of tree diff representation

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file mutation-message-test.cpp
 ** unit test \ref MutationMessage_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/diff/mutation-message.hpp"
#include "lib/diff/tree-diff-application.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <string>
#include <vector>

using lumiera::error::LERR_(ITER_EXHAUST);
using lib::iter_stl::IterSnapshot;
using lib::iter_stl::snapshot;
using lib::time::Time;
using util::contains;
using util::isnil;
using util::join;
using std::string;
using std::vector;


namespace lib {
namespace diff{
namespace test{
  
  namespace {//Test fixture....
    
    int instances = 0; ///< verify instance management

    
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
   * @test properties of a container to transport a diff from an abstracted
   *       source generator to an abstracted consumer.
   *       - this covers a standard usage scenario within Lumiera, where some
   *         producer in the Session core detects changes in session structure
   *         and sends a message to make the UI conform to the new structure
   *       - diff messages are hard to debug and test, since they are designed
   *         as opaque sequences to be consumed only once. Thus for we provide
   *         a snapshot decorator to offer diagnostic facilities
   *       - moreover we provide a simplified builder function to create
   *         hard wired diff messages in a concise way
   *       - and finally this test repeats the scenario of DiffTreeApplication_test,
   *         but this time the diff sequences are encapsulated as MutationMessage.
   * @remarks like all the other _diff related_ tests, this code might be hard
   *  to follow, unless you're familiar with the underlying concepts. Basically,
   *  a _Diff_ is represented as _a linearised sequence of verb tokens_. Together
   *  these tokens form a _diff language_. The semantics of that language are
   *  oriented towards application of this diff onto a target structure. The
   *  goal is to represent structure changes without being bound to a specific
   *  data structure implementation. Yet there is one _meta_ data representation
   *  used within the diff itself, as well as for various test and demonstration
   *  examples: the [generic data record](\ref diff::Record) together with its
   *  [variant node element](\ref diff::GenNode). The key point to note is the
   *  (recursive) usage of Record elements as payload within GenNode, which
   *  allows to represent tree shaped object like data structures.
   * 
   * @see AbstractTangible_test::mutate() concrete usage scenario for UI-elements
   * @see DiffTreeApplication_test change a tree-like data structure by diff
   * @see DiffComplexApplication_test handling arbitrary data structures
   * @see DiffListApplication_test
   * @see MutationMessage
   * @see ui-bus.hpp
   */
  class MutationMessage_test
    : public Test
    , TreeDiffLanguage
    {
      
      virtual void
      run (Arg)
        {
          demonstrate_standardUsage();
          verify_builder();
          verify_diagnostics();
          demonstrate_treeApplication();
        }
      

      /** @test demonstrate the intended usage pattern
       *        - a diff generation context is allocated
       *        - the MutationMessage takes ownership
       *        - and exposes the generated diff sequence
       *        - which is pulled during iteration
       */
      void
      demonstrate_standardUsage()
        {
          using Source = WrappedLumieraIter<IterSnapshot<DiffStep>>;
          
          /* opaque generation context */
          struct Generator
            : TreeDiffLanguage
            , Source
            {
              Generator()
                : Source{snapshot({ins(TYPE_X)
                                  ,set(ATTRIB1)
                                  ,del(CHILD_T)})}
                {
                  ++instances;
                }
              
             ~Generator()
                {
                  --instances;
                }
            };
          
          
          CHECK (0 == instances);
          {
            MutationMessage diffMsg{new Generator};
            CHECK (!isnil (diffMsg));
            CHECK (1 == instances);
            
            CHECK (diffMsg);
            CHECK (ins(TYPE_X) == *diffMsg);
            
            // and this effectively means....
            CHECK ("ins"  == string(diffMsg->verb()));
            CHECK ("type" == diffMsg->elm().idi.getSym());
            CHECK ("X"    == diffMsg->elm().data.get<string>());
            
            // now iterate one step
            ++diffMsg;
            CHECK (diffMsg);
            CHECK (set(ATTRIB1) == *diffMsg);
            CHECK ("set"  == string(diffMsg->verb()));
            CHECK ("α"    == diffMsg->elm().idi.getSym());
            CHECK ( 1     == diffMsg->elm().data.get<int>());
            
            // cloning is allowed, yet implementation defined
            // in the actual case the underlying generator is based on a vector + a pointer
            // and thus the full state can be cloned into an independent instance
            MutationMessage clone{diffMsg};
            CHECK (clone == diffMsg);
            CHECK (set(ATTRIB1) == *clone);
            
            CHECK (1 == instances); // the iterator front-end was cloned, not the generator
            
            ++clone;
            CHECK (del(CHILD_T) == *clone);
            CHECK (set(ATTRIB1) == *diffMsg);
            CHECK (clone != diffMsg);
            
            ++clone;
            CHECK (not clone);
            CHECK (isnil (clone));
            VERIFY_ERROR(ITER_EXHAUST, *clone);
            
            // note the weird behaviour:
            // both instances share a common backend and thus state get mixed up.
            // The diffMsg front-End still points at a state already obsoleted
            CHECK (set(ATTRIB1) == *diffMsg);
            ++diffMsg;
            // So better don't do this at home...
            VERIFY_ERROR(ITER_EXHAUST, *diffMsg);
            
            clone = MutationMessage{new Generator};
            CHECK (2 == instances); // now we got two independent generator instances
            CHECK (clone);
            CHECK (ins(TYPE_X) == *clone);
            ++clone;
            CHECK (set(ATTRIB1) == *clone);
            
            // first instance unaffected as before
            CHECK (isnil (diffMsg));
            
          }// NOTE: automatic clean-up when leaving the scope
          CHECK (0 == instances);
        }
      
      
      void
      verify_builder()
        {
          MutationMessage diffMsg{ins(TYPE_X)
                                 ,set(ATTRIB1)
                                 ,del(CHILD_T)};

          CHECK (!isnil (diffMsg));
          CHECK (ins(TYPE_X)  == *diffMsg);
          CHECK (set(ATTRIB1) == *++diffMsg);
          CHECK (del(CHILD_T) == *++diffMsg);
          CHECK (isnil (++diffMsg));
          VERIFY_ERROR(ITER_EXHAUST, *diffMsg);
          
          
          
          // likewise works with a std::initializer_list
          diffMsg = MutationMessage{{ins(TYPE_X)
                                    ,set(ATTRIB1)
                                    ,del(CHILD_T)}
                                   };
          
          CHECK (!isnil (diffMsg));
          CHECK (ins(TYPE_X)  == *diffMsg);
          CHECK (set(ATTRIB1) == *++diffMsg);
          CHECK (del(CHILD_T) == *++diffMsg);
          CHECK (isnil (++diffMsg));
          
          
          
          // even passing any suitable iterable works
          diffMsg = MutationMessage{snapshot({ins(TYPE_X)
                                             ,set(ATTRIB1)
                                             ,del(CHILD_T)})
                                   };
          
          CHECK (!isnil (diffMsg));
          CHECK (ins(TYPE_X)  == *diffMsg);
          CHECK (set(ATTRIB1) == *++diffMsg);
          CHECK (del(CHILD_T) == *++diffMsg);
          CHECK (isnil (++diffMsg));
          
          
          
          // really anything iterable...
          std::vector<DiffStep> steps;
          CHECK (isnil (steps));
          append_all(snapshot({ins(TYPE_X)
                              ,set(ATTRIB1)
                              ,del(CHILD_T)}), steps);
          
          diffMsg = MutationMessage{steps};
          
          CHECK (!isnil (diffMsg));
          CHECK (ins(TYPE_X)  == *diffMsg);
          CHECK (set(ATTRIB1) == *++diffMsg);
          CHECK (del(CHILD_T) == *++diffMsg);
          CHECK (isnil (++diffMsg));
        }
      
      
      void
      verify_diagnostics()
        {
          MutationMessage diffMsg{ins(TYPE_X)
                                 ,set(ATTRIB1)
                                 ,del(CHILD_T)};
          
          // initially only the default diagnostics of IterSource is shown (rendering the element type)
          CHECK (string(diffMsg) == "IterSource<DiffLanguage<TreeDiffInterpreter, GenNode>::DiffStep>");
          
          // transparently take a snapshot
          diffMsg.updateDiagnostics();
          
          // now the whole sequence is rendered explicitly
          string expectedRendering = join ({ins(TYPE_X), set(ATTRIB1), del(CHILD_T)});
          CHECK (contains (string(diffMsg), expectedRendering));
          
          CHECK (string(set(ATTRIB1)) == "set(GenNode-ID(\"α\")-DataCap|«int»|1)");
          
          // and we can still iterate...
          CHECK (!isnil (diffMsg));
          CHECK (ins(TYPE_X)  == *diffMsg);
          CHECK (set(ATTRIB1) == *++diffMsg);
          
          // NOTE: in fact only the remainder of the sequence is captured...
          diffMsg.updateDiagnostics();
          CHECK (not contains (string(diffMsg), string(ins(TYPE_X))));
          CHECK (    contains (string(diffMsg), string(set(ATTRIB1))));
          CHECK (    contains (string(diffMsg), string(del(CHILD_T))));
          
          // and we can still continue to iterate...
          CHECK (del(CHILD_T) == *++diffMsg);
          CHECK (isnil (++diffMsg));
          
          diffMsg.updateDiagnostics();
          CHECK (string(diffMsg) == "Diff--{}");
        }
      
      
      
      
      
      MutationMessage
      populationDiff()
        {
          return { ins(TYPE_X)
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
                 };
        }
        
        
      MutationMessage
      mutationDiff()
        {
          // prepare for direct assignment of new value
          // NOTE: the target ID will be reconstructed, including hash
          GenNode childA_upper(CHILD_A.idi.getSym(), "A");
          
          return { after(Ref::ATTRIBS)      // fast forward to the first child
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
                 };
        }
      
      /** @test use MutationMessage to transport and apply changes to target data
       * @remarks this almost literally repeats the DiffTreeApplication_test */
      void
      demonstrate_treeApplication()
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
  LAUNCHER (MutationMessage_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
