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
#include "lib/diff/test-mutation-target.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"  //////////TODO necessary?
#include "lib/format-string.hpp"
//#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <string>
#include <vector>
#include <memory>

using lib::iter_stl::snapshot;
using util::isnil;
using util::join;
using util::_Fmt;
using util::join;
using std::unique_ptr;
using std::make_unique;
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
                  GAMMA_PI("γ", 3.14159265);               // happens to have the same identity (ID) as ATTRIB3
    
    
    /**
     * opaque private data structure to apply the diff.
     * This class offers to build a binding for diff messages,
     * which basically maps its internal structures onto the
     * generic "object" scheme underlying the diff language.
     */
    class Opaque
      {
        idi::EntryID<Opaque> key_;
        string type_ = Rec::TYPE_NIL;
        
        int  alpha_   = -1;
        string beta_  = "NIL";
        double gamma_ = -1;
        
        unique_ptr<Opaque> delta_;
        
        vector<Opaque> nestedObj_;
        vector<string> nestedData_;
        
      public:
        Opaque() { }
        
        explicit
        Opaque (string keyID)
          : key_(keyID)
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
        
        
        operator string()  const
          {
            return _Fmt{"%s (α:%d β:%s γ:%7.5f δ:%s\n......|nested:%s\n......|data:%s\n      )"}
                       % identity()
                       % alpha_
                       % beta_
                       % gamma_
                       % delta_
                       % join (nestedObj_, "\n......|")
                       % join (nestedData_)
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
                       .assignElement ([&](string& target, GenNode const& spec) -> bool
                          {
                            target = render(spec.data);
                            return true;
                          }))
                .attach (collection(nestedObj_)
                       .isApplicableIf ([&](GenNode const& spec) -> bool
                          {
                            return type_ == spec.data.recordType();    // »Selector« : require object-like sub scope with matching typeID
                          })
                       .constructFrom ([&](GenNode const& spec) -> Opaque
                          {
                            return Opaque{spec.idi};
                          })
                       .matchElement ([&](GenNode const& spec, Opaque const& elm) -> bool
                          {
                            return spec.idi == elm.key_;
                          })
                       .buildChildMutator ([&](Opaque& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                          {
                            if (target.key_ != subID) return false; // require match on already existing child object
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
                .change("β", [&](string val)
                    {
                      beta_ = val;
                    })
                .change("γ", [&](double val)
                    {
                      gamma_ = val;
                    })
                .mutateAttrib("δ", [&](TreeMutator::Handle buff)
                    {
                      if (not delta_)
                        delta_.reset (new Opaque("δ"));
                      REQUIRE (delta_);
                      
                      delta_->buildMutator(buff);
                    }));
          }
        
        friend constexpr size_t
        treeMutatorSize (const Opaque*)
        {
          return 350;
        }
      };
    
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
   * @todo this test is WIP as of 6/2016 !! What has to be done, is to work out a way how to create a TreeMutator binding. And of course, we need the Implementation of DiffApplicator<DiffMutable>
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
          Opaque subject;
//        auto target = mutatorBinding (subject);
//        DiffApplicator<DiffMutable> application(target);
          DiffApplicator<Opaque> application(subject);
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
