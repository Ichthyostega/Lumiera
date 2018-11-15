/*
  WORKSPACE-WINDOW.hpp  -  the main workspace window of the GUI

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

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


/** @file workspace-window.hpp
 ** This file contains the definition of the main workspace window
 ** parent, which is the toplevel parent of the whole workspace.
 ** 
 ** @see actions.hpp
 */

#ifndef GUI_WORKSPACE_WORKSPACE_WINDOW_H
#define GUI_WORKSPACE_WORKSPACE_WINDOW_H


#include "stage/gtk-base.hpp"
#include "stage/workspace/panel-manager.hpp"


namespace gui {
namespace ctrl {
  class UiManager;
}
namespace workspace {
  
  
  /**
   * The main Lumiera workspace window
   */
  class WorkspaceWindow
    : public Gtk::Window
    {
    public:
      WorkspaceWindow (ctrl::UiManager&);
     ~WorkspaceWindow();
      
      PanelManager& getPanelManager();
      
      
    private:
      void createUI (ctrl::UiManager& uiManager);
      
      
      /* ===== UI ===== */
      Gtk::VBox baseContainer_;
      Gtk::HBox dockContainer_;
      
      PanelManager panelManager_;
      
      //----- Status Bar -----//
      Gtk::Statusbar statusBar_;
      
    };
  
  
}}// namespace gui::workspace
#endif /*GUI_WORKSPACE_WORKSPACE_WINDOW_H*/
