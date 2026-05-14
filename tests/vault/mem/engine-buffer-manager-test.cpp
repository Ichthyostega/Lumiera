/*
  EngineBufferManager(Test)  -  cover the interface to a central buffer manager

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file engine-buffer-manager-test.cpp
 ** unit test \ref EngineBufferManager_test
 */


#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "vault/mem/engine-buffer-manager.hpp"
#include "vault/mem/engine-buffer-allocator.hpp"
#include "lib/scoped-collection.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/sync-barrier.hpp"
#include "lib/thread.hpp"
#include "lib/symbol.hpp"

#include <algorithm>
#include <string>
#include <array>

using lib::explore;
using lib::Literal;
using lib::Thread;
using lib::SyncBarrier;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using std::chrono::microseconds;
using std::chrono_literals::operator ""ms;

namespace vault {
namespace mem   {
namespace test  {
  
  namespace { // Test parameters......
    const size_t ALLOC_REQ = 1024*1024;   ///< use test-allocation request of 1 MiB size
    const size_t NUM_THREADS = 100;       ///< press concurrently
  }
  
  
  
  
  /*****************************************************************************//**
   * @test Show how the central buffer manager of the Render Engine can be accessed.
   */
  class EngineBufferManager_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          demonstrate_AllocatorInterface();
          verify_syncRequest();
          verify_asyncRequest();
        }
      
      
      
      /** @test show how to allocate buffer memory
       *        with the underlying base allocator.
       * @remark 4/2026 for the first integration of the Render Engine,
       *        a simplified implementation is used that just delegates
       *        to the regular heap allocator (which actually performs
       *        quite well on modern C++ STDLib). The plan is to
       *        use some kind of tiling pool allocator eventually,
       *        contingent on the actual performance observations.
       */
      void
      demonstrate_AllocatorInterface()
        {
          using Allo = HeapBufferAllocator;
          CHECK (lib::allo::is_Stateless_v<Allo>);
          
          Allo allo;
          auto a1 = allo.allocate (55);
          auto a2 = allo.allocate (555);
          CHECK (a1.mem);
          CHECK (a2.mem);
          CHECK (a1.siz == Allo::TILE_SIZ);
          CHECK (a2.siz % Allo::TILE_SIZ == 0);
          CHECK (a2.siz > 555);
          
          // can use that memory without blowing up....
          auto doooh = new(a1.mem) f128{std::numeric_limits<f128>::max()};
          *doooh /= 2;
          CHECK (2 == std::numeric_limits<f128>::max() / *doooh);
          
          allo.deallocate (a1);
          allo.deallocate (a2);
        }
      
      
      
      /** @test show that a direct request for allocation is honored
       *      - request some memory
       *      - read and write to that memory
       *      - return all leased allocations
       *      - verify the diagnostic meshes up with the allocation
       * @remark in principle it is not possible to prove that some
       *      memory allocation actually works, other than checking
       *      that access to the given memory is possible, because
       *      the fact that no memory corruption took place is
       *      only semi-decidable (you can show a corruption event,
       *      but not the absence of any such an event).
       */
      void
      verify_syncRequest()
        {
          EngineBufferManager manager;
          CHECK (watch(manager).isEmpty());
          
          Alloc alloc = manager.requestAllocation (555);
          CHECK (not alloc.empty());
          CHECK (alloc.siz >= 555);
          
          CHECK (alloc.siz == watch(manager).bytesAllocd());
          CHECK (alloc.siz == watch(manager).bytesLeased());
          CHECK (alloc.siz % HeapBufferAllocator::TILE_SIZ == 0);
          
          // can use that memory
          char* buf = reinterpret_cast<char*> (alloc.mem);
          std::string ranS{lib::randStr(555-1)};
          auto* p = std::copy (ranS.begin(), ranS.end(), buf);
          CHECK (ranS.length() == size_t(p - buf));
          *p = '\0';
          CHECK (ranS == buf);
          
          CHECK (1 == watch(manager).numAllocs());
          Alloc a2 = manager.requestAllocation (1'000'000);
          CHECK (not a2.empty());
          CHECK (a2.siz >= 1e6);
          CHECK (2 == watch(manager).numAllocs());
          CHECK (1e6+555 < watch(manager).bytesLeased());
          
          manager.supply (a2);
          manager.processPendingRequests();
          CHECK (1 == watch(manager).numAllocs());
          CHECK (1e6 > watch(manager).bytesLeased());
          manager.supply (alloc);
          manager.processPendingRequests();
          CHECK (0 == watch(manager).numAllocs());
          CHECK (0 == watch(manager).bytesLeased());
        }
      
      
      
      /** @test show a working allocation request passed over thread boundaries...
       *      - use a simplified »local pool« that actually just provides an in-queue
       *      - start a lot of threads that perform the typical allocation sequence
       *        + first send an asynchronous allocation request
       *        + some delay while doing other stuff (here: sleep)
       *        + check if allocation has been serviced
       *        + if not, then perform global allocation servicing in this worker
       *        + allocation must be available now, can use it
       *        + when done, send allocation back to the global pool
       */
      void
      verify_asyncRequest()
        {
            struct LocalTestPool
              : AllocReceiver
              {
                Alloc
                retrieveAlloc()
                  {
                    Alloc received{nullptr, 0};
                    inQueue_.pop (received);     // Note: no-op if queue is empty
                    return received;
                  }
                
                bool
                empty()  const
                  {
                    return inQueue_.empty();
                  }
              };
          
          EngineBufferManager manager;
          
          using WorkBuff = std::array<uint64_t, ALLOC_REQ / sizeof(uint64_t)>;
          
          SyncBarrier afterThread{NUM_THREADS+1};
          auto workSeq = [&]{ // sequence-of-actions-within-worker-thread-------------------
                              LocalTestPool myPool;
                              CHECK (myPool.empty());

                              manager.async_requestAllocation (myPool, ALLOC_REQ);

                              // meanwhile the worker does some other "work"...
                              auto gen = makeRandGen(); // local random generator per thread
                              uint delay = 100 + gen.i(900);
                              sleep_for (microseconds(delay));
                              
                              Alloc alloc = myPool.retrieveAlloc();
                              if (alloc.empty())
                                { // they let us down ...
                                  // insist harder to be serviced thusly
                                  manager.processPendingRequests();
                                  alloc = myPool.retrieveAlloc();
                                  // Explanation: since the async_requestAllocation() »happens-before« the processPendingRequests()
                                } //              our request *must* have been processed and dispatched now, unless there was a failure
                              
                              CHECK (not alloc.empty());
                              CHECK (ALLOC_REQ <= alloc.siz);
                              // can use that allocation for some "serious work"...
                              auto content = new(alloc.mem) WorkBuff;
                              std::ranges::generate (*content, [&]{ return gen.u64(); });
                              
                              // coordinate end of all threads
                              afterThread.sync();
                              
                              // pass allocation back to the central hub
                              manager.supply (alloc);
                              CHECK (myPool.empty()); // no further pending / duplicate Alloc in-queue
                            };
          
          // Start a collection of workers....
          using Threads = lib::ScopedCollection<Thread>;
          Threads threads{NUM_THREADS, Threads::fill(workSeq)};
          
          // after that barrier, all threads send back their allocation
          afterThread.sync();
          
          // wait for all threads to finish...
          while (explore(threads).has_any())
            yield();
          
          // all the allocations sent back by the workers
          // should sit now in the EngineBufferManager's input queue
          CHECK (NUM_THREADS           == watch(manager).numAllocs());
          CHECK (NUM_THREADS*ALLOC_REQ <= watch(manager).bytesLeased());
          
          // process in-queue and retrieve all allocations..
          manager.processPendingRequests();
          CHECK (0 == watch(manager).bytesLeased());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (EngineBufferManager_test, "unit engine");
  
  
  
}}} // namespace vault::mem::test
