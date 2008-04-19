/*
  workspace-window.hpp  -  Definition of the main workspace window object
 
  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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

#ifndef WORKSPACE_WINDOW_HPP
#define WORKSPACE_WINDOW_HPP

#include <gtkmm.h>
#include <libgdl-1.0/gdl/gdl-dock-layout.h>

#include "actions.hpp"

#include "../panels/assets.hpp"
#include "../panels/viewer.hpp"
#include "../panels/timeline-panel.hpp"

using namespace lumiera::gui::panels;

namespace lumiera {
namespace gui {

namespace model {
  class Project;
} // model

namespace workspace {

  /** 
   * The main lumiera workspace window
   */
  class WorkspaceWindow : public Gtk::Window
    {
    public:
	    WorkspaceWindow(lumiera::gui::model::Project *source_project);
	    virtual ~WorkspaceWindow();
	
    private:
      void create_ui();

      /* ===== Model ===== */
    private:
      lumiera::gui::model::Project *project;

      /* ===== UI ===== */
    private:
      Glib::RefPtr<Gtk::UIManager> uiManager;
      Gtk::VBox base_container;
      Gtk::HBox dock_container;
      
      Gtk::Widget *dock;
      Gtk::Widget *dockbar;
      GdlDockLayout *layout;

      /* ===== Panels ===== */
    private:  
      Assets assets;
      Viewer viewer;      
      TimelinePanel timeline;
           
      /* ===== Helpers ===== */
    private:
      /**
       * The instantiation of the actions helper class, which
       * registers and handles user action events */
      Actions actions;

      friend class Actions;
    };

}   // namespace workspace
}   // namespace gui
}   // namespace lumiera

#endif // WORKSPACE_WINDOW_HPP
