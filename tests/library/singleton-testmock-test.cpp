/*
  SingletonTestMock(Test)  -  using Singleton for injecting Test-Mocks

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file singleton-testmock-test.cpp
 ** unit test §§TODO§§
 */



#include "lib/test/run.hpp"
#include "lib/depend.hpp"
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
  class TestSingletonO
    {
      int callCnt_;
      Symbol typid_;
      _Fmt msg_;
      
    public:
      TestSingletonO(Symbol ty="TestSingletonO")
        : callCnt_(0)
        , typid_(ty)
        , msg_("%s::doIt() call=%d\n")
        {
          TRACE (test, "ctor %s", typid_.c());
        }
      
      virtual
     ~TestSingletonO()
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
  struct Mock_1 : TestSingletonO
    {
      Mock_1() : TestSingletonO("Mock_1") { };
    };
  
  /**
   * Mock-2 to replace the Client Class...
   */
  struct Mock_2 : TestSingletonO
    {
      Mock_2() : TestSingletonO("Mock_2") { };
    };
  
  
  
  
  
  
  
  
  
  
  /***************************************************************//**
   * @test inject a Mock object into the Singleton Factory,
   *       to be returned and used in place of the original object.
   *       This test covers the full usage cycle: first access the
   *       Client Object, then replace it by two different mocks,
   *       and finally restore the original Client Object.
   * @see  lib::Depend
   * @see  lib::test::Depend4Test
   * @see  DependencyFactory_test
   */
  class SingletonTestMock_test : public Test
    {
      
      void
      run (Arg)
        {
          Depend<TestSingletonO> sing;
          
          sing().doIt();
          sing().doIt();
          CHECK (sing().getCnt() == 2);
          
          Mock_1 mock_1;
          TestSingletonO* original =
          sing.injectReplacement (&mock_1);
          sing().doIt();
          sing().doIt();
          sing().doIt();
          sing().doIt();
          sing().doIt();
          CHECK (sing().getCnt() == 5);
          
          Mock_2 mock_2;
          sing.injectReplacement (&mock_2);
          sing().doIt();
          CHECK (sing().getCnt() == 1);
          
          sing.injectReplacement (original);  // un-shadowing original instance
          CHECK (sing().getCnt() == 2);
          sing().doIt();
          CHECK (sing().getCnt() == 3);
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SingletonTestMock_test, "unit common");
  
  
  
}} // namespace lib::test
