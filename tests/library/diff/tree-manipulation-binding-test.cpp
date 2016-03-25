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
#include "lib/iter-adapter-stl.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

//#include <utility>
#include <string>
//#include <vector>

using util::join;
using util::isnil;
using util::contains;
using util::stringify;
using lib::iter_stl::eachElm;
using lib::time::Time;
using std::string;

//using std::vector;
//using std::swap;

using util::typeStr;


namespace lib {
namespace diff{
namespace test{
  
  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  
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
                  CHILD_NODE = SUB_NODE,                   // yet another child node, same ID as SUB_NODE (!)
                  GAMMA_PI("γ", 3.14159265);               // happens to have the same identity (ID) as ATTRIB3AS
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /********************************************************************************//**
   * @test Building blocks to map generic changes to arbitrary private data structures.
   *       - use a dummy diagnostic implementation to verify the interface
   *       - integrate the standard case of tree diff application to `Rec<GenNode>`
   *       - verify an adapter to apply structure modification to a generic collection
   *       - use closures to translate mutation into manipulation of private attributes
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
      
      
      /** @test diagnostic binding: how to monitor and verify the mutations applied */
      void
      mutateDummy()
        {
          MARK_TEST_FUN;
          TestMutationTarget target;
          auto mutator =
          TreeMutator::build()
            .attachDummy (target);
          
          CHECK (isnil (target));
          CHECK (mutator.emptySrc());
          
          mutator.injectNew (ATTRIB1);
          CHECK (!isnil (target));
          CHECK (contains(target.showContent(), "α = 1"));
          CHECK (target.verifyEvent("injectNew","α = 1")
                       .after("attachMutator"));
          
          mutator.injectNew (ATTRIB3);
          mutator.injectNew (ATTRIB3);
          mutator.injectNew (CHILD_B);
          mutator.injectNew (CHILD_B);
          mutator.injectNew (CHILD_T);
          CHECK (target.verify("attachMutator")
                       .beforeEvent("injectNew","α = 1")
                       .beforeEvent("injectNew","γ = 3.45")
                       .beforeEvent("injectNew","γ = 3.45")
                       .beforeEvent("injectNew","b")
                       .beforeEvent("injectNew","b")
                       .beforeEvent("injectNew","78:56:34.012")
                       );
          CHECK (target.showContent() == "α = 1, γ = 3.45, γ = 3.45, b, b, 78:56:34.012");
          cout << "Content after population; "
               << target.showContent() <<endl;
          
          
          // now attach new mutator for second round...
          auto mutator2 =
          TreeMutator::build()
            .attachDummy (target);
          
          CHECK (target.verify("attachMutator")
                       .beforeEvent("injectNew","78:56:34.012")
                       .before("attachMutator"));
          
          CHECK (isnil (target));                   // the "visible" new content is still void
          CHECK (not mutator2.emptySrc());          // content was moved into hidden "src" buffer
          CHECK (target.showSrcBuffer() == "α = 1, γ = 3.45, γ = 3.45, b, b, 78:56:34.012");
          
          CHECK (mutator2.matchSrc (ATTRIB1));      // current head element of src "matches" the given spec
          CHECK (isnil (target));                   // the match didn't change anything
          
          CHECK (mutator2.findSrc (ATTRIB3));       // search for an element further down into src...              // findSrc
          CHECK (!isnil (target));                  // ...pick and accept it into the "visible" part of target
          CHECK (target.showContent() == "γ = 3.45");
          
          CHECK (mutator2.matchSrc (ATTRIB1));      // element at head of src is still ATTRIB1 (as before)
          CHECK (mutator2.acceptSrc (ATTRIB1));     // now pick and accept this src element                        // acceptSrc
          CHECK (target.showContent() == "γ = 3.45, α = 1");
          
          CHECK (not mutator2.emptySrc());          // next we have to clean up waste 
          mutator2.skipSrc();                       // left behind by the findSrc() operation                      // skipSrc
          CHECK (target.showContent() == "γ = 3.45, α = 1");
          
          mutator2.injectNew (ATTRIB2);                                                                            // injectNew
          CHECK (not mutator2.emptySrc());
          CHECK (mutator2.matchSrc (ATTRIB3));
          CHECK (mutator2.acceptSrc (ATTRIB3));                                                                    // acceptSrc
          CHECK (target.showContent() == "γ = 3.45, α = 1, β = 2, γ = 3.45");
          
          // now proceeding with the children.
          // NOTE: the TestWireTap / TestMutationTarget does not enforce the attribute / children distinction!
          CHECK (not mutator2.emptySrc());
          CHECK (mutator2.matchSrc (CHILD_B));      // first child waiting in src is CHILD_B
          mutator2.skipSrc();                       // ...which will be skipped (and thus discarded)               // skipSrc
          mutator2.injectNew (SUB_NODE);            // inject a new nested sub-structure here                      // injectNew
          CHECK (mutator2.matchSrc (CHILD_B));      // yet another B-child is waiting
          CHECK (not mutator2.findSrc (CHILD_A));   // unsuccessful find operation won't do anything
          CHECK (not mutator2.emptySrc());
          CHECK (mutator2.matchSrc (CHILD_B));      // child B still waiting, unaffected
          CHECK (not mutator2.acceptSrc (CHILD_T)); // refusing to accept/pick a non matching element
          CHECK (mutator2.matchSrc (CHILD_B));      // child B still patiently waiting, unaffected
          CHECK (mutator2.acceptSrc (CHILD_B));                                                                    // acceptSrc
          CHECK (mutator2.matchSrc (CHILD_T));
          CHECK (mutator2.acceptSrc (CHILD_T));                                                                    // acceptSrc
          CHECK (mutator2.emptySrc());              // source contents exhausted
          CHECK (not mutator2.acceptSrc (CHILD_T));
          CHECK (target.verify("attachMutator")
                       .beforeEvent("injectNew","78:56:34.012")
                       .before("attachMutator")
                       .beforeEvent("findSrc","γ = 3.45")
                       .beforeEvent("acceptSrc","α = 1")
                       .beforeEvent("skipSrc","⟂")
                       .beforeEvent("injectNew","β = 2")
                       .beforeEvent("acceptSrc","γ = 3.45")
                       .beforeEvent("skipSrc","b")
                       .beforeEvent("injectNew","Rec()")
                       .beforeEvent("acceptSrc","b")
                       .beforeEvent("acceptSrc","78:56:34.012")
                       );
          CHECK (target.showContent() == "γ = 3.45, α = 1, β = 2, γ = 3.45, Rec(), b, 78:56:34.012");
          cout << "Content after reordering; "
               << target.showContent() <<endl;
          
          
          
          // the third round will cover tree mutation primitives...
          auto mutator3 =
          TreeMutator::build()
            .attachDummy (target);
          
          CHECK (isnil (target));
          CHECK (mutator3.matchSrc (ATTRIB3));      // new mutator starts out anew at the beginning
          CHECK (mutator3.accept_until (ATTRIB2));  // fast forward behind attribute β
          CHECK (mutator3.acceptSrc (ATTRIB3));     // and accept the second copy of attribute γ
          CHECK (mutator3.matchSrc (SUB_NODE));     // this /would/ be the next source element, but...
          
          CHECK (not contains(target.showContent(), "γ = 3.1415927"));
          CHECK (mutator3.assignElm(GAMMA_PI));     // ...we assign a new payload to the current element first
          CHECK (    contains(target.showContent(), "γ = 3.1415927"));
          CHECK (mutator3.accept_until (Ref::END)); // fast forward, since we do not want to re-order anything
          cout << "Content after assignment; "
               << target.showContent() <<endl;
          
          // for mutation of an enclosed scope, in real usage the managing TreeDiffInterpreter
          // would maintain a stack of "mutation frames", where each one provides an OpaqueHolder
          // to place a suitable sub-mutator for this nested scope. At this point, we can't get any further
          // with this TestWireTap / TestMutationTarget approach, since the latter just records actions and
          // otherwise forwards operation to the rest of the TreeMutator. In case there is no /real/ mutator
          // in any "onion layer" below the TestWireTap within this TreeMutator, we'll just get a default (NOP)
          // implementation of TreeMutator without any further functionality.
          
          InPlaceBuffer<TreeMutator, sizeof(mutator3)> subMutatorBuffer;
          TreeMutator::MutatorBuffer placementHandle(subMutatorBuffer);
          
          CHECK (mutator3.mutateChild (SUB_NODE, placementHandle));
          CHECK (subMutatorBuffer->emptySrc());     // ...this is all we can do here
                                                    // the real implementation would instead find a suitable
                                                    // sub-mutator within this buffer and recurse into that.
          
          // error handling: assignment might throw
          GenNode differentTime{CHILD_T.idi.getSym(), Time(11,22)};
          VERIFY_ERROR (LOGIC, mutator3.assignElm (differentTime));
          
          CHECK (target.showContent() == "γ = 3.45, α = 1, β = 2, γ = 3.1415927, Rec(), b, 78:56:34.012");
          CHECK (target.verifyEvent("acceptSrc","78:56:34.012")
                       .before("attachMutator TestWireTap")
                       .beforeEvent("accept_until β","γ = 3.45")
                       .beforeEvent("accept_until β","α = 1")
                       .beforeEvent("accept_until β","β = 2")
                       .beforeEvent("acceptSrc","γ = 3.45")
                       .beforeEvent("assignElm","γ: 3.45 ⤅ 3.1415927")
                       .beforeEvent("accept_until END","Rec()")
                       .beforeEvent("accept_until END","b")
                       .beforeEvent("accept_until END","78:56:34.012")
                       .beforeEvent("mutateChild","_CHILD_Record.001: start mutation...Rec()")
                       );
          
          cout << "____Mutation-Log______________\n"
               << join(target.getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      /** @test map mutation primitives onto a STL collection managed locally. */
      void
      mutateCollection()
        {
          MARK_TEST_FUN;
          
          // some private data structures
          struct Data
            {
              string key;
              string val;
              
              operator string()                const { return _Fmt{"≺%s∣%s≻"} % key % val; }
              bool operator== (Data const& o)  const { return key==o.key and val==o.val; }
              bool operator!= (Data const& o)  const { return not (*this == o); }
            };
          
          using VecD = std::vector<Data>;
          using MapD = std::map<string, VecD>;
          
          VecD target;
          
          // now set up a binding to these opaque private structures...
          auto mutator =
          TreeMutator::build()
            .attach (collection(target)
                       .constructFrom ([&](GenNode const& spec) -> Data
                          {
                            cout << "constructor invoked on "<<spec<<endl;
                            return {spec.idi.getSym(), render(spec.data)};
                          })
                       .matchElement ([&](GenNode const& spec, Data const& elm)
                          {
                            cout << "match? "<<spec.idi.getSym()<<"=?="<<elm.key<<endl;
                            return spec.idi.getSym() == elm.key;
                          })
                       );
          
          CHECK (sizeof(mutator) <= sizeof(VecD)                // the buffer for pending elements
                                  + sizeof(VecD*)               // the reference to the original collection
                                  + sizeof(void*)               // the reference from the ChildCollectionMutator to the CollectionBinding
                                  + 2 * sizeof(VecD::iterator)  // one Lumiera RangeIter (comprised of pos and end iterators)
                                  + 3 * sizeof(void*)           // the three unused default configured binding functions
                                  + 1 * sizeof(void*));         // one back reference from the closures to this scope
          
          
          // --- first round: populate the collection ---
          
          CHECK (isnil (target));
          CHECK (mutator.emptySrc());
          
          mutator.injectNew (ATTRIB1);
          CHECK (!isnil (target));
          CHECK (contains(join(target), "≺α∣1≻"));
          
          mutator.injectNew (ATTRIB3);
          mutator.injectNew (ATTRIB3);
          mutator.injectNew (CHILD_B);
          mutator.injectNew (CHILD_B);
          mutator.injectNew (CHILD_T);
          
          auto contents = stringify(eachElm(target));
          CHECK ("≺α∣1≻" == *contents);
          ++contents;
          CHECK ("≺γ∣3.45≻" == *contents);
          ++contents;
          CHECK ("≺γ∣3.45≻" == *contents);
          ++contents;
          CHECK (contains(*contents, "∣b≻"));
          ++contents;
          CHECK (contains(*contents, "∣b≻"));
          ++contents;
          CHECK (contains(*contents, "∣78:56:34.012≻"));
          ++contents;
          CHECK (isnil (contents));
          
          cout << "injected......" << join(target) <<endl;
          
          
          // --- second round: reorder the collection ---
          
          
          // Mutators are one-time disposable objects,
          // thus we'll have to build a new one for the second round...
          auto mutator2 =
          TreeMutator::build()
            .attach (collection(target)
                       .constructFrom ([&](GenNode const& spec) -> Data
                          {
                            cout << "constructor invoked on "<<spec<<endl;
                            return {spec.idi.getSym(), render(spec.data)};
                          })
                       .matchElement ([&](GenNode const& spec, Data const& elm)
                          {
                            cout << "match? "<<spec.idi.getSym()<<"=?="<<elm.key<<endl;
                            return spec.idi.getSym() == elm.key;
                          }));
          
          CHECK (isnil (target));                   // the "visible" new content is still void
          
          CHECK (mutator2.matchSrc (ATTRIB1));      // current head element of src "matches" the given spec
          CHECK (isnil (target));                   // the match didn't change anything
          
          CHECK (mutator2.findSrc (ATTRIB3));       // search for an element further down into src...              // findSrc
          CHECK (!isnil (target));                  // ...pick and accept it into the "visible" part of target
          CHECK (join(target) == "≺γ∣3.45≻");
          
          CHECK (mutator2.matchSrc (ATTRIB1));      // element at head of src is still ATTRIB1 (as before)
          CHECK (mutator2.acceptSrc (ATTRIB1));     // now pick and accept this src element                        // acceptSrc
          
          mutator2.skipSrc();                       // next we have to clean up waste left over by findSrc()       // skipSrc
          
          mutator2.injectNew (ATTRIB2);                                                                            // injectNew
          CHECK (mutator2.matchSrc (ATTRIB3));
          CHECK (mutator2.acceptSrc (ATTRIB3));                                                                    // acceptSrc
          
          CHECK (mutator2.matchSrc (CHILD_B));      // first child waiting in src is CHILD_B
          mutator2.skipSrc();                       // ...which will be skipped (and thus discarded)               // skipSrc
          mutator2.injectNew (SUB_NODE);            // inject a new nested sub-structure here                      // injectNew
          CHECK (mutator2.matchSrc (CHILD_B));      // yet another B-child is waiting
          CHECK (not mutator2.findSrc (CHILD_A));   // unsuccessful find operation won't do anything
          CHECK (not mutator2.emptySrc());
          CHECK (mutator2.matchSrc (CHILD_B));      // child B still waiting, unaffected
          CHECK (not mutator2.acceptSrc (CHILD_T)); // refusing to accept/pick a non matching element
          CHECK (mutator2.matchSrc (CHILD_B));      // child B still patiently waiting, unaffected
          CHECK (mutator2.acceptSrc (CHILD_B));                                                                    // acceptSrc
          CHECK (mutator2.matchSrc (CHILD_T));
          CHECK (mutator2.acceptSrc (CHILD_T));                                                                    // acceptSrc
          CHECK (mutator2.emptySrc());              // source contents exhausted
          CHECK (not mutator2.acceptSrc (CHILD_T)); // ...anything beyond is NOP

          cout << "Content after reordering...."
               << join(target) <<endl;
//          CHECK (target.showContent() == "γ = 3.45, α = 1, β = 2, γ = 3.45, Rec(), b, 78:56:34.012");
        }
      
      
      void
      mutateAttributeMap ()
        {
          TODO ("define how to translate generic mutation into attribute manipulation");
        }
      
      
      void
      mutateGenNode()
        {
          TODO ("define how to fit GenNode tree mutation into the framework");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TreeManipulationBinding_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
