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
  
  
  
  
  
  
  /**************************************************************************//**
   * @test verify worker memory handling that relies on a thread-local allocator.
   */
  class LocalBufferStore_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          
          verify_DataTransfer();
          verify_lulz();
        }
      
      
      
      /** @test  */
      void
      verify_DataTransfer()
        {
        }
      
      
      
      /** @test 
       */
      void
      verify_lulz()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (LocalBufferStore_test, "unit engine");
  
  
  
}}} // namespace vault::mem::test
