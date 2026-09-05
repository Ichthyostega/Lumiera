/*
  ParameterScale(Test)  -  verify parameter value scales

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file parameter-test.cpp
 ** unit test \ref ParameterScale_test
 */


#include "test/run.hpp"
//#include "test/test-helper.hpp"
#include "lib/par/parameter.hpp"
#include "test/diagnostic-output.hpp"/////////////TODO

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
    
  }//(End)Test fixture
  
  
  
  
  
  
  
  
  
  /**************************************************************************//**
   * @test cover properties of generic parameter containers.
   */
  class ParameterScale_test : public Test
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
          UNIMPLEMENTED ("nebbich");
        }
      
      
      /** @test
       * @todo WIP 9/26 🔁 define ⟶ implement
       */
      void
      verify_typeMarker()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ParameterScale_test, "unit lib");
  
  
  
}}} // namespace lib::par::test
