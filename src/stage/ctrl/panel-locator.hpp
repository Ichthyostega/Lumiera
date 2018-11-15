/*
  PANEL-LOCATOR.hpp  -  manage all top level windows

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


/** @file panel-locator.hpp
 ** Access and query front-end to locate, access and place docking panels.
 ** This service is an implementation detail of the global WindowLocator and
 ** allows to abstract from the concrete top-level window when dealing with
 ** docking panels on a conceptual level.
 ** 
 ** @todo 8/2018 refactoring of docking panel handling is underway    ///////////////////////////////////////TICKET # 1144
 **       The PanelLocator is planned to become the one central coordinating
 **       manager, while the existing panel managers in each window will be
 **       demoted to a mere DockArea, corresponding to a `gdl::Dock`
 ** 
 ** @todo 8/2018 there is some overlap with the (not yet fully functional)
 **       ViewLocator in the InteractionDirector. Right now, PanelLocator
 **       fills some gaps here and offers preliminary solutions for some
 **       lookup and allocation tasks ultimately to be performed on a more
 **       abstract level through the ViewSpec-DSL. This corresponds to the
 **       observation, that PanelLocator and WindowLocator are somewhat
 **       tangled currently. The goal is to turn them into access services,
 **       and shift the allocation logic to the aforementioned higher level.
 ** 
 ** @see view-locator.hpp
 ** @see interaction-director.hpp
 */


#ifndef GUI_CTRL_PANEL_LOCATOR_H
#define GUI_CTRL_PANEL_LOCATOR_H

#include "stage/gtk-base.hpp"
#include "stage/workspace/panel-manager.hpp"
#include "lib/format-string.hpp"
#include "lib/meta/util.hpp"
#include "lib/nocopy.hpp"

#include <type_traits>
#include <memory>
#include <list>


namespace gui {
namespace workspace {
  class WorkspaceWindow;
}
namespace panel {
  class Panel;
}
namespace ctrl {
  
  namespace error = lumiera::error;
  
  using panel::Panel;
  using util::_Fmt;
  using std::list;
  
  
  
  /**
   * Service to access, locate or place panels within top-level windows
   * Access- and query front-end to the individual panel managers, which are
   * located within the top-level windows.
   */
  class PanelLocator
    : util::NonCopyable
    {
      using PWindow = std::shared_ptr<workspace::WorkspaceWindow>;
      using WindowList = list<PWindow>;
      
      WindowList& windowList_;
      
      
    public:
      PanelLocator (WindowList&);
      
      /** lookup or allocate "the" instance of the indicated Panel(subtype).
       *  While there can be multiple instances of some panel type in various
       *  windows, this function will look through a canonical search order
       *  to either retrieve the first instance found, or create a new one
       *  in the _primary window_.
       * @return reference to the indicated Panel subclass (down-casted)
       * @throw error::Invalid when requesting a target class not recognised
       *    as known docking panel entity.
       * @deprecated 8/2018 the (planned) ViewLocator will perform essentially
       *    the same task, albeit on a more flexible and configurable level.
       */
      template<class PAN>
      PAN& find_or_create();
      
      
    private:
      panel::Panel& preliminary_impl_PanelLookup (int typeID);
    };
  
  
  
  /** @remark delegating front-end for type safety and error handling. */
  template<class PAN>
  inline PAN&
  PanelLocator::find_or_create()
  {
    static_assert (std::is_base_of<panel::Panel, PAN>(), "Lumiera Panel subclass expected");
        
    int typeID = workspace::PanelManager::findPanelID<PAN>();
    if (-1 == typeID)
      throw error::Invalid (_Fmt{"Requested type «%s» not usable as docking panel."}
                                % util::typeStr<PAN>()
                           ,error::LERR_(WRONG_TYPE));
    
    return dynamic_cast<PAN&> (preliminary_impl_PanelLookup (typeID));
  }
  
  
  
}}// namespace gui::ctrl
#endif /*GUI_CTRL_PANEL_LOCATOR_H*/
