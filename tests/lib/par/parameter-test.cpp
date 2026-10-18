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
       * @todo WIP 10/26 🔁 define ⟶ 🔁 implement
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
        }
      
      
      /** @test
       * @todo WIP 10/26 🔁 define ⟶ implement
       */
      void
      verify_Identity()
        {
        }
      
      
      /** @test
       * @todo WIP 10/26 🔁 define ⟶ implement
       */
      void
      verify_ParamType()
        {
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
