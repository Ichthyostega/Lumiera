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
 ** 
 ** @todo WIP 9/2017 early draft       ////////////////////////////////////////////////////////////TICKET #1104
 */


#include "gui/interact/view-locator.hpp"
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
  
  /* ==== definitions and concrete bindings for the View-Spec-DSL ==== */
  
  Symbol UIC_CURRENT_WINDOW{"currentWindow"};
  Symbol UIC_FIRST_WINDOW  {"firstWindow"};
  Symbol UIC_ELIDED        {"."};
  
  
  // dtors via smart-ptr invoked from here...
  ViewLocator::~ViewLocator() { }
  
  
  ViewLocator::ViewLocator (ctrl::GlobalCtx& uiTopLevel, std::function<LocationQuery&()> getLocQuery)
    : globals_{uiTopLevel}
    , locationQuery{getLocQuery}
    { }
  
  
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
