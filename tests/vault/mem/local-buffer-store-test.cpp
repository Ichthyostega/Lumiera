/*
  LocalBufferStore(Test)  -  verify thread-local buffer memory allocator proxy

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file local-buffer-store-test.cpp
 ** unit test \ref LocalBufferStore_test
 */


#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "vault/mem/local-buffer-store.hpp"
#include "vault/mem/engine-buffer-manager.hpp"
#include "vault/mem/engine-buffer-allocator.hpp"
#include "lib/depend-inject.hpp"
#include "lib/thread.hpp"
#include "lib/util.hpp"
#include "test/diagnostic-output.hpp"///////////////////////TODO

#include <algorithm>
#include <atomic>

using util::isSameAdr;

using lib::Thread;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using std::chrono_literals::operator ""ms;

namespace vault {
namespace mem   {
namespace test  {
  
//  using LERR_(LOGIC);
//  using LERR_(LIFECYCLE);
  
  
  namespace { // Test helper
    
//    template<typename X>
//    Buff*
//    mark_as_Buffer(X& something)
//      {
//        return reinterpret_cast<Buff*> (std::addressof(something));
//      }
//    
  }
  
  
  
  
  
  
  /**************************************************************************//**
   * @test verify worker memory handling that relies on a thread-local allocator.
   */
  class LocalBufferStore_test : public Test
    {
      
      virtual void
      run (Arg)
        {
//          seedRand();
          
          verify_API();
          verify_announce();
          verify_allocate();
          verify_release();
          verify_heuristic();
        }
      
      
      
      /** @test demonstrate a typical usage cycle of the BufferStore in concurrent setup
       *      - the main thread provides a mock instance of the EngineBufferManager
       *      - furthermore, the LocalBufferStore instance is created here
       *      - yet note: LocalBufferStore includes a LocalMemPool per thread
       *      - a test thread is launched to issue some allocation requests
       *      - announce some memory allocation demand
       *      - claim and use one buffer
       *      - generate further demand to trigger further allocation supply
       *      - terminate the thread
       *      - verify the number of allocations seen in the EngineBufferManager
       */
      void
      verify_API()
        {
          // provide a »central BufferPool« (as a transient instance for this test)
          lib::DependInject<EngineBufferManager>::Local<EngineBufferManager> globalPool;
          
          // provide the *test subject*,
          // which implements the BufferStore API
          // and uses a thread-local allocation pool internally
          LocalBufferStore localStore;
          BufferProviderSetup::Store& storeAPI = localStore;
          
          const size_t BUFFSIZ = 555;
          const HashVal TYPEID = 12345;
          const size_t     CNT = 5;
          const int64_t    ARG = 7;
          const LocalTag  MARK{42};
          
          const size_t   ALLOC = HeapBufferAllocator::numTiles(BUFFSIZ) * HeapBufferAllocator::TILE_SIZ;
          
          Thread testWorker{[&] /* === Play through one typical usage cycle === */
                              {
                                uint avail = storeAPI.prepareBuffers(TYPEID, CNT, BUFFSIZ);
                                CHECK (0 == avail);
                                
                                BuffAlloc storageSlot = storeAPI.provideBuffer (TYPEID, BUFFSIZ, MARK, ARG);
                                auto& [storage,buffSiz,specifics] = storageSlot;
                                CHECK (storage);
                                CHECK (BUFFSIZ <= buffSiz);
                                CHECK (ALLOC ==  buffSiz);
                                CHECK (MARK == specifics);
                                
                                // can use that memory
                                char* buf = reinterpret_cast<char*> (storage);
                                std::string ranS{lib::randStr(555-1)};
                                auto* p = std::copy (ranS.begin(), ranS.end(), buf);
                                CHECK (ranS.length() == size_t(p - buf));
                                *p = '\0';
                                CHECK (ranS == buf);
                                
                                // reserve four additional buffers;
                                // these happen to sit already in the local pool anyway
                                // and will be marked as "reserved" now....
                                avail = storeAPI.prepareBuffers(TYPEID, 4, BUFFSIZ);
                                CHECK (4 == avail);
                                
                                // finish usage and return the single buffer claimed thus far...
                                storeAPI.mark_emitted (TYPEID, storageSlot);
                                storeAPI.detachBuffer (TYPEID, storageSlot);
                                
                                // now we have 5 buffers in the pool, but 4 are marked as "reserved"
                                // thus only one buffer can be confirmed and one further is requested from EngineBufferManager
                                avail = storeAPI.prepareBuffers(TYPEID, 2, BUFFSIZ);
                                CHECK (1 == avail);
                                
                                // but irrespective of reservation, we can immediately use the buffers locally available
                                BuffAlloc slot1 = storeAPI.provideBuffer (TYPEID, BUFFSIZ, MARK, ARG);
                                auto& [sto1,siz1,tag1] = slot1;
                                BuffAlloc slot2 = storeAPI.provideBuffer (TYPEID, BUFFSIZ, MARK, ARG);
                                auto& [sto2,siz2,tag2] = slot2;
                                CHECK (sto1);
                                CHECK (sto2);
                                CHECK (isSameAdr (storage, sto1));
                                CHECK (not isSameAdr (storage, sto2));
                                CHECK (BUFFSIZ <= siz1);
                                CHECK (BUFFSIZ <= siz2);
                                
                                storeAPI.detachBuffer (TYPEID, slot2);
                                storeAPI.detachBuffer (TYPEID, slot1);
                              }};  // LocalMemPool winds down at end of thread
          while (testWorker)
            yield();     // wait for worker to finish
          
          // additional delay for the local pool's destructor
          // that is invoked after the thread has finished
          sleep_for(1ms);
          
          CHECK (globalPool);   // the Mock was accessed (and thus instantiated)
          
          // before shutdown, LocalMemPool has sent back  all allocations;
          // these should sit now in the EngineBufferManager's input queue,
          // unprocessed yet; same for the additional buffer requested later.
          // EngineBufferManager only sees the first CNT allocations actually dispatched
          CHECK (CNT       == watch(*globalPool).numAllocs());
          CHECK (CNT*ALLOC == watch(*globalPool).bytesLeased());
          
          // process the in-queue and retrieve all allocations...
          globalPool->processPendingRequests();
          // everything should be cleaned-up by now...
          CHECK (0 == watch(*globalPool).bytesLeased());
        }
      
      
      
      
      /** @test announcement of capacity demand is propagated eventually
       *      - test thread must proceed in lockstep to make effects observable
       *      - Step-1: test thread announces demand on the subject (`BufferStore` API)
       *      - Step-2: „someone“ processes pending requests in the EngineBufferManager
       *      - Step-3: test thread announces further demand that remains unprocessed
       *      - test thread then terminates, sends back all allocations
       *      - the pending request is ignored transparently
       */
      void
      verify_announce()
        {
          // »central BufferPool« (transient instance for this test)
          lib::DependInject<EngineBufferManager>::Local<EngineBufferManager> globalPool;
          
          // *test subject*
          LocalBufferStore localStore;
          BufferProviderSetup::Store& storeAPI = localStore;
          
          const size_t BUFFSIZ = 1024;
          const HashVal TYPEID = 12345;
          
          // coordination of test steps
          std::atomic_uint step{1};
          
          Thread testWorker{[&] /* === Scenario: test subject announces capacity demand === */
                              {
                                // Step-1
                                uint avail = storeAPI.prepareBuffers(TYPEID, 1, BUFFSIZ);
                                CHECK (0 == avail);
                                
                                CHECK (watch(localStore).isEmpty());
                                
                                // wait for being serviced...
                                ++step;
                                while (step < 3)
                                  sleep_for(1ms);
                                
                                // Step-3
                                CHECK (not watch(localStore).isEmpty());
                                CHECK (watch(localStore).canServe(BUFFSIZ));
                                CHECK (watch(localStore).cntFree() == 1);
                                CHECK (watch(localStore).size()    == 1);
                                
                                // Note: this block was supplied but the local pool does not know why,
                                //       since reservations are only tracked on blocks already available
                                CHECK (watch(localStore).cntResd() == 0);
                                
                                // Now, instead of using this buffer block,
                                // rather generate another capacity announcement...
                                avail = storeAPI.prepareBuffers(TYPEID, 2, BUFFSIZ);
                                CHECK (1 == avail);

                                CHECK (watch(localStore).cntResd() == 1);
                                CHECK (watch(localStore).cntFree() == 1);
                                CHECK (watch(localStore).size()    == 1);
                                CHECK (watch(localStore).canServe(BUFFSIZ));
                                
                                // Note: while a further request is "out there",
                                //       only a single block was received up to now.
                                //       LocalMemPool winds down automatically at end of thread
                              }};
          
          
          /* === Main thread: observe effects in the global pool === */
          
          // wait until the first announcement was made
          while (step < 2)
            sleep_for(1ms);
          
          // Step-2
          CHECK (watch(*globalPool).numAllocs() == 0);
          
          // process the in-queue and service the allocation request...
          globalPool->processPendingRequests();
          
          CHECK (watch(*globalPool).numAllocs() == 1);
          CHECK (watch(*globalPool).bytesLeased() >= BUFFSIZ);
          
          // let the test thread proceed...
          step = 3;
          
          // wait until test thread has wound down
          while (testWorker)
            sleep_for(1ms);
          
          // additional delay to be sure the destructor of the local pool has finished
          sleep_for(1ms);
          
          CHECK (watch(*globalPool).numAllocs() == 1);
          globalPool->processPendingRequests();
          CHECK (watch(*globalPool).bytesLeased() == 0);
        }
      
      
      
      /** @test 
       */
      void
      verify_allocate()
        {
        }
      
      
      
      /** @test 
       */
      void
      verify_release()
        {
        }
      
      
      
      /** @test 
       */
      void
      verify_heuristic()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (LocalBufferStore_test, "unit engine");
  
  
  
}}} // namespace vault::mem::test
