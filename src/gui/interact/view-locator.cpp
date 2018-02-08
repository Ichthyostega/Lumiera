/*
  ViewLocator  -  access and allocation of UI component views

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


/** @file view-locator.cpp
 ** Implementation details of a machinery to allocate UI global component views.
 ** Essentially this compilation unit hosts the implementation backing the [ViewSpec-DSL](view-spec-dsl.hpp)
 ** and the [Location Solver](ui-location-solver.hpp) to drive evaluation of the standard location defaults,
 ** which are [hard-wired into the UI](\ref id-scheme.hpp). For this integration of various facilities and
 ** frameworks we rely on actual definitions for the DSL-tokens, which themselves are functors and tied by
 ** lambda-binding into the implementation side of ViewLocator; which both acts as a front-end to access
 ** the service to "get or create" a component and also holds the UILocationSolver component to perform
 ** the actual location solving mechanism. The DSL thus sits between both poles, since _resolving a view_
 ** draws upon the default locating configuration encoded as DSL tokens, which in turn call back into
 ** the implementation services within ViewLocator to resolve those partial location specifications
 ** against the currently existing UI topology and then to access or create new UI components as
 ** laid out through those definitions.
 ** 
 ** @todo WIP 9/2017 early draft       ////////////////////////////////////////////////////////////TICKET #1104
 */


#include "gui/interact/view-locator.hpp"
#include "gui/interact/ui-location-solver.hpp"
#include "gui/ctrl/panel-locator.hpp"
#include "gui/ctrl/window-locator.hpp"
#include "gui/interact/ui-coord-resolver.hpp"
#include "gui/ctrl/global-ctx.hpp"
#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//using util::cStr;
//using util::isnil;
using lib::Symbol;
using gui::ctrl::PanelLocator;
using gui::ctrl::WindowLocator;


namespace gui {
namespace interact {
  
  namespace error = lumiera::error;
  
  
  /* ==== definitions and concrete bindings for the View-Spec-DSL ==== */
  
  Symbol UIC_CURRENT_WINDOW{"currentWindow"};
  Symbol UIC_FIRST_WINDOW  {"firstWindow"};
  Symbol UIC_ELIDED        {"."};
  
  
  
  namespace {
    const LocationQueryAccess LOCATION_QUERY_SERIVCE_NOT_AVAILABLE
      = []() -> LocationQuery&
          {
            throw error::State (error::LUMIERA_ERROR_LIFECYCLE
                               ,"No LocationQuery service available. Is the GUI running?");
          };
  }
  
  /** @internal global access point to some implementation of the LocationQuery API.
   * Typically, this is provided by the Navigator service in conjunction with the ViewLocator;
   * both are components managed by the InteractionDirector. Thus, when the UI starts, a suitable
   * access functor will be installed here, and likewise removed/disabled on shutdown.
   *                  ///////////////////////////////////////////////////////////////////////////////////////TICKET #1127 looks like we could get rid of that global state
   */
  LocationQueryAccess locationQuery = LOCATION_QUERY_SERIVCE_NOT_AVAILABLE;
  
  
  
  
  
  ViewLocator::ViewLocator (ctrl::GlobalCtx& uiTopLevel, LocationQueryAccess getLocQuery)
    : globals_{uiTopLevel}
    , locResolver_{new UILocationSolver{getLocQuery}}
    {
      locationQuery = getLocQuery;
    }
  
  // dtors via smart-ptr invoked from here...
  ViewLocator::~ViewLocator()
    {
      locationQuery = LOCATION_QUERY_SERIVCE_NOT_AVAILABLE;
    }
  
  
  
  /* === Service accessors within global context === */
  
  PanelLocator&
  ViewLocator::panelLocator()
  {
    return globals_.windowLoc_.locatePanel();
  }
  
  WindowLocator&
  ViewLocator::windowLocator()
  {
    return globals_.windowLoc_;
  }
  
  
  
  
  /** */
  
  
  
}}// namespace gui::interact
