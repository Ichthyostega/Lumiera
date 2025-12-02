/*
  WORK-FORCE.hpp  -  actively coordinated pool of workers for rendering

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file work-force.hpp
 ** A pool of workers for multithreaded rendering.
 ** The Lumiera Render Engine is driven by active workers repeatedly pulling
 ** the next planned chunk of work; maintaining the internal organisation of the
 ** Scheduler is integrated into that pattern as _just another activity_ performed
 ** by the workers. As a consequence, there is no need for a central »master« or
 ** coordinator to dispatch individual jobs. As long as the worker pool holds
 ** active workers, the engine is in running state.
 ** 
 ** The WorkForce (worker pool service) in turn is passive and fulfils the purpose of
 ** holding storage for the active worker objects in a list, pruning terminated entries.
 ** Some parameters and configuration is provided to the workers, notably a _work functor_
 ** invoked actively to »pull« work. The return value from this `doWork()`-function governs
 ** the worker's behaviour, either by prompting to pull further work, by sending a worker
 ** into a sleep cycle, perform contention mitigation, or even asking the worker to terminate.
 ** 
 ** @warning concurrency and synchronisation in the Scheduler (which maintains and operates
 **          WorkForce) is based on the assumption that _all maintenance and organisational
 **          work is done chunk-wise by a single worker._ Other render activities may proceed
 **          in parallel, yet any other worker about to pick the next task has to wait until
 **          it is possible to grab the `GroomingToken` exclusively. For the WorkForce this
 **          usage pattern implies that there is *no explicit synchronisation* -- scaling up
 **          and shutting down must be performed non-concurrently.
 ** @see work-force-test.cpp
 ** @see scheduler-commutator.hpp usage as part of the scheduler
 */


#ifndef SRC_VAULT_GEAR_WORK_FORCE_H_
#define SRC_VAULT_GEAR_WORK_FORCE_H_


#include "vault/common.hpp"
#include "vault/gear/activity.hpp"
#include "lib/meta/function.hpp"
#include "lib/thread.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <utility>
#include <chrono>
#include <atomic>
#include <list>


namespace vault{
namespace gear {
  
  using std::move;
  using std::atomic;
  using util::unConst;
  using std::chrono::milliseconds;
  using std::chrono::microseconds;
  using std::chrono_literals::operator ""ms;
  using std::chrono_literals::operator ""us;
  using std::this_thread::sleep_for;
  
  
  namespace {
    const double MAX_OVERPROVISIONING = 3.0;      ///< safety guard to prevent catastrophic over-provisioning
    
    const size_t CONTEND_SOFT_LIMIT  = 3;                         ///< zone for soft anti-contention measures, counting continued contention events
    const size_t CONTEND_STARK_LIMIT = CONTEND_SOFT_LIMIT + 5;    ///< zone for stark measures, performing a sleep with exponential stepping
    const size_t CONTEND_SATURATION  = CONTEND_STARK_LIMIT + 4;   ///< upper limit for the contention event count
    const size_t CONTEND_SOFT_FACTOR = 100;                       ///< base counter for a spinning wait loop
    const size_t CONTEND_RANDOM_STEP = 11;                        ///< stepping for randomisation of anti-contention measures
    const microseconds CONTEND_WAIT = 100us;                      ///< base time unit for the exponentially stepped-up sleep delay in case of contention
    
    inline size_t
    thisThreadHash()
    {
      return std::hash<std::thread::id>{} (std::this_thread::get_id());
    }
  }
  
  namespace work { ///< Details of WorkForce (worker pool) implementation
    
    using SIG_WorkFun   = activity::Proc(void);   ///< config should define a callable with this signature to perform work
    using SIG_FinalHook = void(bool);             ///< config should define callable invoked at exit (argument: isFailure)
    
    /**
     * Base for configuration of the worker pool.
     * In real usage, a subclass of #Config is used,
     * which additionally defines the two required functors
     * - `doWork` : perform a piece of work and return control code
     * - `finalHook` : callback invoked at work thread termination
     * Obviously these two functors are defined in a way to call into
     * the actual implementation of work management (i.e. the Scheduler).
     */
    struct Config
      {
        static size_t COMPUTATION_CAPACITY;
        
        const milliseconds IDLE_WAIT = 20ms;      ///< wait period when a worker _falls idle_
        const size_t DISMISS_CYCLES  = 100;       ///< number of idle cycles after which the worker terminates
        
        static size_t getDefaultComputationCapacity();
      };
    
    
    
    void   performRandomisedSpin (size_t,size_t);
    microseconds steppedRandDelay(size_t,size_t);
    
    
    using Launch = lib::Thread::Launch;
    
    /*************************************//**
     * Individual worker thread:
     * repeatedly pulls the `doWork` functor.
     */
    template<class CONF>
    class Worker
      : CONF
      , util::NonCopyable
      {
      public:
        Worker (CONF config)
          : CONF{move (config)}
          , thread_{Launch{&Worker::pullWork, this}
                          .threadID("Worker")
                          .decorateCounter()}
          { }
        
        /** emergency break to trigger cooperative halt */
        std::atomic<bool> emergency{false};
        
        /** this Worker starts out active, but may terminate */
        bool isDead() const { return not thread_; }
        
        
      private:
        lib::Thread thread_;
        
        void
        pullWork()
          {
            ASSERT_MEMBER_FUNCTOR (&CONF::doWork,    SIG_WorkFun);
            ASSERT_MEMBER_FUNCTOR (&CONF::finalHook, SIG_FinalHook);
            
            bool regularExit{false};
            try /* ================ pull work ===================== */
              {
                while (true)
                  {
                    activity::Proc res = CONF::doWork();
                    if (emergency.load (std::memory_order_relaxed))
                      break;
                    if (res == activity::KICK)
                      res = contentionWait();
                    else
                      if (kickLevel_)
                        kickLevel_ /= 2;
                    if (res == activity::WAIT)
                      res = idleWait();
                    else
                      idleCycles_ = 0;
                    if (res != activity::PASS)
                      break;
                  }
                regularExit = true;
              }
            ERROR_LOG_AND_IGNORE (threadpool, "defunct worker thread")
            
            try /* ================ thread-exit hook ============== */
              {
                CONF::finalHook (not regularExit);
              }
            ERROR_LOG_AND_IGNORE (threadpool, "failure in thread-exit hook")
          }// Thread will terminate....
        
        activity::Proc
        idleWait()
          {
            ++idleCycles_;
            if (idleCycles_ < CONF::DISMISS_CYCLES)
              {
                sleep_for (CONF::IDLE_WAIT);
                return activity::PASS;
              }
            else  // idle beyond threshold => terminate worker
              return activity::HALT;
          }
        size_t idleCycles_{0};
        
        
        activity::Proc
        contentionWait()
          {
            if (not randFact_)
              randFact_ = thisThreadHash() % CONTEND_RANDOM_STEP;
            
            if (kickLevel_ <= CONTEND_SOFT_LIMIT)
              for (uint i=0; i<kickLevel_; ++i)
                {
                  performRandomisedSpin (kickLevel_,randFact_);
                  std::this_thread::yield();
                }
            else
              {
                auto stepping = util::min (kickLevel_, CONTEND_STARK_LIMIT) - CONTEND_SOFT_LIMIT;
                std::this_thread::sleep_for (steppedRandDelay(stepping,randFact_));
              }
            
            if (kickLevel_ < CONTEND_SATURATION)
              ++kickLevel_;
            return activity::PASS;
          }
        size_t kickLevel_{0};
        size_t randFact_{0};
      };
  }//(End)namespace work
  
  
  
  
  /***********************************************************//**
   * Pool of worker threads for rendering.
   * @note the \tparam CONF configuration/policy base must define:
   *       - `doWork` - the _work-functor_ (with #SIG_WorkFun)
   *       - `finalHook` - called at thread exit
   * @see WorkForce_test
   * @see SchedulerCommutator
   */
  template<class CONF>
  class WorkForce
    : util::NonCopyable
    {
      using Pool = std::list<work::Worker<CONF>>;
      
      CONF setup_;
      Pool workers_;
      
      
    public:
      WorkForce (CONF config)
        : setup_{move (config)}
        , workers_{}
        { }
      
     ~WorkForce()
        {
          try {
            awaitShutdown();
          }
          ERROR_LOG_AND_IGNORE (threadpool, "defunct worker thread")
        }
      
      
      /**
       * Activate or scale up the worker pool.
       * @param degree fraction of the full #COMPUTATION_CAPACITY to activate
       * @note will always activate at least one worker; will never scale down;
       *       setting values > 1.0 leads to over-provisioning...
       */
      void
      activate (double degree =1.0)
        {
          size_t scale{setup_.COMPUTATION_CAPACITY};
          scale = size_t(util::limited (0.0, degree*scale, scale*MAX_OVERPROVISIONING));
          for (uint i = workers_.size(); i < scale; ++i)
            workers_.emplace_back (setup_);
        }
      
      void
      incScale(uint step =+1)
        {
          uint i = workers_.size();
          uint target = util::min (i+step, setup_.COMPUTATION_CAPACITY);
          for ( ; i < target; ++i)
            workers_.emplace_back (setup_);
        }
      
      void
      awaitShutdown()
        {
          for (auto& w : workers_)
            w.emergency.store (true, std::memory_order_relaxed);
          while (0 < size())
            sleep_for (setup_.IDLE_WAIT);
        }
      
      size_t
      size()  const
        {
          unConst(workers_).remove_if([](auto& w){ return w.isDead(); });
          return workers_.size();
        }
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_WORK_FORCE_H_*/
