/*
  Parameter(Test)  -  verify generic parameter values

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file parameter-test.cpp
 ** unit test \ref Parameter_test
 */


#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "lib/par/parameter.hpp"
#include "test/diagnostic-output.hpp"/////////////TODO

//#include <utility>
//#include <string>
//#include <vector>

//using std::string;
//using std::vector;
//using std::swap;
using test::roughEQ;


namespace lib {
namespace par {
namespace test{
  
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  namespace {//Test fixture....
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /**************************************************************************//**
   * @test cover properties of generic parameter containers.
   */
  class Parameter_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verify_Builder();
          verify_Identity();
          verify_ParamType();
          verify_Automation();
        }
      
      
      void
      simpleUsage()
        {
          CHECK (sizeof(Parameter) <= 4 * sizeof(void*));
          CHECK (BASETYPE_MAX_SIZ <= sizeof(ValStorage));
          
          Parameter par = Parameter::forType<int>().build();
          CHECK (0 == par.getVal<int>());
          
          par.setVal (12.3);
          CHECK (12 == par.getVal<int>());
          CHECK (12u == par.getVal<uint64_t>());
          CHECK (12.0 == par.getVal<double>());
          CHECK (true == par.getVal<bool>());
        }
      
      
      /** @test explore ways to define a parameter...
       * @todo WIP 10/26 🔁 define ⟶ ✔ implement
       */
      void
      verify_Builder()
        {
          // build a simple float-parameter
          Parameter p1 = Parameter::forValue(123.45f).build();
          CHECK (123.45f == p1.getVal<float>());
          
          p1.setVal (12u);
          CHECK (12.0 == p1.getVal<double>());
          
          // remould into a double parameter with value limit
          Parameter p2 = Parameter::from<double>(p1)
                                   .maxVal (sqrt(2))
                                   .build();
          
          CHECK (sqrt(2) == p2.getVal<double>());
          CHECK (roughEQ (1.41421, p2.getVal<float>()));
          
          p2.setVal (100);
          CHECK (sqrt(2) == p2.getVal<double>());
          p2.setVal (-10);
          CHECK (    -10 == p2.getVal<double>());
          
          // clone again, but re-use the scale / value-limit
          Parameter p3 = Parameter::from<double>(p2)
                                   .withValue (1000)
                                   .build();
          
          CHECK (sqrt(2) == p3.getVal<double>());
          CHECK (    -10 == p2.getVal<double>());
          
          // build a limited scale
          Parameter p4 = Parameter::forType<uint>()
                                   .limited(5, 10)
                                   .build();
          
          CHECK ( 5 == p4.getVal<int>());
          p4.setVal (12u);
          CHECK (10 == p4.getVal<uint>());
          p4.setVal (-22);
          CHECK ( 5 == p4.getVal<int64_t>());
          p4.setVal (7.1);
          CHECK ( 7 == p4.getVal<uint64_t>());
          
          // build a cyclic wrapping scale
          Parameter p5 = Parameter::forType<int>()
                                   .cyclicWrap(-180, 180)
                                   .withValue(361)
                                   .build();
          
          CHECK ( 1 == p5.getVal<int>());
          p5.setVal (270u);
          CHECK (-90 == p5.getVal<int>());
          p5.setVal (23.5);
          CHECK (23 == p5.getVal<uint64_t>());
        }
      
      
      /** @test equality and instance identity
       *      - demonstrate copy and assignment
       *      - validate that copies have independent state
       * @todo planned as of 10/26 : can attach an ID and metadata
       * @todo WIP 10/26 🔁 define ⟶ 🔁 implement
       */
      void
      verify_Identity()
        {
          Parameter p1 = Parameter::forType<uint>()
                                   .minVal(10)
                                   .build();
          CHECK (10 == p1.getVal<uint>());
          
          // can be copied
          Parameter p2{p1};
          CHECK (10 == p2.getVal<uint>());
          p2.setVal(23.55);
          CHECK (23 == p2.getVal<uint>());
          CHECK (10 == p1.getVal<uint>());
          
          // can be assigned
          p1 = p2;
          CHECK (23 == p2.getVal<uint>());
          // both share the same scale, but operate independently
          p2.setVal(-55);
          CHECK (10 == p2.getVal<uint>());
          CHECK (23 == p1.getVal<uint>());
        }
      
      
      /** @test each parameter has an opaque yet distinct dynamic type.
       *      - types are converted, limited and conformed
       *        automatically, both on setting and getting values
       *      - based on the embedded type information, it is possible
       *        to transfer the value to a differently typed parameter.
       * @todo WIP 10/26 🔁 define ⟶ ✔ implement
       */
      void
      verify_ParamType()
        {
          Parameter p1 = Parameter::forType<uint>()
                                   .maxVal(10)
                                   .build();
          Parameter p2 = Parameter::forType<float>()
                                   .maxVal(20)
                                   .build();
          p1.setVal(5.5);
          p2.setVal(5.5);
          CHECK (5   == p1.getVal<float>());
          CHECK (5.5 == p2.getVal<float>());
          
          // based on type and scale information,
          // a value can be transfered between opaque parameters
          p2.setVal(15.5);
          p1.assignValueFrom(p2);
          CHECK (10   == p1.getVal<uint>());
          CHECK (15.5 == p2.getVal<float>());
          
          Parameter pb = Parameter::forValue(true).build();
          CHECK ( 1.0 == pb.getVal<double>());
          
          p2.setVal(0.2);
          pb.assignValueFrom(p2);
          CHECK ( 0.0 == pb.getVal<double>());
          pb.assignValueFrom(p1);
          CHECK (true == pb.getVal<bool>());
        }
      
      
      /** @test
       * @todo WIP 10/26 🔁 define ⟶ implement
       */
      void
      verify_Automation()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (Parameter_test, "unit lib");
  
  
  
}}} // namespace lib::par::test
