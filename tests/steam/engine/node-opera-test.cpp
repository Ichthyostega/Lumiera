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
#include "vault/mem/buffer-provider-setup.hpp"
#include "vault/mem/engine-buffer-metadata.hpp"
#include "vault/mem/engine-buffer-manager.hpp"
#include "vault/mem/local-buffer-stage.hpp"
#include "vault/mem/local-buffer-store.hpp"
#include "steam/engine/engine-facilities.hpp"
#include "steam/engine/node-builder.hpp"
#include "lib/depend-inject.hpp"
#include "lib/thread.hpp"
//#include "lib/util.hpp"
#include "test/diagnostic-output.hpp"///////////////TODO


//using std::string;
using lib::Thread;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using std::chrono_literals::operator ""us;


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
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeOpera_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
