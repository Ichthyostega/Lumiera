/*
  TestFrame(Test)  -  verify proper operation of dummy data frames

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file testframe-test.cpp
 ** unit test \ref TestFrame_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/engine/testframe.hpp"
#include "lib/util.hpp"

#include <climits>
#include <memory>

using test::Test;
using util::isSameObject;
using std::unique_ptr;


namespace steam {
namespace engine{
namespace test  {
  
  namespace { // used internally
    
    const uint CHAN_COUNT = 30;     // independent families of test frames to generate
    const uint NUM_FRAMES = 1000;   // number of test frames in each of these families
    
    
    void
    corruptMemory(void* base, uint offset, uint count)
    {
      char* accessor = reinterpret_cast<char*> (base);
      while (count--)
        accessor[offset+count] = rani(CHAR_MAX);
    }
  } // (End) internal defs
  
  
  
  /***************************************************************//**
   * @test verify test helper for engine tests: a dummy data frame.
   *       TestFrame instances can be created right away, without any
   *       external library dependencies. A test frame is automatically
   *       filled with random data; multiple frames are arranged in
   *       sequences and channels, causing the random data to be
   *       reproducible yet different in each frame.
   *       
   *       To ease writing unit tests, TestFrame provides comparison
   *       and assignment and tracks lifecycle automatically. As tests
   *       regarding the engine typically have to deal with buffer
   *       management, an arbitrary memory location can be interpreted
   *       as TestFrame and checked for corruption.
   */
  class TestFrame_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          TestFrame::reseed();
          
          simpleUsage();
          verifyDataContent();
          verifyFrameLifecycle();
          verifyFrameSeries();
          useFrameTable();
        }
      
      
      void
      simpleUsage()
        {
          CHECK (1024 < sizeof(TestFrame));
          
          TestFrame frame;
          CHECK (frame.isValid());
          
          ++frame.data()[5];
          CHECK (not frame.isValid());
          
          frame.markChecksum();
          CHECK (frame.isValid());
          
          CHECK (isSameObject(frame, frame.data()));           // payload data stored embedded
          CHECK (sizeof(TestFrame) > frame.data().size());     // additional metadata placed behind
        }
      
      
      void
      verifyDataContent()
        {
          TestFrame frameA;
          TestFrame frameB;
          TestFrame frameC(5);
          
          CHECK (frameA == frameB);
          CHECK (frameA != frameC);
          CHECK (frameB != frameC);
          
          CHECK (frameA.data() == frameB.data());
          CHECK (frameA.data() != frameC.data());
          for (uint i=0; i<frameA.data().size(); ++i)
            CHECK (frameA.data()[i] == frameB.data()[i]);
          
          // can access data as uint64_t
          CHECK (frameA.data()[ 8] == char(frameA.data64()[1]));  // assuming little-endian
          CHECK (frameA.data()[16] == char(frameA.data64()[2]));
          CHECK (frameA.data()[24] == char(frameA.data64()[3]));
          CHECK (frameA.data()[32] == char(frameA.data64()[4]));
          CHECK (frameA.data().size() == 8*frameA.data64().size());
          
          CHECK (frameA.isAlive());
          CHECK (frameB.isAlive());
          CHECK (frameC.isAlive());
          
          CHECK (frameA.isSane());
          CHECK (frameB.isSane());
          CHECK (frameC.isSane());
          
          CHECK (frameA.isValid());
          CHECK (frameB.isValid());
          CHECK (frameC.isValid());
          
          CHECK (frameA.isPristine());
          CHECK (frameB.isPristine());
          CHECK (frameC.isPristine());
          
          void * frameMem = &frameB;
          
          CHECK (frameA == frameMem);
          corruptMemory (frameMem,20,5);
          CHECK (    frameB.isSane());           // still has valid metadata header
          CHECK (not frameB.isValid());          // data checksum does not match any more
          CHECK (not frameB.isPristine());       // data does not match the original generation sequence
          
          frameB.markChecksum();
          CHECK (    frameB.isSane());           // still has valid metadata header
          CHECK (    frameB.isValid());          // data matches the new recorded checksum
          CHECK (not frameB.isPristine());       // but data still does not match the original generation sequence
          
          frameB = frameC;
          
          CHECK (frameB.isSane());
          CHECK (frameA != frameB);
          CHECK (frameA != frameC);
          CHECK (frameB == frameC);
          
          corruptMemory (frameMem, 0,sizeof(TestFrame));
          CHECK (not frameB.isSane());           // now also the metadata was corrupted...
          CHECK (not frameB.isValid());
          VERIFY_FAIL ("corrupted metadata"
                      , frameB.markChecksum() ); // reject to store new checksum in the corrupted header
          VERIFY_FAIL ("target TestFrame already dead or unaccessible"
                      , frameB = frameC);        // reject to assign new content to a corrupted target
        }
      
      
      void
      verifyFrameLifecycle()
        {
          CHECK (not TestFrame::isDead  (this));
          CHECK (not TestFrame::isAlive (this));
          
          static char buffer[sizeof(TestFrame)];
          TestFrame* frame = new(&buffer) TestFrame(23);
          
          CHECK ( TestFrame::isAlive (frame));
          CHECK (not frame->isDead());
          CHECK (    frame->isAlive());
          CHECK (    frame->isValid());
          
          frame->~TestFrame();
          CHECK (    TestFrame::isDead  (frame));
          CHECK (not TestFrame::isAlive (frame));
          CHECK (    frame->isValid());
          CHECK (    frame->isSane());
        }
      
      
      /** @test build sequences of test frames,
       *        organised into multiple families (channels).
       *        Verify that adjacent frames hold differing data
       */
      void
      verifyFrameSeries()
        {
          unique_ptr<TestFrame> thisFrames[CHAN_COUNT];
          unique_ptr<TestFrame> prevFrames[CHAN_COUNT];
          
          for (uint i=0; i<CHAN_COUNT; ++i)
            thisFrames[i].reset (new TestFrame(0, i));
          
          for (uint nr=1; nr<NUM_FRAMES; ++nr)
            for (uint i=0; i<CHAN_COUNT; ++i)
              {
                thisFrames[i].swap (prevFrames[i]);
                thisFrames[i].reset (new TestFrame(nr, i));
                CHECK (thisFrames[i]->isPristine());
                CHECK (prevFrames[i]->isPristine());
                CHECK (prevFrames[i]->isAlive());
                
                CHECK (*thisFrames[i] != *prevFrames[i]);      // differs from predecessor within the same channel
                
                for (uint j=0; j<i; ++j)
                  {
                    ENSURE (j!=i);
                    CHECK (*thisFrames[i] != *thisFrames[j]);  // differs from frames in other channels at this point
                    CHECK (*thisFrames[i] != *prevFrames[j]);  // differs cross wise from predecessors in other channels
        }     }   }
      
      
      
      /** @test the table of test frames computed on demand */
      void
      useFrameTable()
        {
          TestFrame& frX = testData(50,3);
          TestFrame& frY = testData(50,2);
          TestFrame& frZ = testData(50,3);
          
          CHECK (frX.isPristine());
          CHECK (frY.isPristine());
          CHECK (frZ.isPristine());
          
          CHECK (frX != frY);
          CHECK (frX == frZ);
          CHECK (frY != frZ);
          
          CHECK (isSameObject (frX, frZ));
          
          corruptMemory(&frZ,40,20);
          CHECK (not frX.isPristine());
          CHECK (not testData(50,3).isPristine());
          CHECK (    testData(51,3).isPristine());
          CHECK (    testData(49,3).isPristine());
          
          char c = testData(49,3).data()[5];         // some arbitrary content
          
          TestFrame::reseed();
          
          CHECK ( testData(50,3).isPristine());
          CHECK (c != testData(49,3).data()[5]);     // content regenerated with different seed
          
          TestFrame o{49,3};                         // all data content is reproducible with the new seed
          CHECK (not isSameObject(o, testData(49,3)));
          CHECK (o == testData(49,3));
          CHECK (o.data()[5] == testData(49,3).data()[5]);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (TestFrame_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
