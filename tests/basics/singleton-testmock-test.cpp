/*
  SingletonTestMock(Test)  -  using Singleton for injecting Test-Mocks

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file singleton-testmock-test.cpp
 ** unit test \ref SingletonTestMock_test
 */



#include "lib/test/run.hpp"
#include "lib/depend-inject.hpp"
#include "lib/util.hpp"

#include "lib/format-cout.hpp"
#include "lib/format-string.hpp"

using util::_Fmt;
using util::isnil;


namespace lib {
namespace test{
  
  
  /**
   * Client Class normally to be instantiated as Singleton.
   * But for tests, this class should be replaced by a Mock....
   */
  class TestSingO
    {
      int callCnt_;
      Symbol typid_;
      _Fmt msg_;
      
    public:
      TestSingO(Symbol ty="TestSingO")
        : callCnt_(0)
        , typid_(ty)
        , msg_("%s::doIt() call=%d\n")
        {
          TRACE (test, "ctor %s", typid_.c());
        }
      
      virtual
     ~TestSingO()
        {
          TRACE (test, "dtor %s", typid_.c());
        }
      
      void doIt ()
        {
          ++callCnt_;
          cout << msg_ % typid_ % callCnt_;
        }
      
      int getCnt ()
        {
          return callCnt_;
        }
      
    };
  
  
  /**
   * Mock-1 to replace the Client Class...
   */
  struct Mock_1 : TestSingO
    {
      Mock_1() : TestSingO("Mock_1") { };
    };
  
  /**
   * Mock-2 to replace the Client Class...
   * @note no default ctor
   */
  struct Mock_2 : TestSingO
    {
      int id;
      
      Mock_2(Literal specialID, int i)
        : TestSingO{Symbol (_Fmt{"%s_%d"} % specialID % i)}
        , id{i}
      { };
    };
  
  
  
  
  
  
  
  
  
  
  /***************************************************************//**
   * @test inject a Mock object into the Singleton Factory,
   *       to be returned and used in place of the original object.
   *       This test covers the full usage cycle: first access the
   *       Client Object, then replace it by two different mocks,
   *       and finally restore the original Client Object.
   * @see  lib::Depend
   * @see  depend-inject.hpp
   * @see  DependencyFactory_test
   */
  class SingletonTestMock_test : public Test
    {
      
      void
      run (Arg)
        {
          Depend<TestSingO> sing;
          
          sing().doIt();
          sing().doIt();
          CHECK (sing().getCnt() == 2);
          
          {
            // shadow by local Mock instance
            DependInject<TestSingO>::Local<Mock_1> mock_1;
            sing().doIt();
            sing().doIt();
            sing().doIt();
            sing().doIt();
            sing().doIt();
            CHECK (sing().getCnt() == 5);
            
            // shadow again by different local Mock, this time with special ctor call
            int instanceID = 0;
            DependInject<TestSingO>::Local<Mock_2> mock_2 ([&]{ return new Mock_2{"Mock", instanceID}; });
            
            // NOTE: the ctor call for the Mock really happens delayed...
            instanceID = rani(10);
            sing().doIt();             // ctor invoked on first access
            CHECK (sing().getCnt() == 1);
            
            // can access the Mock for instrumentation
            CHECK (instanceID == mock_2->id);
            
          }// original instance automatically un-shadowed here
          
          CHECK (sing().getCnt() == 2);
          sing().doIt();
          CHECK (sing().getCnt() == 3);
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SingletonTestMock_test, "unit common");
  
  
  
}} // namespace lib::test
