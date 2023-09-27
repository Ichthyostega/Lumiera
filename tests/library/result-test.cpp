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
  using error::LUMIERA_ERROR_STATE;
  
  
  namespace {
    const Literal THE_END = "all dead and hero got the girl";
    
    
    #define Type(_EXPR_) lib::test::showType<decltype(_EXPR_)>()
  }

  
  
  
  
  /***********************************************************************************//**
   * @test Verify an intermediary »Either« type, to embody either a successful result,
   *       or document a failure with encountered exception.
   *       - when given a value, the Result captures it and is in »left« state
   *       - various value types can be picked up by perfect forwarding
   *       - when given an exception, the result is in »right« state
   *       - option-style `or-else` usage
   *       - can invoke arbitrary _callable_ and capture result or exception caught
   *       - invocation also works with void functors, and likewise captures failure
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
          CHECK (bool(happy));
          
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
          
          CHECK (42.0 == facepalm.or_else([]{ return 42; }));
          CHECK (42.0 == facepalm.value_or(210/5));
          
          
          // a generic functor (template) to invoke
          auto evil = [](auto it)
                        {
                          if (it % 2)
                            throw error::State{"conspiracy"};
                          else
                            return it;
                        };
          
          // Invoke failsafe and capture result....
          auto seed = Result{evil, '*'};                     // this invocation is successful
          CHECK (Type(seed) == "Result<char>"_expect);       // generic λ instantiated with <char>
          CHECK (42 == seed);                                // int('*') == 42
          
          auto breed = Result{evil, 55ll};                   // an odd number...
          VERIFY_ERROR (STATE, breed.maybeThrow()  );
          CHECK (Type(breed) == "Result<long long>"_expect);
          
          auto dead = Result{[]{ throw 55; }};
          auto deed = Result{[]{ /* :-) */ }};
          
          CHECK (Type(dead) == "Result<void>"_expect);
          CHECK (Type(deed) == "Result<void>"_expect);
          
          CHECK (not dead.isValid());
          CHECK (    deed.isValid());
          
          try { dead.maybeThrow(); }                         // can handle really *anything* thrown
          catch(int oo)
              { CHECK (oo == 55); }
          
          // can also invoke member function....
          auto deaf = Result{&Literal::empty, &THE_END};     // Note: instance "this"-ptr as first argument
          CHECK (deaf.isValid());                            // no exception was thrown => state isValid()
          CHECK (deaf == false);                             // yet invocation of THE_END.empty() yields false
          CHECK (not deaf);                                  // Warning: in this case, the conversion to payload type shadows
          CHECK (Type(deaf) == "Result<bool>"_expect);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (Result_test, "unit common");
  
  
}} // namespace lib::test
