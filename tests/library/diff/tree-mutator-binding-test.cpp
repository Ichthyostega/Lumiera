/*
  TreeMutatorBinding(Test)  -  techniques to map generic changes onto concrete tree shaped data

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
   *       - verify an adapter to apply structure modification to a generic collection
   *       - use closures to translate mutation into manipulation of private attributes
   *       - integrate the standard case of tree diff application to `Rec<GenNode>`
   * 
   * @remark even while this is a very long and detail oriented test, it barely
   *      scratches the surface of what is possible with _layering multiple bindings_
   *      on top of each other. In fact, what follows are several self contained tests,
   *      each performing roughly the same scenario, yet targeted at different local
   *      data structures through appropriate special bindings given as lambda.
   * @remark _you should note_ that the scenario executed in each of these tests
   *      precisely corresponds to the application of the test diff used in
   *      (\ref DiffVirtualisedApplication_test)
   * @remark _to help with understanding this,_ please consider how diff application is
   *      actually implemented on top of a set of "primitives". The TreeMutator interface
   *      on the other hand offers precisely these building blocks necessary to implement
   *      diff application to an arbitrary hierarchical data structure. In this way, the
   *      following test cases demonstrate the intermediary steps executed when applying
   *      this test diff through the concrete binding exemplified in each case
   * @remark the **test diff** referred here reads as follows
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * ins(ATTRIB1)
   * ins(ATTRIB3)
   * ins(ATTRIB3)
   * ins(CHILD_B)
   * ins(CHILD_B)
   * ins(CHILD_T)
   *                          // ==> ATTRIB1, ATTRIB3, ATTRIB3, CHILD_B, CHILD_B, CHILD_T
   * find(ATTRIB3)
   * pick(ATTRIB1)
   * skip(ATTRIB3)
   * ins(ATTRIB2)
   * pick(ATTRIB3)
   * del(CHILD_B)
   * ins(SUB_NODE)
   * pick(CHILD_B)
   * pick(CHILD_T)
   *                          // ==> ATTRIB3, ATTRIB1, ATTRIB2, ATTRIB3, SUB_NODE, CHILD_B, CHILD_T
   * after(ATTRIB2)
   * pick(ATTRIB3)
   * set(GAMMA_PI)
   * after(Ref::END)
   * mut(SUB_NODE)
   *   ins(TYPE_X)
   *   ins(ATTRIB2)
   *   ins(CHILD_B)
   *   ins(CHILD_A)
   * emu(SUB_NODE)
   * ins(ATTRIB_NODE)
   * mut(ATTRIB_NODE)
   *   ins(TYPE_Z)
   *   ins(CHILD_A)
   *   ins(CHILD_A)
   *   ins(CHILD_A)
   * emu(ATTRIB_NODE)
   *                          // ==> ATTRIB3, ATTRIB1, ATTRIB2, ATTRIB3 := π,
   *                          //     SUB_NODE{ type ξ, ATTRIB2, CHILD_B, CHILD_A },
   *                          //     CHILD_B, CHILD_T,
   *                          //     ATTRIB_NODE{ type ζ, CHILD_A, CHILD_A, CHILD_A }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * @see TreeMutator
   * @see TreeMutator_test
   * @see DiffTreeApplication_test
   * @see GenNodeBasic_test
   * @see AbstractTangible_test::mutate()
   */
  class TreeMutatorBinding_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          mutateDummy();
          mutateCollection();
          mutateAttribute();
          mutateGenNode();
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
          CHECK (not mutator.hasSrc());
          
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
          CHECK (mutator.completeScope());
          CHECK (target.verify("attachMutator")
                       .beforeEvent("injectNew","α = 1")
                       .beforeEvent("injectNew","γ = 3.45")
                       .beforeEvent("injectNew","γ = 3.45")
                       .beforeEvent("injectNew","b")
                       .beforeEvent("injectNew","b")
                       .beforeEvent("injectNew","78:56:34.012")
                       .beforeEvent("completeScope","scope completed")
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
          CHECK (mutator2.hasSrc());                // content was moved into hidden "src" buffer
          CHECK (target.showSrcBuffer() == "α = 1, γ = 3.45, γ = 3.45, b, b, 78:56:34.012");
          
          CHECK (mutator2.matchSrc (ATTRIB1));      // current head element of src "matches" the given spec
          CHECK (isnil (target));                   // the match didn't change anything
          
          CHECK (mutator2.findSrc (ATTRIB3));       // search for an element further down into src...              // findSrc
          CHECK (!isnil (target));                  // ...pick and accept it into the "visible" part of target
          CHECK (target.showContent() == "γ = 3.45");
          
          CHECK (mutator2.matchSrc (ATTRIB1));      // element at head of src is still ATTRIB1 (as before)
          CHECK (mutator2.acceptSrc (ATTRIB1));     // now pick and accept this src element                        // acceptSrc
          CHECK (target.showContent() == "γ = 3.45, α = 1");
          
          CHECK (mutator2.hasSrc());                // next we have to clean up waste 
          mutator2.skipSrc();                       // left behind by the findSrc() operation                      // skipSrc
          CHECK (target.showContent() == "γ = 3.45, α = 1");
          
          mutator2.injectNew (ATTRIB2);                                                                            // injectNew
          CHECK (mutator2.hasSrc());
          CHECK (mutator2.matchSrc (ATTRIB3));
          CHECK (mutator2.acceptSrc (ATTRIB3));                                                                    // acceptSrc
          CHECK (target.showContent() == "γ = 3.45, α = 1, β = 2, γ = 3.45");
          
          // now proceeding with the children.
          // NOTE: the TestWireTap / TestMutationTarget does not enforce the attribute / children distinction!
          CHECK (mutator2.hasSrc());
          CHECK (mutator2.matchSrc (CHILD_B));      // first child waiting in src is CHILD_B
          mutator2.skipSrc();                       // ...which will be skipped (and thus discarded)               // skipSrc
          mutator2.injectNew (SUB_NODE);            // inject a new nested sub-structure here                      // injectNew
          CHECK (mutator2.matchSrc (CHILD_B));      // yet another B-child is waiting
          CHECK (not mutator2.findSrc (CHILD_A));   // unsuccessful find operation won't do anything
          CHECK (mutator2.hasSrc());
          CHECK (mutator2.matchSrc (CHILD_B));      // child B still waiting, unaffected
          CHECK (not mutator2.acceptSrc (CHILD_T)); // refusing to accept/pick a non matching element
          CHECK (mutator2.matchSrc (CHILD_B));      // child B still patiently waiting, unaffected
          CHECK (mutator2.acceptSrc (CHILD_B));                                                                    // acceptSrc
          CHECK (mutator2.matchSrc (CHILD_T));
          CHECK (mutator2.acceptSrc (CHILD_T));                                                                    // acceptSrc
          CHECK (not mutator2.hasSrc());            // source contents exhausted
          CHECK (not mutator2.acceptSrc (CHILD_T));
          CHECK (mutator2.completeScope());         // no pending elements left, everything resolved
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
                       .beforeEvent("completeScope","scope completed")
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
          CHECK (mutator3.accept_until (ATTRIB2));  // fast forward behind attribute β                             // accept_until
          CHECK (mutator3.acceptSrc (ATTRIB3));     // and accept the second copy of attribute γ                   // acceptSrc
          CHECK (mutator3.matchSrc (SUB_NODE));     // this /would/ be the next source element, but...
          
          CHECK (not contains(target.showContent(), "γ = 3.1415927"));
          CHECK (mutator3.assignElm(GAMMA_PI));     // ...we assign a new payload to the current element first     // assignElm
          CHECK (    contains(target.showContent(), "γ = 3.1415927"));
          CHECK (not mutator3.completeScope());     // not done yet...
          CHECK (mutator3.accept_until (Ref::END)); // fast forward, since we do not want to re-order anything     // accept_until
          CHECK (    mutator3.completeScope());     // now any pending elements where default-resolved
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
          
          CHECK (mutator3.mutateChild (SUB_NODE, placementHandle));                                                // mutateChild
          CHECK (not subMutatorBuffer->hasSrc());   // ...this is all we can do here
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
                       .beforeEvent("completeScope","scope NOT completed")
                       .beforeEvent("accept_until END","Rec()")
                       .beforeEvent("accept_until END","b")
                       .beforeEvent("accept_until END","78:56:34.012")
                       .beforeEvent("completeScope","scope completed")
                       .beforeEvent("mutateChild","_CHILD_Record.001: start mutation...Rec()")
                       );
          
          cout << "____Mutation-Log______________\n"
               << join(target.getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      
      
      
      /** @test map mutation primitives onto a STL collection managed locally.
       *        - we perform _literally_ the same diff steps as in mutateDummy()
       *        - but now we have a completely opaque implementation data structure,
       *          where even the data type is unknown beyond this functions's scope.
       *        - thus we build a custom mutator, installing lambdas to tie into this
       *          local data structure, without disclosing any details. In fact we even
       *          install different lambdas on each usage cycle, according to the specific
       *          mutation operations to perform. Of course, it would be pointless to do so
       *          in real world usage, yet nicely demonstrates the point that the implementation
       *          really remains in control about anything regarding its private data structure.
       *        - and still, by exposing such a custom configured mutator, this private structure
       *          can be populated, reordered and even altered recursively, by generic instructions.
       */
      void
      mutateCollection()
        {
          MARK_TEST_FUN;
          
          // private data structures to be mutated
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
          MapD subScopes;
          
          // now set up a binding to these opaque private structures...
          auto mutator1 =
          TreeMutator::build()
            .attach (collection(target)
                       .constructFrom ([&](GenNode const& spec) -> Data
                          {
                            cout << "constructor invoked on "<<spec<<endl;
                            return {spec.idi.getSym(), render(spec.data)};
                          }));
          
          CHECK (sizeof(mutator1) <= sizeof(VecD)                // the buffer for pending elements
                                   + sizeof(VecD*)               // the reference to the original collection
                                   + sizeof(void*)               // the reference from the ChildCollectionMutator to the CollectionBinding
                                   + 2 * sizeof(VecD::iterator)  // one Lumiera RangeIter (comprised of pos and end iterators)
                                   + 4 * sizeof(void*)           // the four unused default configured binding functions
                                   + 1 * sizeof(void*));         // one back reference from the closure to this scope
          
          
          // --- first round: populate the collection ---
          
          CHECK (isnil (target));
          CHECK (not mutator1.hasSrc());
          
          mutator1.injectNew (ATTRIB1);
          CHECK (!isnil (target));
          CHECK (contains(join(target), "≺α∣1≻"));
          
          mutator1.injectNew (ATTRIB3);
          mutator1.injectNew (ATTRIB3);
          mutator1.injectNew (CHILD_B);
          mutator1.injectNew (CHILD_B);
          mutator1.injectNew (CHILD_T);
          CHECK (mutator1.completeScope());
          
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
          
          // we have two lambdas now and thus can save on the size of one function pointer....
          CHECK (sizeof(mutator1) - sizeof(mutator2) == sizeof(void*));
          
          
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
          mutator2.injectNew (SUB_NODE);            // inject a nested sub-structure (implementation defined)      // injectNew
          CHECK (mutator2.matchSrc (CHILD_B));      // yet another B-child is waiting
          CHECK (not mutator2.findSrc (CHILD_A));   // unsuccessful find operation won't do anything
          CHECK (mutator2.hasSrc());
          CHECK (mutator2.matchSrc (CHILD_B));      // child B still waiting, unaffected
          CHECK (not mutator2.acceptSrc (CHILD_T)); // refusing to accept/pick a non matching element
          CHECK (mutator2.matchSrc (CHILD_B));      // child B still patiently waiting, unaffected
          CHECK (mutator2.acceptSrc (CHILD_B));                                                                    // acceptSrc
          CHECK (mutator2.matchSrc (CHILD_T));
          CHECK (mutator2.acceptSrc (CHILD_T));                                                                    // acceptSrc
          CHECK (not mutator2.hasSrc());            // source contents exhausted
          CHECK (not mutator2.acceptSrc (CHILD_T)); // ...anything beyond is NOP
          CHECK (mutator2.completeScope());         // no pending elements left, everything resolved
          
          // verify reordered shape
          contents = stringify(eachElm(target));
          CHECK ("≺γ∣3.45≻" == *contents);
          ++contents;
          CHECK ("≺α∣1≻" == *contents);
          ++contents;
          CHECK ("≺β∣2≻" == *contents);
          ++contents;
          CHECK ("≺γ∣3.45≻" == *contents);
          ++contents;
          CHECK (contains(*contents, "∣Rec()≻"));
          ++contents;
          CHECK (contains(*contents, "∣b≻"));
          ++contents;
          CHECK (contains(*contents, "∣78:56:34.012≻"));
          ++contents;
          CHECK (isnil (contents));
          
          cout << "Content after reordering...."
               << join(target) <<endl;
          
          
          // --- third round: mutate data and sub-scopes ---
          
          
          // This time we build the Mutator bindings in a way to allow mutation
          // For one, "mutation" means to assign a changed value to a simple node / attribute.
          // And beyond that, mutation entails to open a nested scope and delve into that recursively.
          // Here, as this is really just a test and demonstration, we implement those nested scopes aside
          // managed within a map and keyed by the sub node's ID.
          auto mutator3 =
          TreeMutator::build()
            .attach (collection(target)
                       .constructFrom ([&](GenNode const& spec) -> Data
                          {
                            cout << "constructor invoked on "<<spec<<endl;
                            return {spec.idi.getSym(), render(spec.data)};
                          })
                       .matchElement ([&](GenNode const& spec, Data const& elm) -> bool
                          {
                            cout << "match? "<<spec.idi.getSym()<<"=?="<<elm.key<<endl;
                            return spec.idi.getSym() == elm.key;
                          })
                       .assignElement ([&](Data& target, GenNode const& spec) -> bool
                          {
                            cout << "assign "<<target<<" <- "<<spec<<endl;
                            CHECK (target.key == spec.idi.getSym(), "assignment to target with wrong identity");
                            target.val = render(spec.data);
                            return true;
                          })
                       .buildChildMutator ([&](Data& target, GenNode::ID const& subID, TreeMutator::MutatorBuffer buff) -> bool
                          {
                            // use our "inside knowledge" to get at the nested scope implementation
                            VecD& subScope = subScopes[subID];
                            buff.create (
                              TreeMutator::build()
                                .attach (collection(subScope)
                                           .constructFrom ([&](GenNode const& spec) -> Data
                                              {
                                                cout << "SubScope| constructor invoked on "<<spec<<endl;
                                                return {spec.idi.getSym(), render(spec.data)};
                                              })));
                            
                            // NOTE: mutation of sub scope has not happened yet
                            //       we can only document the sub scope to be opened now
                            cout << "openSub("<<subID.getSym()<<") ⟻ "<<target<<endl;
                            target.val = "Rec(--"+subID.getSym()+"--)";
                            return true;
                          }));
          
          CHECK (isnil (target));
          CHECK (mutator3.matchSrc (ATTRIB3));      // new mutator starts out anew at the beginning
          CHECK (mutator3.accept_until (ATTRIB2));  // fast forward behind attribute β                             // accept_until
          CHECK (mutator3.acceptSrc (ATTRIB3));     // and accept the second copy of attribute γ                   // acceptSrc
          CHECK (mutator3.matchSrc (SUB_NODE));     // this /would/ be the next source element, but...
          
          CHECK (not contains(join(target), "≺γ∣3.1415927≻"));
          CHECK (mutator3.assignElm(GAMMA_PI));     // ...we assign a new payload to the current element first     // assignElm
          CHECK (    contains(join(target), "≺γ∣3.1415927≻"));
          CHECK (not mutator3.completeScope());
          CHECK (mutator3.accept_until (Ref::END)); // fast forward, since we do not want to re-order anything     // accept_until
          CHECK (    mutator3.completeScope());     // now any pending elements where default-resolved
          cout << "Content after assignment; "
               << join(target) <<endl;
          
          
          // prepare for recursion into sub scope..
          // Since this is a demonstration, we do not actually recurse into anything,
          // rather we invoke the operations on a nested mutator right from here.
          
          InPlaceBuffer<TreeMutator, sizeof(mutator1)> subMutatorBuffer;
          TreeMutator::MutatorBuffer placementHandle(subMutatorBuffer);
          
          CHECK (mutator3.mutateChild (SUB_NODE, placementHandle));                                                // mutateChild
          
          CHECK (isnil (subScopes[SUB_NODE.idi]));  // ...this is where the nested mutator is expected to work on
          CHECK (not subMutatorBuffer->hasSrc());
          
          // now use the Mutator *interface* to talk to the nested mutator...
          // This code might be confusing, because in fact we're playing two roles here!
          // For one, above, in the definition of mutator3 and in the declaration of MapD subScopes,
          // the test code represents what a private data structure and binding would do.
          // But below we enact the TreeDiffAplicattor, which *would* use the Mutator interface
          // to talk to an otherwise opaque nested mutator implementation. Actually, here this
          // nested opaque mutator is created on-the-fly, embedded within the .buildChildMutator(..lambda...)
          // Incidentally, we "just happen to know" how large the buffer needs to be to hold that mutator,
          // since this is a topic beyond the scope of this test. In real usage, the DiffApplicator cares
          // to provide a stack of suitably sized buffers for the nested mutators.
          
          subMutatorBuffer->injectNew (TYPE_X);                                                                    // >> // injectNew
          subMutatorBuffer->injectNew (ATTRIB2);                                                                   // >> // injectNew
          subMutatorBuffer->injectNew (CHILD_B);                                                                   // >> // injectNew
          subMutatorBuffer->injectNew (CHILD_A);                                                                   // >> // injectNew
          
          CHECK (not isnil (subScopes[SUB_NODE.idi]));              // ...and "magically" these instructions happened to insert
          cout << "Sub|" << join(subScopes[SUB_NODE.idi]) <<endl;  //  some new content into our implementation defined sub scope!
          
          // verify contents of nested scope after mutation
          contents = stringify(eachElm(subScopes[SUB_NODE.idi]));
          CHECK ("≺type∣ξ≻" == *contents);
          ++contents;
          CHECK ("≺β∣2≻" == *contents);
          ++contents;
          CHECK (contains(*contents, "∣b≻"));
          ++contents;
          CHECK (contains(*contents, "∣a≻"));
          ++contents;
          CHECK (isnil (contents));
          
          
          // now back to parent scope....
          // ...add a new attribute and immediately recurse into it
          mutator1.injectNew (ATTRIB_NODE);
          CHECK (mutator3.mutateChild (ATTRIB_NODE, placementHandle));  // NOTE: we're just recycling the buffer. InPlaceHolder handles lifecycle properly
          subMutatorBuffer->injectNew (TYPE_Z);
          subMutatorBuffer->injectNew (CHILD_A);
          subMutatorBuffer->injectNew (CHILD_A);
          subMutatorBuffer->injectNew (CHILD_A);
          CHECK (subMutatorBuffer->completeScope());  // no pending "open ends" left in sub-scope
          CHECK (mutator3.completeScope());           // and likewise in the enclosing main scope
          
          // and thus we've gotten a second nested scope, populated with new values
          cout << "Sub|" << join(subScopes[ATTRIB_NODE.idi]) <<endl;
          
          // verify contents of this second nested scope
          contents = stringify(eachElm(subScopes[ATTRIB_NODE.idi]));
          CHECK ("≺type∣ζ≻" == *contents);
          ++contents;
          CHECK (contains(*contents, "∣a≻"));
          ++contents;
          CHECK (contains(*contents, "∣a≻"));
          ++contents;
          CHECK (contains(*contents, "∣a≻"));
          ++contents;
          CHECK (isnil (contents));
          
          
          // back to parent scope....
          // verify the marker left by our "nested sub-scope lambda"
          CHECK (contains (join(target), "Rec(--"+SUB_NODE.idi.getSym()+"--)"));
          CHECK (contains (join(target), "Rec(--"+ATTRIB_NODE.idi.getSym()+"--)"));
          
          cout << "Content after nested mutation; "
               << join(target) <<endl;
        }
      
      
      
      
      
      /** @test translate generic mutation into attribute manipulation */
      void
      mutateAttribute ()
        {
          MARK_TEST_FUN;
          
          // local data fields to be handled as "attributes"
          int alpha    = -1;
          int64_t beta = -1;
          double gamma = -1;
          
          // we'll use this as an attribute with nested scope ("object valued attribute")
          TestMutationTarget delta;
          
          
          #define LOG_SETTER(KEY)  cout << STRINGIFY(KEY) " := "<<val<<endl;
          
          
          // set up a binding to these opaque private structures...
          auto mutator1 =
          TreeMutator::build()
            .change("α", [&](int val)
              {
                LOG_SETTER ("alpha")
                alpha = val;
              })
            .change("γ", [&](double val)
              {
                LOG_SETTER ("gamma")
                gamma = val;
              });
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
          CHECK (sizeof(mutator1) <= sizeof(VecD)                // the buffer for pending elements
                                   + sizeof(VecD*)               // the reference to the original collection
                                   + sizeof(void*)               // the reference from the ChildCollectionMutator to the CollectionBinding
                                   + 2 * sizeof(VecD::iterator)  // one Lumiera RangeIter (comprised of pos and end iterators)
                                   + 4 * sizeof(void*)           // the four unused default configured binding functions
                                   + 1 * sizeof(void*));         // one back reference from the closure to this scope
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
          
          
          // --- first round: introduce new "attributes" ---
          
          CHECK (-1 == alpha);
          CHECK (-1 == beta);
          CHECK (-1 == gamma);
          
          CHECK (mutator1.hasSrc());                // NOTE: the attribute binding always has an implicit "source sequence"
                                                    //       (which is in fact fixed, because it relies on a likewise fixed class definition)
          CHECK (mutator1.completeScope());         // NOTE: this is always true and NOP, for the same reason: the structure of the binding is fixed
          
          mutator1.injectNew (ATTRIB1);
          CHECK ( 1 == alpha);
          CHECK (-1 == beta);
          CHECK (-1 == gamma);
          
          mutator1.injectNew (ATTRIB3);
          CHECK ( 1 == alpha);
          CHECK (-1 == beta);
          CHECK (3.45 == gamma);
          
          mutator1.injectNew (ATTRIB3);
          CHECK ( 1 == alpha);
          CHECK (-1 == beta);
          CHECK (3.45 == gamma);
          
          CHECK (not mutator1.injectNew (ATTRIB2)); // ...because we didn't define a binding for ATTRIB2 (aka "beta")
          
          // any changes to something other than attributes are just delegated to the next "onion layer"
          // since in this case here, there is only one layer (our attribute binding), these other changes will be silently ignored
          mutator1.injectNew (CHILD_B);
          mutator1.injectNew (CHILD_B);
          mutator1.injectNew (CHILD_T);
          CHECK (mutator1.completeScope());         // this invocation typically happens at this point, but is NOP (see above)
          
          CHECK ( 1 == alpha);
          CHECK (-1 == beta);
          CHECK (3.45 == gamma);
          cout << "successfully 'injected' new attributes." <<endl;
          
          
          // --- second round: reordering ---
          
          
          // in fact any re-ordering of "attributes" is prohibited,
          // because "attributes" are mapped to object or data fields,
          // which are fixed by definition and don't expose any ordering.
          // While any mutations beyond attributes are passed on / ignored
          auto mutator2 =
          TreeMutator::build()
            .change("α", [&](int val)
              {
                LOG_SETTER ("alpha")
                alpha = val;
              })
            .change("β", [&](int64_t val)
              {
                LOG_SETTER ("beta")
                beta = val;
              })
            .change("γ", [&](double val)
              {
                LOG_SETTER ("gamma")
                gamma = val;
              });
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
          // we have two lambdas now and thus can save on the size of one function pointer....
          CHECK (sizeof(mutator1) - sizeof(mutator2) == sizeof(void*));
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
          
          
          CHECK ( 1 == alpha);
          CHECK (-1 == beta);
          CHECK (3.45 == gamma);                    // values not affected by attaching a new mutator
          
          CHECK (mutator2.matchSrc (ATTRIB1));      // current head element of src "matches" the given spec
          CHECK ( 1 == alpha);                      // the match didn't change anything...
          CHECK (-1 == beta);
          CHECK (3.45 == gamma);
          
          VERIFY_ERROR (LOGIC, mutator2.findSrc (ATTRIB3));
                                                    // search for an element and thus reordering is explicitly rejected...
                                                    // If we hadn't defined a binding for "γ", then the same operation
                                                    // would have been passed on silently to other binding layers.
          
          CHECK (mutator2.matchSrc (ATTRIB1));      // element at head of src is still ATTRIB1 (as before)
          CHECK (mutator2.acceptSrc (ATTRIB1));     // now pick and accept this src element (also a NOP)           // acceptSrc
          
          mutator2.skipSrc();                       // and 'skip' likewise is just not implemented for attributes  // skipSrc
          CHECK ( 1 == alpha);
          CHECK (-1 == beta);
          CHECK (3.45 == gamma);                    // all these non-operations actually didn't change anything...
          
          mutator2.injectNew (ATTRIB2);                                                                            // injectNew
          
          CHECK ( 1 == alpha);
          CHECK ( 2 == beta);                       // the first operation actually causing a tangible effect
          CHECK (3.45 == gamma);
          
          CHECK (mutator2.matchSrc (ATTRIB3));
          CHECK (mutator2.acceptSrc (ATTRIB3));                                                                    // acceptSrc
          
          // for sake of completeness, we'll be applying the same sequence of operations as in the other tests
          // but since all those operations are not relevant for our attribute binding, they will be passed on
          // to lower binding layers. And since, moreover, there /are no lower binding layers/ in our setup,
          // they will just do nothing and return false
          mutator2.skipSrc();                                                                                      // skipSrc
          CHECK (not mutator2.injectNew (SUB_NODE));// ...no setter binding, thus no effect                        // injectNew
          CHECK (not mutator2.matchSrc (CHILD_B));
          CHECK (not mutator2.acceptSrc (CHILD_B));                                                                // acceptSrc
          CHECK (not mutator2.matchSrc (CHILD_T));
          CHECK (not mutator2.acceptSrc (CHILD_T));                                                                // acceptSrc
          
          CHECK ( 1 == alpha);
          CHECK ( 2 == beta);
          CHECK (3.45 == gamma);                    // no further effect on our attribute fields
          
          cout << "all 'reordering' operations ignored as expected..." <<endl;
          
          
          
          // --- third round: mutate data and sub-scopes ---
          
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
          // This time we build the Mutator bindings in a way to allow mutation
          // For one, "mutation" means to assign a changed value to a simple node / attribute.
          // And beyond that, mutation entails to open a nested scope and delve into that recursively.
          // Here, as this is really just a test and demonstration, we implement those nested scopes aside
          // managed within a map and keyed by the sub node's ID.
          auto mutator3 =
          TreeMutator::build()
            .attach (collection(target)
                       .constructFrom ([&](GenNode const& spec) -> Data
                          {
                            cout << "constructor invoked on "<<spec<<endl;
                            return {spec.idi.getSym(), render(spec.data)};
                          })
                       .matchElement ([&](GenNode const& spec, Data const& elm) -> bool
                          {
                            cout << "match? "<<spec.idi.getSym()<<"=?="<<elm.key<<endl;
                            return spec.idi.getSym() == elm.key;
                          })
                       .assignElement ([&](Data& target, GenNode const& spec) -> bool
                          {
                            cout << "assign "<<target<<" <- "<<spec<<endl;
                            CHECK (target.key == spec.idi.getSym(), "assignment to target with wrong identity");
                            target.val = render(spec.data);
                            return true;
                          })
                       .buildChildMutator ([&](Data& target, GenNode::ID const& subID, TreeMutator::MutatorBuffer buff) -> bool
                          {
                            // use our "inside knowledge" to get at the nested scope implementation
                            VecD& subScope = subScopes[subID];
                            buff.create (
                              TreeMutator::build()
                                .attach (collection(subScope)
                                           .constructFrom ([&](GenNode const& spec) -> Data
                                              {
                                                cout << "SubScope| constructor invoked on "<<spec<<endl;
                                                return {spec.idi.getSym(), render(spec.data)};
                                              })));
                            
                            // NOTE: mutation of sub scope has not happened yet
                            //       we can only document the sub scope to be opened now
                            cout << "openSub("<<subID.getSym()<<") ⟻ "<<target<<endl;
                            target.val = "Rec(--"+subID.getSym()+"--)";
                            return true;
                          }));
          
          CHECK (isnil (target));
          CHECK (mutator3.matchSrc (ATTRIB3));      // new mutator starts out anew at the beginning
          CHECK (mutator3.accept_until (ATTRIB2));  // fast forward behind attribute β                             // accept_until
          CHECK (mutator3.acceptSrc (ATTRIB3));     // and accept the second copy of attribute γ                   // acceptSrc
          CHECK (mutator3.matchSrc (SUB_NODE));     // this /would/ be the next source element, but...
          
          CHECK (not contains(join(target), "≺γ∣3.1415927≻"));
          CHECK (mutator3.assignElm(GAMMA_PI));     // ...we assign a new payload to the current element first     // assignElm
          CHECK (    contains(join(target), "≺γ∣3.1415927≻"));
          CHECK (not mutator3.completeScope());
          CHECK (mutator3.accept_until (Ref::END)); // fast forward, since we do not want to re-order anything     // accept_until
          CHECK (    mutator3.completeScope());     // now any pending elements where default-resolved
          cout << "Content after assignment; "
               << join(target) <<endl;
          
          
          // prepare for recursion into sub scope..
          // Since this is a demonstration, we do not actually recurse into anything,
          // rather we invoke the operations on a nested mutator right from here.
          
          InPlaceBuffer<TreeMutator, sizeof(mutator1)> subMutatorBuffer;
          TreeMutator::MutatorBuffer placementHandle(subMutatorBuffer);
          
          CHECK (mutator3.mutateChild (SUB_NODE, placementHandle));                                                // mutateChild
          
          CHECK (isnil (subScopes[SUB_NODE.idi]));  // ...this is where the nested mutator is expected to work on
          CHECK (not subMutatorBuffer->hasSrc());
          
          // now use the Mutator *interface* to talk to the nested mutator...
          // This code might be confusing, because in fact we're playing two roles here!
          // For one, above, in the definition of mutator3 and in the declaration of MapD subScopes,
          // the test code represents what a private data structure and binding would do.
          // But below we enact the TreeDiffAplicattor, which *would* use the Mutator interface
          // to talk to an otherwise opaque nested mutator implementation. Actually, here this
          // nested opaque mutator is created on-the-fly, embedded within the .buildChildMutator(..lambda...)
          // Incidentally, we "just happen to know" how large the buffer needs to be to hold that mutator,
          // since this is a topic beyond the scope of this test. In real usage, the DiffApplicator cares
          // to provide a stack of suitably sized buffers for the nested mutators.
          
          subMutatorBuffer->injectNew (TYPE_X);                                                                    // >> // injectNew
          subMutatorBuffer->injectNew (ATTRIB2);                                                                   // >> // injectNew
          subMutatorBuffer->injectNew (CHILD_B);                                                                   // >> // injectNew
          subMutatorBuffer->injectNew (CHILD_A);                                                                   // >> // injectNew
          
          CHECK (not isnil (subScopes[SUB_NODE.idi]));              // ...and "magically" these instructions happened to insert
          cout << "Sub|" << join(subScopes[SUB_NODE.idi]) <<endl;  //  some new content into our implementation defined sub scope!
          
          // verify contents of nested scope after mutation
          contents = stringify(eachElm(subScopes[SUB_NODE.idi]));
          CHECK ("≺type∣ξ≻" == *contents);
          ++contents;
          CHECK ("≺β∣2≻" == *contents);
          ++contents;
          CHECK (contains(*contents, "∣b≻"));
          ++contents;
          CHECK (contains(*contents, "∣a≻"));
          ++contents;
          CHECK (isnil (contents));
          
          
          // now back to parent scope....
          // ...add a new attribute and immediately recurse into it
          mutator1.injectNew (ATTRIB_NODE);
          CHECK (mutator3.mutateChild (ATTRIB_NODE, placementHandle));  // NOTE: we're just recycling the buffer. InPlaceHolder handles lifecycle properly
          subMutatorBuffer->injectNew (TYPE_Z);
          subMutatorBuffer->injectNew (CHILD_A);
          subMutatorBuffer->injectNew (CHILD_A);
          subMutatorBuffer->injectNew (CHILD_A);
          CHECK (subMutatorBuffer->completeScope());  // no pending "open ends" left in sub-scope
          CHECK (mutator3.completeScope());           // and likewise in the enclosing main scope
          
          // and thus we've gotten a second nested scope, populated with new values
          cout << "Sub|" << join(subScopes[ATTRIB_NODE.idi]) <<endl;
          
          // verify contents of this second nested scope
          contents = stringify(eachElm(subScopes[ATTRIB_NODE.idi]));
          CHECK ("≺type∣ζ≻" == *contents);
          ++contents;
          CHECK (contains(*contents, "∣a≻"));
          ++contents;
          CHECK (contains(*contents, "∣a≻"));
          ++contents;
          CHECK (contains(*contents, "∣a≻"));
          ++contents;
          CHECK (isnil (contents));
          
          
          // back to parent scope....
          // verify the marker left by our "nested sub-scope lambda"
          CHECK (contains (join(target), "Rec(--"+SUB_NODE.idi.getSym()+"--)"));
          CHECK (contains (join(target), "Rec(--"+ATTRIB_NODE.idi.getSym()+"--)"));
          
          cout << "Content after nested mutation; "
               << join(target) <<endl;
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
        }
      
      
      
      
      
      void
      mutateGenNode()
        {
          TODO ("define how to fit GenNode tree mutation into the framework");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TreeMutatorBinding_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
