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
 ** docking panels on a conceptual level
 ** 
 ** @see view-locator.hpp
 ** @see interaction-director.hpp
 */


#ifndef GUI_CTRL_PANEL_LOCATOR_H
#define GUI_CTRL_PANEL_LOCATOR_H

#include "gui/gtk-base.hpp"
#include "lib/nocopy.hpp"

#include <memory>
#include <list>


namespace gui {
namespace workspace { class WorkspaceWindow; }
namespace ctrl {
  
  
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
      
    private:
      
    };
  
  
  
  /** */
  
  
  
}}// namespace gui::ctrl
#endif /*GUI_CTRL_PANEL_LOCATOR_H*/
