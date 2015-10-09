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


#include "lib/test/run.hpp"
#include "lib/diff/tree-diff-application.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/util.hpp"

#include <string>
#include <vector>

using lib::iter_stl::snapshot;
// using util::isnil;
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
                  ATTRIB2("β", 2L),                        // attribute α = 2L   (int64_t)
                  ATTRIB3("γ", 3.45),                      // attribute γ = 3.45 (double)
                  TYPE_X("type", "X"),                     // a "magic" type attribute "X" 
                  TYPE_Y("type", "Y"),                     // 
                  CHILD_A("a"),                            // unnamed string child node
                  CHILD_B('b'),                            // unnamed char child node
                  CHILD_T(Time(12,34,56,78)),              // unnamed time value child
                  SUB_NODE = MakeRec().genNode(),          // empty anonymous node used to open a sub scope
                  ATTRIB_NODE = MakeRec().genNode("δ"),    // empty named node to be attached as attribute δ 
                  CHILD_NODE = Ref(SUB_NODE);              // use a Node-Reference as child node (!)
    
    
    using Interpreter = TreeDiffInterpreter;
    using DiffStep = TreeDiffLanguage::DiffStep;
    using DiffSeq = iter_stl::IterSnapshot<DiffStep>;
    
    DiffStep_CTOR(ins);
    DiffStep_CTOR(del);
    DiffStep_CTOR(pick);
    DiffStep_CTOR(find);
    DiffStep_CTOR(skip);
    
    DiffStep_CTOR(after);
    DiffStep_CTOR(mut);
    DiffStep_CTOR(emu);
    
    
    inline DiffSeq
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
    
    
    inline DiffSeq
    mutationDiff()
    {
      return snapshot({after(Ref::ATTRIBS)
                     , find(CHILD_T)
                     , pick(CHILD_A)
                     , skip(CHILD_T)
                     , del(CHILD_T)
                     , pick(Ref::CHILD)
                     , mut(Ref::THIS)
                       , ins(ATTRIB3)
                       , ins(ATTRIB_NODE)
                       , find(CHILD_A)
                       , del(CHILD_B)
                       , ins(CHILD_NODE)
                       , ins(CHILD_T)
                       , skip(CHILD_A)
                       , mut(CHILD_NODE)
                         , ins(TYPE_Y)
                         , ins(ATTRIB2)
                       , emu(CHILD_NODE)
                       , mut(ATTRIB_NODE)
                         , ins(CHILD_A)
                         , ins(CHILD_A)
                         , ins(CHILD_A)
                       , emu(ATTRIB_NODE)
                     , emu(Ref::THIS)
                     });
    }
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /***********************************************************************//**
   * @test Demonstration/Concept: a description language for list differences.
   *       The representation is given as a linearised sequence of verb tokens.
   *       This test demonstrates the application of such a diff representation
   *       to a given source list, transforming this list to hold the intended
   *       target list contents.
   *       
   * @see session-structure-mapping-test.cpp
   */
  class DiffTreeApplication_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          Rec target;
          DiffApplicator<Rec> application(target);
          application.consume(populationDiff());
          
          // TODO Check population
          
          application.consume(mutationDiff());
          
          // TODO Check mutation
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffTreeApplication_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
