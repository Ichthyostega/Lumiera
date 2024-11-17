/*
  UiState  -  manage persistent interface state

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file ui-state.cpp
 ** Implementation of a manager to establish and store global persistent UI state.
 ** 
 */


#include "stage/ctrl/ui-state.hpp"
#include "stage/ctrl/state-manager.hpp"
#include "stage/interact/focus-tracker.hpp"
//#include "stage/ctrl/global-ctx.hpp"
//#include "lib/util.hpp"

//using util::cStr;
//using util::isnil;


namespace stage {
namespace ctrl {
  
  using interact::FocusTracker;
  
  
  // emit VTable here...
  StateManager::~StateManager()
    { }
  
  // emit nested dtor code here...
  UiState::~UiState()
    { }
  
  
  /**
   * Initialise storage for global interface state
   * @see ctrl::CoreService
   */
  UiState::UiState (StateManager& stateManager, FocusTracker& focusTracker)
    : stateManager_{stateManager}
    , tracker_{focusTracker}
    { }
  
  
  /** */
  
  
  
}}// namespace stage::ctrl
