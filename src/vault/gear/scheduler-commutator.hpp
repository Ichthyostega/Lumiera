/*
  SCHEDULER-COMMUTATOR.hpp  -  coordination layer of the render engine scheduler

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


/** @file scheduler-commutator.hpp
 ** Layer-2 of the Scheduler: coordination and interaction of activities.
 ** This is the upper layer of the implementation and provides high-level functionality.
 ** 
 ** @see SchedulerCommutator_test
 ** @see scheduler.hpp usage
 ** 
 ** @todo WIP-WIP-WIP 6/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_SCHEDULER_COMMUTATOR_H_
#define SRC_VAULT_GEAR_SCHEDULER_COMMUTATOR_H_


#include "vault/common.hpp"
#include "vault/gear/activity.hpp"
#include "vault/gear/scheduler-invocation.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/nocopy.hpp"

//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>


namespace vault{
namespace gear {
  
  using lib::time::Time;
//  using util::isnil;
//  using std::string;
  
  
  /**
   * Scheduler Layer-2 : coordination.
   * 
   * @see SchedulerInvocation (Layer-1)
   * @see SchedulerCommutator_test
   */
  class SchedulerCommutator
    : util::NonCopyable
    {
      
    public:
//      explicit
      SchedulerCommutator()
        { }
      
      Activity*
      findWork (SchedulerInvocation& layer1)
        {
          layer1.feedPrioritisation();
          UNIMPLEMENTED("wtf??");
          while (layer1.isDue (currentSchedulerTime()))
            {
              Activity* activity = layer1.pullHead();
              if (isInternalWork (activity))
                performInternalWork (activity);
              else
                {
                  performExternalWork (activity);
                  UNIMPLEMENTED("return");
                }
            }
        }
      
      Time
      currentSchedulerTime()  const
        {
          UNIMPLEMENTED ("how to manage the current scheduler time");
        }
      
      bool
      isInternalWork (Activity* activity)
        {
          UNIMPLEMENTED ("determine if an Activity counts as internal management task");
        }
      
      void
      performInternalWork (Activity* activity)
        {
          UNIMPLEMENTED ("evaluate a term");
        }
      
      void
      performExternalWork (Activity* activity)
        {
          UNIMPLEMENTED ("drop the exclusion flag and then engage into the external render activity");
        }
      
      template<class EXE>
      activity::Proc
      postDispatch (Activity* chain, EXE& executionCtx)
        {
          UNIMPLEMENTED ("core function: maybe perform activity");
        }
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_SCHEDULER_COMMUTATOR_H_*/
