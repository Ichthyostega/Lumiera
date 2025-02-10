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
 ** Unit test \ref NodeDevel_test verifies helpers for testing of render nodes.
 */


#include "lib/test/run.hpp"
#include "lib/hash-combine.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/engine/node-builder.hpp"
#include "steam/engine/test-rand-ontology.hpp"
#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "lib/test/diagnostic-output.hpp"/////////////////TODO
#include "lib/iter-zip.hpp"
#include "lib/random.hpp"
//#include "lib/util.hpp"

#include <vector>

using lib::zip;
using lib::izip;
using std::vector;
using std::make_tuple;
using lib::test::showType;


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
          processing_duplicateMultichan();
          processing_manipulateMultichan();
          processing_manipulateFrame();
          processing_combineFrames();
          
          testRand_simpleUsage();
          testRand_buildFilterNode();
          testRand_buildMixNode();
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
          
          ont::generateFrame (buff, frameNr, flavour);
          CHECK ( buff->isSane());
          CHECK ( buff->isPristine());
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
          
          Buffer buff[50];
          for (uint i=0; i<channels; ++i)
            CHECK (not buff[i]->isSane());
          
          ont::generateMultichan (buff[0], channels, frameNr, flavour);
          for (uint i=0; i<channels; ++i)
            {
              CHECK (buff[i]->isPristine());
              CHECK (*(buff[i]) == TestFrame(frameNr,flavour+i));
            }
        }
      
      
      /** @test clone copy of multichannel test data */
      void
      processing_duplicateMultichan()
        {
          size_t frameNr = defaultGen.u64();
          uint flavour   = defaultGen.u64();
          uint channels  = 1 + rani(50);
          Buffer srcBuff[50];
          ont::generateMultichan (srcBuff[0], channels, frameNr, flavour);
          
          Buffer clone[50];
          for (uint i=0; i<channels; ++i)
            CHECK (not clone[i]->isSane());
          
          ont::duplicateMultichan (clone[0],srcBuff[0], channels);
          for (uint i=0; i<channels; ++i)
            {
              CHECK (clone[i]->isPristine());
              CHECK (*(clone[i]) == *(srcBuff[i]));
            }
        }
      
      
      /** @test multichannel data hash-chain manipulation
       *   - use multichannel pseudo random input data
       *   - store away a clone copy before manipulation
       *   - the #manipulateMultichan() operates in-place in the buffers
       *   - each buffer has been marked with a new checksum afterwards
       *   - and each buffer now differs from original state
       *   - verify that corresponding data points over all channels
       *     have been linked by a hashcode-chain, seeded with the `param`
       *     and then consecutively hashing in data from each channel.
       */
      void
      processing_manipulateMultichan()
        {
          size_t frameNr = defaultGen.u64();
          uint flavour   = defaultGen.u64();
          uint channels  = 1 + rani(50);
          Buffer buff[50], refData[50];
          ont::generateMultichan (buff[0], channels, frameNr, flavour);
          // stash away a copy of the test data for verification
          ont::duplicateMultichan(refData[0],buff[0], channels);
          
          for (uint c=0; c<channels; ++c)
            CHECK (buff[c]->isPristine());
          
          uint64_t param = defaultGen.u64();
          ont::manipulateMultichan(buff[0], channels, param);
          
          const uint SIZ = buff[0]->data64().size();
          vector<uint64_t> xlink(SIZ, param);     // temporary storage for verifying the hash-chain
          for (uint c=0; c<channels; ++c)
            {
              CHECK (buff[c]->isSane());          // checksum matches
              CHECK (not buff[c]->isPristine());  // data was indeed changed
              
              CHECK (*(buff[c]) != *(refData[c]));
              
              for (auto& [i, link] : izip(xlink))
                {
                  auto const& refPoint = refData[c]->data64()[i];
                  lib::hash::combine (link, refPoint);
                  CHECK (link != refPoint);
                  CHECK (link == buff[c]->data64()[i]);
                }
            }
        }
      
      /** @test function to apply a numeric computation to test data frames;
       * @remark here basically the same hash-chaining is used as for #manipulateMultichan,
       *   but only one hash-chain per data point is used and output is written to a different buffer.
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
          ont::manipulateFrame (oBuff, iBuff, param);
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
          ont::manipulateFrame (iBuff, iBuff, param);
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
          ont::combineFrames (oBuff, i1Buff, i2Buff, mix);
          CHECK (    oBuff->isValid());
          CHECK (not oBuff->isPristine());
          CHECK (    i1Buff->isPristine());
          CHECK (    i2Buff->isPristine());
          
          for (auto [oDat,i1Dat,i2Dat] : zip (oBuff->data()
                                             ,i1Buff->data()
                                             ,i2Buff->data()))
            CHECK (oDat == std::lround((1-mix)*i1Dat + mix*i2Dat));
          
          // can also process in-place
          ont::combineFrames (i1Buff, i1Buff, i2Buff, mix);
          CHECK (not i1Buff->isPristine());
          CHECK (    i1Buff->isValid());
          CHECK (*i1Buff == *oBuff); // second invocation exactly reproduced data from first invocation
        }
      
      
      
      /** @test demonstrate simple usage of test-render setup
       *   - access the TestRandOntology as singleton
       *   - create a Spec record
       *   - retrieve a functor bound suitably to invoke
       *     data processing code from the TestRandOntology
       */
      void
      testRand_simpleUsage()
        {
          auto spec = testRand().setupGenerator();
          CHECK (spec.PROTO == "generate-TestFrame"_expect);
          
          // generate a binding as processing-functor
          auto procFun = spec.makeFun();
          using Sig = lib::meta::_Fun<decltype(procFun)>::Sig;
          CHECK (showType<Sig>() == "void (tuple<ulong, uint>, engine::test::TestFrame*)"_expect);

          // Behaves identical to processing_generateFrame() — see above...
          size_t frameNr = defaultGen.u64();
          uint flavour   = defaultGen.u64();
          
          Buffer buff;
          CHECK (not buff->isSane());
          
          procFun (make_tuple (frameNr,flavour), buff);
          CHECK ( buff->isSane());
          CHECK ( buff->isPristine());
          CHECK (*buff == TestFrame(frameNr,flavour));
          
          // Build a node using this processing-functor...
          ProcNode node{prepareNode(spec.nodeID())
                          .preparePort()
                            .invoke(spec.procID(), procFun)
                            .setParam(frameNr,flavour)
                            .completePort()
                          .build()};
          
          CHECK (watch(node).isSrc());
          CHECK (watch(node).getNodeSpec() == "Test:generate-◎"_expect);
          CHECK (watch(node).getPortSpec(0) == "generate(TestFrame)"_expect);
          
          BufferProvider& provider = DiagnosticBufferProvider::build();
          BuffHandle buffHandle = provider.lockBuffer (provider.getDescriptorFor(sizeof(TestFrame)));
          
          uint port{0};
          
          CHECK (not buffHandle.accessAs<TestFrame>().isSane());
          
          // Trigger Node invocation...
          buffHandle = node.pull (port, buffHandle, Time::ZERO, ProcessKey{0});
          
          TestFrame& result = buffHandle.accessAs<TestFrame>();
          CHECK (result.isSane());
          CHECK (result.isPristine());
          CHECK (result == *buff);
          buffHandle.release();
        }
      
      /** shortcut to simplify the following test cases */
      static ProcNode
      makeSrcNode (ont::FraNo frameNr, ont::Flavr flavour)
        {
          auto spec = testRand().setupGenerator();
SHOW_EXPR(spec.nodeID())
          return prepareNode(spec.nodeID())
//          ProcNode n{prepareNode(spec.nodeID())
                      .preparePort()
                        .invoke(spec.procID(), spec.makeFun())
                        .setParam(frameNr,flavour)
                        .completePort()
                      .build();
//SHOW_EXPR(watch(n).getNodeName()  );
//          return move(n);
        }
      
      
      
      /** @test use the »TestRand«-framework to setup a filter node
       * 
       */
      void
      testRand_buildFilterNode()
        {
          auto spec = testRand().setupManipulator();
          CHECK (spec.PROTO == "manipulate-TestFrame"_expect);
          
          // generate a binding as processing-functor
          auto procFun = spec.makeFun();
          using Sig = lib::meta::_Fun<decltype(procFun)>::Sig;
          CHECK (showType<Sig>() == "void (ulong, engine::test::TestFrame const*, engine::test::TestFrame*)"_expect);
          
          // Results can be verified by ont::manipulateFrame() — see above
          size_t frameNr = defaultGen.u64();
          uint flavour   = defaultGen.u64();
          uint64_t param = defaultGen.u64();
          
          Buffer buff;
          buff.buildData(frameNr,flavour);
          CHECK (buff->isPristine());
          
          // Invoke the processing-functor directly
          procFun (param, buff,buff);
          CHECK (    buff->isValid());
          CHECK (not buff->isPristine());
          HashVal checksum = buff->markChecksum();
          
          // reproduce the same checksum...
          buff.buildData(frameNr,flavour);
          CHECK (buff->isPristine());
          CHECK (checksum != buff->markChecksum());
          ont::manipulateFrame (buff, buff, param);
          CHECK (checksum == buff->markChecksum());
          
          // Build a node using this processing-functor...
          ProcNode nSrc = makeSrcNode (frameNr,flavour);
SHOW_EXPR(watch(nSrc).getNodeName()  );
  ProcID& px = ProcID::describe("Test:generate","(TestFrame)");
SHOW_EXPR(px.genNodeName())
          ProcNode nFilt{prepareNode(spec.nodeID())
                          .preparePort()
                            .invoke(spec.procID(), procFun)
                            .setParam(param)
                            .connectLead(nSrc)
                            .completePort()
                          .build()};
          
          CHECK (watch(nSrc).isSrc());
          CHECK (not watch(nFilt).isSrc());
SHOW_EXPR(watch(nSrc).getNodeSpec()  );
SHOW_EXPR(watch(nFilt).getNodeSpec()  );
SHOW_EXPR(watch(nFilt).getPortSpec(0) );
        }
      
      
      /** @test use the »TestRand«-framework to setup a two-chain mixer node
       * 
       */
      void
      testRand_buildMixNode()
        {
          UNIMPLEMENTED ("Mixer Node");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeDevel_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
