/*
  BufferProviderLoad(Test)  -  cover the interface to a central buffer manager

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file engine-buffer-manager-test.cpp
 ** unit test \ref BufferProviderLoad_test
 */


#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "vault/mem/engine-buffer-manager.hpp"
#include "vault/mem/engine-buffer-allocator.hpp"
#include "vault/gear/test-chain-load.hpp"
#include "lib/scoped-collection.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/sync-barrier.hpp"
#include "lib/thread.hpp"
#include "lib/symbol.hpp"
#include "lib/random.hpp"

#include <algorithm>
#include <string>
#include <array>

using lib::explore;
using lib::Random;
using lib::Literal;
using lib::Thread;
using lib::SyncBarrier;
using vault::gear::test::ComputationalLoad;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using std::chrono::microseconds;
using std::chrono_literals::operator ""us;
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
  class BufferProviderLoad_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          demonstrate_EngineSetup();
          verify_massiveOverload();
          verify_controlledLoad();
        }
      
      
      
      /** @test show a setup of the buffer manager that is
       *        structurally equivalent to the Render Engine.
       */
      void
      demonstrate_EngineSetup()
        {
        }
      
      
      
      struct DefaultSetup
        {
          static constexpr uint NUM_JOBS = 10;
          static constexpr size_t BASE_BUFFSIZ = 100_KiB;
          
          /// local random generator per thread
          Random rand_;
          ComputationalLoad load_;
          
          DefaultSetup()
            : rand_{makeRandGen()}
            {
              load_.timeBase = 1ms;
              load_.sizeBase = BASE_BUFFSIZ;
              load_.useAllocation = true;
            }
        };
      
      /**
       * Emulation of the load produced by a worker thread
       */
      template<class CONF = DefaultSetup>
      class DummyWorkerThread
        : CONF
        , public Thread
        {
          
          
          void
          workActivity()
            {
              for (uint i=0; i<CONF::NUM_JOBS; ++i)
                {
                  uint delay = CONF::rand_.i(1000);
                  sleep_for (microseconds(delay));
                  jobActivity();
                }
            }
          
          void
          jobActivity()
            {
              ///////OOO Idea : use an IterQueue of BuffHandles, fill these by a random-walk
            }
          
        public:
          DummyWorkerThread()
            : Thread{"Load-Test-Worker"
                    ,[this]{ workActivity(); }}
            { }
        };
      
      
      /** @test 
       * 
       */
      void
      verify_massiveOverload()
        {
        }
      
      
      
      /** @test 
       * 
       */
      void
      verify_controlledLoad()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BufferProviderLoad_test, "unit engine");
  
  
  
}}} // namespace vault::mem::test
