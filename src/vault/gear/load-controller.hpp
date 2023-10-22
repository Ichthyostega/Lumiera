/*
  LOAD-CONTROLLER.hpp  -  coordinator for scheduler resource usage

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


/** @file load-controller.hpp
 ** Scheduler resource usage coordination.
 ** Operating the render activities in the engine involves several low-level
 ** support systems, which must be actively guided to remain within sustainable
 ** limits. While all parts of the engine are tuned towards typical expected
 ** scenarios, a wide array of load patterns may be encountered, complicating
 ** any generic performance optimisation. Rather, the participating components
 ** are designed to withstand a short-term imbalance, expecting that general
 ** engine parametrisation will be adjusted based on moving averages.
 **
 ** @see scheduler.hpp
 ** @see SchedulerStress_test
 ** 
 ** @todo WIP-WIP-WIP 10/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_LOAD_CONTROLLER_H_
#define SRC_VAULT_GEAR_LOAD_CONTROLLER_H_


#include "lib/error.hpp"
//#include "vault/gear/block-flow.hpp"
#include "vault/gear/activity-lang.hpp"
//#include "lib/symbol.hpp"
#include  "lib/nocopy.hpp"
//#include "lib/util.hpp"

//#include <string>
#include <chrono>


namespace vault{
namespace gear {
  
//  using util::isnil;
//  using std::string;
  using std::chrono::microseconds;
  
  
  /**
   * Controller to coordinate resource usage related to the Scheduler.
   * @todo WIP-WIP 10/2023 just a placeholder for now
   * @see BlockFlow
   * @see Scheduler
   */
  class LoadController
    : util::NonCopyable
    {
      BlockFlowAlloc& allocator_;
      
    public:
      explicit
      LoadController (BlockFlowAlloc& blockFlow)
        : allocator_{blockFlow}
        { }
      
      
      /**
       * @note const and non-grooming
       */
      bool
      tendedNext()  const
        {
          UNIMPLEMENTED ("Predicate to determine if next foreseeable Activity was tended for");
        }
      
      void
      tendNext()
        {
          UNIMPLEMENTED ("tend for the next foreseeable Activity");
        }
      
      enum
      Capacity {SPINTIME   ///< imminent activities
               ,NEARTIME   ///< capacity for active processing required
               ,WORKTIME   ///< typical stable work task rhythm expected
               ,IDLETIME   ///< time to go to sleep
               };
      
      Capacity
      classifyCapacity()  const
        {
          UNIMPLEMENTED ("establish a categorisation for available capacity");
        }
      
      
      microseconds
      scatteredDelayTime()
        {
          UNIMPLEMENTED ("establish a randomised targeted delay time");
        }

    };
  
  
  
}}// namespace vault::gear
#endif /*SRC_VAULT_GEAR_LOAD_CONTROLLER_H_*/
