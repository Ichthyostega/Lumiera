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
//#include "lib/depend-inject.hpp"
//#include "lib/thread.hpp"
//#include "lib/error.hpp"


//using std::this_thread::yield;
//using lib::Thread;

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
       */
      void
      verify_matchAlloc()
        {
          LocalMemPool pool;
          CHECK (watch(pool).isEmpty());
          
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
          CHECK (pool.size() == 3);
          CHECK (watch(pool).cnt(SIZ10) == 1);
          CHECK (watch(pool).cnt(SIZ20) == 1);
          CHECK (watch(pool).cnt(SIZ30) == 1);
          CHECK (watch(pool).cntFree()  == 3);
          
          auto [m1,s1] = pool.retrieve (SIZ20 -2);
          CHECK (SIZ20 == s1);
          CHECK (MEM2 == m1);
          CHECK (watch(pool).cntFree()  == 2);
          auto [m2,s2] = pool.retrieve (SIZ10);
          CHECK (SIZ10 == s2);
          CHECK (MEM1 == m2);
          CHECK (watch(pool).cntFree()  == 1);
          CHECK (pool.canServe(SIZ10));
          CHECK (pool.canServe(SIZ20));
          CHECK (pool.canServe(SIZ30));
        }
      
      
      
      /** @test 
       */
      void
      verify_selectAlloc()
        {
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
