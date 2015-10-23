/*
  GenericRecordUpdate(Test)  -  manipulate and reshape the generic record contents

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
#include "lib/test/test-helper.hpp"
#include "lib/diff/record-content-mutator.hpp"
//#include "lib/iter-adapter-stl.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/format-util.hpp"
#include "lib/diff/record.hpp"
#include "lib/itertools.hpp"
#include "lib/util.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <iostream> ///TODO

//using lib::iter_stl::snapshot;
using lib::append_all;
using util::isnil;
//using util::join;
//using std::string;
using std::vector;
//using lib::time::Time;

using lumiera::error::LUMIERA_ERROR_ITER_EXHAUST;


namespace lib {
namespace diff{
namespace test{
  
  namespace {//Test fixture....
    
    using Seq  = vector<string>;
    using RecS = Record<string>;
    
    template<class IT>
    inline Seq
    contents (IT const& it)
    {
      Seq collected;
      append_all (it, collected);
      return collected;
    }
    
    inline Seq
    contents (RecS const& rec_of_strings)
    {
      return contents (rec_of_strings.begin());
    }
    
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /*****************************************************************************//**
   * @test cover technical details of rearranging the contents of lib::diff::Record.
   *       The implementation of our generic record (abstract object representation)
   *       uses two lists to hold the data of the attribute and content scopes.
   *       When receiving a diff message, we have to rearrange and alter the contents,
   *       which are by default immutable. Thus, for this specific task, embedded
   *       data is moved into this adapter, which exposes the mutating operation
   *       required to apply such a diff message.
   *       
   * @see generic-record-representation-test.cpp
   * @see tree-diff-application-test.cpp
   */
  class GenericRecordUpdate_test
    : public Test
    {
      
      virtual void
      run (Arg)
        {
          RecS subject({"b = β", "a = α", "γ", "ε"});
          RecS::Mutator mut(subject);
          mut.appendChild("δ");
          mut.setType("🌰");
          std::cout << string(subject) << std::endl;
          
          RecS::ContentMutator content;
          
          CHECK (!isnil (mut));
          CHECK ( isnil (content));
          mut.swapContent(content);
          CHECK (!isnil (content));
          CHECK ( isnil (mut));
          
          CHECK (content.pos == content.attribs.begin());
          CHECK (content.currIsAttrib());
          CHECK (!content.currIsChild());
          CHECK ("b = β" == *content.pos);
          
          void* rawElm = &content.attribs[0];
          swap (content.attribs[0], content.attribs[1]);
          CHECK ("a = α" == *content.pos);
          CHECK (rawElm == & *content.pos);
          
          ++content;
          CHECK ("b = β" == *content.pos);
          CHECK (rawElm != &*content.pos);
          CHECK (content.currIsAttrib());
          CHECK (!content.currIsChild());
          
          std::sort (content.children.begin(), content.children.end());
          
          ++content;
          CHECK (!content.currIsAttrib());
          CHECK (content.currIsChild());
          CHECK ("γ" == *content.pos);
          
          ++content;
          CHECK ("δ" == *content.pos);
          ++content;
          CHECK ("ε" == *content.pos);
          
          ++content;
          CHECK (content.pos == content.end());
          CHECK (!content.currIsAttrib());
          CHECK (!content.currIsChild());
          
          VERIFY_ERROR (ITER_EXHAUST, ++content);
          
          content.resetPos();
          CHECK (rawElm == & *content.pos);
          ++content;
          CHECK ("b = β" == *content.pos);
          
          CHECK ( isnil (mut));
          CHECK (!isnil (content));
          mut.swapContent(content);
          CHECK ( isnil (content));
          CHECK (!isnil (mut));
          
          mut.replace(subject);
          CHECK (Seq({"a = α", "b = β", "γ", "δ", "ε"}) == contents(subject));
          std::cout << string(subject) << std::endl;
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (GenericRecordUpdate_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
