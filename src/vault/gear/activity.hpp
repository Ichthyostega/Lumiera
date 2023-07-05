/*
  ACTIVITY.hpp  -  elementary operation to be handled by the scheduler

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


/** @file activity.hpp
 ** Descriptor for a piece of operational logic performed by the scheduler.
 ** The Scheduler in Lumiera's Vault coordinates the rendering activities,
 ** by passing and responding to _Activity messages,_ thereby observing
 ** temporal limits (deadlines) and dependency constraints. A predefined
 ** and fixed set of _Activity verbs_ defines atomic operations the engine
 ** is able to perform, directed by the scheduler. Complex calculation
 ** tasks are constructed from interconnected activities, passing through
 ** the messaging and priority queue of the scheduler. While an Activity
 ** is in processing, the corresponding descriptor data record is maintained
 ** by the BlockStream custom memory manager.
 ** 
 ** @note right now this is a pipe dream
 ** @see ////TODO_test usage example
 ** @see scheduler.cpp implementation
 ** 
 ** @todo WIP-WIP-WIP 6/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_ACTIVITY_H_
#define SRC_VAULT_GEAR_ACTIVITY_H_


#include "vault/common.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>


namespace vault{
namespace gear {
  
//  using util::isnil;
//  using std::string;
  
  
  /**
   * Term to describe an Activity,
   * to happen within the Scheduler's control flow.
   */
  class Activity
    {
      
    public:
      enum Verb {INVOKE     ///< dispatches a JobFunctor into a worker thread
                ,DEPEND     ///< verify a given number of dependencies has been satisfied
                ,TIMESTART  ///< signal start of some processing (for timing measurement)
                ,TIMESTOP   ///< correspondingly signal end of some processing
                ,NOTIFY     ///< push a message to another Activity
                ,PROBE      ///< evaluate a condition and inhibit another target Activity
                ,GATE       ///< probe window + count-down; activate next Activity, else re-schedule
                ,TICK       ///< internal engine »heart beat« for internal maintenance hook(s)
                };
      
      const Verb verb_;
      
      explicit
      Activity (Verb verb)
        : verb_{verb}
        { }
      
      // using default copy/assignment
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_ACTIVITY_H_*/
