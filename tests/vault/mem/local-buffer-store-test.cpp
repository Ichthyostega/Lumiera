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
//#include "vault/mem/engine-buffer-metadata.hpp"
#include "vault/mem/local-buffer-store.hpp"
#include "vault/mem/engine-buffer-manager.hpp"
#include "vault/mem/engine-buffer-allocator.hpp"
#include "lib/depend-inject.hpp"
#include "lib/thread.hpp"
//#include "lib/error.hpp"
#include "lib/util.hpp"
#include "test/diagnostic-output.hpp"

#include <algorithm>

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
      
      
      
      /** @test  */
      void
      verify_API()
        {
          // provide a »central BufferPool« (as a transient instance for this test)
          lib::DependInject<EngineBufferManager>::Local<EngineBufferManager> globalPool;
          
          // provide the *test subject*,
          // which implements the BufferStage API
          // and uses a thread-local metadata table internally
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
SHOW_EXPR(watch(*globalPool).numAllocs());
          CHECK ( CNT        == watch(*globalPool).numAllocs());
          CHECK ((CNT)*ALLOC == watch(*globalPool).bytesLeased());
          
          // process in-queue and retrieve all allocations..
SHOW_EXPR("Kaka")
          globalPool->processPendingRequests();
SHOW_EXPR("Boom")
SHOW_EXPR(watch(*globalPool).bytesLeased());
//          CHECK (0 == watch(*globalPool).bytesLeased());
          sleep_for(1000ms);
SHOW_EXPR("bye")
        }
      
      
      
      /** @test 
       */
      void
      verify_announce()
        {
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
