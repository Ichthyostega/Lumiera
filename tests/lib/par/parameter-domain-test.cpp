/*
  ParameterDomain(Test)  -  document the handling of parameter type domain

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file parameter-domain-test.cpp
 ** unit test \ref ParameterDomain_test
 */


#include "test/run.hpp"
//#include "test/test-helper.hpp"
#include "lib/par/domain.hpp"
#include "test/diagnostic-output.hpp"////////////TODO

//#include <utility>
//#include <string>
//#include <vector>

//using std::string;
//using std::vector;
//using std::swap;


namespace lib {
namespace par {
namespace test{
  
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  namespace {//Test fixture....
    
    class Thing
      { };
    
    template<typename X>
    struct Some
      {
        X x;
      };
    
    typedef Some<Thing> SomeThing;

  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /**************************************************************************//**
   * @test cover properties of generic parameter containers.
   */
  class ParameterDomain_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          basics();
          verify_valueLimits();
          verify_valueAccess();
        }
      
      
      void
      basics()
        {
        }
      
      
      void
      verify_valueLimits()
        {
SHOW_EXPR(preClamp<int> (23))
SHOW_EXPR(preClamp<int> (-55))
SHOW_EXPR(preClamp<uint> (23))
SHOW_EXPR(preClamp<uint> (-55))
SHOW_EXPR((sub_domain<uint,int>))
SHOW_EXPR((sub_domain<int,uint>))
SHOW_EXPR((sub_domain<short,long>))
SHOW_EXPR((sub_domain<long,short>))
SHOW_EXPR(std::numeric_limits<uint>::max())
SHOW_EXPR(std::numeric_limits<int>::max())
SHOW_EXPR(std::numeric_limits<uint>::lowest())
SHOW_EXPR(std::numeric_limits<int>::lowest())

SHOW_EXPR(preClamp<short> (INT64_MAX))
SHOW_EXPR(preClamp<short> (INT64_MIN))

SHOW_EXPR(preClamp<ushort> (numeric_limits<float>::max()))
SHOW_EXPR(preClamp<ushort> (numeric_limits<float>::lowest()))

SHOW_EXPR(preClamp<float> (numeric_limits<uint64_t>::max()))
SHOW_EXPR(preClamp<float> (numeric_limits<double>::lowest()))

SHOW_EXPR(preClamp<bool> (0.5));
SHOW_EXPR(preClamp<bool> (0.5 + numeric_limits<double>::epsilon()))
SHOW_EXPR(preClamp<bool> (numeric_limits<double>::min()))
        }
      
      
      void
      verify_valueAccess()
        {
          BaseDomain<int> domInt;
          Domain& d1{domInt};
          
          int val1 = 1 + rani (1000);
          ValBuff& src1 = asValBuff (val1);
          
          uint target1{0};
          CHECK (not target1);
          
          TypeHandler<uint>& du1{d1};
          du1.extractAs (target1, src1);
          CHECK (target1 == uint(val1));
          
          float target2{0};
          
          TypeHandler<float>& df1{d1};
          df1.extractAs (target2, src1);
          CHECK (target2 == float(val1));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ParameterDomain_test, "unit lib");
  
  
  
}}} // namespace lib::par::test
