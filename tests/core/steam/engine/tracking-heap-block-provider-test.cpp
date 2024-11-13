/*
  TrackingHeapBlockProvider(Test)  -  verify a support facility for diagnostic/test purposes

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file tracking-heap-block-provider-test.cpp
 ** unit test \ref TrackingHeapBlockProvider_test
 */


#include "lib/error.hpp"
#include "lib/test/run.hpp"
#include "steam/engine/tracking-heap-block-provider.hpp"
#include "steam/engine/buffhandle-attach.hpp"
#include "steam/engine/testframe.hpp"

#include <vector>


namespace steam  {
namespace engine{
namespace test  {
  
  
  
  namespace { // Test fixture
    
    const size_t TEST_ELM_SIZE = sizeof(uint);
    const uint   MAX_ELMS = 50;
    
    std::vector<uint> testNumbers(MAX_ELMS);
    
    
    bool 
    has_expectedContent (uint nr, diagn::Block& memoryBlock)
    {
      void* mem = memoryBlock.accessMemory();
      uint data = *static_cast<uint*> (mem);
      
      return data == testNumbers[nr];
    }
    
    bool
    verifyUsedBlock (uint nr, diagn::Block& memoryBlock)
    {
      return memoryBlock.was_used()
          && memoryBlock.was_closed()
          && has_expectedContent (nr, memoryBlock);
    }
  }
  
  
  /******************************************************************//**
   * @test verify a test support facility, used to write mock components
   *       to test the lumiera engine. The TrackingHeapBlockProvider is a
   *       braindead implementation of the BufferProvider interface: it just
   *       claims new heap blocks and never de-allocates them, allowing other
   *       test and mock objects to verify allocated buffers after the fact.
   */
  class TrackingHeapBlockProvider_test : public Test
    {
      virtual void
      run (Arg) 
        {
          seedRand();
          
          simpleExample();
          verifyStandardCase();
          verifyTestProtocol();
        }
      
      
      void
      simpleExample()
        {
          TrackingHeapBlockProvider provider;
          
          BuffHandle testBuff = provider.lockBufferFor<TestFrame>();
          CHECK (testBuff);
          CHECK (testBuff.accessAs<TestFrame>().isSane());
          
          uint dataID = 1 + rani(29);
          testBuff.accessAs<TestFrame>() = testData(dataID);
          
          provider.emitBuffer   (testBuff);
          provider.releaseBuffer(testBuff);
          
          diagn::Block& block0 = provider.access_emitted(0);
          CHECK (testData(dataID) == block0.accessMemory());
        }
      
      
      void
      verifyStandardCase()
        {
          TrackingHeapBlockProvider provider;
          
          BuffDescr buffType = provider.getDescriptorFor(TEST_ELM_SIZE);
          uint numElms = provider.announce(MAX_ELMS, buffType);
          CHECK (0 < numElms);
          CHECK (numElms <= MAX_ELMS);
          
          for (uint i=0; i<numElms; ++i)
            {
              BuffHandle buff = provider.lockBuffer(buffType);
              buff.accessAs<uint>() = testNumbers[i] = rani(100000);
              provider.emitBuffer   (buff);
              provider.releaseBuffer(buff);
            }
          
          for (uint nr=0; nr<numElms; ++nr)
            {
              CHECK (verifyUsedBlock (nr, provider.access_emitted(nr)));
            }
        }
      
      
      void
      verifyTestProtocol()
        {
          TrackingHeapBlockProvider provider;
          
          BuffDescr buffType = provider.getDescriptorFor(TEST_ELM_SIZE);
          
          BuffHandle bu1 = provider.lockBuffer (buffType);
          BuffHandle bu2 = provider.lockBuffer (buffType);
          BuffHandle bu3 = provider.lockBuffer (buffType);
          BuffHandle bu4 = provider.lockBuffer (buffType);
          BuffHandle bu5 = provider.lockBuffer (buffType);
          
          // buffers are locked, 
          // but still within the per-type allocation pool
          // while the output sequence is still empty
          CHECK (!provider.access_emitted(0).was_used());
          CHECK (!provider.access_emitted(1).was_used());
          CHECK (!provider.access_emitted(2).was_used());
          CHECK (!provider.access_emitted(3).was_used());
          CHECK (!provider.access_emitted(4).was_used());
          
          // can use the buffers for real
          bu1.accessAs<uint>() = 1;
          bu2.accessAs<uint>() = 2;
          bu3.accessAs<uint>() = 3;
          bu4.accessAs<uint>() = 4;
          bu5.accessAs<uint>() = 5;
          
          CHECK (0 == provider.emittedCnt());
          
          // now emit buffers in shuffled order
          provider.emitBuffer (bu3);
          provider.emitBuffer (bu1);
          provider.emitBuffer (bu5);
          provider.emitBuffer (bu4);
          provider.emitBuffer (bu2);
          
          CHECK (5 == provider.emittedCnt());
          
          CHECK (3 == provider.accessAs<uint>(0));
          CHECK (1 == provider.accessAs<uint>(1));
          CHECK (5 == provider.accessAs<uint>(2));
          CHECK (4 == provider.accessAs<uint>(3));
          CHECK (2 == provider.accessAs<uint>(4));
          
          CHECK ( provider.access_emitted(0).was_used());
          CHECK ( provider.access_emitted(1).was_used());
          CHECK ( provider.access_emitted(2).was_used());
          CHECK ( provider.access_emitted(3).was_used());
          CHECK ( provider.access_emitted(4).was_used());
          
          CHECK (!provider.access_emitted(0).was_closed());
          CHECK (!provider.access_emitted(1).was_closed());
          CHECK (!provider.access_emitted(2).was_closed());
          CHECK (!provider.access_emitted(3).was_closed());
          CHECK (!provider.access_emitted(4).was_closed());
          
          bu5.release();
          CHECK (!provider.access_emitted(0).was_closed());
          CHECK (!provider.access_emitted(1).was_closed());
          CHECK ( provider.access_emitted(2).was_closed());
          CHECK (!provider.access_emitted(3).was_closed());
          CHECK (!provider.access_emitted(4).was_closed());
          
          bu2.release();
          bu2.release();
          bu5.release();
          CHECK (!provider.access_emitted(0).was_closed());
          CHECK (!provider.access_emitted(1).was_closed());
          CHECK ( provider.access_emitted(2).was_closed());
          CHECK (!provider.access_emitted(3).was_closed());
          CHECK ( provider.access_emitted(4).was_closed());
          
          CHECK (!bu2);
          CHECK (bu3);
          
          bu1.release();
          bu3.release();
          bu4.release();
          
          CHECK (5 == provider.emittedCnt());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TrackingHeapBlockProvider_test, "unit player");
  
  
  
}}} // namespace steam::engine::test
