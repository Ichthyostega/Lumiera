/*
  GenericIdFunction(Test)  -  cover instance and type id generation

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file generic-id-function-test.cpp
 ** unit test \ref GenericIdFunction_test
 */


#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"
#include "lib/idi/genfunc.hpp"
//#include "lib/format-cout.hpp"

//#include <utility>
//#include <string>
//#include <vector>

//using std::string;
//using std::vector;
//using std::swap;


namespace lib {
namespace idi {
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
   * @test cover a standard scheme to generate type and instance identifiers.
   *       
   * @see EntryID
   * @see StructFactory
   */
  class GenericIdFunction_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verify_typeSymbol();
          verify_fullTypeID();
          verify_prefix();
          verify_typeHash();
          verify_symbolicInstanceID();
        }
      
      
      void
      simpleUsage()
        {
          CHECK ("int" == typeSymbol<int>());
          CHECK ("bool" == typeSymbol<bool>());
          
          CHECK ("Some" == categoryFolder<SomeThing>());
        }
      
      
      void
      verify_typeSymbol()
        {
        }
      
      
      void
      verify_fullTypeID()
        {
          CHECK("Some_Thing" == typeFullID<SomeThing>());
        }
      
      
      void
      verify_prefix()
        {
        }
      
      
      void
      verify_typeHash()
        {
        }
      
      
      void
      verify_symbolicInstanceID()
        {
          class Unique { };
          
          CHECK ("Unique.001" == generateSymbolicID<Unique>());
          CHECK ("Unique.002" == generateSymbolicID<Unique>());
          CHECK ("Unique.003" == generateSymbolicID<Unique>());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (GenericIdFunction_test, "unit common");
  
  
  
}}} // namespace lib::idi::test
