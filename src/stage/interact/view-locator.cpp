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
 ** Essentially this compilation unit hosts the implementation backing the [ViewSpec-DSL](\ref view-spec-dsl.hpp)
 ** and the [Location Solver](\ref ui-location-solver.hpp) to drive evaluation of the standard location defaults,
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


#include "stage/gtk-base.hpp"
#include "stage/interact/view-locator.hpp"
#include "stage/interact/view-spec-dsl.hpp"
#include "stage/interact/ui-location-solver.hpp"
#include "stage/ctrl/panel-locator.hpp"
#include "stage/model/element-access.hpp"
#include "stage/interact/ui-coord-resolver.hpp"
#include "lib/depend.hpp"
#include "lib/symbol.hpp"
//#include "lib/util.hpp"

//using util::cStr;
//using util::isnil;
using lib::Symbol;
//using stage::ctrl::PanelLocator;
//using stage::ctrl::WindowLocator;
using stage::model::ElementAccess;


namespace stage {
  namespace error = lumiera::error;
  
  
  
  /* ==== definitions and concrete bindings for the View-Spec-DSL ==== */
  
  namespace idi { // Namespace for the actual ViewSpec DSL tokens
    
    
    AllocSpec<uint> limitAllocation =[](UICoord target, uint limit)
                                       {
                                         lib::Depend<model::ElementAccess> rawElementAccessor;
                                         return rawElementAccessor().locate_or_create (target, limit);
                                       };
  
  }
  namespace interact {
    
    const Symbol UIC_CURRENT_WINDOW{"currentWindow"};
    const Symbol UIC_FIRST_WINDOW  {"firstWindow"};
    const Symbol UIC_ELIDED        {"."};
    
    
    
    
    ViewLocator::ViewLocator ()
      : locResolver_{LocationQuery::service}
      { }
    
    // dtors via smart-ptr invoked from here...
    ViewLocator::~ViewLocator() { }
    
    
    
    /* === Service accessors within global context === */
    
  //  PanelLocator&
  //  ViewLocator::panelLocator()
  //  {
  //    return windowLoc_.locatePanel();
  //  }
    
    
    
    
    /** */
    
    
    
}}// namespace stage::interact
