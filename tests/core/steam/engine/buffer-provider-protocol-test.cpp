/*
  BufferProviderProtocol(Test)  -  demonstration of buffer provider usage cycle

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

/** @file buffer-provider-protocol-test.cpp
 ** unit test \ref BufferProviderProtocol_test
 */


#include "lib/error.hpp"
#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/testdummy.hpp"
#include "lib/util-foreach.hpp"
#include "steam/engine/testframe.hpp"
#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "steam/engine/buffhandle-attach.hpp"
#include "steam/engine/bufftable.hpp"

using util::isSameObject;
using util::for_each;


namespace proc  {
namespace engine{
namespace test  {
  
  using lib::test::Dummy;
  
  using proc::engine::BuffHandle;
  using error::LERR_(LOGIC);
  using error::LERR_(LIFECYCLE);
  
  
  namespace { // Test fixture
    
    const uint TEST_SIZE = 1024*1024;
    const uint TEST_ELMS = 20;
    
    
    void
    do_some_calculations (BuffHandle const& buffer)
    {
      UNIMPLEMENTED ("some verifiable test/dummy buffer accessing operations");
    }
    
  }
  
  
  /**************************************************************************//**
   * @test verify and demonstrate the usage cycle of data buffers for the engine
   *       based on the BufferProvider interface. This is kind of a "dry run"
   *       for documentation purposes, because the BufferProvider implementation
   *       used here is just a diagnostics facility, allowing to investigate
   *       the state of individual buffers even after "releasing" them.
   *       
   *       This test should help understanding the sequence of buffer management
   *       operations performed at various stages while passing an calculation job
   *       through the render engine.
   */
  class BufferProviderProtocol_test : public Test
    {
      virtual void
      run (Arg) 
        {
          verifySimpleUsage();
          verifyStandardCase();
          verifyObjectAttachment();
          verifyObjectAttachmentFailure();
        }
      
      
      void
      verifySimpleUsage()
        {
          // Create Test fixture.
          // In real usage, a suitable memory/frame/buffer provider
          // will be preconfigured, depending on the usage context
          BufferProvider& provider = DiagnosticBufferProvider::build();
          
          BuffHandle buff = provider.lockBufferFor<TestFrame>();
          CHECK (buff.isValid());
          CHECK (sizeof(TestFrame) <= buff.size());
          buff.accessAs<TestFrame>() = testData(0);
          
          TestFrame& content = buff.accessAs<TestFrame>();
          CHECK (testData(0) == content);
          
          buff.emit();
          buff.release();
          CHECK (!buff.isValid());
          VERIFY_ERROR (LIFECYCLE, buff.accessAs<TestFrame>() );
          
          DiagnosticBufferProvider& checker = DiagnosticBufferProvider::access(provider);
          CHECK (checker.buffer_was_used (0));
          CHECK (checker.buffer_was_closed (0));
          
          CHECK (testData(0) == checker.accessMemory (0));
        }
      
      
      void
      verifyStandardCase()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #829
          // Create Test fixture.
          // In real usage, a suitable memory/frame/buffer provider
          // will be preconfigured, depending on the usage context
          BufferProvider& provider = DiagnosticBufferProvider::build();
          
          BufferDescriptor desc1 = provider.getDescriptor<TestFrame>();   // note: implies also sizeof(TestFrame)
          BufferDescriptor desc2 = provider.getDescriptorFor(TEST_SIZE);
          CHECK (desc1.verifyValidity());
          CHECK (desc2.verifyValidity());
          
          uint num1 = provider.announce(TEST_ELMS, desc1);
          uint num2 = provider.announce(TEST_ELMS, desc2);
          CHECK (num1 == TEST_ELMS);
          CHECK (0 < num2 && num2 <=TEST_ELMS);
          
          const size_t STORAGE_SIZE = BuffTable::Storage<2*TEST_ELMS>::size; 
          char storage[STORAGE_SIZE];
          BuffTable& tab =
              BuffTable::prepare(STORAGE_SIZE, storage)
                        .announce(num1, desc1)
                        .announce(num2, desc2)
                        .build(); 
          
          tab.lockBuffers();
          for_each (tab.buffers(), do_some_calculations);
          tab.releaseBuffers();
          
          DiagnosticBufferProvider& checker = DiagnosticBufferProvider::access(provider);
          CHECK (checker.all_buffers_released());
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #829
        }
      
      
      void
      verifyObjectAttachment()
        {
          BufferProvider& provider = DiagnosticBufferProvider::build();
          BufferDescriptor type_A = provider.getDescriptorFor(sizeof(TestFrame));
          BufferDescriptor type_B = provider.getDescriptorFor(sizeof(int));
          BufferDescriptor type_C = provider.getDescriptor<int>();
          
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
          BufferProvider& provider = DiagnosticBufferProvider::build();
          BufferDescriptor type_D = provider.getDescriptorFor(sizeof(Dummy));
          
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
    };
  
  
  /** Register this test class... */
  LAUNCHER (BufferProviderProtocol_test, "unit player");
  
  
  
}}} // namespace proc::engine::test
