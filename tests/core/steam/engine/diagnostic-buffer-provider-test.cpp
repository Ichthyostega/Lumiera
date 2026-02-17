/*
  DiagnosticBufferProvider(Test)  -  verify a support facility for diagnostic/test purposes

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file tracking-heap-block-provider-test.cpp
 ** unit test \ref DiagnosticBufferProvider_test
 */


#include "lib/error.hpp"
#include "lib/test/run.hpp"
#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "steam/engine/testframe.hpp"
#include "lib/util.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>


namespace steam {
namespace engine{
namespace test  {
  
  using util::isSameAdr;
  using util::isSameObject;
  
  namespace { // Test fixture
    
    const uint   NUM_ELMS = 50;
    std::vector<uint> testNumbers(NUM_ELMS);
  }
  
  
  /******************************************************************//**
   * @test verify support to write mock components for buffer management.
   *     - HeapMemBufferStore is a „braindead“ implementation of the BufferProvider
   *       interface: it just claims new heap blocks and never de-allocates them.
   *     - adding a layer of instrumentation, the DiagnosticBufferProvider allows
   *       to count and verify allocated buffers and their content after the fact.
   */
  class DiagnosticBufferProvider_test : public Test
    {
      virtual void
      run (Arg)
        {
          seedRand();
          
          simpleExample();
          verifyStandardCase();
          verifyIntrospection();
        }
      
      
      void
      simpleExample()
        {
          DiagnosticBufferProvider provider;
          
          BuffHandle testBuff = provider.lockBufferFor<TestFrame>();
          CHECK (testBuff);
          CHECK (testBuff.accessAs<TestFrame>().isSane());
          
          uint dataID = 1 + rani(29);
          testBuff.accessAs<TestFrame>() = testData(dataID);
          
          provider.emitBuffer   (testBuff);
          provider.releaseBuffer(testBuff);
          
          CHECK (not testBuff);
          CHECK (1 == watch(provider).created.cnt());
          CHECK (1 == watch(provider).emitted.cnt());
          CHECK (1 == watch(provider).released.cnt());
          diagn::Block block0 = watch(provider).emitted[0];
          CHECK (testData(dataID) == block0.accessMemory());
        }
      
      
      void
      verifyStandardCase()
        {
          DiagnosticBufferProvider provider;
          
          BuffDescr buffType = provider.getDescriptorFor(sizeof(int));
          uint numElms = provider.announce(NUM_ELMS, buffType);
          CHECK (0 < numElms);
          CHECK (numElms <= NUM_ELMS);
          
          for (uint i=0; i<numElms; ++i)
            {
              BuffHandle buff = provider.lockBuffer(buffType);
              buff.accessAs<uint>() = testNumbers[i] = rani(100000);
              provider.emitBuffer   (buff);
              provider.releaseBuffer(buff);
            }
          
          for (uint nr=0; nr<numElms; ++nr)
            {
              diagn::Block block = watch(provider).created[nr];
              CHECK (HashVal(block) == block.stateKey);
              CHECK (HashVal(buffType) == block.typeKey);
              CHECK (testNumbers[nr] == block.accessAs<uint>());
              
              auto id = HashVal(block);  // that would also be the ID of the BuffHandle
              CHECK (watch(provider).was_created (id));
              CHECK (watch(provider).was_released (id));
              CHECK (not watch(provider).is_in_use (id));
            }
        }
      
      
      void
      verifyIntrospection()
        {
          DiagnosticBufferProvider provider;
          
          // using large yet untyped buffers this time....
          const size_t BUFF_SIZ = sizeof(TestFrame) * (1+rani(100));
          BuffDescr buffType = provider.getDescriptorFor(BUFF_SIZ);
          
          BuffHandle bu1 = provider.lockBuffer (buffType);
          BuffHandle bu2 = provider.lockBuffer (buffType);
          BuffHandle bu3 = provider.lockBuffer (buffType);
          BuffHandle bu4 = provider.lockBuffer (buffType);
          BuffHandle bu5 = provider.lockBuffer (buffType);
          
          // buffers are all locked now;
          // thus the instrumentation built into DiagnosticBufferProvider
          // should have encountered and recorded each of them, indexed by hash-ID.
          auto inspect = watch(provider);
          CHECK (5 == inspect.created.cnt());
          CHECK (inspect.was_created(bu1));
          CHECK (inspect.was_created(bu2));
          CHECK (inspect.was_created(bu3));
          CHECK (inspect.was_created(bu4));
          CHECK (inspect.was_created(bu5));
          CHECK (inspect.is_in_use  (bu1));
          CHECK (inspect.is_in_use  (bu2));
          CHECK (inspect.is_in_use  (bu3));
          CHECK (inspect.is_in_use  (bu4));
          CHECK (inspect.is_in_use  (bu5));
          CHECK (not inspect.was_used(bu1));
          CHECK (not inspect.was_used(bu2));
          CHECK (not inspect.was_used(bu3));
          CHECK (not inspect.was_used(bu4));
          CHECK (not inspect.was_used(bu5));
          CHECK (not inspect.was_released(bu1));
          CHECK (not inspect.was_released(bu2));
          CHECK (not inspect.was_released(bu3));
          CHECK (not inspect.was_released(bu4));
          CHECK (not inspect.was_released(bu5));
          
          // can use the buffers for real
          using std::byte;
          byte* raw = & bu1.accessAs<byte>();
          std::fill (raw, raw+BUFF_SIZ, byte{0xff});
          CHECK (bu1.accessAs<uint32_t>() == 0xffffffff);
          CHECK (not bu1.accessAs<TestFrame>().isSane());
          
          new(bu1.rawStorage()) TestFrame{1};
          new(bu2.rawStorage()) TestFrame{2};
          new(bu3.rawStorage()) TestFrame{3};
          new(bu4.rawStorage()) TestFrame{4};
          new(bu5.rawStorage()) TestFrame{5};
          
          CHECK (bu1.accessAs<TestFrame>().isPristine());
          
          CHECK (0 == inspect.emitted.cnt());
          
          // now emit buffers in shuffled order
          provider.emitBuffer (bu3);
          provider.emitBuffer (bu1);
          provider.emitBuffer (bu5);
          provider.emitBuffer (bu4);
          provider.emitBuffer (bu2);
          
          CHECK (5 == inspect.emitted.cnt());
          
          CHECK (testData(3) == inspect.emitted[0].get().storage);
          CHECK (testData(1) == inspect.emitted[1].get().storage);
          CHECK (testData(5) == inspect.emitted[2].get().storage);
          CHECK (testData(4) == inspect.emitted[3].get().storage);
          CHECK (testData(2) == inspect.emitted[4].get().storage);

          CHECK (0 == inspect.released.cnt());
          
          CHECK (inspect.was_emitted(bu1));
          CHECK (inspect.was_emitted(bu2));
          CHECK (inspect.was_emitted(bu3));
          CHECK (inspect.was_emitted(bu4));
          CHECK (inspect.was_emitted(bu5));
          // Note fine point: they are in transitory state,
          // thus usage is neither present, nor in the past...
          CHECK (not inspect.is_in_use(bu1));
          CHECK (not inspect.is_in_use(bu2));
          CHECK (not inspect.is_in_use(bu3));
          CHECK (not inspect.is_in_use(bu4));
          CHECK (not inspect.is_in_use(bu5));
          CHECK (not inspect.was_used (bu1));
          CHECK (not inspect.was_used (bu2));
          CHECK (not inspect.was_used (bu3));
          CHECK (not inspect.was_used (bu4));
          CHECK (not inspect.was_used (bu5));
          
          bu5.release();
          CHECK (not bu5);
          CHECK (not bu5.isAllotted());
          CHECK (not inspect.is_in_use(bu5));
          CHECK (    inspect.was_used (bu5));
          
          CHECK (not bu2);
          CHECK (    bu2.isAllotted());
          CHECK (not inspect.was_used (bu2));
          CHECK (not inspect.was_released(bu2));
          CHECK (1 == inspect.released.cnt());
          
          bu2.release();
          bu2.release();
          bu5.release();
          CHECK (2 == inspect.released.cnt());
          // simply because the redundant call was suppressed
          CHECK (testData(5) == inspect.released[0].get().storage);
          CHECK (testData(2) == inspect.released[1].get().storage);
          
          // when invoked directly on BufferProvider interface,
          // redundant call is suppressed with a warning log
          provider.releaseBuffer(bu5);
          CHECK (2 == inspect.released.cnt());
          
          CHECK (not bu2.isValid());
          CHECK (not bu3.isValid());
          CHECK (not bu2.isAllotted());
          CHECK (    bu3.isAllotted());
          
          bu1.release();
          bu3.release();
          bu4.release();
          
          CHECK (not bu1.isAllotted());
          CHECK (not bu2.isAllotted());
          CHECK (not bu3.isAllotted());
          CHECK (not bu4.isAllotted());
          CHECK (not bu5.isAllotted());
          
          CHECK (5 == inspect.released.cnt());
          // buffer data is recorded in order of occurrence
          CHECK (testData(5) == inspect.released[0].get().storage);
          CHECK (testData(2) == inspect.released[1].get().storage);
          CHECK (testData(1) == inspect.released[2].get().storage);
          CHECK (testData(3) == inspect.released[3].get().storage);
          CHECK (testData(4) == inspect.released[4].get().storage);
          // Instrumentation keeps separate records for each stage of the protocol
          CHECK (testData(3) == inspect.emitted[0].get().storage);
          CHECK (testData(1) == inspect.emitted[1].get().storage);
          CHECK (testData(5) == inspect.emitted[2].get().storage);
          CHECK (testData(4) == inspect.emitted[3].get().storage);
          CHECK (testData(2) == inspect.emitted[4].get().storage);
          
          CHECK (not isSameObject (inspect.created[0]              , inspect.emitted[1]));
          CHECK (not isSameObject (inspect.emitted[1]              , inspect.released[2]));
          CHECK (    isSameAdr    (inspect.created[0].get().storage, inspect.emitted[1].get().storage));
          CHECK (    isSameAdr    (inspect.emitted[1].get().storage, inspect.released[2].get().storage));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DiagnosticBufferProvider_test, "unit player");
  
  
  
}}} // namespace steam::engine::test
