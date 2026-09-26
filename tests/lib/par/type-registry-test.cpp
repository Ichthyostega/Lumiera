/*
  TypeRegistry(Test)  -  verify nominal and ordinal scale enumerations

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file type-registry.cpp
 ** unit test \ref TypeRegistry_test
 */


#include "test/run.hpp"
//#include "test/test-helper.hpp"
#include "lib/par/type-registry.hpp"
#include "test/tracking-dummy.hpp"
#include "lib/util.hpp"
#include "test/diagnostic-output.hpp"/////////////TODO

//#include <utility>
//#include <string>
//#include <vector>

//using std::string;
//using std::vector;
//using std::swap;
using util::isSameObject;


namespace lib {
namespace par {
namespace test{
  
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  namespace {//Test fixture....
    
  }//(End)Test fixture
  
  using ::test::Dummy;
  using Registry = TypeRegistry<Dummy>;
  using RegID = Registry::ID;
  
  
  
  
  
  
  /**************************************************************************//**
   * @test verify a index/extent based registration table for type descriptors
   *     - new slots can be claimed and then re-accessed by ID
   *     - storage is stable even while new extents are added, and not leaked
   *     - read access can happen concurrently with allocating new slots.
   */
  class TypeRegistry_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          
          simpleUsage();
          verify_storageSanity();
          verify_concurrentAccess();
        }
      
      
      void
      simpleUsage()
        {
          const int MARK = 1 + rani (1000);
          
          Registry registry;
          RegID id = registry.makeSlot (MARK);
          
          Dummy& entry = registry[id];
          CHECK (MARK == entry.getID());
          CHECK (isSameObject (entry, registry[id]));
          
          VERIFY_FAIL ("not yet allocated", registry[12345]);
        }
      
      
      /** @test
       * @todo WIP 9/26 🔁 define ⟶ implement
       */
      void
      verify_storageSanity()
        {
          UNIMPLEMENTED ("verify coherent storage handling and proper clean-up");
        }
      
      
      /** @test
       * @todo WIP 9/26 🔁 define ⟶ implement
       */
      void
      verify_concurrentAccess()
        {
          UNIMPLEMENTED ("press slot allocation and data access concurrently");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypeRegistry_test, "unit lib");
  
  
  
}}} // namespace lib::par::test
