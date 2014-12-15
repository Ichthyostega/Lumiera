/*
  DiffListApplication(Test)  -  demonstrate linearised representation of list diffs

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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
#include "lib/verb-token.hpp"
#include "lib/util.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/format-string.hpp"

#include <boost/noncopyable.hpp>
#include <functional>
#include <algorithm>
#include <string>
#include <vector>
#include <tuple>

using util::isnil;
using std::string;
using util::_Fmt;
using std::vector;
using std::move;


namespace lib {
namespace test{
  
  namespace {//Test fixture....
    
    using DataSeq = vector<string>;
    
    #define TOK(id) id(STRINGIFY(id))
    
    string TOK(a1), TOK(a2), TOK(a3), TOK(a4), TOK(a5);
    string TOK(b1), TOK(b2), TOK(b3), TOK(b4);
    
    using Interpreter = ListDiffInterpreter<string>;
    using DiffStep = ListDiffLanguage<string>::DiffStep;
    using DiffSeq = iter_stl::IterSnapshot<DiffStep>;
    
    DiffStep_CTOR(ins);
    DiffStep_CTOR(del);
    DiffStep_CTOR(pick);
    DiffStep_CTOR(push);
    
    
    inline DiffSeq
    generateTestDiff()
    {
      return snapshot({del(a1)
                     , del(a2)
                     , ins(b1)
                     , pick(a3)
                     , push(a5)
                     , pick(a5)
                     , ins(b2)
                     , ins(b3)
                     , pick(a4)
                     , ins(b4)
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
  class DiffListApplication_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          DataSeq src({a1,a2,a3,a4,a5});
          auto diff = generateTestDiff();
          CHECK (!isnil (diff));
          
          DataSeq target = src;
          DiffApplicator<DataSeq> application(target);
          application.consume(diff);
          
          CHECK (isnil (diff));
          CHECK (!isnil (target));
          CHECK (src != target);
          CHECK (target == DataSeq({b1,a3,a5,b2,b3,a4,b4}));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiffListApplication_test, "unit common");
  
  
  
}} // namespace lib::test
