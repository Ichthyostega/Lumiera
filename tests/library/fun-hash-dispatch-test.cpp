/*
  FunHashDispatch(Test)  -  verify dispatching of opaque functions keyed by hash-ID

   Copyright (C)
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file fun-hash-dispatch-test.cpp
 ** unit test \ref FunHashDispatch_test
 */




#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/fun-hash-dispatch.hpp"
#include "lib/format-obj.hpp"
#include "lib/test/diagnostic-output.hpp"////////////////////TODO
//#include "lib/util.hpp"

//#include <cstdlib>
#include <string>



namespace lib {
namespace test{
  
  using std::string;
  using util::toString;
  
  
  
  /***********************************************************************************//**
   * @test Verify generic helper to provide a hash-based function dispatch table.
   *     - instances are tied to one specific function signature
   *     - entries are keyed by a hash-ID
   *     - given that ID, the registered functions can be invoked
   *     - once enrolled, entries can not be replaced
   * @see fun-hash-dispatch.hpp
   * @see NodeMeta_test
   */
  class FunHashDispatch_test
    : public Test
    {
      
      void
      run (Arg)
        {
          FunHashDispatch<string(int)> dispatch;
          
          auto one = [](int i) { return toString(i);   };
          auto two = [](int i) { return string(size_t(i),'*'); };
          
          auto* res = dispatch.enrol (1, one);
          CHECK (res == one);
          CHECK (dispatch.contains(1));
          CHECK (dispatch.select(1)(42) == "42"_expect);
          
          dispatch.enrol (2, two);
          CHECK (dispatch.contains(1));
          CHECK (dispatch.contains(2));
          CHECK (dispatch.select(1)(5) == "5"_expect);
          CHECK (dispatch.select(2)(5) == "*****"_expect);
          
          res = dispatch.enrol (1,two);
          CHECK (res == one);
          res = dispatch.enrol (3,two);
          CHECK (res == two);
          CHECK (dispatch.select(3)(2) == "**"_expect);
          
          VERIFY_FAIL ("Expect function for given hash"
                      , dispatch.select(5) );
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (FunHashDispatch_test, "unit common");
  
  
}} // namespace lib::test
