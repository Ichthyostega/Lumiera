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
//#include "test/test-helper.hpp"
#include "lib/par/parameter.hpp"
//#include "lib/format-cout.hpp"

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
  class Parameter_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verify_typeMarker();
        }
      
      
      void
      simpleUsage()
        {
          Parameter par = Parameter::forType<int>().build();
          CHECK (0 == par.getVal<int>());
          
          par.setVal (12.3);
          CHECK (12 == par.getVal<int>());
          CHECK (12u == par.getVal<uint64_t>());
          CHECK (12.0 == par.getVal<double>());
          CHECK (true == par.getVal<bool>());
        }
      
      
      void
      verify_typeMarker()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (Parameter_test, "unit lib");
  
  
  
}}} // namespace lib::par::test
