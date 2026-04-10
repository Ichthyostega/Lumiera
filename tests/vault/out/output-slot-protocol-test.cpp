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

#include "vault/out/diagnostic-output-slot.hpp"
#include "vault/mem/buffhandle.hpp"
#include "vault/real-clock.hpp"
#include "test/test-frame.hpp"


using test::TestFrame;
using test::testData;
using vault::RealClock;
using vault::mem::BuffHandle;
using lib::time::FSecs;

namespace vault{
namespace out  {
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
          
          // Note: an OutputSlot is automatically activated / allocated
          // for exclusive use by this client.
          
          // Now the client is able to prepare
          // "calculation streams" for the individual
          // Channels to be output through this slot.
          OutputSlot::OpenedSinks sinks = oSlot.getOpenedSinks();
          DataSink sink0 = *sinks;
          DataSink sink1 = *++sinks;
          
          // some test markers....
          const FrameCnt START = 123;
          TimeVar testTime = Time::ZERO;
          oSlot.fixCurrentTime(testTime);
          auto markTime = [&]{ return Time( testTime += FSecs(1) ); };
          
          // within the frame-calculation "loop"
          // we perform a data exchange cycle
          auto MARK_0 = markTime();
          BuffHandle buff00 = sink0(START);
          BuffHandle buff10 = sink1(START);
          
          // rendering process calculates content....
          buff00.emplace<TestFrame> (testData(0,0));
          
          // while further frames might be processed in parallel
          auto MARK_1 = markTime();
          BuffHandle buff11 = sink1(START+1);
          buff11.emplace<TestFrame> (testData(1,1));
          buff10.emplace<TestFrame> (testData(1,0));
          
          // Now it's time to emit the output
          auto MARK_2 = markTime();
          buff11.emit();
          buff00.emit();
          
          // Buffers must be marked as released when done
          auto MARK_3 = markTime();
          buff00.release();
          buff10.release();
          buff11.release();
          // that's all for the client
          
          // Verify sane operation....
          CHECK (watch(oSlot).cntLocked() == 3);
          CHECK (watch(oSlot).cntEmitted() == 2);
          CHECK (watch(oSlot).cntReleased() == 3);
          
          CHECK (    watch(oSlot).feed(0).frame(START  ).wasLocked());
          CHECK (not watch(oSlot).feed(0).frame(START+1).wasLocked());
          CHECK (    watch(oSlot).feed(1).frame(START  ).wasLocked());
          CHECK (    watch(oSlot).feed(1).frame(START+1).wasLocked());
          
          CHECK (    watch(oSlot).feed(0).frame(START  ).wasEmitted());
          CHECK (not watch(oSlot).feed(0).frame(START+1).wasEmitted());
          CHECK (not watch(oSlot).feed(1).frame(START  ).wasEmitted());
          CHECK (    watch(oSlot).feed(1).frame(START+1).wasEmitted());
          
          CHECK (    watch(oSlot).feed(0).frame(START  ).wasReleased());
          CHECK (not watch(oSlot).feed(0).frame(START+1).wasReleased());
          CHECK (    watch(oSlot).feed(1).frame(START  ).wasReleased());
          CHECK (    watch(oSlot).feed(1).frame(START+1).wasReleased());
          
          auto stream0 = watch(oSlot).getFeed(0, START);
          auto stream1 = watch(oSlot).getFeed(1, START);
          
          CHECK ( stream0);
          CHECK (*stream0 == testData(0,0)); ++stream0;
          CHECK (not stream0->wasLocked());
          
          CHECK ( stream1);
          CHECK (*stream1 == testData(1,0)); ++stream1;
          CHECK (*stream1 == testData(1,1)); ++stream1;
          CHECK (not stream1->wasLocked());
          
          // Verify that recorded times match the »step stones« defined above...
          auto recordedTime_0 = watch(oSlot).feed(0).frame(START).locked;     // Frame 0.0
          auto recordedTime_1 = watch(oSlot).feed(1).frame(START+1).locked;   // Frame 1.1
          auto recordedTime_2 = watch(oSlot).feed(0).frame(START  ).emitted;  // Frame 0.0
          auto recordedTime_3 = watch(oSlot).feed(0).frame(START  ).released; // Frame 0.0
          // ..while Frame#1 on Sink#0 was actually never requested...
          auto recordedTime_N = watch(oSlot).feed(0).frame(START+1).locked;   // Frame 0.1
          
          CHECK (recordedTime_0 < recordedTime_1);
          CHECK (recordedTime_1 < recordedTime_2);
          CHECK (recordedTime_2 < recordedTime_3);
          CHECK (recordedTime_0 == MARK_0);
          CHECK (recordedTime_1 == MARK_1);
          CHECK (recordedTime_2 == MARK_2);
          CHECK (recordedTime_3 == MARK_3);
          CHECK (Time::NEVER == recordedTime_N);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (OutputSlotProtocol_test, "unit play");
  
  
  
}}} // namespace vault::out::test
