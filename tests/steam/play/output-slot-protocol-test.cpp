/*
  OutputSlotProtocol(Test)  -  covering the basic usage cycle of an output slot

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file output-slot-protocol-test.cpp
 ** unit test \ref OutputSlotProtocol_test
 */


#include "test/run.hpp"

#include "steam/play/diagnostic-output-slot.hpp"
#include "vault/mem/buffhandle.hpp"
#include "test/test-frame.hpp"


using test::TestFrame;
using test::testData;
using vault::mem::BuffHandle;

namespace steam {
namespace play {
namespace test {
  
  
  
  /***************************************************************//**
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
          DiagnosticOutputSlot oSlot;
          
          // Client claims the OutputSlot
          // and opens it for exclusive use.
          OutputSlot::Allocation& alloc = oSlot.allocate();
          
          // Now the client is able to prepare
          // "calculation streams" for the individual
          // Channels to be output through this slot.
          OutputSlot::OpenedSinks sinks = alloc.getOpenedSinks();
          DataSink sink0 = *sinks;
          DataSink sink1 = *++sinks;
          
          // within the frame-calculation "loop"
          // we perform a data exchange cycle
          FrameCnt frameNr = 123;
          BuffHandle buff00 = sink0.lockBufferFor (frameNr);
          BuffHandle buff10 = sink1.lockBufferFor (frameNr);
          
          // rendering process calculates content....
          buff00.accessAs<TestFrame>() = testData(0,0);
          
          // while further frames might be processed in parallel
          BuffHandle buff11 = sink1.lockBufferFor (frameNr+1);
          buff11.accessAs<TestFrame>() = testData(1,1);
          buff10.accessAs<TestFrame>() = testData(1,0);
          
          // Now it's time to emit the output
          buff11.emit();
          buff00.emit();
          
          // Buffers must be marked as released when done
          buff00.release();
          buff10.release();
          buff11.release();
          // that's all for the client
          
          // Verify sane operation....
          CHECK (watch(oSlot).cntLocked() == 3);
          CHECK (watch(oSlot).cntEmitted() == 2);
          CHECK (watch(oSlot).cntReleased() == 3);
          
          CHECK (    watch(oSlot).feed(0).frame(123).wasLocked());
          CHECK (not watch(oSlot).feed(0).frame(124).wasLocked());
          CHECK (    watch(oSlot).feed(1).frame(123).wasLocked());
          CHECK (    watch(oSlot).feed(1).frame(124).wasLocked());
          
          CHECK (    watch(oSlot).feed(0).frame(123).wasEmitted());
          CHECK (not watch(oSlot).feed(0).frame(124).wasEmitted());
          CHECK (not watch(oSlot).feed(1).frame(123).wasEmitted());
          CHECK (    watch(oSlot).feed(1).frame(124).wasEmitted());
          
          CHECK (    watch(oSlot).feed(0).frame(123).wasReleased());
          CHECK (not watch(oSlot).feed(0).frame(124).wasReleased());
          CHECK (    watch(oSlot).feed(1).frame(123).wasReleased());
          CHECK (    watch(oSlot).feed(1).frame(124).wasReleased());
          
          auto stream0 = watch(oSlot).getFeed(0);
          auto stream1 = watch(oSlot).getFeed(1);
          
          CHECK ( stream0);
          CHECK (*stream0 == testData(0,0)); ++stream0;
          CHECK (!stream0);
          
          CHECK ( stream1);
          CHECK (*stream1 == testData(1,0)); ++stream1;
          CHECK (*stream1 == testData(1,1)); ++stream1;
          CHECK (!stream1);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (OutputSlotProtocol_test, "unit play");
  
  
  
}}} // namespace steam::play::test
