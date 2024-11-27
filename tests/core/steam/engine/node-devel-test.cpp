/*
  NodeDevel(Test)  -  Render Node development and test support

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file node-devel-test.cpp
 ** unit test \ref NodeDevel_test
 */


#include "lib/test/run.hpp"
#include "lib/hash-combine.hpp"
#include "steam/engine/test-rand-ontology.hpp" ///////////TODO
#include "lib/test/diagnostic-output.hpp"/////////////////TODO
#include "lib/iter-zip.hpp"
#include "lib/random.hpp"
//#include "lib/util.hpp"


using lib::zip;


namespace steam {
namespace engine{
namespace test  {
  
  
  namespace {
    /** uninitialised local storage that can be passed
     *  as working buffer and accessed as TestFrame */
    struct Buffer
      : util::NonCopyable
      {
        alignas(TestFrame)
          std::byte storage[sizeof(TestFrame)];
        
        operator TestFrame*  () { return   std::launder (reinterpret_cast<TestFrame* > (&storage)); }
        TestFrame* operator->() { return   std::launder (reinterpret_cast<TestFrame* > (&storage)); }
        TestFrame& operator* () { return * std::launder (reinterpret_cast<TestFrame* > (&storage)); }
        
        TestFrame&
        buildData (uint seq=0, uint family=0)
          {
            return * new(&storage) TestFrame{seq,family};
          }
      };
  }
  
  
  
  /***************************************************************//**
   * @test verify support for developing Render Node functionality.
   */
  class NodeDevel_test : public Test
    {
      virtual void
      run (Arg)
        {
          seedRand();
          TestFrame::reseed();
          
          processing_generateFrame();
          processing_generateMultichan();
          processing_manipulateFrame();
          processing_combineFrames();
        }
      
      
      /** @test function to generate random test data frames
       */
      void
      processing_generateFrame()
        {
          size_t frameNr = defaultGen.u64();
          uint flavour   = defaultGen.u64();
          
          Buffer buff;
          CHECK (not buff->isSane());
          
          generateFrame (buff, frameNr, flavour);
          CHECK ( buff->isSane());
          CHECK (*buff == TestFrame(frameNr,flavour));
        }
      
      /** @test function to generate an array of random test data frames
       *        for consecutive channels
       */
      void
      processing_generateMultichan()
        {
          size_t frameNr = defaultGen.u64();
          uint flavour   = defaultGen.u64();
          
          uint channels  = 1 + rani(50);
          CHECK (1 <= channels and channels <= 50);
          
          Buffer buffs[50];
          for (uint i=0; i<channels; ++i)
            CHECK (not buffs[i]->isSane());
          
          generateMultichan (buffs[0], channels, frameNr, flavour);
          for (uint i=0; i<channels; ++i)
            {
              CHECK (buffs[i]->isSane());
              CHECK (*(buffs[i]) == TestFrame(frameNr,flavour+i));
            }
        }
      
      /** @test function to apply a numeric computation to test data frames
       */
      void
      processing_manipulateFrame()
        {
          size_t frameNr = defaultGen.u64();
          uint flavour   = defaultGen.u64();
          
          Buffer iBuff, oBuff;
          iBuff.buildData(frameNr,flavour);
          oBuff.buildData(frameNr,flavour);
          CHECK (iBuff->isPristine());
          CHECK (oBuff->isPristine());
          
          uint64_t param = defaultGen.u64();
          manipulateFrame (oBuff, iBuff, param);
          CHECK (    oBuff->isValid());
          CHECK (not oBuff->isPristine());
          CHECK (    iBuff->isPristine());
          
          for (auto [iDat,oDat] : zip (iBuff->data64()
                                      ,oBuff->data64()))
            {
              CHECK (oDat != iDat);
              uint64_t feed = param;
              lib::hash::combine (feed, iDat);
              CHECK (feed  != param);
              CHECK (feed  != iDat);
              CHECK (feed  == oDat);
            }
           // can also process in-place
          manipulateFrame (iBuff, iBuff, param);
          CHECK (not iBuff->isPristine());
          CHECK (    iBuff->isValid());
          CHECK (*iBuff == *oBuff);  // second invocation exactly reproduced data from first invocation
        }
      
      /** @test function to mix two test data frames
       */
      void
      processing_combineFrames()
        {
          size_t frameNr = defaultGen.u64();
          uint flavour   = defaultGen.u64();
          
          Buffer i1Buff, i2Buff, oBuff;
          i1Buff.buildData(frameNr,flavour+0);
          i2Buff.buildData(frameNr,flavour+1);
          oBuff.buildData();
          CHECK (i1Buff->isPristine());
          CHECK (i2Buff->isPristine());
          CHECK (oBuff->isPristine());
          
          double mix = defaultGen.uni();
          combineFrames (oBuff, i1Buff, i2Buff, mix);
          CHECK (    oBuff->isValid());
          CHECK (not oBuff->isPristine());
          CHECK (    i1Buff->isPristine());
          CHECK (    i2Buff->isPristine());
          
          for (auto [oDat,i1Dat,i2Dat] : zip (oBuff->data()
                                             ,i1Buff->data()
                                             ,i2Buff->data()))
            CHECK (oDat == std::lround((1-mix)*i1Dat + mix*i2Dat));
          
          // can also process in-place
          combineFrames (i1Buff, i1Buff, i2Buff, mix);
          CHECK (not i1Buff->isPristine());
          CHECK (    i1Buff->isValid());
          CHECK (*i1Buff == *oBuff); // second invocation exactly reproduced data from first invocation
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeDevel_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
