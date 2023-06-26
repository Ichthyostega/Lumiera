/*
  SCHEDULER-CONTROL.hpp  -  coordination layer of the render engine scheduler

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


/** @file scheduler-control.hpp
 ** Layer-2 of the Scheduler: coordination of activities.
 ** This is the upper layer of the implementation and provides high-level functionality.
 ** 
 ** @see ////TODO_test usage example
 ** @see scheduler.cpp implementation
 ** 
 ** @todo WIP-WIP-WIP 6/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_SCHEDULER_CONTROL_H_
#define SRC_VAULT_GEAR_SCHEDULER_CONTROL_H_


#include "vault/common.hpp"
#include "lib/nocopy.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>


namespace vault{
namespace gear {
  
//  using util::isnil;
//  using std::string;
  
  
  /**
   * Scheduler Layer-2 : coordination.
   * 
   * @see SomeSystem
   * @see NA_test
   */
  class SchedulerControl
    : util::NonCopyable
    {
      
    public:
//      explicit
      SchedulerControl()
        { }
      
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_SCHEDULER_CONTROL_H_*/
