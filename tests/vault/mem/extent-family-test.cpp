/*
  ExtentFamily(Test)  -  verify cyclic extents allocation scheme

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file extent-family-test.cpp
 ** unit test \ref ExtentFamily_test
 */


#include "lib/test/run.hpp"
#include "vault/mem/extent-family.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/util.hpp"

#include <utility>

using test::Test;
using util::isnil;
using util::isSameObject;
using lib::explore;


namespace vault{
namespace mem  {
namespace test {
  
  using Extents = ExtentFamily<int, 10>;
  using Extent  = Extents::Extent;
  using Iter    = Extents::iterator;
  
  
  
  
  /***************************************************************//**
   * @test document and verify a memory management scheme to maintain
   *       a flexible set of _»memory extents«_ for cyclic usage.
   * @see BlockFlow_test
   */
  class ExtentFamily_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
           simpleUsage();
           use_and_drop();
           iteration();
           reuseUnclean();
           wrapAround();
        }
      
      
      /** @test demonstrate a simple usage scenario
       */
      void
      simpleUsage()
        {
          Extents extents{5};
          extents.openNew();
          Extent& extent = *extents.begin();
          CHECK (10 == extent.size());
          
          int num = rani(1000);
          extent[2] = num;
          extent[5] = num+5;
          CHECK (num   == extent[2]);
          CHECK (num+5 == extent[5]);
        }
      
      
      
      /** @test verify claiming new and discarding old slots
       */
      void
      use_and_drop()
        {
          Extents extents{5};
          CHECK ( 0 == watch(extents).first());
          CHECK ( 0 == watch(extents).last());
          CHECK ( 0 == watch(extents).active());
          CHECK ( 5 == watch(extents).size());
          
          extents.openNew(3);
          CHECK ( 0 == watch(extents).first());
          CHECK ( 3 == watch(extents).last());
          CHECK ( 3 == watch(extents).active());
          CHECK ( 5 == watch(extents).size());
          
          extents.dropOld(2);
          CHECK ( 2 == watch(extents).first());
          CHECK ( 3 == watch(extents).last());
          CHECK ( 1 == watch(extents).active());
          CHECK ( 5 == watch(extents).size());
        }
      
      
      
      /** @test verify access to the extents by iteration,
       *        thereby possibly claiming the next extents
       */
      void
      iteration()
        {
          Extents extents{5};
          CHECK (isnil (extents));
          Iter it = extents.begin();
          CHECK (isnil (it));  // no extents provided yet
          
          extents.openNew(2); //  allot two extents for active use
          CHECK (it);
          CHECK (0 == it.getIndex());
          CHECK (isSameObject(*it, *extents.begin()));
          
          Extent& extent{*it};
          CHECK (10 == extent.size());
          
          int num = rani(1000);
          extent[2] = num;
          CHECK (num == extent[2]);
          
          ++it;
          CHECK (it);
          CHECK (1 == it.getIndex());
          Extent& nextEx{*it};
          CHECK (not isSameObject(extent, nextEx));
          CHECK (isSameObject(nextEx, *extents.last()));
          nextEx[5] = extent[2] + 1;
          CHECK (num   == extent[2]);
          CHECK (num+1 == nextEx[5]);
          
          ++it;
          CHECK (it == extents.end());
          CHECK (isnil (it));      // only two allocated
          it.expandAlloc();       //  but can expand allocation
          CHECK (it);
          
          // iterate again to verify we get the same memory blocks
          it = extents.begin();
          CHECK (isSameObject(*it, extent));
          CHECK ((*it)[2] == num);
          ++it;
          CHECK (isSameObject(*it, nextEx));
          CHECK ((*it)[5] == num+1);
        }
      
      
      
      /** @test verify that neither constructors nor destructors are invoked
       *        automatically when discarding or re-using extents.
       */
      void
      reuseUnclean()
        {
            struct Probe
              {
                short val;
                Probe() : val(1 + rani(1000)) { }
               ~Probe() { val = 0; }
              };
          
          using SpecialExtents = ExtentFamily<Probe, 1000>;
          
          SpecialExtents spex{3};
          spex.openNew(2);
          CHECK ( 0 == watch(spex).first());
          CHECK ( 2 == watch(spex).last());
          
          // implant a new Probe object into each »slot« of the new extent
          auto& extent = *spex.begin();
          for (Probe& probe : extent)
            new(&probe) Probe;
          
          auto calcChecksum = [](SpecialExtents::Extent& extent) -> size_t
                                {
                                  size_t sum{0};
                                  for (Probe& probe : extent)
                                    sum += probe.val;
                                  return sum;
                                };
          
          size_t checksum = calcChecksum (*spex.begin());
          
          // discard first extent, i.e. mark it as unused
          // while the underlying memory block remains allocated
          // and data within this block is not touched
          spex.dropOld(1);
          CHECK ( 1 == watch(spex).first());
          CHECK ( 2 == watch(spex).last());
          
          // the »begin« (i.e. the first active extent is now another memory block
          CHECK (not isSameObject (extent, *spex.begin()));
          size_t checkSecond = calcChecksum (*spex.begin());
          CHECK (checkSecond != checksum);
          
          // but the random data generated above still sits in the original (first) memory block
          CHECK (checksum == calcChecksum (extent));
          
          // now let the actively allotted extents "wrap around"...
          spex.dropOld(1);
          CHECK ( 2 == watch(spex).first());
          CHECK ( 2 == watch(spex).last());
          spex.openNew(2);
          CHECK ( 2 == watch(spex).first());
          CHECK ( 1 == watch(spex).last());
          
          auto iter = spex.begin();
          CHECK ( 2 == iter.getIndex());
          ++iter;
          CHECK ( 0 == iter.getIndex());
          CHECK (isSameObject(*iter, extent));
          
          // and during all those allotting and dropping, data in the memory block was not touched,
          // which also proves that constructors or destructors of the nominal "content" are not invoked
          CHECK (checksum == calcChecksum (extent));
        }
      
      
      
      /** @test verify in detail how iteration wraps around to also reuse
       *        previously dropped extents, possibly rearranging the internal
       *        management-vector to allow growing new extents at the end.
       *        - existing allocations are re-used cyclically
       *        - this may lead to a »wrapped« internal state
       *        - necessitating to expand allocations in the middle
       *        - yet all existing Extent addresses remain stable
       */
      void
      wrapAround()
        {
          // Helper to capture the storage addresses of all currently active Extents
          auto snapshotAdr  = [](Extents& extents)
                                  {
                                    auto takeAdr  = [](auto& x){ return &*x; };
                                    return explore(extents).transform(takeAdr).effuse();
                                  };
          auto verifyAdr    = [](auto snapshot, auto it)
                                  {
                                    for (auto oldAddr : snapshot)
                                      {
                                        if (not isSameObject(*oldAddr, *it))
                                          return false;
                                        ++it;
                                      }
                                    return true;
                                  };
          
          
          Extents extents{5};
          CHECK ( extents.empty());
          CHECK ( 0 == watch(extents).first());
          CHECK ( 0 == watch(extents).last());
          CHECK ( 0 == watch(extents).active());
          CHECK ( 5 == watch(extents).size());
          
          extents.openNew(4);
          CHECK ( 0 == watch(extents).first());
          CHECK ( 4 == watch(extents).last());
          CHECK ( 4 == watch(extents).active());
          CHECK ( 5 == watch(extents).size());
          
          auto snapshot = snapshotAdr(extents);            // capture *addresses* of currently active Extents
          CHECK (4 == snapshot.size());
          
          extents.openNew();
          CHECK ( 0 == watch(extents).first());
          CHECK ( 5 == watch(extents).last());
          CHECK ( 5 == watch(extents).active());
          CHECK (10 == watch(extents).size());             // Note: heuristics to over-allocate to some degree
          CHECK (verifyAdr (snapshot, extents.begin()));
          
          extents.dropOld(3);                              // place the active window such as to start on last snapshotted Extent
          CHECK ( 3 == watch(extents).first());
          CHECK ( 5 == watch(extents).last());
          CHECK ( 2 == watch(extents).active());
          CHECK (10 == watch(extents).size());
          CHECK (isSameObject (*extents.begin(), *snapshot.back()));
          
          extents.openNew(6);                              // now provoke a »wrapped« state of internal management of active Extents
          CHECK ( 3 == watch(extents).first());            // ...Note: the position of the *first* active Extent...
          CHECK ( 1 == watch(extents).last());             // ... is *behind* the position of the last active Extent
          CHECK ( 8 == watch(extents).active());           // ... implying that the active strike wraps at allocation end
          CHECK (10 == watch(extents).size());
          snapshot = snapshotAdr (extents);                // take a new snapshot; this also verifies proper iteration
          CHECK (8 == snapshot.size());
          
          extents.openNew(2);                              // ask for more than can be accommodated without ambiguity
          CHECK ( 8 == watch(extents).first());            // ...Note: new allocation was inserted, existing tail shifted
          CHECK ( 3 == watch(extents).last());             // ... allowing for the requested two »slots« to be accommodated
          CHECK (10 == watch(extents).active());
          CHECK (15 == watch(extents).size());
          CHECK (verifyAdr (snapshot, extents.begin()));   // ... yet all existing Extent addresses have been rotated transparently
          
          extents.dropOld(10);                             // close out all active slots, wrapping the first-pos to approach last
          CHECK ( 3 == watch(extents).first());
          CHECK ( 3 == watch(extents).last());
          CHECK ( 0 == watch(extents).active());
          CHECK (15 == watch(extents).size());
          
          extents.openNew(12);                             // provoke a special boundary situation, where the end is *just wrapped*
          CHECK ( 3 == watch(extents).first());
          CHECK ( 0 == watch(extents).last());
          CHECK (12 == watch(extents).active());
          CHECK (15 == watch(extents).size());
          
          extents.dropOld(11);                             // and make this boundary situation even more nasty, just sitting on the rim
          CHECK (14 == watch(extents).first());
          CHECK ( 0 == watch(extents).last());
          CHECK ( 1 == watch(extents).active());
          CHECK (15 == watch(extents).size());
          
          CHECK (14 == extents.begin().getIndex());
          snapshot = snapshotAdr (extents);                // verify iteration end just after wrapping properly detected
          CHECK (1 == snapshot.size());
          CHECK (isSameObject (*extents.begin(), *snapshot.front()));
          
          extents.openNew(14);                             // and now provoke further expansion, adding new allocation right at start
          CHECK (19 == watch(extents).first());            // ...Note: first must be relocated to sit again at the very rim
          CHECK (14 == watch(extents).last());             // ... to allow last to sit at the index previously used by first
          CHECK (15 == watch(extents).active());
          CHECK (20 == watch(extents).size());
          
          CHECK (19 == extents.begin().getIndex());        // ... yet address of the first Extent remains the same, just held in another slot
          CHECK (isSameObject (*extents.begin(), *snapshot.front()));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ExtentFamily_test, "unit memory");
  
  
  
}}} // namespace vault::mem::test
