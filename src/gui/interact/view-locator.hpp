/*
  VIEW-LOCATOR.hpp  -  access and allocation of UI component views

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


/** @file view-locator.hpp
 ** Access and allocation of UI component views.
 ** Within the Lumiera UI, a _component view_ is a building block to deal with some component
 ** of relevance to _»the model«. As such, all component views exhibit some distinctive traits:
 ** - they conform to a built-in fixed list of view types, each of which is unique and dedicated
 **   to a very specific purpose: *Timeline*, *Viewer*, (Asset)*Bin*, *Infobox*, *Playcontrol*,...
 ** - each component view has a distinguishable identity and is connected to and addressable
 **   through the [UI-Bus](\ref ui-bus.hpp).
 ** - it can be hosted only at a dedicated location within one or several specific [docking panels](\ref panel.hpp).
 ** - multiplicity (one, one per window, many) depends on the type of view and needs to be managed.
 ** - such a view is not just _created_ -- rather it needs to be _allocated_
 ** 
 ** ## LocationQuery and the View-Spec DSL
 ** 
 ** Implementation wise, there is a tight connection between the ViewLocator service, the Navigator service
 ** and the configuration how and where to create standard view elements (the "View-Spec DSL"). Several interactions
 ** require the UI to access or create some specific view widget by side-effect. And the logic how and where to create
 ** views can be quite intricate and is closely related to global, overarching topics of interaction design. Lumiera
 ** thus relies on a framework for default configuration, and a matching mechanism to determine the location and
 ** creation modes of such views. This matching mechanism in turn requires an abstracted view on the UI seen as
 ** a topological tree structure of relevant entities (windows, panels, views,...) -- which is basically the
 ** service provided by the Navigator; yet this Navigator service can be abstracted into the gui::interact::LocationQuery
 ** API, and this abstraction allows to keep all the intricacies of navigating concrete UI widgets confined within the
 ** implementation of the Navigator service.
 ** 
 ** @todo WIP 9/2017 early draft       ////////////////////////////////////////////////////////////TICKET #1104
 ** 
 ** @see interaction-director.hpp
 ** @see panel-locator.hpp
 */


#ifndef GUI_INTERACT_VIEW_LOCATOR_H
#define GUI_INTERACT_VIEW_LOCATOR_H

#include "lib/depend-inject.hpp"
#include "gui/id-scheme.hpp"
#include "gui/interact/view-spec-dsl.hpp"
#include "gui/model/element-access.hpp"
#include "lib/idi/genfunc.hpp"
#include "lib/symbol.hpp"
#include "lib/nocopy.hpp"

#include <functional>
#include <string>
#include <memory>


namespace gui {
namespace ctrl{
  class PanelLocator;
}
namespace interact {
  
  using std::unique_ptr;
  using std::string;
  using lib::Symbol;
  
  class UILocationSolver;
  
  
  
  /**
   * Access or allocate a UI component view
   * 
   * @todo initial draft as of 9/2017 -- actual implementation need to be filled in
   */
  class ViewLocator
    : ::util::NonCopyable
    {
      using Service_LocationSolver = lib::DependInject<UILocationSolver>::ServiceInstance<>;
      
      lib::Depend<model::ElementAccess>  elementAccess;
      Service_LocationSolver locResolver_;
      
      
    public:
      ViewLocator ();
     ~ViewLocator();
      
      
      /**
       * Access and possibly create _just some_ component view of the desired type
       */
      template<class V>
      V& get();
      
    private:
      /* === accessors to sibling global services  === */
      
    };
  
  
  
  template<class V>
  inline V&
  ViewLocator::get()
  {
    auto& viewSpec = idi::viewSpec<V>();
    Symbol viewID{lib::idi::typeSymbol<V>()};
    
    UICoord targetLocation = viewSpec.locate(viewID);
    UICoord realView       = viewSpec.alloc (targetLocation);
    
    return elementAccess().access<V> (realView);
  }
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_VIEW_LOCATOR_H*/
