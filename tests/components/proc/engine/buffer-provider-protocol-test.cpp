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
//#include "lib/util.hpp"
//#include "proc/play/diagnostic-output-slot.hpp"
#include "proc/engine/diagnostic-buffer-provider.hpp"
#include "proc/engine/buffhandle.hpp"

//#include <boost/format.hpp>
//#include <iostream>

//using boost::format;
//using std::string;
//using std::cout;


namespace engine{
namespace test  {
  
//  using lib::AllocationCluster;
//  using mobject::session::PEffect;
  using ::engine::BuffHandle;
  using lumiera::error::LUMIERA_ERROR_LIFECYCLE;
  
  
  namespace { // Test fixture
    
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
          verifyStandardCase();
        }
      
      
      void
      verifySimpleUsage()
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #829
          // Create Test fixture.
          // In real usage, a suitable memory/frame/buffer provider
          // will be preconfigured, depending on the usage context
          BufferProvider& provider = DiagnosticBufferProvider::build();
          
          BuffHandle buff = provider.lockBufferFor<TestFrame>();
          CHECK (buff.isValid());
          CHECK (sizeof(TestFrame) <= buff.size());
          buff.create<TestFrame>() = testData(0);
          
          TestFrame* storage = *buff;
          CHECK (testData(0) == *storage);
          
          buff.release();
          CHECK (!buff.isValid());
          VERIFY_ERROR (LIFECYCLE, *buff );
          
          DiagnosticBufferProvider checker = DiagnosticBufferProvider::access(provider);
          CHECK (checker.buffer_was_used (0));
          CHECK (checker.buffer_was_closed (0));
          CHECK (checker.object_was_attached<TestFrame> ());
          CHECK (checker.object_was_destroyed<TestFrame> ());
          
          CHECK (testData(0) == checker.accessStorage (0));
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #829
        }
      
      
      void
      verifyStandardCase()
        {
          // Create Test fixture.
          // In real usage, a suitable memory/frame/buffer provider
          // will be preconfigured, depending on the usage context
          BufferProvider& provider = DiagnosticBufferProvider::build();
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #829
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #829
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BufferProviderProtocol_test, "unit player");
  
  
  
}} // namespace engine::test
