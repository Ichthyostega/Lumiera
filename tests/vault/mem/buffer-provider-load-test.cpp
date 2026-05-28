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
#include "vault/mem/buffer-provider-setup.hpp"
#include "vault/mem/engine-buffer-metadata.hpp"
#include "vault/mem/engine-buffer-manager.hpp"
#include "vault/mem/local-buffer-stage.hpp"
#include "vault/mem/local-buffer-store.hpp"
#include "vault/gear/test-chain-load.hpp"
#include "lib/scoped-collection.hpp"
#include "lib/depend-inject.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/sync-barrier.hpp"
#include "lib/iter-stack.hpp"
#include "lib/thread.hpp"
#include "lib/symbol.hpp"
#include "lib/random.hpp"
#include "test/diagnostic-output.hpp"////////////TODO

#include <algorithm>
#include <string>
#include <array>

using lib::explore;
using lib::Depend;
using lib::DependInject;
using lib::Random;
using lib::Literal;
using lib::Thread;
using lib::SyncBarrier;
using lib::ScopedCollection;
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
    const size_t NUM_THREADS = 100;       ///< press concurrently
    
    const size_t MAX_SCALE = 5;           ///< maximum scale step for randomised resources
    
    
    /** seed a per-thread random generator */
    inline Random
    makeRandGen()
    {
      return Random{lib::seedFromDefaultGen()};
    }
    
    class MultithreadBufferSetup
      : public BufferProviderSetup
      {
      public:
        MultithreadBufferSetup()
          : BufferProviderSetup{*this}
          { }
          
          auto buildStage() { return std::make_unique<LocalBufferStage>("BufferProviderLoad_test"); }
          auto buildStore() { return std::make_unique<LocalBufferStore>(); }
      };
    
    
    using BuffTypes = ScopedCollection<BuffDescr>;
    
    /**
     * Mock dependency injection context
     * to emulate a Render Engine setup
     */
    struct Ctx
      {
        static Depend<MultithreadBufferSetup> bufferProvider;
        static BuffTypes buffType_;
        
        // install mock instances for the relevant services
        DependInject<EngineBufferMetadata>::Local<EngineBufferMetadata> metaHub;
        DependInject<EngineBufferManager>::Local<EngineBufferManager>  globalPool;
        DependInject<BufferProvider>::Local<MultithreadBufferSetup>    frontEnd;
      };
    
    // static storage for the mock-engine entrance point
    Depend<MultithreadBufferSetup> Ctx::bufferProvider;
    BuffTypes Ctx::buffType_{MAX_SCALE};
  }
  
  
  
  
  struct WorkSetup
    {
      static constexpr uint NUM_JOBS = 10;
      static constexpr size_t BASE_BUFFSIZ = 100_KiB;
      
      /// local random generator per thread
      Random rand_;
      ComputationalLoad load_;
      
      WorkSetup()
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
  template<class CONF = WorkSetup>
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
SHOW_EXPR(delay)
              sleep_for (microseconds(delay));
              jobActivity();
            }
        }
      
      void
      jobActivity()
        {
          ///////OOO Idea : use an IterQueue of BuffHandles, fill these by a random-walk
          static uint ivo{0};
SHOW_EXPR(++ivo)
        }
      
      
      void
      registerBufferTypes()
        {
          if (Ctx::buffType_.empty()
              or Ctx::buffType_[0].buffSize() != CONF::BASE_BUFFSIZ )
            {
              Ctx::buffType_.clear();
              Ctx::buffType_.populate_by(
                BuffTypes::invoke(
                  [&, i=0]() mutable
                    {
                      return Ctx::bufferProvider().getDescriptorFor (++i * CONF::BASE_BUFFSIZ);
                    }));
              
            }
        }
    public:
      DummyWorkerThread()
        : Thread{"Load-Test-Worker"
                ,[this]{ workActivity(); }}
        {
          registerBufferTypes();
        }
    };
  
  
  
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
          Ctx mockEngine;
          
          DummyWorkerThread worker;
          
          while (worker)
            sleep_for (200us);
          
SHOW_EXPR("hurgha")
SHOW_EXPR(bool(mockEngine.globalPool))
SHOW_EXPR(bool(mockEngine.metaHub))
SHOW_EXPR(bool(mockEngine.frontEnd))
        }
      
      
      
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
