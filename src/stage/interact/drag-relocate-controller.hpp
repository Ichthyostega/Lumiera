/*
  DRAG-RELOCATE-CONTROLLER.hpp  -  concrete gesture controller to relocate a widget by dragging

  Copyright (C)         Lumiera.org
    2021,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file drag-relocate-controller.hpp
 ** Concrete implementation of a dragging gesture to relocate an UI entity.
 ** This gesture controller is maintained as an InteractionState holder within
 ** the InteractionDirector and serves as target to receive signals, in order to
 ** observe a draggable widget and possibly activate on formation of a dragging
 ** gesture. When this happens, the DragRelocateController is responsible for
 ** observing the mouse movements, to integrate a movement delta, and finally
 ** to recognise the end of the dragging gesture and invoke the associated
 ** command on the entity to be dragged, which serves as \ref Subject for
 ** this gesture and the resulting command.
 ** 
 ** @note this implementation level header is meant to be included
 **       solely for the purpose of creating an instance from within
 **       GestureState, the facility managing InvocationState instances.
 ** 
 ** @todo WIP and prototypical implementation as of 3/2021
 ** 
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef STAGE_INTERACT_DRAG_RELOCATE_CONTROLLER_H
#define STAGE_INTERACT_DRAG_RELOCATE_CONTROLLER_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "stage/interact/interaction-state.hpp"
#include "stage/interact/cmd-context.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>


namespace stage {
namespace interact {
  
//  using lib::HashVal;
//  using util::isnil;
//  using std::string;
  
  
  /**
   * Abstract foundation context dependent UI interactions.
   * While forming an interaction gesture, context state is picked up
   * incrementally, and maintained here, together with parameters of degree,
   * amount or relative position. Typically, an concrete implementation subclass
   * is geared for one specific kind of interaction or gesture and used as target
   * for wiring Signals to trigger and carry out this specific interaction.
   * The InteractionDirector is responsible for allocating and maintaining those
   * concrete InteractionState instances, which can then be used to issue complex
   * commands for subjects and further arguments picked up from the interaction.
   * 
   * @todo write type comment...
   * @todo WIP-WIP as of /3/2021
   * ///////////////////////////////////TODO do we need a translation unit interaction-state.cpp (otherwise delete it!)
   */
  class DragRelocateController
    : public InteractionState
    {
      
      void
      linkTrigger (Subject& subject, Symbol cmdID)  override
        {
          UNIMPLEMENTED ("use the Subject interface to hook up a trigger signal");
        }
      
    public:
      DragRelocateController()
//      :
        { }
      
    private:
    };
  
  
  
}} // namespace stage::interact
#endif /*STAGE_INTERACT_DRAG_RELOCATE_CONTROLLER_H*/
