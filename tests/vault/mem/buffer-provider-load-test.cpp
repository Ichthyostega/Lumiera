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
#include "test/microbenchmark.hpp"
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
#include "lib/random.hpp"

using lib::Random;
using lib::Depend;
using lib::DependInject;
using lib::explore;
using lib::Thread;
using lib::SyncBarrier;
using lib::ScopedCollection;
using test::benchmarkTime;
using vault::gear::test::ComputationalLoad;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using std::chrono::microseconds;
using std::chrono_literals::operator ""us;
using std::chrono_literals::operator ""ms;

namespace vault {
namespace mem   {
namespace test  {
  
  namespace { // ========== Test Setup ==========
    
    const size_t FULL_LOAD_CORES = 4;     ///< workers to use for a _full load run_
    const size_t MAX_SCALE = 5;           ///< maximum scale step for randomised buffer types
    
    size_t MANY_THREADS     = 100;        ///< press concurrently with an overload of threads
    const size_t NOT_SO_MANY = 16;        ///< a _light variant_ for use in the standard test suite
    
    /** seed a per-thread random generator */
    inline Random
    chainedRandGen()
    {
      return Random{lib::seedFromDefaultGen()};
    }
    
    /**
     * BufferProvider implementation
     * configured similarly to the real Render Engine.
     * Using a distinct BufferStage and BufferStore (local pool)
     * for each new thread, together with the global EngineBufferMetadata
     * and EngineBufferManager, accessed through lib::Depend.
     */
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
    
    
    /** use a common selection of pre-configured buffer sizes */
    using BuffTypes = ScopedCollection<BuffDescr>;
    
    /**************************************************//**
     * Mock dependency injection context
     * to emulate a Render Engine setup
     * @remarks
     *  - MultithreadedBufferSetup is only used for this test
     *    and creates per-thread implementation backend instances
     *  - during the lifetime of such a Ctx instance in the local scope,
     *    the global EngineBufferMetadata- and EngineBufferManager-Singletons
     *    are shadowed by pristine new instances created temporarily
     */
    struct Ctx
      {
        static Depend<MultithreadBufferSetup> bufferProvider;
        static BuffTypes buffType_;
        static size_t calibrated_;
        
        // install mock instances for the relevant services
        DependInject<EngineBufferMetadata>::Local<EngineBufferMetadata> metaHub;
        DependInject<EngineBufferManager>::Local<EngineBufferManager>  globalPool;
      };
    
    // static storage for the mock-engine entrance point
    Depend<MultithreadBufferSetup> Ctx::bufferProvider;
    BuffTypes Ctx::buffType_{MAX_SCALE};
    size_t Ctx::calibrated_{0};
  }
  
  
  
  
  /* ============= Simulated Worker Load ============= */
  
  struct WorkSetup
    {
      static constexpr uint NUM_JOBS = 10;     ///< number of work-jobs to perform in this thread
      static constexpr uint JOB_STEPS = 10;    ///< number of processing steps in a single job
      static constexpr uint STEP_SPREAD = 4;   ///< max random spread of allocations / deallocations
      static constexpr uint LOAD_SPREAD = 5;   ///< max spread of »buffer types« to pick
      static constexpr uint MAX_DELAY = 1000;  ///< maximum delay in µs between jobs
      
      static constexpr size_t BASE_BUFFSIZ = 100_KiB;  ///< @note scaled by sizeof(size_t) * LOAD_SPREAD
    };
  
  /**
   * Simulation of the load produced by a worker thread.
   * Performs a sequence of work actions, requesting
   * buffer memory an causing computational load.
   */
  template<class CONF = WorkSetup>
  class DummyWorkerThread
    : CONF
    , SyncBarrier
    , public Thread
    {
      ComputationalLoad load_;
      /// local random generator per thread
      Random rand_;
      
      /**********//**
       * Thread-Main
       */
      void
      workActivity()
        {
          SyncBarrier::sync();
          
          for (uint i=0; i<CONF::NUM_JOBS; ++i)
            {
              uint delay = rand_.i (CONF::MAX_DELAY);
              sleep_for (microseconds(delay));
              
              jobActivity();
            }
        }
      
      /**
       * Simulated calculation job.
       * A random-walk-like memory load pattern is generated
       * by claiming and releasing a random number of buffers, and picking one
       * of the preconfigured »buffer types« (size) randomly. The corresponding
       * BuffHandle(s) are passed through a queue; the head element from that queue
       * is used to generate a combined CPU and memory load in each step.
       * Deliberately, the pre-announced capacity does not exactly fit the generated
       * load, so that constant rebalancing between local and global pool is provoked.
       * @remark this test attempts to be _nasty_ — because the goal is to
       *         demonstrate correct behaviour under pressure.
       */
      void
      jobActivity()
        {
          lib::IterQueue<BuffHandle> buffers;
          
          auto randomAlloc = [&]{
                                  uint t = rand_.i (CONF::LOAD_SPREAD);
                                  uint c = rand_.i (CONF::STEP_SPREAD);
                                  for ( ; c>0; --c)
                                    buffers.feed (Ctx::buffType_[t].lockBuffer());
                                };
          auto randomFree  = [&]{
                                  uint c = rand_.i (CONF::STEP_SPREAD);
                                  for ( ; c>0; --c)
                                    if (buffers)
                                      {
                                        buffers->release();
                                        ++buffers;
                                      }
                                };
          auto randomWork  = [&]{
                                  ENSURE (buffers);
                                  size_t availSpace = buffers->size() / sizeof(size_t);
                                  uint scaleStep    = availSpace / CONF::BASE_BUFFSIZ;
                                  ENSURE (availSpace*sizeof(size_t) >= load_.requiredBuffSiz(scaleStep));
                                  load_.externalBuff = & buffers->accessAs<size_t>();
                                  
                                  // repeatedly add contents of buffer to produce computational load
                                  load_.invoke (scaleStep);
                                };
          
          
          //---Announce-some-random-capacity-beforehand------
          for (BuffDescr& buffType : Ctx::buffType_)
            buffType.announce (rand_.i (CONF::STEP_SPREAD));
          
          //---Process-sequence-of-work-steps-----
          for (uint i=0; i<CONF::JOB_STEPS; ++i)
            {
              do randomAlloc();
                while (buffers.empty());
              randomWork();
              randomFree();
            }
          //---Free-up-remaining-buffers--
          while (buffers)
            {
              buffers->release();
              ++buffers;
            }
        }
      
      
      /** one-time static pre-configuration of buffer types, shared by all threads.
       * @remark this emulates a similar behaviour of the Render Node graph, where
       *   embedded BuffDescr instances are established by the Builder for each node.
       *   Furthermore, ComputationalLoad is calibrated here, so that at scaleStep≡1
       *   the imposed delay turns out close to the configured timeBase of 1ms.
       *   As a special twist, this test will use the allocated buffer memory
       *   to perform the computation to generate the load -- similar to what
       *   happens with a real CPU based media computation in memory.
       */
      void
      registerBufferTypes()
        {
          auto isKnown = [](HashVal typeID)
                            { // deliberately check the central metadata store
                              // without accessing the local store of the main thread,
                              // to avoid carrying stale information beyond test case boundaries
                              // (the thread constructor runs in the main thread)
                              Depend<EngineBufferMetadata> metaHub;
                              return metaHub().isKnown (typeID);
                            };
          if (Ctx::buffType_.empty()
              or not isKnown (Ctx::buffType_[0]))
            {
              Ctx::buffType_.clear();
              Ctx::buffType_.populate_by(
                  BuffTypes::invoke(
                    [&, i=0]() mutable
                      { // preconfigure »buffer types« with suitable size
                        // as required by the Mem-method of ComputationalLoad
                        size_t siz = load_.requiredBuffSiz (++i);
                        return Ctx::bufferProvider().getDescriptorFor (siz);
                      }));
            }
          if (Ctx::calibrated_ != load_.sizeBase)
            {
              load_.calibrate();
              Ctx::calibrated_ = load_.sizeBase;
            }
        }
      
    public:
      DummyWorkerThread()                ///< @note this constructor runs in the main thread
        : Thread{"Load-Test-Worker"
                ,[this]{ workActivity(); }
                }
        , rand_{chainedRandGen()}
        {
          REQUIRE (CONF::LOAD_SPREAD <= MAX_SCALE);
          
          load_.timeBase = 1ms;
          load_.sizeBase = CONF::BASE_BUFFSIZ;
          load_.useAllocation = true;
          registerBufferTypes();
          
          SyncBarrier::sync(); // allow the workActivity() to commence now...
        }
    };
  
  
  
  
  /**************************************************************************//**
   * @test Integration test of the Render Engine buffer management subsystem.
   *     - this test uses a setup and wiring of components similar to the
   *       actual Render Engine, with thread-local memory pools for each
   *       worker, and a central buffer allocator and metadata hub.
   *     - a load pattern similar to the actual render workers is generated
   *       by the DummyWorkerThread instances, exhibiting a randomly fluctuating
   *       demand of buffer memory, while keeping the CPU(s) busy with a load
   *       of memory access and simple computations.
   *     - the prime goal is to demonstrate correctness, balancing memory demand
   *       mostly through lock-free queues, without loosing any allocation.
   */
  class BufferProviderLoad_test : public Test
    {
      
      virtual void
      run (Arg arg)
        {
          seedRand();
           if ("quick" == firstTok (arg))
             MANY_THREADS = NOT_SO_MANY;
          
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
          
          // the central services were indeed requested / created
          CHECK (mockEngine.globalPool);
          CHECK (mockEngine.metaHub);
          
          // 5 distinct buffer types registered
          CHECK (5 == mockEngine.metaHub->cntEntries());

          sleep_for (500us); // additional delay for the local pool's destructor
                            //  (which is invoked after the thread has terminated)

          // there must be some allocations, still sitting in the return queue
          CHECK (0 < watch(*mockEngine.globalPool).numAllocs());
          
          // process and close remaining allocations
          // sent back by the local pool's destructor...
          mockEngine.globalPool->processPendingRequests();
          // now there should be no memory leaks.......
          CHECK (0 == watch(*mockEngine.globalPool).bytesLeased());
        }
      
      
      
      /** @test press the same setup using a
       *        massive overload of concurrent workers
       */
      void
      verify_massiveOverload()
        {
          Ctx mockEngine;
          
          // Start a collection of workers....
          using Threads = ScopedCollection<DummyWorkerThread<>>;
          Threads threads{MANY_THREADS, Threads::fill()};
          
          // wait for all threads to finish...
          while (explore(threads).has_any())
            sleep_for (5ms);
          
          CHECK (mockEngine.globalPool);
          // again 5 distinct buffer types registered
          CHECK (5 == mockEngine.metaHub->cntEntries());
          CHECK (0 < watch(*mockEngine.globalPool).numAllocs());
          
          sleep_for (500us);
          mockEngine.globalPool->processPendingRequests();
          CHECK (0 == watch(*mockEngine.globalPool).bytesLeased());
        }
      
      
      
          /** worker configuration for seamless full load */
          struct FullLoadSetup : WorkSetup
            {
              static constexpr uint JOB_STEPS = 100;   // number of processing steps in a single job
              static constexpr uint STEP_SPREAD = 2;   // reduced irregularity of allocations
              static constexpr uint LOAD_SPREAD = 0;   // effectively disable variation of buffer types
              static constexpr uint MAX_DELAY   = 0;   // also disable random sleep between jobs
            };
      
      /** @test attempt to create a coordinated _full load_
       *      - configure the workers to compute without pause
       *      - use more repetitions in a single job
       *      - configured load time should be 1000ms per thread
       *      - use small number of workers; each gets its own core
       *      - capture the overall run time of the workers
       *      - coordination overhead should not be excessive
       */
      void
      verify_controlledLoad()
        {
          Ctx mockEngine;
          using Threads = ScopedCollection<DummyWorkerThread<FullLoadSetup>>;
          
          double time_us = benchmarkTime ([]{ // run a set of threads with full-load config...
                                              Threads threads{FULL_LOAD_CORES, Threads::fill()};
                                              while (explore(threads).has_any())
                                                sleep_for (1ms);
                                            });
          
          CHECK (time_us < 1.6e6);
          sleep_for (500us);
          mockEngine.globalPool->processPendingRequests();
          CHECK (0 == watch(*mockEngine.globalPool).bytesLeased());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BufferProviderLoad_test, "unit engine");
  
  
  
}}} // namespace vault::mem::test
