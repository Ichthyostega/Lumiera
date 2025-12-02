/*
  ViewerPanel  -  Dockable panel to hold the video display widgets and controls

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file viewer-panel.cpp
 ** Implementation of a dockable panel with a video display widget
 */

#include "stage/gtk-base.hpp"
#include "stage/panel/viewer-panel.hpp"

#include "stage/workspace/workspace-window.hpp"
#include "stage/ui-bus.hpp"  ///////////////////////////////////TODO why are we forced to include this after workspace-window.hpp ??  Ambiguity between std::ref and boost::reference_wrapper


namespace stage {
namespace panel {
  
  ViewerPanel::ViewerPanel (workspace::PanelManager& panelManager
                           ,Gdl::DockItem& dockItem)
    : Panel{panelManager, dockItem, getTitle(), getStockID()}
    , display_{}
    , demoPlayback_{}
    {
      // wire control and frame-output signals...
      display_.signal_activate.connect(
        sigc::mem_fun (demoPlayback_, &ctrl::DemoController::activate));
      demoPlayback_.output_.connect(
        sigc::mem_fun (display_, &widget::VideoDisplayWidget::pushFrame));
      
      pack_start(display_, Gtk::PACK_EXPAND_WIDGET);
    }
  
  const char*
  ViewerPanel::getTitle()
  {
    return _("Viewer");
  }
  
  const gchar*
  ViewerPanel::getStockID()
  {
    return "panel_viewer";
  }
  
  
}}// namespace stage::panel
