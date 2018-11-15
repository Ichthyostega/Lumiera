/*
  UiState  -  manage persistent interface state

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

* *****************************************************/


/** @file ui-state.cpp
 ** Implementation of a manager to establish and store global persistent UI state.
 ** 
 */


#include "gui/ctrl/ui-state.hpp"
#include "gui/ctrl/state-manager.hpp"
#include "gui/interact/focus-tracker.hpp"
//#include "gui/ctrl/global-ctx.hpp"
//#include "lib/util.hpp"

//using util::cStr;
//using util::isnil;


namespace gui {
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
  
  
  
}}// namespace gui::ctrl
