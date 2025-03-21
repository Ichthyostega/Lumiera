/*
  WORKSPACE-WINDOW.hpp  -  the main workspace window of the GUI

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file workspace-window.hpp
 ** This file contains the definition of the main workspace window
 ** parent, which is the toplevel parent of the whole workspace.
 ** 
 ** @see actions.hpp
 */

#ifndef STAGE_WORKSPACE_WORKSPACE_WINDOW_H
#define STAGE_WORKSPACE_WORKSPACE_WINDOW_H


#include "stage/gtk-base.hpp"
#include "stage/workspace/panel-manager.hpp"


namespace stage {
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
  
  
}}// namespace stage::workspace
#endif /*STAGE_WORKSPACE_WORKSPACE_WINDOW_H*/
