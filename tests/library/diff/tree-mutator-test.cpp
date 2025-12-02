/*
  TreeMutator(Test)  -  customisable intermediary to abstract tree changing operations

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file tree-mutator-test.cpp
 ** unit test \ref TreeMutator_test
 */


#include "lib/test/run.hpp"
#include "lib/format-util.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/diff/tree-mutator.hpp"
#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <string>
#include <vector>

using util::isnil;
using util::join;
using util::typeStr;

using std::string;
using std::vector;


namespace lib {
namespace diff{
namespace test{
  
  
  
  
  
  /*****************************************************************************//**
   * @test Demonstrate a customisable component for flexible bindings
   *       to enable generic tree changing and mutating operations to
   *       arbitrary hierarchical data structures.
   *       - we use lambdas to link into our private implementation
   *       - this test demonstrates the behaviour of an attribute setter
   *       - plus some of the _primitive operations_ available on collections
   *       
   * @see TreeMutator
   * @see DiffComplexApplication_test a way more complex usage scenario
   * @see GenNode_test
   * @see GenericRecord_test
   */
  class TreeMutator_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleAttributeBinding();
          simpleCollectionBinding();
        }
      
      
      void
      simpleAttributeBinding()
        {
          MARK_TEST_FUN;
          string localData;
          auto mutator =
          TreeMutator::build()
            .change("data", [&](string val)
              {
                cout << "\"data\" closure received something "<<val<<endl;
                localData = val;
              });
          
          cout << "concrete TreeMutator size=" << sizeof(mutator)
               << " type="<< typeStr(mutator)
               << endl;
          
          mutator.init();
          
          CHECK (isnil (localData));
          string testValue{"that would be acceptable"};
          mutator.assignElm ({"lore", testValue});
          CHECK ( isnil (localData)); // nothing happens, nothing changed
          mutator.assignElm ({"data", testValue});
          CHECK (!isnil (localData));
          cout << "localData changed to: "<<localData<<endl;
          CHECK (localData == "that would be acceptable");
        }
      
      
      void
      simpleCollectionBinding()
        {
          MARK_TEST_FUN;
          vector<string> values;
          values.push_back("a");
          values.push_back("b");
          
          cout << join(values) <<endl;
          CHECK (2 == values.size());
          CHECK ("a, b" == join(values));
          
          auto mutator =
          TreeMutator::build()
            .attach (collection(values));
          
          cout << "concrete TreeMutator size=" << sizeof(mutator)
               << " type="<< typeStr(mutator)
               << endl;
          
          mutator.init();
          
          CHECK (isnil (values));
          CHECK (mutator.matchSrc (GenNode("a")));
                 mutator.skipSrc (GenNode("a"));
          CHECK (mutator.matchSrc (GenNode("b")));
          CHECK (mutator.injectNew (GenNode("c")));
          CHECK (mutator.acceptSrc (GenNode("b")));
          
          cout << join(values) <<endl;
          CHECK (2 == values.size());
          CHECK ("c, b" == join(values));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TreeMutator_test, "unit common");
  
  
  
}}} // namespace lib::diff::test
