/*
  DiffComplexApplication(Test)  -  apply structural changes to unspecific private data structures

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

/** @file diff-complex-application-test.cpp
 ** unit test \ref DiffComplexApplication_test.
 ** Demonstrates the concept of tree mutation by diff messages.
 ** This is an elaborate demonstration setup to highlight some
 ** of the more intricate, the flexibility and support for
 ** and complex opaque implementation variations.
 */


#include "lib/test/run.hpp"
#include "lib/format-util.hpp"
#include "lib/diff/tree-diff-application.hpp"
#include "lib/diff/test-mutation-target.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <string>
#include <vector>
#include <memory>

using util::isnil;
using util::join;
using util::_Fmt;
using util::BOTTOM_INDICATOR;
using lib::iter_stl::snapshot;
using lib::time::Time;
using std::unique_ptr;
using std::string;
using std::vector;


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
    
    
    /**
     * opaque private data structure to apply the diff.
     * This class offers to build a binding for diff messages,
     * which basically maps its internal structures onto the
     * generic "object" scheme underlying the diff language.
     */
    class Opaque
      {
        idi::BareEntryID key_;
        string type_ = Rec::TYPE_NIL;
        
        int  alpha_   = -1;
        int64_t beta_ = -1;
        double gamma_ = -1;
        
        unique_ptr<Opaque> delta_;
        
        vector<Opaque> nestedObj_;
        vector<string> nestedData_;
        
      public:
        Opaque()
          : key_(idi::EntryID<Opaque>())
          { }
        
        explicit
        Opaque (string keyID)
          : key_(idi::EntryID<Opaque>(keyID))
          { }
        
        explicit
        Opaque (idi::BareEntryID id)
          : key_(id)
          { }
        
        Opaque (Opaque const& o)
          : key_(o.key_)
          , type_(o.type_)
          , alpha_(o.alpha_)
          , beta_(o.beta_)
          , gamma_(o.gamma_)
          , delta_()
          , nestedObj_(o.nestedObj_)
          , nestedData_(o.nestedData_)
          {
            if (o.delta_)
              delta_.reset(new Opaque(*o.delta_));
          }
        
        Opaque&
        operator= (Opaque const& o)
          {
            if (&o != this)
              {
                Opaque tmp(o);
                swap (*this, tmp);
              }
            return *this;
          }
        
        bool verifyType(string x)    const { return x == type_; }
        bool verifyAlpha(int x)      const { return x == alpha_;}
        bool verifyBeta(int64_t x)   const { return x == beta_; }
        bool verifyGamma(double x)   const { return x == gamma_;}
        bool verifyData(string desc) const { return desc == join(nestedData_); }
        const Opaque* nestedDelta()  const { return not delta_? NULL : delta_.get(); }
        const Opaque* nestedObj_1()  const { return isnil(nestedObj_)? NULL : &nestedObj_[0]; }
        
        
        operator string()  const
          {
            return _Fmt{"%s__(α:%d β:%s γ:%7.5f δ:%s\n......|nested:%s\n......|data:%s\n      )__END_%s"}
                       % identity()
                       % alpha_
                       % beta_
                       % gamma_
                       % delta_
                       % join (nestedObj_, "\n......|")
                       % join (nestedData_)
                       % identity()
                       ;
          }
        
        string
        identity()  const
          {
            string symbol = key_.getSym() + (isTyped()? "≺"+type_+"≻" : "");
            return lib::idi::format::instance_hex_format(symbol, key_.getHash());
          }
        
        bool
        isTyped()  const
          {
            return Rec::TYPE_NIL != type_;
          }
        
        
        /** the _only way_ this opaque object exposes itself for mutation through diff messages.
         *  This function builds a TreeMutator implementation into the given buffer space
         * @note some crucial details for this binding to work properly...
         *       - we define several "onion layers" of binding to deal with various scopes.
         *       - the priority of these bindings is layered backwards from lowest to highest,
         *         i.e. the resulting mutator will fist check for attribute δ and then work
         *         its way down do the `collection(nestedData_)`
         *       - actually this is a quite complicated setup, including object fields
         *         to represent attributes, where only one specific attribute actually holds
         *         a nested object and thus needs special treatment; beyond that we have both
         *         a collection of child objects and a collection of child data values
         *       - the selector predicate (`isApplicableIf`) actually decides if a binding layer
         *         becomes responsible for a given diff verb. Here, this decision is based on
         *         the classification of the verb or spec to be handled, either being an
         *         attribute (named, key-value pair), a nested sub-scope ("object") and
         *         finally just any unnamed (non attribute) value
         *       - the recursive mutation of nested scopes is simply initiated by invoking
         *         the same Opaque::buildMutator on the respective children recursively.
         *       - such an unusually complicated TreeMutator binding leads to increased
         *         buffer space requirements for the actual TreeMutator to be generated;
         *         Thus we need to implement the _extension point_ treeMutatorSize()
         *         to supply a sufficient buffer size value. This function is
         *         picked up through ADL, based on the target type `Opaque`
         */
        void
        buildMutator (TreeMutator::Handle buff)
          {
            buff.create (
              TreeMutator::build()
                .attach (collection(nestedData_)
                       .isApplicableIf ([&](GenNode const& spec) -> bool
                          {
                            return not spec.isNamed();                // »Selector« : accept anything unnamed value-like
                          })
                       .matchElement ([&](GenNode const& spec, string const& elm) -> bool
                          {
                            return elm == render(spec.data);
                          })
                       .constructFrom ([&](GenNode const& spec) -> string
                          {
                            return render (spec.data);
                          })
                       .assignElement ([&](string& target, GenNode const& spec) -> bool
                          {
                            target = render (spec.data);
                            return true;
                          }))
                .attach (collection(nestedObj_)
                       .isApplicableIf ([&](GenNode const& spec) -> bool
                          {
                            return spec.data.isNested();              // »Selector« : require object-like sub scope
                          })
                       .matchElement ([&](GenNode const& spec, Opaque const& elm) -> bool
                          {
                            return spec.idi == elm.key_;
                          })
                       .constructFrom ([&](GenNode const& spec) -> Opaque
                          {
                            return Opaque{spec.idi};
                          })
                       .buildChildMutator ([&](Opaque& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                          {
                            if (target.key_ != subID) return false;    // require match on already existing child object
                            target.buildMutator (buff);               //  delegate to child to build nested TreeMutator
                            return true;
                          }))
                .change("type", [&](string typeID)
                    {
                      type_ = typeID;
                    })
                .change("α", [&](int val)
                    {
                      alpha_ = val;
                    })
                .change("β", [&](int64_t val)
                    {
                      beta_ = val;
                    })
                .change("γ", [&](double val)
                    {
                      gamma_ = val;
                    })
                .mutateAttrib("δ", [&](TreeMutator::Handle buff)
                    {
                      if (not delta_)                     // note: object is managed automatically,
                        delta_.reset (new Opaque("δ"));  //        thus no INS-implementation necessary
                      REQUIRE (delta_);
                      
                      delta_->buildMutator(buff);
                    }));
          }
        
        /** override default size traits
         *  to allow for sufficient buffer,
         *  able to hold the mutator defined above.
         */
        friend constexpr size_t
        treeMutatorSize (const Opaque*)
        {
          return 430;
        }
      };
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  /***********************************************************************//**
   * @test Demonstration: apply a structural change to unspecified private
   *       data structures, with the help of an [dynamic adapter](\ref TreeMutator)
   *       - we use private data classes, defined right here in the test fixture
   *         to represent "just some" pre-existing data structure.
   *       - we re-assign some attribute values
   *       - we add, re-order and delete child "elements", without knowing
   *         what these elements actually are and how they are to be handled.
   *       - we recurse into mutating such an _"unspecified"_ child element.
   * 
   * @note this test uses the same verb sequence as is assumed for the
   *       coverage of diff building blocks in TreeMutatorBinding_test
   * 
   * @see DiffTreeApplication_test generic variant of tree diff application
   * @see TreeMutatorBinding_test coverage of the "building blocks"
   * @see TreeMutator_test base operations of the adapter
   * @see diff-tree-application.hpp
   * @see tree-diff.hpp
   */
  class DiffComplexApplication_test
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
          Opaque subject;
          DiffApplicator<Opaque> application(subject);
          //
          cout << "before..."<<endl << subject<<endl;
          CHECK (subject.verifyAlpha(-1));
          CHECK (subject.verifyBeta(-1));
          CHECK (subject.verifyGamma(-1));
          CHECK (not subject.nestedDelta());
          CHECK (not subject.nestedObj_1());
          CHECK (subject.verifyData(""));
          
          
          // Part I : apply attribute changes
          application.consume(populationDiff());
          //
          cout << "after...I"<<endl << subject<<endl;
          // ==> ATTRIB1, ATTRIB3, (ATTRIB3), CHILD_B, CHILD_B, CHILD_T
          CHECK (subject.verifyAlpha(1));
          CHECK (subject.verifyGamma(ATTRIB3.data.get<double>()));
          CHECK (subject.verifyData("b, b, 78:56:34.012"));
          // unchanged...
          CHECK (subject.verifyBeta(-1));
          CHECK (not subject.nestedDelta());
          CHECK (not subject.nestedObj_1());
          
          
          // Part II : apply child population
          application.consume(reorderingDiff());
          //
          cout << "after...II"<<endl << subject<<endl;
          // ==> ATTRIB1, ATTRIB3, (ATTRIB3), ATTRIB2, SUB_NODE, CHILD_T, CHILD_B
          CHECK (subject.verifyAlpha(1));
          CHECK (subject.verifyBeta (2));                     // attribute β has been set
          CHECK (subject.verifyGamma(3.45));
          CHECK (subject.verifyData("78:56:34.012, b"));      // one child deleted, the other ones re-ordered
          CHECK (subject.nestedObj_1());                      // plus inserted a nested child object
          CHECK (subject.nestedObj_1()->verifyType(Rec::TYPE_NIL));
          CHECK (subject.nestedObj_1()->verifyBeta(-1));      // ...which is empty (default constructed)
          CHECK (subject.nestedObj_1()->verifyData(""));
          
          
          // Part III : apply child mutations
          application.consume(mutationDiff());
          //
          cout << "after...III"<<endl << subject<<endl;
          // ==> ATTRIB1, ATTRIB3 := π, (ATTRIB3), ATTRIB2,
          //     ATTRIB_NODE{ type ζ, CHILD_A, CHILD_A, CHILD_A }
          //     SUB_NODE{ type ξ, ATTRIB2, CHILD_B, CHILD_A },
          //     CHILD_T, CHILD_B
          CHECK (subject.verifyAlpha(1));
          CHECK (subject.verifyBeta (2));
          CHECK (subject.verifyGamma(GAMMA_PI.data.get<double>())); // new value assigned to attribute γ
          CHECK (subject.nestedDelta());                      // attribute δ (object valued) is now present
          CHECK (subject.nestedDelta()->verifyType("ζ"));     // ...and has an explicitly defined type field
          CHECK (subject.nestedDelta()->verifyData("a, a, a"));//...plus three similar child values
          CHECK (subject.verifyData("78:56:34.012, b"));      // the child values weren't altered
          CHECK (subject.nestedObj_1()->verifyType("ξ"));     // but the nested child object's type has been set
          CHECK (subject.nestedObj_1()->verifyBeta(2));       // ...and the attribute β has been set on the nested object
          CHECK (subject.nestedObj_1()->verifyData("b, a"));  // ...plus some child values where added here     
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffComplexApplication_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
