/*
  WorkspaceWindow  -  the main workspace window of the GUI

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file workspace-window.cpp
 ** Implementation of base functionality for workspace windows.
 */

#include "stage/gtk-base.hpp"
#include "stage/config-keys.hpp"
#include "stage/workspace/workspace-window.hpp"
#include "stage/workspace/panel-manager.hpp"
#include "stage/ctrl/ui-manager.hpp"
#include "include/logging.h"




namespace stage {
namespace workspace {
  
  using ctrl::UiManager;
  
  
  WorkspaceWindow::WorkspaceWindow (UiManager& uiManager)
    : panelManager_(*this)
    {    
      createUI (uiManager);
    }
  
  
  WorkspaceWindow::~WorkspaceWindow()
  {
    INFO (gui_dbg, "Closing workspace window...");
  }
  
  
  
  PanelManager&
  WorkspaceWindow::getPanelManager()
  {
    return panelManager_;
  }
  
  
  void
  WorkspaceWindow::createUI (UiManager& uiManager)
  {
    signal_show ().connect_notify(mem_fun(uiManager, &UiManager::updateWindowFocusRelatedActions));   ///////////////TICKET #1076  find out how to handle this properly
    add_accel_group (uiManager.get_accel_group());
    
    // RTL Test Code
    //set_default_direction (TEXT_DIR_RTL);
    
    //----- Configure the Window -----//
    set_title (Config::get (KEY_TITLE));
    set_default_size (1024, 768);
    
    //----- Set up the UI Manager -----//
    // The UI will be nested within a VBox
    add (baseContainer_);

    //----- Set up the Menu Bar -----//
    Gtk::Widget* menu_bar = uiManager.get_widget ("/MenuBar");
    REQUIRE (menu_bar != NULL);
    baseContainer_.pack_start(*menu_bar, Gtk::PACK_SHRINK);
    
    //----- Create the Docks -----//
    panelManager_.setupDock();
    dockContainer_.pack_start (panelManager_.getDockBar(), false,false,0);
    dockContainer_.pack_start (panelManager_.getDock(), true,true,0);
    baseContainer_.pack_start (dockContainer_, Gtk::PACK_EXPAND_WIDGET);
    
    //----- Create the status bar -----//
    //statusBar.set_has_resize_grip();
    statusBar_.set_resize_mode (Gtk::RESIZE_PARENT);
    baseContainer_.pack_start (statusBar_, Gtk::PACK_SHRINK);
    
    show_all_children();
  }
  
  
  
}}// namespace stage::workspace
