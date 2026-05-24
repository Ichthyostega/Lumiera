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
#include "vault/mem/local-mem-pool.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/iter-stack.hpp"
#include "lib/format-util.hpp"
#include "test/diagnostic-output.hpp"////////////TODO

#include <concepts>

using util::join;
using lib::explore;
using std::invocable;

namespace vault {
namespace mem   {
namespace test  {
  
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
   *     - demonstrate usage of the API
   *     - show how allocations are selected and discarded
   * @remark this covers the mechanics how allocation entries are
   *       managed in the pool, without any infrastructure and
   *       notably without the connection to a global manager.
   */
  class LocalMemPool_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUse();
          verify_matchAlloc();
          verify_selectAlloc();
          verify_turnoverTrigger();
        }
      
      
      
      /** @test show how to place an allocation into the pool */
      void
      simpleUse()
        {
          LocalMemPool pool;
          pool.supply (MEM1, SIZ20);
          CHECK (pool.canServe(SIZ20));
          CHECK (pool.canServe(SIZ10));
          
          auto [mem,siz] = pool.retrieve (SIZ10);
          CHECK (MEM1 == mem);
          CHECK (SIZ20 == siz);
          CHECK (not pool.canServe(SIZ10));
          
          pool.reSupply (MEM1);
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
          pool.supply (MEM1, SIZ10);
          CHECK (watch(pool).cnt(SIZ10) == 0);                 // new allotment not ingested yet,....
          CHECK (not watch(pool).isEmpty());                   // however, there is an entry in the in-queue
          
          CHECK (pool.canServe(SIZ10));                        // query causes a scan and retrieval from the in-queue
          CHECK (watch(pool).cnt(SIZ10) == 1);
          CHECK (not watch(pool).isEmpty());
          
          pool.supply (MEM2, SIZ20);
          pool.supply (MEM3, SIZ30);
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
          pool.reSupply (m2);
          pool.reSupply (m1);
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
          pool.supply (MEM3, SIZ30);
          CHECK (watch(pool).size()     == 1);
          CHECK (    pool.canServe(SIZ30));
          CHECK (watch(pool).size()     == 2);
          
          cnt = 0;
          cnt = pool.cleanup(1.0 ,returnAlloc);                // complete clean-up requested
          CHECK (cnt == 2);
          CHECK (join(returned) == "30, 20, 30, 10"_expect);
          CHECK (watch(pool).isEmpty());
        }
      
      
      
      /** @test the pool picks and discards allocations heuristically:
       *      - allocations are only marked as reserved when they are
       *        pretty close above the requested amount of memory
       *      - however, all allocations in the pool are used
       *        to handle requests, irrespective of reservation status
       *      - the prime criterion to select an entry is the match quality
       *      - yet frequently used blocks are preferred for a rather poor match
       *      - each usage adds to the score, but better matches score higher
       *      - but when an entry is too small to be used, it is marked with a penalty
       *      - entries with low score are cleaned and returned first, while
       *        successful entries with high score are retained
       */
      void
      verify_selectAlloc()
        {
          LocalMemPool pool;
          pool.supply (MEM11, SIZ10);
          pool.supply (MEM31, SIZ30);
          pool.supply (MEM32, SIZ30);
          pool.supply (MEM33, SIZ30);
          
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
          CHECK (m1 == MEM11);                                 // the first request gets the good match
          CHECK (m2 == MEM33);                                 // the following ones are satisfied with an over-allocation
          CHECK (m3 == MEM32);
          CHECK (not m4);
          CHECK (watch(pool).cntFree()  == 1);
          CHECK (watch(pool).size()     == 4);
          
          // return all blocks...
          pool.reSupply (m1);
          pool.reSupply (m2);
          pool.reSupply (m3);
          VERIFY_FAIL ("unknown allocation", pool.reSupply (m4))
          VERIFY_FAIL ("unknown allocation", pool.reSupply (fake_Buffer(12345)))
          VERIFY_FAIL ("not marked as used", pool.reSupply (MEM31))
          CHECK (watch(pool).cntFree()  == 4);
          
          // all entries in the pool were scored....
          CHECK (watch(pool).getScore(MEM31) == -2);           // never used and -2 penalty for being once to small to be useful (for SIZ20+SIZ20)
          CHECK (watch(pool).getScore(MEM32) ==  3);           // these were used once, but as over-allocation using only 1/3 of their size SIZ30
          CHECK (watch(pool).getScore(MEM33) ==  3);
          CHECK (watch(pool).getScore(MEM11) == 10);           // this one was used as a perfect match and thus got the full score
          
          // this score determines how they are used further....
          pool.supply (MEM12, SIZ10);
          auto [m5,s5] = pool.retrieve (SIZ10-5);
          auto [m6,s6] = pool.retrieve (SIZ10);
          auto [m7,s7] = pool.retrieve (SIZ10);
          auto [m8,s8] = pool.retrieve (SIZ20);
          auto [m9,s9] = pool.retrieve (SIZ20);
          CHECK (m5 == MEM11);                                 // used first because it matches and has the best score
          CHECK (m6 == MEM12);                                 // used next because it's a perfect match
          CHECK (m7 == MEM33);                                 // this is a bad match, but it's the one left with the largest score
          CHECK (m8 == MEM32);                                 // the next best score is used to satisfy this request for SIZ20
          CHECK (m9 == MEM31);                                 // only one left (which has the worst score because it was never used yet)
          CHECK (watch(pool).cntFree()  == 0);
          CHECK (watch(pool).size()     == 5);
          
          pool.reSupply (m8);
          pool.reSupply (m7);
          pool.reSupply (m6);
          pool.reSupply (m5);
          CHECK (watch(pool).cntFree()  == 4);
          CHECK (watch(pool).getScore(MEM11) == 15);           // got +5 score because the match quality was only 50% (SIZ10 but only 5 needed)
          CHECK (watch(pool).getScore(MEM12) == 10);           // this one was new, but got a +10 score due to the perfect match
          CHECK (watch(pool).getScore(MEM31) ==  4);           // this had -1 score but got +6 due to 60% match quality (SIZ30 but SIZ20 requested)
          CHECK (watch(pool).getScore(MEM32) ==  9);           // previous score was +3 and it likewise got +6
          CHECK (watch(pool).getScore(MEM33) ==  6);           // this had also +3 and was first in the list, but got only +3 (SIZ30 used to satisfy SIZ10)
          
          // entries removed from pool...
          lib::IterQueue<size_t> returned;
          auto returnAlloc = [&](Buff* mem, size_t){ returned.feed (size_t(mem)); };
          
          // perform a rather aggressive clean-up
          cnt = pool.cleanup(0.7 ,returnAlloc);                // max score is 15, so anything below 0.7*15 ≡ 10 will be expunged
          CHECK (cnt == 3);
          CHECK (join(returned) == "12, 33, 32"_expect);       // MEM12, MEM33 and MEM32 returned, since their score was <= 10
          CHECK (watch(pool).cntFree()  == 1);
          CHECK (watch(pool).size()     == 2);
          
          CHECK (not watch(pool).isFree(MEM31));               // Note: MEM31 is still in use and thus remains in pool, even with that low score of 4
          
          CHECK (watch(pool).getScore(MEM11) ==  5);           // Scores were reduced accordingly, cutting away a the »kill level« of +10
          CHECK (watch(pool).getScore(MEM31) == -6);
          
          pool.reSupply (MEM31);
          auto [mX,sX] = pool.retrieve (SIZ20);
          CHECK (mX == MEM31);
          CHECK (sX == SIZ30);
          pool.reSupply (MEM31);
          CHECK (watch(pool).cntFree()  == 2);
          CHECK (watch(pool).getScore(MEM11) ==  3);           // was penalised with -2 since MEM11 was too small to satisfy a SIZ20 request
          CHECK (watch(pool).getScore(MEM31) ==  0);           // while MEM31 got a score of +6 for a usage with 60% match quality (SIZ30 used for SIZ20)
          
          // shut down the pool...
          cnt = pool.cleanup(1.0 ,returnAlloc);
          CHECK (cnt == 2);
          CHECK (join(returned) == "12, 33, 32, 31, 11"_expect);
          CHECK (pool.empty());
        }
      
      
      
      /** @test heuristic trigger mechanism for pool clean-up
       *      - use a setup where the global pool is just a queue of buffer pointers
       *      - create abbreviated notation to play through various scenarios
       *      - watch how turnover and max-score are computed for each step
       *      - document the trigger points for heuristic clean-up.
       */
      void
      verify_turnoverTrigger()
        {
          LocalMemPool pool;
          lib::IterQueue<Buff*> globalPool;
          lib::IterStack<Buff*> usedBuff;
          size_t SIZ = SIZ10;
          
          //------------ pretty printing of "allocations"
          auto buffID  = [](Buff* buff){ return size_t(buff); };
          auto show    = [&](auto coll){ return join (explore(coll).transform(buffID)); };
          
          
          //------------ define abstracted form of the basic actions on the pool...
          
          auto pullMem = [&]{ // retrieve one allocation from global pool
                              if (globalPool)
                                {
                                  pool.supply (*globalPool, SIZ);
                                  pool.ingest();
                                  ++globalPool;
                                }
                            };
          
          auto pushMem = [&](Buff* mem, size_t)
                            { // return one allocation into global pool
                              globalPool.feed (mem);
                            };
          
          auto useMem  = [&]{ // claim one allocation from local pool
                              auto [mem,_] = pool.retrieve (SIZ);
                              usedBuff.push(mem);
                            };
          
          auto freeMem = [&]{ // return one used allocation into local pool
                              if (usedBuff)
                                {
                                  pool.reSupply (*usedBuff);
                                  ++usedBuff;
                                }
                            };
          
          auto repeat  = [](uint n, invocable<> auto action)
                            {
                              for (uint i=0; i<n; ++i)
                                action();
                            };
          
          
          //------------ define four building blocks for scenarios
          
          auto reserve = [&](uint n){ pool.reserve(n, SIZ); };
          auto supply  = [&](uint n){ repeat(n, pullMem);   };
          auto claim   = [&](uint n){ repeat(n, useMem );   };
          auto free    = [&](uint n){ repeat(n, freeMem);   };
          
          // populate the global pool with some "allocations"
          globalPool.feed(MEM11)
                    .feed(MEM12)
                    .feed(MEM13)
                    .feed(MEM21)
                    .feed(MEM31)
                    .feed(MEM32)
                    .feed(MEM33);
          
SHOW_EXPR(show(globalPool));
SHOW_EXPR(watch(pool).size())
          
          supply(3);
          
SHOW_EXPR(show(globalPool));
SHOW_EXPR(watch(pool).size())
SHOW_EXPR(watch(pool).cntFree())
          
          reserve(2);
          
SHOW_EXPR(watch(pool).size())
SHOW_EXPR(watch(pool).cntFree())
SHOW_EXPR(watch(pool).cntResd())
          
          claim(3);
          
SHOW_EXPR(watch(pool).size())
SHOW_EXPR(watch(pool).cntFree())
SHOW_EXPR(watch(pool).cntResd())
SHOW_EXPR(show(usedBuff));
          
          free(3);
          
SHOW_EXPR(watch(pool).size())
SHOW_EXPR(watch(pool).cntFree())
SHOW_EXPR(watch(pool).cntResd())
SHOW_EXPR(show(usedBuff));
          
          pool.yield (3, SIZ, pushMem);
          
SHOW_EXPR(watch(pool).size())
SHOW_EXPR(show(globalPool));
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (LocalMemPool_test, "unit engine");
  
  
  
}}} // namespace vault::mem::test
