/*
  UI-LOCATION-SOLVER.hpp  -  decide upon a possible location for some UI component

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file ui-location-resolver.hpp
 ** A solver to match incomplete coordinate specifications against the actual UI topology.
 ** Within the Lumiera UI, a _component view_ is typically _created or retrieved_ to live at some position
 ** within the tree-like topology of the interface. Such happens as a consequence of interaction or other
 ** events, and the logic as to where and how to place a new UI element shall not be intermingled with the
 ** actual event handling code. Rather, the ViewLocator, as a service related to the InteractionDirector,
 ** can be invoked to draw on some default configuration plus the actual UI topology present at this time.
 ** 
 ** @todo WIP 2/2018 early draft       ////////////////////////////////////////////////////////////TICKET #1127
 ** 
 ** @see UILocationResolver_test
 ** @see ViewSpecDSL_test
 ** @see UICoordResolver
 ** @see view-locator.hpp
 */


#ifndef GUI_INTERACT_UI_LOCATION_SOLVER_H
#define GUI_INTERACT_UI_LOCATION_SOLVER_H

#include "lib/error.hpp"
//#include "lib/symbol.hpp"
//#include "lib/meta/function.hpp"
//#include "lib/meta/tuple-helper.hpp"
//#include "lib/meta/function-closure.hpp"
#include "gui/interact/ui-coord.hpp"
//#include "gui/interact/ui-coord-resolver.hpp"

//#include <functional>
#include <utility>


namespace gui {
namespace interact {
  
//  using std::forward;
  
  class LocationQuery;
  
  
  /**
   * Access or allocate a UI component view
   * 
   * @todo initial draft as of 9/2017 -- actual implementation need to be filled in
   */
  class UILocationSolver
    : boost::noncopyable
    {
//      ctrl::GlobalCtx& globals_;
      
    public:
      UILocationSolver()
        { }
      
      
      /**
       * Access and possibly create _just some_ component view of the desired type
       */
      UICoord solve();
      
    private:
      
    };
  
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_UI_LOCATION_SOLVER_H*/
