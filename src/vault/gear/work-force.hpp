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
#include "lib/nocopy.hpp"
//#include "lib/symbol.hpp"
#include "lib/util.hpp"

//#include <string>
#include <functional>
#include <utility>
#include <vector>
#include <thread>
#include <atomic>


namespace vault{
namespace gear {
  
//  using util::isnil;
//  using std::string;
  using std::move;
  using std::atomic;
  
  
  /**
   * Pool of worker threads for rendering.
   * 
   * @see SomeSystem
   * @see NA_test
   */
  class WorkForce
    : util::NonCopyable
    {
      using WorkFun = std::function<activity::Proc(void)>;
      using Pool = std::vector<std::thread>;
      
      WorkFun workFun_;
      Pool workers_;
      
      atomic<bool> halt_{false};
      
    public:
      static const size_t FULL_SIZE;
      
      explicit
      WorkForce (WorkFun&& fun)
        : workFun_{move (fun)}
        , workers_{}
        { 
          workers_.reserve (1.5*FULL_SIZE);
        }
      
     ~WorkForce()
        {
          try {
            deactivate();
          }
          ERROR_LOG_AND_IGNORE (threadpool, "defunct worker thread")
        }
      
      
      void
      activate (double degree =1.0)
        {
          halt_ = false;
          size_t scale = util::max (size_t(degree*FULL_SIZE), 1u);
          for (uint i = workers_.size(); i < scale; ++i)
            workers_.emplace_back ([this]{ pullWork(); });
        }
      
      void
      deactivate()
        {
          halt_ = true;
          for (auto& w : workers_)
            if (w.joinable())
              w.join();
          workers_.clear();
        }
      
    private:
      void
      pullWork()
        {
          try {
              while (true)
                {
                  activity::Proc res = workFun_();
                  if (halt_ or res != activity::PASS)
                    break;
                }
            }
          ERROR_LOG_AND_IGNORE (threadpool, "defunct worker thread")
        }
      
      
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_WORK_FORCE_H_*/
