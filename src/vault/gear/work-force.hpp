/*
  WORK-FORCE.hpp  -  actively coordinated pool of workers for rendering

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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
 ** into a sleep cycle, or even asking the worker to terminate.
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
        
        const milliseconds IDLE_WAIT = 20ms;
        const size_t DISMISS_CYCLES = 100;
        
        static size_t getDefaultComputationCapacity();
      };
    
    
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
                    if (res == activity::WAIT)
                      res = idleWait();
                    else
                      idleCycles = 0;
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
            ++idleCycles;
            if (idleCycles < CONF::DISMISS_CYCLES)
              {
                sleep_for (CONF::IDLE_WAIT);
                return activity::PASS;
              }
            else  // idle beyond threshold => terminate worker
              return activity::HALT;
          }
        size_t idleCycles{0};
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
          scale *= degree;
          scale = util::max (scale, 1u);
          for (uint i = workers_.size(); i < scale; ++i)
            workers_.emplace_back (setup_);
        }
      
      void
      incScale()
        {
          if (size() >= setup_.COMPUTATION_CAPACITY)
            return;
          else
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
