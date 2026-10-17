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
#include "test/test-helper.hpp"
#include "lib/par/type-registry.hpp"
#include "lib/scoped-collection.hpp"
#include "test/tracking-dummy.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/sync-barrier.hpp"
#include "lib/thread.hpp"
#include "lib/random.hpp"
#include "lib/util.hpp"

#include <chrono>

using lib::explore;
using util::isSameObject;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using std::chrono_literals::operator ""ms;


namespace lib {
namespace par {
namespace test{
  
  using ::test::Dummy;
  using Registry = TypeRegistry<Dummy>;
  using RegID = Registry::ID;
  
  namespace {//Test configuration....
    
    const uint NUM_THREADS = util::max (2u, std::thread::hardware_concurrency());
    const uint NUM_ENTRIES = Registry::MAX_ELEMS / NUM_THREADS;
    
  }//(End)Test setup
  
  
  
  
  
  
  /**************************************************************************//**
   * @test verify an index/extent based registration table for type descriptors
   *     - new slots can be claimed and then re-accessed by ID
   *     - storage is stable even while new extents are added, and not leaked
   *     - read access can happen concurrently while allocating new slots.
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
       *      - new extents are opened precisely when the
       *        previous Extent is filled completely
       *      - Exceptions during slot-allocation are absorbed,
       *        without blocking the slot or corrupting data
       *      - the designate storage space can be utilised completely
       *      - when the registry is full, further allocations are blocked
       *      - but even a then the registry remains in sane state
       *      - all destructors of all slots are invoked
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
      
      
      /** @test press slot allocation and data access concurrently
       *      - for maximum pressure avoid sleeping threads and scheduling overhead
       *      - each worker adds the numbers 1...N to the registry
       *      - each worker uses just a tiny delay to shift processing pattern
       *      - after adding the next number, read recent new entries and sum up
       *      - at the end, each thread must have seen all entries, and all the
       *        summed-up numbers must be consistent.
       */
      void
      verify_concurrentAccess()
        {
          TypeRegistry<uint> testRegistry;
          lib::SyncBarrier startTrigger{NUM_THREADS+1};
          auto expectedSum = [](size_t n){ return n/2 * (n+1); }; // Σ 1...n
          
          auto genLoad = [&]{ return [&testRegistry
                                     ,&startTrigger
                                     ,expectedSum
                                     ,random = 2 + rani(4)   // evaluated for each thread anew, and in the main thread
                                     ]{
                                        size_t mySum{0};
                                        int lastSeen{-1};
                                        volatile size_t blackHole{0};
                                        auto randomDelay = [&]{ if (0 == lastSeen % random) blackHole += lastSeen; };
                                        
                                        // all threads start at once
                                        startTrigger.sync();
                                        
                                        for (uint i=1; i <= NUM_ENTRIES; ++i)
                                          {
                                            randomDelay();
                                            
                                            //--------------------------------------- allocate new entry
                                            auto entryID = testRegistry.makeSlot(i);
                                            
                                            randomDelay();
                                            
                                            //--------------------------------------- read other entries
                                            for (int e=entryID; e > lastSeen; --e)
                                              mySum += testRegistry[e];
                                            
                                            lastSeen = int(entryID);
                                          }
                                        // to sleep for an eternity
                                        sleep_for (50ms);
                                        
                                        // complete to read all other entries
                                        for (uint e=lastSeen+1; e < NUM_THREADS*NUM_ENTRIES; ++e)
                                          mySum += testRegistry[e];
                                        
                                        CHECK (mySum == NUM_THREADS * expectedSum (NUM_ENTRIES));
                                      };
                            };
          
          using Threads = lib::ScopedCollection<lib::Thread>;
          Threads threads{NUM_THREADS, Threads::invoke(genLoad)};
          
          // unleash
          startTrigger.sync();
          
          sleep_for(60ms);
          
          while (explore(threads).has_any())
            yield();
          
          CHECK (testRegistry.size() == NUM_THREADS * NUM_ENTRIES);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypeRegistry_test, "unit lib");
  
  
  
}}} // namespace lib::par::test
