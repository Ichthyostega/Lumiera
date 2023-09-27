/*
  Result(Test)  -  verify the either-result-or-failure intermediary wrapper

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file result-test.cpp
 ** unit test \ref Result_test
 */




#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/result.hpp"
#include "lib/util.hpp"

#include <cstdlib>



namespace lib {
namespace test{
  
  using util::isSameObject;
  using std::rand;
  
  namespace error = lumiera::error;
  using error::LUMIERA_ERROR_FATAL;
  
  
  namespace {
    const Literal THE_END = "all dead and hero got the girl";
    
    
    #define Type(_EXPR_) lib::test::showType<decltype(_EXPR_)>()
  }

  
  
  
  
  /***********************************************************************************//**
   * @test Verify an intermediary »Either« type, to embody either a successful result,
   *       or document a failure with encountered exception.
   * @see result.hpp
   * @see lib::ThreadJoinable usage example
   */
  class Result_test
    : public Test
    {
      
      void
      run (Arg)
        {
          auto happy = Result{THE_END};
          CHECK (happy == THE_END);
          CHECK (happy.isValid());
          CHECK (true == happy);
          
          happy.maybeThrow();    // still alive...
          
          CHECK (Type(happy) == "Result<Literal const&>"_expect);
          
          // Note type deduction: RValue moved into the Result...
          auto sequel = Result{Literal{THE_END}};
          CHECK (sequel.isValid());
          CHECK (Type(sequel) == "Result<Literal>"_expect);
          
          CHECK (    isSameObject (happy.get<Literal const&>(), THE_END));
          CHECK (not isSameObject (sequel.get<Literal const&>(), THE_END));
          
          // »Either Right« case : mark as failure
          Result<double> facepalm{error::Fatal("zOMG")};
          CHECK (not facepalm.isValid());
          
          VERIFY_ERROR (FATAL, (double)facepalm       );
          VERIFY_ERROR (FATAL, facepalm.get<double&>());
          VERIFY_ERROR (FATAL, facepalm.maybeThrow()  );
          
          CHECK (42.0 == facepalm.getOrElse([]{ return 42; }));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (Result_test, "unit common");
  
  
}} // namespace lib::test
