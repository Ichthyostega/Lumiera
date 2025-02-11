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
#include "lib/iter-zip.hpp"
#include "lib/random.hpp"

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
   *     - raw processing functions to generate and manipulate
   *       \ref TestFrame data, including hash chaining.
   *     - a »TestRand-Ontology«, which is a test helper framework,
   *       and mimics a real _Domain Ontology_ (as would be accessible
   *       through the adapter plug-in of a specific media handling library.
   *     - some convenience shortcuts to build test-nodes
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
          return prepareNode(spec.nodeID())
                      .preparePort()
                        .invoke(spec.procID(), spec.makeFun())
                        .setParam(frameNr,flavour)
                        .completePort()
                      .build();
        }
      
      
      
      
      /** @test use the »TestRand«-framework to setup a filter node
       *      - implementation is backed by the ont::manipulateFrame() function
       *      - it thus operates on \ref TestFrame data and results can be verified
       *      - the generated spec-recod provides a processing-functor binding and node-spec
       *      - can build and wire a Node processing chain with a source node and a »filter«
       *        node based on this data manipulation, which exactly reproduces the data
       *        content generated by the stand-alone invocation.
       * @remark such a test-setup thus not only allows to prove that the function was invoked,
       *        but also the order in which the processing took place, due to hash-chaining
       *        applied to every single data word in the `TestFrame` buffer.
       * @see NodeLink_test::trigger_node_port_invocation()
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
          CHECK (checksum != buff->getChecksum());
          ont::manipulateFrame (buff, buff, param);
          CHECK (checksum == buff->getChecksum());
          
          // Build a node using this processing-functor...
          ProcNode nSrc = makeSrcNode (frameNr,flavour);
          ProcNode nFilt{prepareNode(spec.nodeID())
                          .preparePort()
                            .invoke(spec.procID(), procFun)
                            .setParam(param)
                            .connectLead(nSrc)
                            .completePort()
                          .build()};
          
          CHECK (watch(nSrc).isSrc());
          CHECK (not watch(nFilt).isSrc());
          CHECK (watch(nSrc).getNodeSpec() == "Test:generate-◎"_expect );
          CHECK (watch(nFilt).getNodeSpec() == "Test:manipulate◁—Test:generate-◎"_expect );
          CHECK (watch(nFilt).getPortSpec(0) == "manipulate(TestFrame)"_expect );
          
          // prepare to invoke this Node chain...
          BufferProvider& provider = DiagnosticBufferProvider::build();
          BuffHandle buffHandle = provider.lockBuffer (provider.getDescriptorFor(sizeof(TestFrame)));
          uint port{0};
          
          CHECK (not buffHandle.accessAs<TestFrame>().isValid());
          
          // Trigger Node invocation...
          buffHandle = nFilt.pull (port, buffHandle, Time::ZERO, ProcessKey{0});
          
          TestFrame& result = buffHandle.accessAs<TestFrame>();
          CHECK (    result.isValid());
          CHECK (not result.isPristine());
          CHECK (result == *buff);
          buffHandle.release();
        }
      
      
      
      /** @test use the »TestRand«-framework to setup a two-chain mixer node
       *      - demonstrate convenience setup to package the ont::combineFrames() as »mix« Node
       *      - this time, we need two source chains, both generating \ref TestFrame data
       *      - complete processing with all steps can be verified by performing similar
       *        computations directly and comparing the result checksum.
       */
      void
      testRand_buildMixNode()
        {
          auto spec = testRand().setupCombinator();
          CHECK (spec.PROTO == "combine-TestFrame"_expect);
          
          // generate a binding as processing-functor
          auto procFun = spec.makeFun();
          using Sig = lib::meta::_Fun<decltype(procFun)>::Sig;
          CHECK (showType<Sig>() == "void (double, array<engine::test::TestFrame const*, 2ul>, "
                                                  "engine::test::TestFrame*)"_expect);  //^^/////////////////TICKET #1391 needlessly rendered as `long`
          size_t frameNr = defaultGen.u64();
          uint flavour   = defaultGen.u64();
          double mix     = defaultGen.uni();
          
          // Build node graph to combine two chains
          ProcNode nS1 = makeSrcNode (frameNr,flavour+0);
          ProcNode nS2 = makeSrcNode (frameNr,flavour+1);
          ProcNode nMix{prepareNode(spec.nodeID())
                          .preparePort()
                            .invoke(spec.procID(), procFun)
                            .setParam(mix)
                            .connectLead(nS1)
                            .connectLead(nS2)
                            .completePort()
                          .build()};
          
          CHECK (not watch(nMix).isSrc());
          CHECK (watch(nS1).getNodeSpec() == "Test:generate-◎"_expect );
          CHECK (watch(nS2).getNodeSpec() == "Test:generate-◎"_expect );
          CHECK (watch(nMix).getNodeSpec() == "Test:combine┉┉{Test:generate}"_expect );
          CHECK (watch(nMix).getPortSpec(0) == "combine(TestFrame/2)"_expect );
          
          // prepare to invoke this Node chain...
          BufferProvider& provider = DiagnosticBufferProvider::build();
          BuffHandle buffHandle = provider.lockBuffer (provider.getDescriptorFor(sizeof(TestFrame)));
          CHECK (not buffHandle.accessAs<TestFrame>().isValid());
          uint port{0};
          
          // Trigger Node invocation...
          buffHandle = nMix.pull (port, buffHandle, Time::ZERO, ProcessKey{0});
          
          CHECK (buffHandle.accessAs<TestFrame>().isValid());
          HashVal checksum = buffHandle.accessAs<TestFrame>().getChecksum();
          buffHandle.release();
          
          // verify the result data by reproducing it through direct computation
          Buffer bu1, bu2;
          bu1.buildData(frameNr,flavour+0);
          bu2.buildData(frameNr,flavour+1);
          ont::combineFrames (bu1, bu1, bu2, mix);
          CHECK (bu1->getChecksum() == checksum);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeDevel_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
