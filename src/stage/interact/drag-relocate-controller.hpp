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


#include "stage/gtk-base.hpp"
#include "stage/interact/interaction-state.hpp"
#include "stage/interact/cmd-context.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/symbol.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

//#include <string>


namespace stage {
namespace interact {
  
//  using lib::HashVal;
//  using util::isnil;
//  using std::string;
  using util::isnil;
  using util::_Fmt;
  

  
  /**
   * Gesture controller for dragging objects within the Timeline display.
   * The gesture to drag an entity is triggered by observing mouse movements
   * while a mouse key and possibly some modifier key is pressed. To recognise
   * this condition, every possible subject for a drag gesture will be wired through
   * the #linkTrigger call into this controller. When #detectActivation is fulfilled
   * for one specific subject, the corresponding context data will be tracked as state
   * of the ongoing gesture in formation, maintained within the member fields of this
   * controller.
   * 
   * @todo write type comment...
   * @todo WIP-WIP as of /3/2021
   * ///////////////////////////////////TODO do we need a translation unit interaction-state.cpp (otherwise delete it!)
   */
  class DragRelocateController
    : public InteractionState
    {
      bool buttonPressed_ = false;
      
      void
      linkTrigger (Subject& subject, Symbol cmdID)  override
        {
          REQUIRE (not isnil (cmdID));
          auto& widget = subject.exposeWidget();
          widget.signal_button_press_event().connect(
              sigc::mem_fun (*this, &DragRelocateController::watchButton));
          widget.signal_button_release_event().connect(
              sigc::mem_fun (*this, &DragRelocateController::watchButton));
          widget.signal_motion_notify_event().connect(
              [&](GdkEventMotion* motion) -> bool
                {
                  try{ return detectActivation(subject, motion); }
                  ON_EXCEPTION_RETURN (false, "activate dragging gesture")
                }
          );
        }
      
      bool
      watchButton (GdkEventButton* button_event)  noexcept
        {
          REQUIRE (button_event);
          if (button_event->type & GDK_BUTTON_PRESS)
            buttonPressed_ = true;
          else
          if (button_event->type & GDK_BUTTON_RELEASE)
            buttonPressed_ = true;
          return false;
        }
      
      bool
      detectActivation (Subject& subject, GdkEventMotion* motion_event)
        {
          if (not buttonPressed_)
            return false;
          REQUIRE (motion_event);
          std::cerr << _Fmt{"MOVE x=%3.1f y=%3.1f subject=%s"}
                       % motion_event->x
                       % motion_event->y
                       % subject
                    << std::endl;
          return true; // Event handled
        }
      
      
    public:
      DragRelocateController()
//      :
        { }
      
    private:
    };
  
  
  
}} // namespace stage::interact
#endif /*STAGE_INTERACT_DRAG_RELOCATE_CONTROLLER_H*/
