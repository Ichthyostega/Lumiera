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

//#include <utility>

using test::Test;
//using std::move;
//using util::isSameObject;


namespace vault{
namespace mem  {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
//  using lib::time::Time;
  
  
  
  
  
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
           calculateDeadline();
           setupDependentJob();
        }
      
      
      /** @test demonstrate a simple usage scenario
       */
      void
      simpleUsage()
        {
          ExtentFamily extents;
        }
      
      
      
      /** @test verify the timing calculations to establish
       *        the scheduling deadline of a simple render job
       */
      void
      calculateDeadline()
        {
        }
      
      
      
      /** @test verify the setup of a prerequisite job in relation
       *        to the master job depending on this prerequisite
       */
      void
      setupDependentJob()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (ExtentFamily_test, "unit memory");
  
  
  
}}} // namespace vault::mem::test
