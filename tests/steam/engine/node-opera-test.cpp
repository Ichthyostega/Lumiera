/*
  NodeOpera(Test)  -  verify proper render node operation modes

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file node-opera-test.cpp
 ** Let the nodes sing with \ref NodeOpera_test.
 */


#include "test/run.hpp"
#include "lib/allocation-cluster.hpp"
#include "test/test-rand-ontology.hpp"
#include "vault/mem/buffer-provider-setup.hpp"
#include "vault/mem/engine-buffer-metadata.hpp"
#include "vault/mem/engine-buffer-manager.hpp"
#include "vault/mem/local-buffer-stage.hpp"
#include "vault/mem/local-buffer-store.hpp"
#include "steam/engine/engine-facilities.hpp"
#include "steam/engine/node-builder.hpp"
#include "steam/asset/meta/time-grid.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time/timecode.hpp"
#include "lib/depend-inject.hpp"
#include "lib/thread.hpp"
//#include "lib/symbol.hpp"
#include "lib/util.hpp"
#include "test/diagnostic-output.hpp"///////////////TODO


//using std::string;
using lib::Thread;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using std::chrono_literals::operator ""us;

using lib::time::Time;
using lib::time::QuTime;
using lib::time::FrameNr;
using lib::time::FrameCnt;
using lib::time::FrameRate;
using lib::time::PGrid;
using steam::asset::meta::TimeGrid;
//using lib::Symbol;
//using util::isnil;
//using util::isSameObject;
using util::isLimited;
using test::TestFrame;
using test::testRand;
namespace ont = test::ont;

namespace steam {
namespace engine{
namespace test  {
  
  namespace { // ========== Test Setup ==========
    
    using vault::mem::BufferProviderSetup;
    using vault::mem::LocalBufferStage;
    using vault::mem::LocalBufferStore;
    
    using vault::mem::EngineBufferMetadata;
    using vault::mem::EngineBufferManager;
    
    class TestEngineBufferSetup
      : public BufferProviderSetup
      , public EngineFacilities
      {
      public:
        TestEngineBufferSetup()
          : BufferProviderSetup{*this}
          { }
          
          auto buildStage() { return std::make_unique<LocalBufferStage>("NodeOpera_test"); }
          auto buildStore() { return std::make_unique<LocalBufferStore>(); }
          
          BufferProvider& setupBufferProvider() override { return *this; }
      };
    
    struct TestEngineCtx
      {
        // install mock instances for the relevant services
        lib::DependInject<EngineBufferMetadata>::Local<>  metaHub;
        lib::DependInject<EngineBufferManager>::Local<> globalPool;
        
        // install a rigged access front-end for the buffer management
        lib::DependInject<EngineFacilities>::Local<TestEngineBufferSetup> bufferSetup;
        lib::DependInject<EngineCtx>::Local<EngineCtx>                    frontEndCtx;
      };
  
  }
  
  
  
  
  /***************************************************************//**
   * @test check render node operation modes and collaboration.
   * @todo 6/2026 this test shell demonstrate all relevant operation modes
   *       of render nodes, at the level of an component-integration-test.
   *       It is not clear yet what further operation modes this entails,
   *       beyond (obviously) invoking a render job...       ////////////////////////////////////////////////TICKET #1367 : build a node invocation
   * @remark the purpose of this test is to accomplish component integration
   *       regarding relevant usages of the render node network, seen from the
   *       perspective of an (exit) render node that is used / invoked / pulled.
   */
  class NodeOpera_test : public Test
    {
      virtual void
      run (Arg)
        {
          seedRand();
          
          invokeSimpleNode();
          invokeNodeTree();
        }
      
      
      /** @test directly invoke a simple source node.
       *      - use the node builder to create a simple node
       *        immediately into the current stack frame
       *      - the processing function feeds a fixed random number
       *        into the given output buffer
       *      - then launch a worker thread to pull this node
       *      - create an output buffer directly from the worker thread
       *      - invoke the single port of the node, which is visible
       *        since it was created before starting the local worker thread
       *      - after the invocation, the fixed random number should have been
       *        placed into the output buffer
       *      - after the worker thread has finished, a single buffer allocation
       *        should have happened; this allocation was sent back automatically
       *        and can be claimed and released by the global buffer manager.
       */
      void
      invokeSimpleNode()
        {
          int randNum{rani()};
          auto fun = [&](int* buff){ *buff = randNum; };
          
          TestEngineCtx mockEngine;
          
          ProcNode node{prepareNode("Test")
                          .preparePort()
                            .invoke("fun()", fun)
                            .completePort()
                          .build()};
          
          Thread testWorker{[&] /* === directly perform a node invocation === */
                              {
                                BuffHandle buff = EngineCtx::access().mem.lockBufferFor<int> (-55);
                                Time nomTime{Time::ZERO};
                                ProcessKey key{0};
                                uint port{0};
                                
                                CHECK (-55 == buff.accessAs<int>());
                                
                                // Trigger Node invocation...
                                buff = node.pull (port, buff, nomTime, key);
                                
                                CHECK (randNum == buff.accessAs<int>());
                                buff.release();
                              }};
          
          while (testWorker)
            yield();     // wait for worker to finish
          
          // the central services were indeed requested / created
          CHECK (mockEngine.bufferSetup);
          CHECK (mockEngine.globalPool);
          CHECK (mockEngine.metaHub);
          
          // at least two buffer types registered (one generic for int)
          CHECK (2 <= mockEngine.metaHub->cntEntries());

          // one buffer was requested...
          CHECK (1 == watch(*mockEngine.globalPool).numAllocs());

          sleep_for (500us); // additional delay for the local pool's destructor
                            //  (which is invoked after the thread has terminated)
          
          // process and close remaining allocations
          // sent back by the local pool's destructor...
          mockEngine.globalPool->processPendingRequests();
          // now there should be no memory leaks.......
          CHECK (0 == watch(*mockEngine.globalPool).bytesLeased());
        }
      
      
      
      
      /** @test perform a RenderInvocation to pull from a complex node tree into an output sink.
       *      - use a test helper class to build a node topology similar to NodeLink_test
       *      - the ProcNodes and all backing structures are managed by an AllocationCluster
       */
      void
      invokeNodeTree()
        {
          TestEngineCtx mockEngine;
          RenderPipeline pipeline;
          
          Thread testWorker{[&] /* === perform a render invocation === */
                              {
                                Time nomTime{rani(60'000),0};  // drive test with a random »nominal Time« <60s with ms granularity
                                ProcessKey key{0};
                                uint port{0};
                                
                                BuffHandle buff = EngineCtx::access().mem.lockBufferFor<TestFrame>();
                                TestFrame& result = buff.accessAs<TestFrame>();
                                CHECK (result.isPristine());
                                
                                // Trigger Node invocation...
                                buff = pipeline.exitNode.pull (port, buff, nomTime, key);
                                
                                CHECK (result.isValid());
                                CHECK (not result.isPristine());
                                CHECK (result.getChecksum() == pipeline.expectedChecksum (nomTime));
                                buff.release();
                              }};
          
          while (testWorker)
            yield();      // wait for worker to finish
          sleep_for (500us);
          
          mockEngine.globalPool->processPendingRequests();
          // now there should be no memory leaks.......
          CHECK (0 == watch(*mockEngine.globalPool).bytesLeased());
        }
      
      
      /**
       * Helper context to build and manage a processing chain of Render Nodes.
       * In the real Render Engine, these processing chains are generated by the Builder,
       * and organised into »Segments« of the timeline, where each segment features a
       * distinct node connectivity (i.e. typically for one clip).
       * For this test, a single top-level _exit node_ is exposed.
       * @note this helper context is constructed in _RAII style_,
       *       and the embedded AllocationCluster unwinds automatically.
       * @see \ref NodeLink_test for a detailed breakdown of a similar topology
       */
      class RenderPipeline
        : util::NonCopyable
        {
          static constexpr ont::Flavr SRC_A = 10;         ///< »chain-A« arbitrary source frame marker
          static constexpr ont::Flavr SRC_B = 20;         ///< similar for »chain-B«
          
          PGrid secondsGrid_{TimeGrid::build (FrameRate{1})};
          FrameCnt quantSecs (Time time) { return secondsGrid_->gridPoint (time); };
           
          /** custom allocator for this node tree */
          lib::AllocationCluster alloc_;
          
        public:
          ProcNode& exitNode{buildTree()};
          
          HashVal
          expectedChecksum (Time nomTime)
            {
              ont::FraNo fraNo = quantSecs(nomTime);
              ont::Param param = stepFilter(fraNo);
              ont::Factr mix   = stepMixer (fraNo);
              
              TestFrame f1{uint(fraNo),SRC_A};
              TestFrame f2{uint(fraNo),SRC_B};
              
              ont::manipulateFrame (&f1, &f1, param);
              ont::combineFrames (&f1, &f1, &f2, mix);
              return f1.getChecksum();
            }
          
        private:
          ont::Param stepFilter(FrameCnt id) { return util::limited (10, -10 + id, 50);        };
          ont::Factr stepMixer (FrameCnt id) { return util::limited (0,      + id, 50) / 50.0; };
          
          /** @internal wire the processing node topology.
           * This fake "render pipeline" is based on hash chaining computations,
           * operating on TestFrame (with reproducible random data content).
           * Two processing chains are built, each producing test frames;
           * the first chain even adds a "filter". Some "parameters" are added
           * to change the random data in a deterministic way. Finally, both chains
           * are "mixed" together into a single result TestFrame. Furthermore, the
           * parameter automation is based on a precomputed _frame number_ parameter,
           * that is derived from the _absolute nominal time_ by quantisation into a
           * time grid with a stepping of 1 second (-> `FrameRate{1}`). This precomputed
           * frame number parameter is stored in a parameter data block on the call stack,
           * prior to recursively evaluating the node chain attached below.
           * @note the generated Render Nodes are placed into the AllocationCluster,
           *       together with all the further storage used by those nodes internally.
           *       The real Render Engine uses an AllocationCluster per Segment of the timeline.
           */
          ProcNode&
          buildTree()
            {
              // Prepare a precomputed parameter for the complete tree
              auto selectFrameNo = [&](TurnoutSystem& turSys){ return quantSecs (turSys.getNomTime()); };
              auto paramSpec = buildParamSpec()
                                .addSlot (selectFrameNo);
              auto accFrameNo = paramSpec.makeAccessor<0>();
          
              // Automation functions using the accessor for the precomputed FrameNo-parameter
              auto autoFilter = [&](TurnoutSystem& turSys){ return stepFilter (turSys.get (accFrameNo)); };
              auto autoMixer  = [&](TurnoutSystem& turSys){ return stepMixer  (turSys.get (accFrameNo)); };
              
              // use processing functions from the »TestRand Ontology«
              auto testGen = testRand().setupGenerator();
              auto testMan = testRand().setupManipulator();
              auto testMix = testRand().setupCombinator();
              
              // A source node to produce random test frames
              auto& n1s = alloc_.create<ProcNode>(
                            prepareNode("srcA")
                              .withAllocator(alloc_)
                              .preparePort()
                                .invoke(testGen.procID(), testGen.makeFun())   // params(frameNo, flavour)
                                .closeParam<1>(SRC_A)                          // --> fix the flavour ≔ SRC_A
                                .retrieveParam(accFrameNo)
                                .completePort()
                              .build());
              
              // A node to »filter« the data in chain-A
              auto& n1f = alloc_.create<ProcNode>(
                            prepareNode("filterA")
                              .withAllocator(alloc_)
                              .preparePort()
                                .invoke(testMan.procID(), testMan.makeFun())
                                .attachParamFun(autoFilter)                    // filter-param <-- autoFilter(frameNo)
                                .connectLead(n1s)
                                .completePort()
                              .build());
              
              
              // A secondary source Node for the »chain-B«
              auto& n2s = alloc_.create<ProcNode>(
                            prepareNode("srcB")
                              .withAllocator(alloc_)
                              .preparePort()
                                .invoke(testGen.procID(), testGen.makeFun())   // params(frameNo, flavour)
                                .closeParam<1>(SRC_B)                          // --> fix the flavour ≔ SRC_B
                                .retrieveParam(accFrameNo)
                                .completePort()
                              .build());
              
              
              // A mixing Node to combine both chains
              auto& mix = alloc_.create<ProcNode>(
                            prepareNode("mix")
                              .withAllocator(alloc_)
                              .preparePort()
                                .invoke(testMix.procID(), testMix.makeFun())
                                .attachParamFun(autoMixer)                     // mixer-param <-- autoMixer(frameNo)
                                .connectLead(n1f)
                                .connectLead(n2s)
                                .completePort()
                              .build());
              
              
              // Place a »Param-Agent«-Node on top to pre-compute the FrameNo
              auto& parNode = alloc_.create<ProcNode>(
                            prepareNode("Param")
                              .withAllocator(alloc_)
                              .preparePort()
                                .computeParam(paramSpec)
                                .delegateLead(mix)
                                .completePort()
                              .build());
              
              // the AllocationCluster managed some storage...
              CHECK (isLimited (1_KiB, alloc_.numBytes(), 5_KiB));
              
              // here the »Param-Agent« also acts as the Exit-Node
              return parNode;
            }
        };
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeOpera_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
