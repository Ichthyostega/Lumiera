/*
  DRAG-RELOCATE-CONTROLLER.hpp  -  concrete gesture controller to relocate a widget by dragging

   Copyright (C)
     2021,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
#include "lib/opaque-holder.hpp"
#include "lib/format-string.hpp"//////////////////////////////////TODO
#include "lib/format-cout.hpp"  //////////////////////////////////TODO
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
  using util::_Fmt;//////////////////////////////////TODO
  
  namespace {
    const gdouble DISTANCE_THRESHOLD = 5.0;
    
    /** heuristics for sizing the inline buffer
        where the Subject will construct its Observer/Adapter */
    const size_t OBSERVER_BUFF_SIZ = sizeof(void*)
                                   + sizeof(Symbol)
                                   + 4 * sizeof(void*);
  }

  
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
   * @remarks
   *     - tracking of signal connections _is not relevant here,_ since this controller
   *       is managed by the UI backbone and thus ensured to outlive any event processing.
   *     - this concrete controller handles _all_ drag-relocate gestures for all widgets
   *     - the actual Subject (widget) and command are bound into the signal wiring
   *     - whenever a new gesture possibly starts, we pick up these arguments and
   *       store them into the current gesture state within this object's fields.
   * @todo WIP-WIP as of /3/2021
   * ///////////////////////////////////TODO do we need a translation unit interaction-state.cpp (otherwise delete it!)
   */
  class DragRelocateController
    : public InteractionState
    {
      bool buttonPressed_ = false;
      Subject* subject_ = nullptr;
      bool isInFormation_ = false;
      gdouble anchorX_ = 0.0;
      gdouble anchorY_ = 0.0;
      
      using Observer = lib::InPlaceBuffer<GestureObserver, OBSERVER_BUFF_SIZ, InactiveObserver>;
      
      Observer observer_;
      
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
              [&, cmdID](GdkEventMotion* motion) -> bool
                {
                  try{ return maybeActivate(cmdID, subject, motion); }
                  ON_EXCEPTION_RETURN (false, "activate dragging gesture")
                }
          );
        }
      
      bool
      watchButton (GdkEventButton* button_event)  noexcept
        {
          REQUIRE (button_event);
          if (GDK_BUTTON_PRESS == button_event->type)
            buttonPressed_ = true;
          else
          if (GDK_BUTTON_RELEASE == button_event->type)
            {
              buttonPressed_ = false;
              if (isActive())
                doCompleteGesture();
              resetState();
            }
          std::cerr << _Fmt{"BUTT %s flag=%d"} % buttonPressed_ % button_event->type << std::endl;
          return false;
        }
      
      /** Gesture detection state logic */
      bool
      maybeActivate (Symbol cmdID, Subject& subject, GdkEventMotion* motion_event)
        {
          if (not buttonPressed_)
              return false; // Event not handled by this controller
          REQUIRE (motion_event);
          std::cerr << _Fmt{"MOVE x=%3.1f y=%3.1f subject=%s"}
                       % motion_event->x_root
                       % motion_event->y_root
                       % subject
                    << std::endl;
          if (not isAnchored())
            anchor (cmdID, subject, motion_event);
          if (not isActive())
            {
              probeActivation (motion_event);
              if (isActive())
                initGestureTracking(cmdID, subject);
              return false;
            }
          else
            {
              doTrackGesture (motion_event);
              return true; // Event handled
            }
        }
      
      
      /* === gesture implementation === */
      bool
      isActive()
        {
          return subject_
             and isInFormation_;
        }
      
      bool
      isAnchored()
        {
          return bool{subject_};
        }
      
      void
      anchor (Symbol cmdID, Subject& subject, GdkEventMotion* motion_event)
        {
          REQUIRE (motion_event);
          this->subject_ = & subject;
          this->anchorX_ = motion_event->x_root;
          this->anchorY_ = motion_event->y_root;
          std::cerr << _Fmt{"ANCHOR at x=%3.1f y=%3.1f ('%s')"}
                           % anchorX_
                           % anchorY_
                           % cmdID
                    << std::endl;
        }
      
      void
      probeActivation (GdkEventMotion* motion_event)
        {
          isInFormation_ = DISTANCE_THRESHOLD < abs (motion_event->x_root - anchorX_)
                        or DISTANCE_THRESHOLD < abs (motion_event->y_root - anchorY_);
        }
      
      void
      initGestureTracking(Symbol cmdID, Subject& subject)
        {
          subject.buildGestureObserver (cmdID, Observer::Handle{observer_});
        }
      
      void
      doTrackGesture (GdkEventMotion* motion_event)
        {
          REQUIRE (motion_event);
          gdouble deltaX = motion_event->x_root - this->anchorX_;
          gdouble deltaY = motion_event->y_root - this->anchorY_;
          // notify Subject to feed current delta
          observer_->updateOffset (deltaX, deltaY);
        }
      
      void
      doCompleteGesture()
        {
          observer_->markGestureCompleted();
        }
      
      void
      resetState()
        {
          isInFormation_ = false;
          anchorX_ = anchorY_ = 0.0;
          subject_ = nullptr;
          observer_.reset();
        }
      
      
    public:
      DragRelocateController()
        : observer_{}
        { }
      
    private:
    };
  
  
  
}} // namespace stage::interact
#endif /*STAGE_INTERACT_DRAG_RELOCATE_CONTROLLER_H*/
