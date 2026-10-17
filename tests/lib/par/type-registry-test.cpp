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
      
      
      /** @test verify coherent storage handling and proper clean-up
       *      - slots are filled-up in sequence
       *      - the ID corresponds to the slot-count
       *      - new extents are opend precisely when the
       *        previous Extent is filled completely
       *      - Exceptions during slot-allocation are absorbed,
       *        without blocking the slot or corrupting data
       *      - the designate storage space can be utilised completely
       *      - when the registry is full, further allocations are blocked
       *      - but even a then the registry remains in sane state
       *      - all destructors of all slots are invoked
       * @todo WIP 10/26 ✔ define ⟶ ✔ implement
       */
      void
      verify_storageSanity()
        {
          Dummy::checksum() = 0;
          {  //  track instances by checksum...
            Registry registry;
            CHECK (0 == registry.size());
            CHECK (0 == registry.numExtents());
            
            RegID i0 = registry.makeSlot();
            CHECK (1 == registry.size());
            CHECK (1 == registry.numExtents());
            
            CHECK (i0 == 0);
            int rand0 = registry[i0].getID();
            CHECK (0 < rand0);
            CHECK (Dummy::checksum() == registry[i0].getID());
            
            RegID i1 = registry.makeSlot();
            CHECK (2 == registry.size());
            CHECK (1 == registry.numExtents());
            int rand1 = registry[i1].getID();
            
            CHECK (i1 == 1);
            CHECK (Dummy::checksum() == registry[i0].getID()
                                      + registry[i1].getID());
            
            for (uint i=0; i<256; ++i)
              registry.makeSlot();
            // first Extent was filled up
            CHECK (256+2 == registry.size());
            CHECK (2     == registry.numExtents());
            CHECK (rand1 == registry[i1] .getID());
            CHECK (rand0 == registry[i0] .getID());
            
            // Verify no leak in case of Exceptions during slot allocation
            int rand257 = registry[257].getID();
            CHECK (0 < rand257);
            long checkPre = Dummy::checksum();
            Dummy::activateCtorFailure();
            try {
                registry.makeSlot(-55);
                NOTREACHED ("did not throw");
              }
            catch (int& magic)
              {
                CHECK (magic == -55);
                // since the dtor is not called on ctor failure,
                // we have to fix the checksum explicitly here
                Dummy::checksum() -= (-55);
                Dummy::activateCtorFailure(false);
              }
            // now the important point:
            CHECK (256+2 == registry.size());    // next slot was not marked as claimed
            CHECK (2     == registry.numExtents());
            CHECK (Dummy::checksum() == checkPre);
            CHECK (rand257 == registry[257].getID());
            CHECK (rand1   == registry[i1] .getID());
            CHECK (rand0   == registry[i0] .getID());
            
            // fill-up the second Extent completely
            for (uint i=2; i<256; ++i)
              registry.makeSlot();
            CHECK (2*256 == registry.size());
            CHECK (2     == registry.numExtents());
            
            // and then fill up all the designated storage
            for (uint i=registry.size(); i<256*256; ++i)
              registry.makeSlot();
            CHECK (256*256 == registry.size());
            CHECK (256     == registry.numExtents());
            
            // can not allocate further...
            checkPre = Dummy::checksum();
            VERIFY_FAIL ("space exhausted for type Dummy"
                        , registry.makeSlot() );
            
            // yet the registry is in sane state
            CHECK (Dummy::checksum() == checkPre);
            CHECK (rand0   == registry[i0] .getID());
            CHECK (rand1   == registry[i1] .getID());
            CHECK (rand257 == registry[257].getID());
            
          }//automatic clean-up of Registry triggered here
          CHECK (0 == Dummy::checksum());
        }
      
      
      /** @test
       * @todo WIP 10/26 🔁 define ⟶ implement
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
