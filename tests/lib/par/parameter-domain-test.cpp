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
#include <concepts>

//using std::string;
//using std::vector;
//using std::swap;


namespace lib {
namespace par {
namespace test{
  
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  using std::floating_point;
  
  namespace {//Test fixture....
    
    template<typename NUM>
    constexpr NUM _MAX = std::numeric_limits<NUM>::max();
    
    template<typename NUM>
    constexpr NUM _MIN = std::numeric_limits<NUM>::lowest();
    
    template<floating_point FLO>
    constexpr FLO _EPSILON = std::numeric_limits<FLO>::epsilon();
    
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
      
      
      /** @test base operation to bracket a given value
       *        so that it fits within another target type's value domain.
       *  @note properly handles the signed vs. unsigned mismatch, so that
       *        conversion from signed to unsigned can not wrap.
       */
      void
      verify_valueLimits()
        {
          // Generic check if the first (target) type can hold
          // only a limited part of the second (source) type's domain
          CHECK ((sub_domain<uint,int> == true));
          CHECK ((sub_domain<int,uint> == true));
          CHECK ((sub_domain<short,long> ==  true));
          CHECK ((sub_domain<long,short> == false));
          CHECK ((sub_domain<float,double> ==  true));
          CHECK ((sub_domain<double,float> == false));
          
          // Limit a value to within a target type's domain
          // (without actually converting into the target type)
          CHECK (preClamp<int>  (23) ==  23);
          CHECK (preClamp<int> (-55) == -55);
          CHECK (preClamp<uint> (23) ==  23);
          CHECK (preClamp<uint>(-55) ==   0);

          CHECK (preClamp<short> (_MAX<int64_t>) ==  32767);
          CHECK (preClamp<short> (_MIN<int64_t>) == -32768);
          
          CHECK (preClamp<ushort> (_MAX<float>)  == 65535);
          CHECK (preClamp<ushort> (_MIN<float>)  == 0    );
          
          // the return value has the same type as the argument...
          CHECK (preClamp<float> (_MAX<uint64_t>) == _MAX<uint64_t>);
          CHECK (preClamp<float> (_MIN<double>) == double(_MIN<float>) );
          
          // Warning: the following is not standard C++, yet supported on GCC / CLang / libstdc++
          static_assert (_MAX<float> < _MAX<unsigned __int128>);
          CHECK (float(_MAX<unsigned __int128>) == "inf"_expect );
          CHECK ((unsigned __int128)_MAX<float> == "340282346638528859811704183484516925440"_expect );
          
          CHECK (preClamp<float> (_MAX<unsigned __int128>) == (unsigned __int128)_MAX<float>);

          // Note: a trigger at 0.5+ε is built into the clamp towards bool
          CHECK (preClamp<bool> (0.5)                  == 0 );
          CHECK (preClamp<bool> (0.5+_EPSILON<double>) == 1 );
          CHECK (preClamp<bool> (_MIN<double>)         == 0 );
          
          
          // Systematic coverage of signed ⟷ unsigned
          CHECK (preClamp<int8_t> (_MAX<int16_t>) ==  127);
          CHECK (preClamp<int8_t> (   int16_t{0}) ==    0);
          CHECK (preClamp<int8_t> (_MIN<int16_t>) == -128);

          CHECK (preClamp<int8_t> (_MAX<uint16_t>) == 127);
          CHECK (preClamp<int8_t> (_MIN<uint16_t>) ==   0);

          CHECK (preClamp<uint8_t> (_MAX<int16_t>) == 255);
          CHECK (preClamp<uint8_t> (   int16_t{0}) ==   0);
          CHECK (preClamp<uint8_t> (_MIN<int16_t>) ==   0);

          CHECK (preClamp<int16_t> (_MAX<int8_t>) ==  127);
          CHECK (preClamp<int16_t> (   int8_t{0}) ==    0);
          CHECK (preClamp<int16_t> (_MIN<int8_t>) == -128);

          CHECK (preClamp<int16_t> (_MAX<uint8_t>) == 255);
          CHECK (preClamp<int16_t> (_MIN<uint8_t>) ==   0);

          CHECK (preClamp<uint16_t> (_MAX<int8_t>) == 127);
          CHECK (preClamp<uint16_t> (   int8_t{0}) ==   0);
          CHECK (preClamp<uint16_t> (_MIN<int8_t>) ==   0);

          CHECK (preClamp<int8_t> (_MAX<int8_t>) ==  127);
          CHECK (preClamp<int8_t> (   int8_t{0}) ==    0);
          CHECK (preClamp<int8_t> (_MIN<int8_t>) == -128);

          CHECK (preClamp<int8_t> (_MAX<uint8_t>) == 127);
          CHECK (preClamp<int8_t> (_MIN<uint8_t>) ==   0);

          CHECK (preClamp<uint8_t> (_MAX<int8_t>) == 127);
          CHECK (preClamp<uint8_t> (   int8_t{0}) ==   0);
          CHECK (preClamp<uint8_t> (_MIN<int8_t>) ==   0);
        }
      
      
      
      /** @test base function to access the contents of an opaque buffer
       *        with known type, and convert into another target type.
       */
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
          
          // value clamped to target domain...
          val1 = _MIN<int>;
          uint64_t target3{55};
          TypeHandler<uint64_t>& du64{d1};
          du64.extractAs (target3, src1);
          CHECK (target3 == _MIN<uint64_t>);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ParameterDomain_test, "unit lib");
  
  
  
}}} // namespace lib::par::test
