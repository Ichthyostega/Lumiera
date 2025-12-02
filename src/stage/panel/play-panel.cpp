/*
  PlayPanel  -  Dockable panel to hold the play control and switchboard

   Copyright (C)
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file play-panel.cpp
 ** Implementation of a dockable panel for player control and timecode display
 ** 
 ** @todo 5/2025 This is an experiment or demo to find out if our XV-Widget is still usable.
 **       The actual solution obviously should not use a direct wired connection to the ViewerPanel,
 **       but rather communicate with the »Player« subsystem down in Steam-Layer
 **       ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1403 : try to port XvDisplayer to GTK-3
 */

#include "stage/gtk-base.hpp"
#include "stage/panel/play-panel.hpp"
#include "stage/panel/viewer-panel.hpp"

#include "stage/workspace/workspace-window.hpp"
#include "stage/ui-bus.hpp"  ///////////////////////////////////TODO why are we forced to include this after workspace-window.hpp ??  Ambiguity between std::ref and boost::reference_wrapper


namespace stage {
namespace panel {
  
  using Gtk::StockID;
  using sigc::mem_fun;
  
  
  PlayPanel::PlayPanel (workspace::PanelManager& panelManager
                       ,Gdl::DockItem& dockItem)
    : Panel{panelManager, dockItem, getTitle(), getStockID()}
    , buttons_{}
    , buttonStop_{StockID(GTK_STOCK_MEDIA_STOP)}
    , buttonPlay_{StockID(GTK_STOCK_MEDIA_PLAY)}
    , buttonPause_{StockID(GTK_STOCK_MEDIA_PAUSE)}                                   ////////////////////////TICKET #1030 : Stock-items are deprecated and will be removed with GTK-4
    {
      buttons_.append (buttonPlay_,  mem_fun(*this, &PlayPanel::dispatchPlay));
      buttons_.append (buttonPause_, mem_fun(*this, &PlayPanel::dispatchPause));
      buttons_.append (buttonStop_,  mem_fun(*this, &PlayPanel::dispatchStop));
      pack_start(buttons_, Gtk::PACK_SHRINK);
    }
  
  const char*
  PlayPanel::getTitle()
  {
    return _("Play");
  }
  
  const gchar*
  PlayPanel::getStockID()
  {
    return "panel_play";
  }
  
  
  /* === create an internal wiring to the Controller === */
  
  // Preliminary workaround -- use a shortcut           /////////////////////////////////////////////////////TICKET #1105 : need a generic scheme to access UI components
  //                           Directly grab into the first ViewerPanel we can get hold off
  //                           The real solution should use the UI-Bus!
  
  PlayPanel::ViewLink&
  PlayPanel::accessViewer()
  {
    if (not viewer_)
      {
        int panelID = workspace::PanelManager::findPanelID<ViewerPanel>();
        if (panelManager_.hasPanel (panelID))
          viewer_.connect (dynamic_cast<ViewerPanel&> (panelManager_.showPanel (panelID)));
      }
    return viewer_;
  }

  void
  PlayPanel::dispatchStop()
  {
    auto& view{accessViewer()};
    if (view)
      view->demoPlayback_.stop();
  }
  
  void
  PlayPanel::dispatchPlay()
  {
    auto& view{accessViewer()};
    if (view)
      view->demoPlayback_.play();
  }
  
  void
  PlayPanel::dispatchPause()
  {
    auto& view{accessViewer()};
    if (view)
      view->demoPlayback_.pause();
  }
  
  
}}// namespace stage::panel
