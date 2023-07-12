/*
  ExtentFamily(Test)  -  verify cyclic extents allocation scheme

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/

/** @file extent-family-test.cpp
 ** unit test \ref ExtentFamily_test
 */


#include "lib/test/run.hpp"
#include "vault/mem/extent-family.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/format-cout.hpp"////////////////////TODO
#include "lib/test/diagnostic-output.hpp"////////////////////TODO
#include "lib/iter-explorer.hpp"
//#include "lib/util.hpp"

#include <utility>

using test::Test;
//using std::move;
using util::isnil;
using util::isSameObject;
using lib::explore;


namespace vault{
namespace mem  {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
//  using lib::time::Time;
  
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
          
          int num = rand() % 1000;
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
          Iter it = extents.begin();
          CHECK (isnil (it));  // no extents provided yet
          
          extents.openNew(2); //  allot two extents for active use
          CHECK (it);
          CHECK (0 == it.getIndex());
          
          Extent& extent{*it};
          CHECK (10 == extent.size());
          
          int num = rand() % 1000;
          extent[2] = num;
          CHECK (num == extent[2]);
          
          ++it;
          CHECK (it);
          CHECK (1 == it.getIndex());
          Extent& nextEx{*it};
          CHECK (not isSameObject(extent, nextEx));
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
                Probe() : val(1 + rand() % 1000) { }
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
       */
      void
      wrapAround()
        {
          Extents extents{5};
          CHECK ( 0 == watch(extents).first());
          CHECK ( 0 == watch(extents).last());
          CHECK ( 0 == watch(extents).active());
          CHECK ( 5 == watch(extents).size());
          
          extents.openNew(4);
          CHECK ( 0 == watch(extents).first());
          CHECK ( 4 == watch(extents).last());
          CHECK ( 4 == watch(extents).active());
          CHECK ( 5 == watch(extents).size());
          
          auto takeAdr  = [](auto& x){ return &*x; };
          auto snapshot = explore(extents).transform(takeAdr).effuse();
SHOW_EXPR(snapshot.size())
SHOW_EXPR(snapshot[0])
SHOW_EXPR(snapshot[3])
          
          extents.openNew();
SHOW_EXPR(watch(extents).first())
SHOW_EXPR(watch(extents).last())
SHOW_EXPR(watch(extents).active())
SHOW_EXPR(watch(extents).size())
          CHECK ( 0 == watch(extents).first());
          CHECK ( 5 == watch(extents).last());
          CHECK ( 5 == watch(extents).active());
          CHECK (10 == watch(extents).size());             // Note: heuristics to over-allocate to some degree
          auto it = extents.begin();
          for (auto oldAddr : snapshot)
            {
              CHECK (isSameObject(*oldAddr, *it));
              ++it;
            }
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ExtentFamily_test, "unit memory");
  
  
  
}}} // namespace vault::mem::test
