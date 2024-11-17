/*
  ViewerPanel  -  Dockable panel to hold the video display widgets and controls

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file viewer-panel.cpp
 ** Implementation of a dockable panel to hold the video viewers
 */

#include "stage/gtk-base.hpp"
#include "stage/panel/viewer-panel.hpp"

#include "stage/workspace/workspace-window.hpp"
#include "stage/ui-bus.hpp"  ///////////////////////////////////TODO why are we forced to include this after workspace-window.hpp ??  Ambiguity between std::ref and boost::reference_wrapper
#include "stage/display-service.hpp"


using namespace Gtk;                   ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace stage::widget;           ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace stage::controller;       ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace stage {
namespace panel {
  
  ViewerPanel::ViewerPanel (workspace::PanelManager& panelManager
                           ,Gdl::DockItem& dockItem)
    : Panel(panelManager, dockItem, getTitle(), getStockID())
    , playbackController_{}
    {
      //----- Pack in the Widgets -----//
      pack_start(display_, PACK_EXPAND_WIDGET);
      
      FrameDestination outputDestination (sigc::mem_fun(this, &ViewerPanel::on_frame));
      playbackController_.useDisplay (DisplayService::setUp (outputDestination));
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
  
  void
  ViewerPanel::on_frame (void* buffer)
  {
    Displayer *displayer = display_.getDisplayer();
    REQUIRE(displayer);
    
    displayer->put(buffer);
  }
  
  
}}// namespace stage::panel
