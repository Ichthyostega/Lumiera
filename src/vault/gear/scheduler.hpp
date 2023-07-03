/*
  SCHEDULER.hpp  -  coordination of render activities under timing and dependency constraints

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


/** @file scheduler.hpp
 ** Service for coordination and dispatch of render activities.
 ** The implementation of scheduling services is provided by an integration
 ** of two layers of functionality:
 ** - Layer-1 allows to enqueue and prioritise render activity records
 ** - Layer-2 connects and coordinates activities to conduct complex calculations 
 ** 
 ** @see SchedulerUsage_test Component integration test
 ** @see scheduler.cpp implementation details
 ** @see SchedulerInvocation Layer-1
 ** @see SchedulerCommutator Layer-2
 ** 
 ** @todo WIP-WIP-WIP 6/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_SCHEDULER_H_
#define SRC_VAULT_GEAR_SCHEDULER_H_


#include "lib/error.hpp"
#include "vault/gear/block-flow.hpp"
#include "vault/gear/scheduler-commutator.hpp"
#include "vault/gear/scheduler-invocation.hpp"
//#include "lib/symbol.hpp"
#include  "lib/nocopy.hpp"
//#include "lib/util.hpp"

//#include <string>


namespace vault{
namespace gear {
  
//  using util::isnil;
//  using std::string;
  
  
  /**
   * Schedule and coordinate render activities.
   * @todo WIP-WIP 6/2023
   * @see BlockFlow
   * @see SchedulerUsage_test
   */
  class Scheduler
    {
      SchedulerInvocation layer1_;
      SchedulerCommutator layer2_;
      
    public:
      explicit
      Scheduler()
        : layer1_{}
        , layer2_{}
        { }
      
      // using default copy/assignment
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_SCHEDULER_H_*/
