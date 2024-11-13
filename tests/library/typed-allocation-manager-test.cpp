/*
  TypedAllocationManager(Test)  -  check interface to pooled allocations

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file typed-allocation-manager-test.cpp
 ** unit test \ref TypedAllocationManager_test
 */


#include "lib/test/run.hpp"
#include "lib/typed-allocation-manager.hpp"
#include "lib/util.hpp"

#include <memory>


namespace lib {
namespace test{
  
  using util::isSameObject;
  using std::shared_ptr;
  
  
  
  namespace { // test data and helpers...
  
    long checksum_ = 0;
    
    /**
     * Yet-another ctor/dtor-tracking test dummy object....
     */
    template<uint siz>
    class DummyObj
      {
        char crap_[siz];
        
      public:
        DummyObj()
          {
            REQUIRE (siz);
            for (uint i=0; i<siz; ++i)
              checksum_ += (crap_[i] = rani(128));
          }
        
       ~DummyObj()
          {
            for (uint i=0; i<siz; ++i)
              checksum_ -= crap_[i];
          }
      };
  }
  
  
  
  /***********************************************************************************//**
   * @test cover the basic implementation of a custom allocator, delegating to mpool.
   *       TypedAllocationManager is a base class, used e.g. to build the CommandRegistry.
   * 
   * @todo as of 9/09 we do heap allocation, but we should use mpool --   see also Ticket #219
   *        
   * @see CommandRegistry
   * @see command-registry-test.cpp
   * @see allocation-cluster-test.cpp
   */
  class TypedAllocationManager_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          CHECK (0 == checksum_);
          seedRand();
          
          TypedAllocationManager allocator;
          
          typedef shared_ptr<DummyObj<1>>  PD1;
          typedef shared_ptr<DummyObj<22>> PD22;
          CHECK (sizeof(DummyObj<1>) != sizeof(DummyObj<22>));
          
            {
              PD1  pD11 = allocator.create<DummyObj<1> >();
              PD1  pD12 = allocator.create<DummyObj<1> >();
              PD22 pD21 = allocator.create<DummyObj<22>>();
              PD22 pD22 = allocator.create<DummyObj<22>>();
              CHECK (pD11);
              CHECK (pD12);
              CHECK (pD21);
              CHECK (pD22);
              CHECK (1 == pD11.use_count());
              CHECK (1 == pD12.use_count());
              CHECK (1 == pD21.use_count());
              CHECK (1 == pD22.use_count());
              CHECK (!isSameObject (*pD11, *pD12));
              CHECK (!isSameObject (*pD11, *pD21));
              CHECK (!isSameObject (*pD11, *pD22));
              CHECK (!isSameObject (*pD12, *pD21));
              CHECK (!isSameObject (*pD12, *pD22));
              CHECK (!isSameObject (*pD21, *pD22));
              
              PD22 pD2x = pD21;
              CHECK (pD2x);
              CHECK (2 == pD21.use_count());
              CHECK (2 == pD2x.use_count());
              CHECK (isSameObject (*pD21, *pD2x));
              
              CHECK (2 == allocator.numSlots<DummyObj<1> >());
              CHECK (2 == allocator.numSlots<DummyObj<22>>());
              
              CHECK (0 == allocator.numSlots<long>()); // query just some unrelated type...
            }
          
          CHECK (0 == allocator.numSlots<DummyObj<1> >());
          CHECK (0 == allocator.numSlots<DummyObj<22>>());
          CHECK (0 == checksum_);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TypedAllocationManager_test, "unit common");
  
  
}} // namespace lib::test
