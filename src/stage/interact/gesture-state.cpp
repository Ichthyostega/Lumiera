/*
  GestureState  -  holder for interaction-state dedicated to specific gestures

   Copyright (C)
     2021,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file gesture-state.cpp
 ** Implementation of a holder and manager for interaction state.
 ** 
 ** @todo do we really need a segregated translation unit for the
 **     InteractionState implementations? The alternative would be
 **     to emit all the related code from interaction-director.cpp;
 **     and in this alternative solution, we'd also allocate all the
 **     interaction state right within GestureState, without PImpl.
 **     The benefit would to avoid yet another indirection, while
 **     the downside would be to include all those implementations
 **     unnecessarily also into cmd-context.cpp, with the danger that
 **     someone later on starts to "short circuit" directly into some
 **     implementation, by switching on type.
 */


#include "stage/gtk-base.hpp"
#include "stage/interact/gesture-state.hpp"
#include "stage/interact/drag-relocate-controller.hpp"
#include "lib/format-string.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//#include <string>
//#include <map>
using util::_Fmt;

//using std::map;
//using std::string;

//using util::contains;
//using util::isnil;

namespace stage {
namespace interact {
  
  namespace error = lumiera::error;
  
  namespace { // internal details
    
  } // internal details
  
  
  
  GestureState::~GestureState() { }  // emit dtors of managed objects here....
  
  GestureState::GestureState()
    : dragRelocate_{new DragRelocateController{}}
    { }
  
  
  
  
  
  /**
   * @todo as of 4/2021, interactions and UI state handling can be considered an early draft;
   *       since the UI needs to be fleshed out in more detail in order to decide on a proper
   *       organisation and possibly flexible configuration. In the meantime, we use some
   *       hard wired mappings to actual InteractionState implementations
   */
  InteractionState&
  GestureState::getStateFor (Action action, Scope qualifier)
  {
    if (DRAG==action and ON_TIMELINE==qualifier)
      return *dragRelocate_;
    else
      throw error::State (_Fmt{"Unforeseen interaction state in Lumiera UI requested. "
                               "GestureState::getStateFor... Action=%d Scope=%d"}
                              % action
                              % qualifier);
  }
  
  
}} // namespace stage::interact
