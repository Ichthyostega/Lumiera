/*
  INTERACTION-STATE.hpp  -  facility to watch and guide one aspect of UI interaction

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


/** @file interaction-state.hpp
 ** Abstraction: a component to watch, maintain and guide UI state.
 ** Typically, each such component is focused on one specific aspect
 ** of complex interface interactions. An example would be a component
 ** to track gestures for trimming edits.
 ** 
 ** @todo as of 11/2015 this is complete WIP-WIP-WIP
 ** @todo and as of 3/2021 this is at least just WIP-WIP
 ** 
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef STAGE_INTERACT_INTERACTION_STATE_H
#define STAGE_INTERACT_INTERACTION_STATE_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
//#include "stage/ctrl/bus-term.hpp"
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
  
  class Subject;
  
  
  /**
   * Abstract foundation for context dependent UI interactions.
   * While forming an interaction gesture, context state is picked up
   * incrementally, and maintained here, together with parameters of degree,
   * amount or relative position. Typically, a concrete implementation subclass
   * is geared for one specific kind of interaction or gesture and used as target
   * for wiring Signals to trigger and carry out this specific interaction.
   * The GestureState holder within InteractionDirector is responsible for allocating
   * and maintaining those concrete InteractionState instances, which can then be used to
   * issue complex commands for subjects and further arguments picked up from the interaction.
   * 
   * @todo write type comment...
   * @todo WIP-WIP as of /3/2021
   * ///////////////////////////////////TODO do we need a translation unit interaction-state.cpp (otherwise delete it!)
   */
  class InteractionState
    : util::NonCopyable
    {
    protected:
      virtual ~InteractionState();   ///< this is an interface
      
    public:
      /**
       * Hook up a trigger signal to initiate a specific interaction gesture.
       * Within the concrete controller implementation for each gesture, the Subject interface
       * shall be used to access an actual widget and to wire this widget's signals to a private
       * implementation function within the concrete gesture controller. The purpose is then
       * to observe this widget and to activate recognition of the gesture when applicable.
       * @param Subject the subject of the interaction gesture; typically implemented by a
       *                controller or widget responsible for the UI entity involved.
       * @param cmdID   the actual command to be issued on completion of the interaction
       * @remark typically this contextual information is bound into the signal wiring,
       *                meaning that the actual interaction context will be established "late",
       *                i.e. only when an actual gesture is about to commence
       */
      virtual void linkTrigger (Subject&, Symbol cmdID)    =0;
      
    private:
    };
  
  
  
}} // namespace stage::interact
#endif /*STAGE_INTERACT_INTERACTION_STATE_H*/
