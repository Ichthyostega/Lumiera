/*
  BufferProviderProtocol(Test)  -  demonstration of buffer provider usage cycle

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file buffer-provider-protocol-test.cpp
 ** unit test \ref BufferProviderProtocol_test
 */


#include "lib/error.hpp"
#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/tracking-dummy.hpp"
#include "lib/util-foreach.hpp"
#include "lib/iter-zip.hpp"
#include "steam/engine/testframe.hpp"
#include "steam/engine/test-rand-ontology.hpp"
#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "lib/test/diagnostic-output.hpp"////////////////////TODO

using util::isSameObject;
using util::for_each;
using lib::HashVal;
using lib::zip;


namespace steam {
namespace engine{
namespace test  {
  
  using lib::test::Dummy;
  
  using steam::engine::BuffHandle;
  using LERR_(LIFECYCLE);
  using LERR_(LOGIC);
  
  
  namespace { // Test fixture
    
    const uint TEST_ELMS = 20;
    
    
    HashVal
    do_some_computation (TestFrame const& src, TestFrame& work, int64_t* data)
    {
      REQUIRE (data);
      ont::Param param = src.getChecksum();
      ont::manipulateFrame (&work, &src, param);
      for (uint i=0; i<TestFrame::BUFFSIZ; ++i)
        *(data+i) = work.data()[i];
      return work.getChecksum();
    }
  }
  
  
  
  /**************************************************************************//**
   * @test document the usage patterns of the »Buffer Provider Protocol«
   * Notably this protocol is used by the Render Engine during recursive `pull()`
   * calls to claim and release the working buffers for each step; furthermore,
   * also the output buffers are packaged as `BuffHandle` and used through this
   * protocol.
   * 
   * However, the actual `BufferProvider` implementation used here is instrumented
   * for diagnostics and does not actually _manage_ buffers; rather each buffer
   * is allocated on the heap and never released, so that any allocation and
   * all buffer contents can be verified after the fact.
   */
  class BufferProviderProtocol_test : public Test
    {
      virtual void
      run (Arg)
        {
          verifySimpleUsage();
          verifyRenderingUse();
//          verifyObjectAttachment();
//          verifyObjectAttachmentFailure();  //////////////////////////OOO need a completely new test case to cover behaviour of failed ctor calls
        }
      
      
      void
      verifySimpleUsage()
        {
          // Create Test fixture.
          // In real usage, a suitable memory/frame/buffer provider
          // will be preconfigured, depending on the usage context
          DiagnosticBufferProvider provider;
          
          BuffHandle buff = provider.lockBufferFor<TestFrame>();
          CHECK (buff.isValid());
          CHECK (sizeof(TestFrame) <= buff.size());
          buff.accessAs<TestFrame>() = testData(0);
          
          TestFrame& content = buff.accessAs<TestFrame>();
          CHECK (testData(0) == content);
          
          buff.emit();
          buff.release();
          CHECK (not buff.isValid());
          VERIFY_ERROR (LIFECYCLE, buff.accessAs<TestFrame>() );
          
          auto inspect = watch(provider);
          CHECK (inspect.was_used (buff));
          CHECK (inspect.was_released(buff));
          
          CHECK (testData(0) == inspect.accessMemory(buff));
        }
      
      
      /** @test demonstrate the full sequence of invocations during rendering
       */
      void
      verifyRenderingUse()
        {
          // Test fixture: allows to track/verify allocations after the fact
          DiagnosticBufferProvider provider;
          
          using DataBuff = std::array<int64_t, TestFrame::BUFFSIZ>;
          constexpr auto BUFFSIZ = sizeof(DataBuff);
          
          BuffDescr desc1 = provider.getDescriptor<size_t>();
          BuffDescr desc2 = provider.getDescriptor<TestFrame>();   // note: implies also sizeof(TestFrame)
          BuffDescr desc3 = provider.getDescriptorFor(BUFFSIZ);    //       Can also request fixed sized raw storage.
          CHECK (desc1.isValid());
          CHECK (desc2.isValid());
          CHECK (desc3.isValid());
          
          uint num1 = desc1.announce(TEST_ELMS);
          uint num2 = desc2.announce(TEST_ELMS + 1);
          uint num3 = desc3.announce(TEST_ELMS * 2); // announce more than we actually use (which is tolerated)
          CHECK (num1 == TEST_ELMS);
          CHECK (num2 == TEST_ELMS + 1);
          CHECK (num3 == TEST_ELMS * 2);
          
          // emulate a progressive processing
          BuffHandle workBuff = desc2.lockBuffer();
          workBuff.accessAs<TestFrame>() = testData(0);
          for (uint i=0; i<TEST_ELMS; ++i)
            {
              BuffHandle srcBuff = workBuff;
              workBuff = desc2.lockBuffer();
              
              BuffHandle markBuff = desc1.lockBuffer();
              BuffHandle dataBuff = desc3.lockBuffer();
              CHECK (srcBuff.isValid());
              CHECK (markBuff.isValid());
              CHECK (workBuff.isValid());
              CHECK (dataBuff.isValid());
              
              TestFrame& src  = srcBuff.accessAs<TestFrame>();
              size_t&    mark = markBuff.accessAs<size_t>();
              TestFrame& work = workBuff.accessAs<TestFrame>();
              DataBuff&  data = dataBuff.accessAs<DataBuff>();
              
              mark = do_some_computation (src,work, &data[0]);
              
              srcBuff.emit();
              dataBuff.emit();
              markBuff.emit();
              
              srcBuff.release();
              dataBuff.release();
              markBuff.release();
              // note : workBuff passed to next iteration
            }
          CHECK (workBuff.isValid());
          auto endSum = workBuff.accessAs<TestFrame>().getChecksum();
          workBuff.release(); // note: not every buffer need be emitted.
          CHECK (!workBuff);
          
          auto inspect = watch(provider);
          CHECK (inspect.created.cnt() == 3*TEST_ELMS + 1);
          CHECK (inspect.was_used (workBuff));
          CHECK (inspect.was_released (workBuff));
          CHECK (not inspect.was_emitted (workBuff));
          CHECK (inspect.all_buffers_released());
          
          // now peek into buffer memory
          // to prove that that the complete computation chain
          // was in fact performed, and was using the memory as intended
          uint last = inspect.released.cnt() - 1;
          diagn::Block lastWork = inspect.released[last];
          diagn::Block lastMark = inspect.released[last-1];
          diagn::Block lastData = inspect.released[last-2];
          
          // due to the way this chained-hash dummy computation was defined,
          // the last data buffer should contain the same random numbers as the last work buffer...
          auto& workBuffContent = lastWork.accessAs<TestFrame>().data();
          auto& dataBuffContent = lastData.accessAs<DataBuff>();
          
          for (auto [c,v] : zip(workBuffContent,dataBuffContent))
            CHECK (c == v);
          
          // furthermore, the last recorded checksum mark
          // should match the checksum of the last work buffer,
          // because that is where the last computation was carried out.
          CHECK (endSum == lastMark.accessAs<HashVal>());
          
          // we can also verify that all data in emitted buffers is correct,
          // by re-computing the checksum for each step, using data in the buffers
          for (uint i=0; i<TEST_ELMS; ++i)
            {
              diagn::Block srcBuff  = inspect.emitted[i*3 + 0];
              diagn::Block markBuff = inspect.emitted[i*3 + 2];
              TestFrame const& src = srcBuff.accessAs<TestFrame>();  // note const -- can not corrupt data in memory
              ont::Param param = src.getChecksum();
              TestFrame newWorkBuff;
              ont::manipulateFrame (&newWorkBuff, &src, param);
              
              // the checksum recoded during the test run matches the re-computed sum,
              // based on combining data in memory for a second time with the same chained-hash function
              CHECK (markBuff.accessAs<HashVal>() == newWorkBuff.getChecksum());
            }
        }
      
      
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code to disentangle BufferProvider implementation
      void
      verifyObjectAttachment()
        {
          DiagnosticBufferProvider provider;
          BuffDescr type_A = provider.getDescriptorFor(sizeof(TestFrame));
          BuffDescr type_B = provider.getDescriptorFor(sizeof(int));
          BuffDescr type_C = provider.getDescriptor<int>();
          
          BuffHandle handle_A = provider.lockBuffer(type_A);
          BuffHandle handle_B = provider.lockBuffer(type_B);
          BuffHandle handle_C = provider.lockBuffer(type_C);
          
          CHECK (handle_A);
          CHECK (handle_B);
          CHECK (handle_C);
          
          CHECK (sizeof(TestFrame) == handle_A.size());
          CHECK (sizeof( int )     == handle_B.size());
          CHECK (sizeof( int )     == handle_C.size());
          
          TestFrame& embeddedFrame = handle_A.create<TestFrame>();
          CHECK (isSameObject (*handle_A, embeddedFrame));
          CHECK (embeddedFrame.isAlive());
          CHECK (embeddedFrame.isSane());
          
          VERIFY_ERROR (LOGIC,     handle_B.create<TestFrame>());   // too small to hold a TestFrame
          VERIFY_ERROR (LIFECYCLE, handle_C.create<int>());         // has already an attached TypeHandler (creating an int)
          
          handle_A.release();
          handle_B.release();
          handle_C.release();
          
          CHECK (embeddedFrame.isDead());
          CHECK (embeddedFrame.isSane());
        }
      
      
      void
      verifyObjectAttachmentFailure()
        {
          DiagnosticBufferProvider provider;
          BuffDescr type_D = provider.getDescriptorFor(sizeof(Dummy));
          
          Dummy::checksum() = 0;
          BuffHandle handle_D = provider.lockBuffer(type_D);
          CHECK (0 == Dummy::checksum());  // nothing created thus far
          
          handle_D.create<Dummy>();
          CHECK (0 < Dummy::checksum());
          
          handle_D.release();
          CHECK (0 == Dummy::checksum());
          
          BuffHandle handle_DD = provider.lockBuffer(type_D);
          
          CHECK (0 == Dummy::checksum());
          Dummy::activateCtorFailure();
          
          CHECK (handle_DD.isValid());
          try
            {
              handle_DD.create<Dummy>();
              NOTREACHED ("Dummy ctor should fail");
            }
          catch (int val)
            {
              CHECK (!handle_DD.isValid());
              
              CHECK (0 < Dummy::checksum());
              CHECK (val == Dummy::checksum());
            }
          
          VERIFY_ERROR (LIFECYCLE, handle_DD.accessAs<Dummy>() );
          VERIFY_ERROR (LIFECYCLE, handle_DD.create<Dummy>() );
        }
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
    };
  
  
  /** Register this test class... */
  LAUNCHER (BufferProviderProtocol_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
