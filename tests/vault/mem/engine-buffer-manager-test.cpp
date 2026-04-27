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
//#include "vault/mem/engine-buffer-metadata.hpp"
#include "vault/mem/engine-buffer-manager.hpp"
//#include "lib/depend-inject.hpp"
//#include "lib/thread.hpp"
//#include "lib/error.hpp"
#include "test/diagnostic-output.hpp"////////////TODO


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
  
  
  
  
  
  
  /*****************************************************************************//**
   * @test Show how the central buffer manager of the Render Engine can be accessed.
   */
  class EngineBufferManager_test : public Test
    {
      
      virtual void
      run (Arg)
        {
//          seedRand();
          
          demonstrate_AllocatorInterface();
          verify_DataTransfer();
          verify_lulz();
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
          using Allo = EngineBufferManager::BufferAllocator;
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
  LAUNCHER (EngineBufferManager_test, "unit engine");
  
  
  
}}} // namespace vault::mem::test
