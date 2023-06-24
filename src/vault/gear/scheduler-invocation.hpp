/*
  SCHEDULER-INVOCATION.hpp  -  invocation layer of the render engine scheduler

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


/** @file scheduler-invocation.hpp
 ** Layer-1 of the Scheduler: dispatch and invocation of activities.
 ** This is the lower layer of the implementation and provides low-level functionality.
 ** 
 ** @note as of X/2023 this is complete bs
 ** @see ////TODO_test usage example
 ** @see scheduler.cpp implementation
 ** 
 ** @todo WIP-WIP-WIP 6/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_SCHEDULER_INVOCATION_H_
#define SRC_VAULT_GEAR_SCHEDULER_INVOCATION_H_


#include "vault/common.hpp"
#include "lib/nocopy.hpp"
//#include "lib/symbol.hpp"
#include "vault/gear/activity.hpp"
//#include "lib/util.hpp"

//#include <string>
#include <queue>
#include <boost/lockfree/queue.hpp>

namespace vault{
namespace gear {
  
//  using util::isnil;
//  using std::string;
  
  
  /**
   * Scheduler Layer-2 : invocation.
   * 
   * @see SomeSystem
   * @see NA_test
   */
  class SchedulerInvocation
    : util::NonCopyable
    {
      struct ActOrder
        {
          size_t  waterlevel{0};
          Activity* activity{nullptr};
        };
      
      using InstructQueue = boost::lockfree::queue<ActOrder>;
      using PriorityQueue = std::priority_queue<ActOrder>;
      
      InstructQueue instruct_;
      PriorityQueue priority_;
      
    public:
      explicit
      SchedulerInvocation (int const& moo)
        { }
      
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_SCHEDULER_INVOCATION_H_*/
