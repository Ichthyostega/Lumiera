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
#include "lib/random.hpp"
//#include "lib/util.hpp"


//using std::string;


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
          
          generateMultichan (channels, buffs[0], frameNr, flavour);
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
          CHECK (iBuff->isPristine());
          
          uint64_t param = defaultGen.u64();
          manipulateFrame (oBuff, iBuff, param);
          CHECK (    oBuff->isValid());
          CHECK (not oBuff->isPristine());
          CHECK (    iBuff->isPristine());
          
          for (uint i=0; i<oBuff->data64().size(); ++i)
            {
              uint64_t feed = param;
              uint64_t data = iBuff->data64()[i];
              lib::hash::combine (feed, data);
              CHECK (data  == iBuff->data64()[i]);
              CHECK (feed  != iBuff->data64()[i]);
              CHECK (feed  == oBuff->data64()[i]);
            }
           // can also process in-place
          manipulateFrame (iBuff, iBuff, param);
          CHECK (not iBuff->isPristine());
          CHECK (    iBuff->isValid());
          CHECK (*iBuff == *oBuff);  // second invocation exactly reproduced data from first invocation
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeDevel_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
