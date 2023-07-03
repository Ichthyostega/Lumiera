/*
  ACTIVITY-LANG.hpp  -  definition language framework for scheduler activities

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


/** @file activity-lang.hpp
 ** A language framework to define and interconnect scheduler activity verbs.
 ** This is an interface to build valid activity descriptors, connect them with
 ** other activities and provide parameter data. The resulting definition terms
 ** can be passed to the scheduler's messaging queue to cause the described
 ** activities to be performed in the context of the Lumiera render engine.
 ** 
 ** @warning mostly this is planned functionality
 ** @see SchedulerActivity_test
 ** @see activity.hpp definition of verbs
 ** 
 ** @todo WIP-WIP-WIP 6/2023 »Playback Vertical Slice«
 ** 
 */


#ifndef SRC_VAULT_GEAR_ACTIVITY_LANG_H_
#define SRC_VAULT_GEAR_ACTIVITY_LANG_H_


#include "vault/gear/activity.hpp"
#include "vault/gear/block-flow.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>


namespace vault{
namespace gear {
  
//  using util::isnil;
//  using std::string;
  
  
  /**
   * TODO write type comment...
   * 
   * @see Activity
   * @see SchedulerActivity_test
   */
  class ActivityLang
    {
      BlockFlow& mem_;
      
    public:
//      explicit
      ActivityLang (BlockFlow& memManager)
        : mem_{memManager}
        { }
      
      // using default copy/assignment
    };
  
  
  
}} // namespace vault::gear
#endif /*SRC_VAULT_GEAR_ACTIVITY_LANG_H_*/
