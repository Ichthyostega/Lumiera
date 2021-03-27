/*
  GESTURE-STATE.hpp  -  holder for interaction-state dedicated to specific gestures

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file gesture-state.hpp
 ** Sub-Service of the InteractionDirector: Hold and maintain InteractionState.
 ** Typically, each distinct gesture or elaborate interaction pattern has a dedicated
 ** InteractionState subclass to maintain the state of ongoing gestures within the UI.
 ** Client code attaches and accesses this state through the CmdContext front-end.
 ** The InteractionDirector exposes a singleton instance of the GestureState manager
 ** through the lib::Depend access framework (Dependency-Injection).
 ** 
 ** @todo WIP-WIP as of 3/2021 about to create a prototype implementation for dragging clips...
 ** 
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef STAGE_INTERACT_GESTURE_STATE_H
#define STAGE_INTERACT_GESTURE_STATE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
//#include "stage/ctrl/bus-term.hpp"
#include "stage/interact/interaction-state.hpp"
//#include "lib/idi/entry-id.hpp"
#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>


namespace stage {
namespace interact {
  
//  using lib::HashVal;
  using lib::Symbol;
//  using util::isnil;
//  using std::string;
  
//class Subject;
  
  
  /**
   * Holder for InteractionState dedicated to UI gestures and complex interactions.
   * 
   * @todo write type comment...
   * @todo WIP-WIP as of /3/2021
   * ///////////////////////////////////TODO do we need a translation unit interaction-state.cpp (otherwise delete it!)
   */
  class GestureState
    : util::NonCopyable
    {
    public:
     ~GestureState();   ///////////TODO required??
      
    public:
      /**
       */
      InteractionState&
      getStateFor (std::string ctxID)
        {
          UNIMPLEMENTED ("how to designate and access state for specific gestures");
        }
      
    private:
    };
  
  
  
}} // namespace stage::interact
#endif /*STAGE_INTERACT_GESTURE_STATE_H*/
