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
#include "lib/util.hpp"

#include <string>
#include <vector>

using lib::iter_stl::snapshot;
using util::isnil;
using std::string;
using std::vector;


namespace lib {
namespace diff{
namespace test{
  
  namespace {//Test fixture....
    
    
    
    using Interpreter = TreeDiffInterpreter;
    using DiffStep = TreeDiffLanguage::DiffStep;
    using DiffSeq = iter_stl::IterSnapshot<DiffStep>;
    
    DiffStep_CTOR(ins);
    DiffStep_CTOR(del);
    DiffStep_CTOR(pick);
    DiffStep_CTOR(find);
    DiffStep_CTOR(skip);
    
    
    inline DiffSeq
    populationDiff()
    {
      return snapshot({ins(typeX)
                     , ins(attrib1)
                     , ins(attrib2)
                     , ins(attrib3)
                     , ins(childA)
                     , ins(childT)
                     , ins(childT)
                     , ins(node)
                     , mut(THIS)
                       , ins(childB)
                       , ins(childA)
                     , emu(THIS)
                     });
    }
    
    
    inline DiffSeq
    mutationDiff()
    {
      return snapshot({after(ATTRIBS)
                     , find(childT)
                     , pick(childA)
                     , skip(childT)
                     , del(childT)
                     , pick(CHILD)
                     , mut(THIS)
                       , ins(attrib3)
                       , ins(node(ID("δ")))
                       , find(childA)
                       , del(childB)
                       , ins(node(CHILD("ω")))
                       , ins(childT)
                       , skip(childA)
                       , mut(CHILD("ω"))
                         , ins(typeY)
                         , ins(attrib2)
                       , emu(CHILD("ω"))
                       , mut(ID("δ"))
                         , ins(childA)
                         , ins(childA)
                         , ins(childA)
                       , emu(ID("δ"))
                     , emu(THIS)
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
