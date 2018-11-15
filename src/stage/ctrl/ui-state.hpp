/*
  UI_STATE.hpp  -  manage persistent interface state

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


/** @file ui-state.hpp
 ** Establish and handle persistent interface state.
 ** For an elaborate interface to support work like editing film, it is crucial
 ** that the user can leave a part of the application for some time, only to find
 ** it in precisely the way as it was beforehand. And, most importantly, this property
 ** needs to hold beyond the bounds of a single editing session. This requirement covers
 ** various aspects of the interface, like what elements where expanded and collapsed,
 ** what detail- or zoom level was set, how the view window was positioned, but also
 ** some more specific details regarding configuration of individual widgets, like
 ** e.g. what time code format was used on some input. All these tiny details together
 ** allow the user to return to a familiar location; they encourage to put effort
 ** into arrange matters in a way well suited to the specific working style and
 ** requirements at hand.
 ** 
 ** # State persistence protocol
 ** For the Lumiera UI, the foundation to deliver such an experience are rooted
 ** right in the _backbone of the UI,_ which is the [UI-Bus](\ref ui-bus.hpp). The
 ** protocol complementing that structure defines a set of operations to emit _State Mark_
 ** messages, and later to play back rsp. receive such captured _State Marks._ The expectation
 ** regarding that operation protocol is for any [UI-Element](\ref tangible.hpp) to emit such
 ** messages, whenever some change happened, which this element deemed relevant to be conserved
 ** as part of its global persistent state. Obviously, each element is likewise expected to
 ** implement a handling function to receive those _State Marks_ later and to re-establish
 ** its presentation state to the shape as previously captured.
 ** 
 ** @see interaction-director.hpp
 ** @see ui-bus.hpp
 */


#ifndef GUI__H
#define GUI__H

#include "gui/gtk-base.hpp"
#include "lib/nocopy.hpp"

//#include <string>
//#include <memory>


namespace gui {
namespace interact { class FocusTracker; }
namespace ctrl {
  
//  using std::unique_ptr;
//  using std::string;
  
//  class GlobalCtx;
  class StateManager;
  
  
  
  /**
   * Store and manage persistent interface state.
   * A Service attached to the UiBus to handle and play back
   * _Stat Mark Messages_. Beyond that, the UiState service is
   * responsible for various global aspects of persistent UI state.
   * 
   * @todo initial draft as of 2/2017 -- actual implementation has to be filled in
   */
  class UiState
    : util::NonCopyable
    {
      StateManager& stateManager_;
      interact::FocusTracker& tracker_;
      
    public:
      UiState (StateManager&, interact::FocusTracker&);
     ~UiState();
      
    private:
      
    };
  
  
  
}}// namespace gui::workspace
#endif /*GUI_CTRL_UI_MANAGER_H*/
