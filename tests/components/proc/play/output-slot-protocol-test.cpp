/*
  OutputSlotProtocol(Test)  -  covering the basic usage cycle of an output slot

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


#include "lib/test/run.hpp"
//#include "lib/util.hpp"
#include "proc/play/diagnostic-output-slot.hpp"
#include "proc/engine/buffhandle.hpp"
#include "proc/engine/testframe.hpp"

//#include <boost/format.hpp>
//#include <iostream>

//using boost::format;
//using std::string;
//using std::cout;


namespace proc {
namespace play {
namespace test {
  
//  using lib::AllocationCluster;
//  using mobject::session::PEffect;
  using proc::engine::BuffHandle;
  using proc::engine::test::testData;
  using proc::engine::test::TestFrame;
  
  
  namespace { // Test fixture
    
  }
  
  
  /*******************************************************************
   * @test verify the OutputSlot interface and base implementation
   *       by performing full data exchange cycle. This is a
   *       kind of "dry run" for documentation purposes,
   *       both the actual OutputSlot implementation
   *       as the client using this slot are Mocks.
   */
  class OutputSlotProtocol_test : public Test
    {
      virtual void
      run (Arg) 
        {
          verifyStandardCase();
        }
      
      
      void
      verifyStandardCase()
        {
          // Create Test fixture.
          // In real usage, the OutputSlot will be preconfigured
          // (Media format, number of channels, physical connections)
          // and then registered with / retrieved from an OutputManager
          OutputSlot& oSlot = DiagnosticOutputSlot::build();
          
          // Client claims the OutputSlot
          // and opens it for exclusive use.
          OutputSlot::Allocation& alloc = oSlot.allocate();
          
          // Now the client is able to prepare
          // "calculation streams" for the individual
          // Channels to be output through this slot.
          OutputSlot::OpenedSinks sinks = alloc.getOpenedSinks();
          DataSink sink1 = *sinks++;
          DataSink sink2 = *sinks++;
          
          // within the frame-calculation "loop"
          // we perform an data exchange cycle
          int64_t frameNr = 123;
          BuffHandle buff00 = sink1.lockBufferFor (frameNr);
          BuffHandle buff10 = sink2.lockBufferFor (frameNr);
          buff00.create<TestFrame>();
          buff10.create<TestFrame>();
          
          // rendering process calculates content....
          buff00.accessAs<TestFrame>() = testData(0,0);
          
          // while further frames might be processed in parallel
          BuffHandle buff11 = sink2.lockBufferFor (++frameNr);
          buff11.create<TestFrame>();
          buff11.accessAs<TestFrame>() = testData(1,1);
          buff10.accessAs<TestFrame>() = testData(1,0);
          
          // Now it's time to emit the output
          sink2.emit (frameNr-1, buff10);
          sink2.emit (frameNr  , buff11);
          sink1.emit (frameNr-1, buff00);
          // that's all for the client
          
          // Verify sane operation....
          DiagnosticOutputSlot& checker = DiagnosticOutputSlot::access(oSlot);
          CHECK (checker.buffer_was_used (0,0));
          CHECK (checker.buffer_unused   (0,1));
          CHECK (checker.buffer_was_used (1,0));
          CHECK (checker.buffer_was_used (1,1));
          
          CHECK (checker.buffer_was_closed (0,0));
          CHECK (checker.buffer_was_closed (1,0));
          CHECK (checker.buffer_was_closed (1,1));
          
          CHECK ( checker.emitted (0,0));
          CHECK (!checker.emitted (0,1));
          CHECK ( checker.emitted (1,0));
          CHECK ( checker.emitted (1,1));
          
          DiagnosticOutputSlot::OutFrames stream0 = checker.getChannel(0); 
          DiagnosticOutputSlot::OutFrames stream1 = checker.getChannel(1);
          
          CHECK ( stream0);
          CHECK (*stream0++ == testData(0,0));
          CHECK (!stream0);
          
          CHECK ( stream1);
          CHECK (*stream1++ == testData(1,0));
          CHECK (*stream1++ == testData(1,1));
          CHECK (!stream1);
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #819
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #819
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (OutputSlotProtocol_test, "unit player");
  
  
  
}}} // namespace proc::play::test
