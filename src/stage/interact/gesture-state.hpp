/*
  GESTURE-STATE.hpp  -  holder for interaction-state dedicated to specific gestures

   Copyright (C)
     2015,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
#include <memory>


namespace stage {
namespace interact {
  
//  using lib::HashVal;
  using lib::Symbol;
//  using util::isnil;
//  using std::string;
  using std::unique_ptr;
  
//class Subject;
  class DragRelocateController;
  
  
  /**
   * Holder for InteractionState dedicated to UI gestures and complex interactions.
   * 
   * @todo write type comment...
   * @todo WIP-WIP as of 3/2021
   * @todo as of 3/2021 the whole scheme for addressing gestures and actual state implementations is rather just guesswork
   * ///////////////////////////////////TODO do we need a translation unit interaction-state.cpp (otherwise delete it!)
   */
  class GestureState
    : util::NonCopyable
    {
      unique_ptr<DragRelocateController> dragRelocate_;
      
    public:
      GestureState();
     ~GestureState();
      
    public:
      enum Action {
        DRAG,
        UNKNOWN //////TODO
      };
      enum Scope {
        ON_TIMELINE,
        OTHER //////TODO
      };
      
      /**
       * Decode the classification of the kind of interaction and gesture,
       * and thus translate to a concrete InteractionState implementation.
       * @return reference to the actual UI state with respect to the
       *     specific gesture requested, which can then e.g. be used
       *     to wire a specific UI widget to become responsive to
       *     this kind of gesture.
       */
      InteractionState& getStateFor (Action action, Scope qualifier);
      
    private:
    };
  
  
  
}} // namespace stage::interact
#endif /*STAGE_INTERACT_GESTURE_STATE_H*/
