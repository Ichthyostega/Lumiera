/*
  ArgumentTupleAccept(Test)  -  verify synthesising a bind(...) function

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file argument-tuple-accept-test.cpp
 ** unit test \ref ArgumentTupleAccept_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/control/argument-tuple-accept.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/meta/function.hpp"
#include "lib/format-cout.hpp"

#include <functional>
#include <tuple>


namespace steam {
namespace control {
namespace test    {
  
  using lib::test::showSizeof;
  using lib::test::randTime;
  
  using lib::time::TimeVar;
  using std::function;
  using lib::meta::_Fun;
  using lib::meta::Tuple;
  
  
  
  
  
  
  namespace { // build a test dummy class....
    
    template<typename SIG>
    struct _Tup
      {
        using Args = typename _Fun<SIG>::Args;
        using Ret = typename _Fun<SIG>::Ret;
        using Ty = Tuple<Args>;
      };
    
    
    template<typename SIG>
    class TestClass
      : public AcceptArgumentBinding< SIG                     // to derive the desired signature
                                    , TestClass<SIG>          // the target class providing the implementation
                                    , typename _Tup<SIG>::Ty  // base class to inherit from
                                    >
      {
        using ATuple  = typename _Tup<SIG>::Ty;
        using RetType = typename _Tup<SIG>::Ret;
        
      public:
        
        RetType
        bindArg (ATuple const& tuple)
          {
            static_cast<ATuple&> (*this) = tuple;
            return RetType();
          }
      };
    
    
  } // test-helper implementation
  
  
  
  
  
  
  /*********************************************************//**
   * @test Build a test object, which accepts a bind(...) call
   *       with specifically typed arguments.
   *       
   * @see  control::CommandStorageHolder
   */
  class ArgumentTupleAccept_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          TestClass<void()> testVoid;
          TestClass<int(TimeVar,int)> testTime;
          
          testVoid.bind();
          testTime.bind(randTime(),23);
          
          cout << showSizeof(testVoid) << endl;
          cout << showSizeof(testTime) << endl;
          
          cout << std::get<0> (testTime) << endl;
          CHECK (23 == std::get<1> (testTime));
        }
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (ArgumentTupleAccept_test, "unit controller");
  
  
}}} // namespace steam::control::test
