/*
  LocalMemPool(Test)  -  verify low-level handling of buffer blocks in a local pool

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file local-mem-pool-test.cpp
 ** unit test \ref LocalMemPool_test
 */


#include "test/run.hpp"
#include "test/test-helper.hpp"
//#include "vault/mem/engine-buffer-metadata.hpp"
#include "vault/mem/local-mem-pool.hpp"
#include "lib/iter-stack.hpp"
//#include "lib/depend-inject.hpp"
//#include "lib/thread.hpp"
//#include "lib/error.hpp"
#include "lib/format-util.hpp"
#include "test/diagnostic-output.hpp"


//using std::this_thread::yield;
//using lib::Thread;
using util::join;

namespace vault {
namespace mem   {
namespace test  {
  
//  using LERR_(LOGIC);
//  using LERR_(LIFECYCLE);
  
  
  namespace { // Test helper
    
    inline constexpr Buff*
    fake_Buffer(size_t id)
      {
        return reinterpret_cast<Buff*> (id);
      }
    
    Buff* const MEM1 = fake_Buffer(1);
    Buff* const MEM2 = fake_Buffer(2);
    Buff* const MEM3 = fake_Buffer(3);
    
    const size_t SIZ10{10};
    const size_t SIZ20{20};
    const size_t SIZ30{30};

    Buff* const MEM11 = fake_Buffer(11);
    Buff* const MEM12 = fake_Buffer(12);
    Buff* const MEM13 = fake_Buffer(13);
    Buff* const MEM21 = fake_Buffer(21);
    Buff* const MEM31 = fake_Buffer(31);
    Buff* const MEM32 = fake_Buffer(32);
    Buff* const MEM33 = fake_Buffer(33);
}
  
  
  
  
  
  
  /**********************************************************************//**
   * @test verify handling of prospective buffer allocations in a local pool.
   */
  class LocalMemPool_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          
          simpleUse();
          verify_matchAlloc();
          verify_selectAlloc();
          verify_pruneAlloc();
        }
      
      
      
      /** @test show how to place an allocation into the pool */
      void
      simpleUse()
        {
          LocalMemPool pool;
          pool.add (MEM1, SIZ20);
          CHECK (pool.canServe(SIZ20));
          CHECK (pool.canServe(SIZ10));
          
          auto [mem,siz] = pool.retrieve (SIZ10);
          CHECK (MEM1 == mem);
          CHECK (SIZ20 == siz);
          CHECK (not pool.canServe(SIZ10));
          
          pool.reAdd (MEM1);
          CHECK (pool.canServe(SIZ20));
        }
      
      
      
      /** @test allocations are accounted by size
       *      - new memory is posted through an in-queue
       *      - for each request, the closest matching allocation is handed out
       *      - a partial clean-up removes unused / bad matching allocations first
       *      - a complete clean-up clears out the pool
       */
      void
      verify_matchAlloc()
        {
          LocalMemPool pool;
          CHECK (watch(pool).isEmpty());
          
          // Feed new allocations through the in-queue...
          pool.add (MEM1, SIZ10);
          CHECK (watch(pool).cnt(SIZ10) == 0);                 // new allotment not ingested yet,....
          CHECK (not watch(pool).isEmpty());                   // however, there is an entry in the in-queue
          
          CHECK (pool.canServe(SIZ10));                        // query causes a scan and retrieval from the in-queu
          CHECK (watch(pool).cnt(SIZ10) == 1);
          CHECK (not watch(pool).isEmpty());
          
          pool.add (MEM2, SIZ20);
          pool.add (MEM3, SIZ30);
          CHECK (pool.canServe(SIZ20));
          CHECK (pool.canServe(SIZ10));
          CHECK (watch(pool).size()     == 3);
          CHECK (watch(pool).cnt(SIZ10) == 1);
          CHECK (watch(pool).cnt(SIZ20) == 1);
          CHECK (watch(pool).cnt(SIZ30) == 1);
          CHECK (watch(pool).cntFree()  == 3);
          
          // Request some allocations from the pool....
          auto [m1,s1] = pool.retrieve (SIZ10 -2);
          CHECK (SIZ10 == s1);
          CHECK (MEM1 == m1);
          CHECK (watch(pool).cntFree()  == 2);
          auto [m2,s2] = pool.retrieve (SIZ20);
          CHECK (SIZ20 == s2);
          CHECK (MEM2 == m2);
          CHECK (watch(pool).size()     == 3);
          CHECK (watch(pool).cntFree()  == 1);
          CHECK (pool.canServe(SIZ10));                        // only the SIZ30-allocation is available from the pool
          CHECK (pool.canServe(SIZ20));                        // which however could serve also smaller requests...
          CHECK (pool.canServe(SIZ30));
          
          // Return allocations back into the pool
          pool.reAdd (m2);
          pool.reAdd (m1);
          CHECK (watch(pool).cntFree()  == 3);
          
          // Shrink / clean-up will return allocations through a consumer
          lib::IterQueue<size_t> returned;
          auto returnAlloc = [&](auto, size_t siz){ returned.feed (siz); };
          
          uint cnt = pool.cleanup(0.5 ,returnAlloc);           // heuristic partial clean-up
          CHECK (cnt == 1);
          CHECK (join(returned) == "30"_expect);               // the SIZ30 allocation was not used yet, and thus returned first
          CHECK (watch(pool).cntFree()  == 2);
          CHECK (watch(pool).size()     == 2);
          
          cnt = 0;
          cnt = pool.yield(2, SIZ20, returnAlloc);             // explicit request to return two SIZ20 allocations...
          CHECK (cnt == 1);                                    // yet there is only one such block in the pool
          CHECK (join(returned) == "30, 20"_expect);
          CHECK (watch(pool).cnt(SIZ10) == 1);
          CHECK (watch(pool).cnt(SIZ20) == 0);
          CHECK (watch(pool).cnt(SIZ30) == 0);
          CHECK (watch(pool).cntFree()  == 1);
          CHECK (watch(pool).size()     == 1);
          
          // add a further allocation with SIZ30
          CHECK (not pool.canServe(SIZ30));
          pool.add (MEM3, SIZ30);
          CHECK (watch(pool).size()     == 1);
          CHECK (    pool.canServe(SIZ30));
          CHECK (watch(pool).size()     == 2);
          
          cnt = 0;
          cnt = pool.cleanup(1.0 ,returnAlloc);                // complete clean-up requested
          CHECK (cnt == 2);
          CHECK (join(returned) == "30, 20, 30, 10"_expect);
          CHECK (watch(pool).isEmpty());
        }
      
      
      
      /** @test the pool picks allocations heuristically
       */
      void
      verify_selectAlloc()
        {
          LocalMemPool pool;
          pool.add (MEM11, SIZ10);
          pool.add (MEM31, SIZ30);
          pool.add (MEM32, SIZ30);
          pool.add (MEM33, SIZ30);
          
          uint cnt = pool.reserve(4, SIZ10);                   // request to reserve 4 blocks of SIZ10
          CHECK (cnt == 1);                                    // yet only one block is a good match and thus reserved
          cnt = pool.reserve(2, SIZ30-5);
          CHECK (cnt == 2);                                    // these two reservation requests can be satisfied
          cnt = pool.reserve(2, SIZ10);
          CHECK (cnt == 0);                                    // but the pool is now unable to reserve more SIZ10 (without wasting memory)
          
          auto [m1,s1] = pool.retrieve (SIZ10);                // actual allocation requests are always served however, if possible at all
          auto [m2,s2] = pool.retrieve (SIZ10);
          auto [m3,s3] = pool.retrieve (SIZ10);
          auto [m4,s4] = pool.retrieve (SIZ20+SIZ20);          // this one can certainly not be satisfied
          CHECK (MEM11 == m1);                                 // the first request gets the good match
          CHECK (MEM33 == m2);                                 // the following ones are satisfied with an over-allocation
          CHECK (MEM32 == m3);
          CHECK (not m4);
          CHECK (watch(pool).cntFree()  == 1);
          CHECK (watch(pool).size()     == 4);
          
          // return all blocks...
          pool.reAdd (m1);
          pool.reAdd (m2);
          pool.reAdd (m3);
          VERIFY_FAIL ("unknown allocation", pool.reAdd (m4))
          VERIFY_FAIL ("unknown allocation", pool.reAdd (fake_Buffer(12345)))
          VERIFY_FAIL ("not marked as used", pool.reAdd (MEM31))
          CHECK (watch(pool).cntFree()  == 4);
SHOW_EXPR(join(watch(pool).allScores()))
        }
      
      
      
      /** @test 
       */
      void
      verify_pruneAlloc()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (LocalMemPool_test, "unit engine");
  
  
  
}}} // namespace vault::mem::test
