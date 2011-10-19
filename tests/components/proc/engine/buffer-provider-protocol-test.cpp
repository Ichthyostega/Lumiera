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


#include "lib/error.hpp"
#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util-foreach.hpp"
//#include "proc/play/diagnostic-output-slot.hpp"
#include "proc/engine/testframe.hpp"
#include "proc/engine/diagnostic-buffer-provider.hpp"
#include "proc/engine/buffhandle.hpp"
#include "proc/engine/bufftable.hpp"

//#include <boost/format.hpp>
//#include <iostream>

//using boost::format;
//using std::string;
//using std::cout;
using util::for_each;


namespace engine{
namespace test  {
  
//  using lib::AllocationCluster;
//  using mobject::session::PEffect;
  using ::engine::BuffHandle;
  using lumiera::error::LUMIERA_ERROR_LIFECYCLE;
  
  
  namespace { // Test fixture
    
    const uint TEST_SIZE = 1024*1024;
    const uint TEST_ELMS = 20;
    
    
    void
    do_some_calculations (BuffHandle const& buffer)
    {
      UNIMPLEMENTED ("some verifiable test/dummy buffer accessing operations");
    }
    
  }
  
  
  /*******************************************************************
   * @test verify the OutputSlot interface and base implementation
   *       by performing full data exchange cycle. This is a
   *       kind of "dry run" for documentation purposes,
   *       both the actual OutputSlot implementation
   *       as the client using this slot are Mocks.
   */
  class BufferProviderProtocol_test : public Test
    {
      virtual void
      run (Arg) 
        {
          UNIMPLEMENTED ("build a diagnostic buffer provider and perform a full lifecycle");
          verifySimpleUsage();
          verifyStandardCase();
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
          buff.create<TestFrame>() = testData(0);
          
          TestFrame& storage = buff.accessAs<TestFrame>();
          CHECK (testData(0) == storage);
          
          buff.release();
          CHECK (!buff.isValid());
          VERIFY_ERROR (LIFECYCLE, buff.accessAs<TestFrame>() );
          
          DiagnosticBufferProvider& checker = DiagnosticBufferProvider::access(provider);
          CHECK (checker.buffer_was_used (0));
          CHECK (checker.buffer_was_closed (0));
          CHECK (checker.object_was_attached<TestFrame> (0));
          CHECK (checker.object_was_destroyed<TestFrame> (0));
          
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
    };
  
  
  /** Register this test class... */
  LAUNCHER (BufferProviderProtocol_test, "unit player");
  
  
  
}} // namespace engine::test
