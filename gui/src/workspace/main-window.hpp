/*
  main-window.hpp  -  Definition of the main workspace window object
 
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
/** @file mainwindow.hpp
 ** This file contains the definition of the main workspace window
 ** parent, which is the toplevel parent of the whole workspace.
 **
 ** @see actions.hpp
 */

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <gtkmm.h>
#include <libgdl-1.0/gdl/gdl-dock-layout.h>

#include "actions.hpp"

#include "../panels/assets.hpp"
#include "../panels/viewer.hpp"
#include "../panels/timeline.hpp"

using namespace lumiera::gui::panels;

namespace lumiera {
namespace gui {
namespace workspace {

  /** 
   * The main lumiera workspace window
   */
  class MainWindow : public Gtk::Window
    {
    public:
	    MainWindow();
	    virtual ~MainWindow();
	
    protected:
      void create_ui();

      /* ===== UI ===== */
    protected:
      Glib::RefPtr<Gtk::UIManager> uiManager;
      Gtk::VBox base_container;
      Gtk::HBox dock_container;
      
      Gtk::Widget *dock;
      Gtk::Widget *dockbar;
      GdlDockLayout *layout;

      /* ===== Panels ===== */
    protected:  
      Assets assets;
      Viewer viewer;      
      Timeline timeline;
            
      /* ===== Helpers ===== */
    protected:
      /**
       * The instantiation of the actions helper class, which
       * registers and handles user action events */
      Actions actions;
    };

}   // namespace workspace
}   // namespace gui
}   // namespace lumiera

#endif // MAIN_WINDOW_HPP
