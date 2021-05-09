/*
  CMD-CONTEXT.hpp  -  interface to access command context binding services within the UI

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file cmd-context.hpp
 ** Abstraction: support for binding command invocation into an UI context.
 ** The interact::CmdContext front-end is used when commands need to be bound to a whole context
 ** of UI elements rather than to a specific widget -- especially for picking up command arguments
 ** from interaction state found within that context. To invoke a command, typically it is necessary
 ** to prepare a _command instance_ and to pick up and bind _command arguments._ In most cases, this
 ** can be done directly, using a fixed command-ID and locally known argument values. But in the
 ** aforementioned special situation, we have to deal with ongoing user interactions, which might
 ** span several widgets, where some interaction events just happen to make invocation of a specific
 ** command viable. The prototypical example is when some element is dragged around, and, depending
 ** on the vicinity, might cause quite different actions when dropped. Such behaviour can be achieved
 ** with the help of an intermediary, known as ["interaction state manager"](\ref InteractionState).
 ** The command context accessor allows to discover a specific InteractionState instance, which
 ** - is responsible for the specific command to be invoked
 ** - can handle context information related to a specific _control system_ (e.g mouse, keyboard,
 **   hardware controller, pen)
 ** - might observe changes in contextual state and thus decide if a command can be invoked
 ** From the InteractionState instance, it is possible to retrieve a notification when a specific,
 ** context-bound command becomes executable by picking up suitable parameter values from this context.
 ** 
 ** @todo as of 3/2017 this is a early design draft and WIP-WIP-WIP
 ** @todo after initial design draft, postponed as of 10/2017
 ** 
 ** @see CmdContext_test
 ** @see ////TODO_test usage example
 ** 
 */


#ifndef STAGE_INTERACT_CMD_CONTEXT_H
#define STAGE_INTERACT_CMD_CONTEXT_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
//#include "stage/ctrl/bus-term.hpp"
//#include "lib/idi/entry-id.hpp"
#include "lib/hash-indexed.hpp"
#include "lib/opaque-holder.hpp"
#include "lib/symbol.hpp"
#include "lib/nocopy.hpp"
//#include "lib/util.hpp"

//#include <string>
#include <utility>

namespace Gtk {
  class Widget;
}
namespace stage {
namespace interact {
  
//  using lib::HashVal;
  using lib::hash::LuidH;
  using lib::Symbol;
//  using util::isnil;
//  using std::string;
  
  class InteractionState;
  
  
  
  /**
   * Collaboration interface for tracking the formation of a gesture.
   * The Subject (of the Gesture) provides an actual GestureObserver
   * as an adapter, and the gesture controller uses this adapter to push
   * events to track and form the gesture. When the conditions for completing
   * the gesture are met, the Subject will be notified through this adapter
   * and might issue a command to propagate the results of the gesture,
   * while the gesture controller, after this notification, proceeds to
   * disable and discard the gesture context and return to passive mode,
   * waiting for the next trigger condition.
   */
  class GestureObserver
    : public util::MoveAssign
    {
      Symbol cmdID_;
      
    public:
      virtual ~GestureObserver();  ///< this is an interface
      
      GestureObserver ()             : cmdID_(Symbol::BOTTOM){ }
      GestureObserver (Symbol cmdID) : cmdID_(cmdID)         { }
      
      GestureObserver (GestureObserver&&)             = default;
      GestureObserver& operator= (GestureObserver&&)  = default;
      
      
      Symbol getCmdID()  const { return cmdID_; }
      
      virtual void updateOffset (double deltaX, double deltaY) =0;
      virtual void markGestureCompleted()                      =0;
    };
  
  
  /** »Null Object« when no gesture is being tracked currently */
  class InactiveObserver
    : public GestureObserver
    {
      void updateOffset (double, double)  override { /*NOOP*/ }
      void markGestureCompleted()         override { /*NOOP*/ }
    };
  
  
  
  /**
   * Role-Interface: the Subject of Interaction.
   * An entity implementing the Subject interface can be targeted by gestures,
   * finally leading to the invocation of a specific command on that subject,
   * with parameters picked up from the gesture invocation (e.g dragging).
   */
  class Subject
    : util::NonCopyable
    {
    protected:
        virtual ~Subject();  ///< this is an interface
        
    public:
        virtual Gtk::Widget& exposeWidget()                                     =0;
        virtual void fireGesture (Symbol cmdID)                                 =0;
        virtual void gestureOffset (Symbol cmdID, double deltaX, double deltaY) =0;
        
        using Buffer = lib::PlantingHandle<GestureObserver, InactiveObserver>;
        virtual void buildGestureObserver (Symbol cmdID, Buffer)                =0;
    };
  
  
  
  /**
   * Builder to define a binding to relate some entity or place within the UI
   * with a specific from of interaction gesture or context dependent command invocation.
   * @todo write type comment...
   */
  class CmdContext
    : util::MoveOnly
    {
      InteractionState& iState_;
      Symbol cmdID_;
      
      /* ===== Builder State ===== */
      Subject* subject_{nullptr};
      
      CmdContext (InteractionState& iState, Symbol cmdID)
        : iState_{iState}
        , cmdID_{cmdID}
        { }
    public:
      // using the default move-ctor
        
      /* === access front-end === */
      static CmdContext of (Symbol cmdID, Symbol ctxID);
      
      struct Resolver;
      
      /** Builder operation: define the subject to use for the following interaction bindings. */
      CmdContext&& linkSubject (Subject& subj) { this->subject_ = &subj; return std::move (*this); }
      
      /** Terminal builder operation: establish the infrastructure
       *  for the already defined participants to be involved into a
       *  dragging gesture, for the purpose of relocating the subject.
       * @remark Typical example would be dragging a Clip within the timeline.
       *    This invocation would then create a suitable InteractionState subclass
       *    and use the previously given Subject to hook up signal bindings for the
       *    trigger condition to start forming the "drag a clip" gesture.
       */
      void setupRelocateDrag();
      
    protected:
    private:
    };
  
  
  /**
   * Helper for dynamic command argument resolution.
   * @todo 4/2021 this was part of a design draft in 2017, not clear yet if we need it
   */
  struct CmdContext::Resolver
    {
      
      /** query current interaction state to resolve the element in question
       * @return hash-ID to indicate the element or scope
       */
      operator LuidH ();
      
      
      enum Spec {
        HERE,
        RECENT
      };
      
      
      friend Resolver
      scope (Spec scopeSpec)
      {
        return Resolver{scopeSpec};
      }
      
      
      friend Resolver
      element (Spec elementSpec)
      {
        return Resolver{elementSpec};
      }
      
    private:
      const Spec spec = HERE;
      
      Resolver(Spec s)
        : spec(s)
        { }
    };
  
  
}} // namespace stage::interact
#endif /*STAGE_INTERACT_CMD_CONTEXT_H*/
