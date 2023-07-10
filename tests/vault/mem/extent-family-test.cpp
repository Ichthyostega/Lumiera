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
//#include "lib/format-cout.hpp"
//#include "lib/util.hpp"

#include <utility>

using test::Test;
//using std::move;
using util::isSameObject;


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
           wrapAround();
        }
      
      
      /** @test demonstrate a simple usage scenario
       */
      void
      simpleUsage()
        {
          Extents extents{5};
          Extent& extent = *extents.active();
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
          ExtentFamily<int, 10> extents{5};
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
          Iter it = extents.active();
          CHECK (it);
          
          Extent& extent{*it};
          CHECK (10 == extent.size());
          
          int num = rand() % 1000;
          extent[2] = num;
          CHECK (num == extent[2]);
          
          ++it;
          CHECK (it);
          Extent& nextEx{*it};
          CHECK (not isSameObject(extent, nextEx));
          nextEx[5] = extent[2] + 1;
          CHECK (num   == extent[2]);
          CHECK (num+1 == nextEx[5]);
        }
      
      
      
      /** @test verify in detail how iteration wraps around to also reuse
       *        previously dropped extents, possibly rearranging the internal
       *        management-vector to allow growing new extents at the end.
       */
      void
      wrapAround()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ExtentFamily_test, "unit memory");
  
  
  
}}} // namespace vault::mem::test
