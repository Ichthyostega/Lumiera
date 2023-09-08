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
 ** 
 ** @see ////TODO_test usage example
 ** @see scheduler.cpp implementation
 ** 
 ** @todo WIP-WIP-WIP 6/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_WORK_FORCE_H_
#define SRC_VAULT_GEAR_WORK_FORCE_H_


#include "vault/common.hpp"
#include "vault/gear/activity.hpp"
#include "lib/meta/function.hpp"
#include "lib/nocopy.hpp"
//#include "lib/symbol.hpp"
#include "lib/util.hpp"

//#include <string>
#include <utility>
#include <chrono>
#include <thread>
#include <atomic>
#include <list>


namespace vault{
namespace gear {
  
//  using util::isnil;
//  using std::string;
  using std::move;
//  using std::forward;
  using std::atomic;
  using util::unConst;
  
  using std::chrono_literals::operator ""ms;  /////////////WIP

  
  
  namespace work {
    using std::chrono::milliseconds;
    
    using SIG_WorkFun = activity::Proc(void);
    
    struct Config
      {
        static const size_t COMPUTATION_CAPACITY;
        const size_t EXPECTED_MAX_POOL = 1.5*COMPUTATION_CAPACITY;
        
        const milliseconds IDLE_WAIT = 20ms;
        const size_t DISMISS_CYCLES = 100;
      };
    
    template<class CONF>
    class Runner
      : CONF
      , util::NonCopyable
      , public std::thread
      {
      public:
        Runner (CONF config)
          : CONF{move (config)}
          , thread{[this]{ pullWork(); }}
          { }
        
        /** emergency break to trigger cooperative halt */
        std::atomic<bool> emergency{false};
        
      private:
        void
        pullWork()
          {
            ASSERT_VALID_SIGNATURE (decltype(CONF::doWork), SIG_WorkFun);
            
            try {
              while (true)
                {
                  activity::Proc res = CONF::doWork();
                  if (emergency.load (std::memory_order_relaxed))
                    break;
                  if (res == activity::WAIT)
                    res = idleWait();
                  if (res != activity::PASS)
                    break;
                }
              }
            ERROR_LOG_AND_IGNORE (threadpool, "defunct worker thread")
            ////////////////////////////////////////////////////////////////////////////OOO very important to have a reliable exit-hook here!!!
            thread::detach();
          }
        
        activity::Proc
        idleWait()
          {
            std::this_thread::sleep_for (CONF::IDLE_WAIT);
                                ////////////////////////////////WIP extended inactivity detector here
            return activity::PASS;
          }
      };
  }//(End)namespace work
  
  
  /**
   * Pool of worker threads for rendering.
   * 
   * @see SomeSystem
   * @see NA_test
   */
  template<class CONF>
  class WorkForce
    : util::NonCopyable
    {
      using Pool = std::list<work::Runner<CONF>>;
      
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
       * @note will always activate at least one worker;
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
          if (size() > setup_.COMPUTATION_CAPACITY)
            return;
          else
            workers_.emplace_back (setup_);
        }
      
      void
      awaitShutdown()
        {
          for (auto& w : workers_)
            w.emergency.store(true, std::memory_order_relaxed);
          using namespace std::chrono_literals;  ///////////////////////7///WIP
          do
            std::this_thread::sleep_for(10ms);
          while (0 < size());
        }
      
      size_t
      size()  const
        {
          unConst(workers_).remove_if([](auto& w){ return not w.joinable(); });
          return workers_.size();
        }
      
    private:
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_WORK_FORCE_H_*/
