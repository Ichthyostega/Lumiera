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
    
//    template<typename X>
//    Buff*
//    mark_as_Buffer(X& something)
//      {
//        return reinterpret_cast<Buff*> (std::addressof(something));
//      }
//    
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
          
          verify_blah();
          verify_lolz();
        }
      
      
      
      /** @test  */
      void
      verify_blah()
        {
        }
      
      
      
      /** @test 
       */
      void
      verify_lolz()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (LocalMemPool_test, "unit engine");
  
  
  
}}} // namespace vault::mem::test
