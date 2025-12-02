/*
  PLAY-PANEL.hpp  -  Dockable panel to hold the play control and switchboard

   Copyright (C)
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file play-panel.hpp
 ** Definition of a dockable panel for playback control        //////////////////////////////////////////////TICKET #1097 : me can haz more play panelz? wanna chase teh rat
 */


#ifndef STAGE_PANEL_PLAY_PANEL_H
#define STAGE_PANEL_PLAY_PANEL_H


#include "stage/panel/panel.hpp"
#include "stage/widget/button-bar.hpp"
#include "stage/model/w-link.hpp"

namespace stage {
namespace panel {
  
  class ViewerPanel;
  
  
  /**
   * A »media player« panel.
   */
  class PlayPanel
    : public Panel
    {
      widget::ButtonBar buttons_;
      widget::MiniButton buttonStop_;
      widget::MiniButton buttonPlay_;
      widget::MiniButton buttonPause_;
      
    public:
      PlayPanel (workspace::PanelManager&, Gdl::DockItem&);
      
      static const char* getTitle();
      static const gchar* getStockID();
      
    private:
      void dispatchStop();
      void dispatchPlay();
      void dispatchPause();
      
      using ViewLink = model::WLink<ViewerPanel>;
      ViewLink viewer_;
      
      ViewLink& accessViewer();
    };
  
}}// namespace stage::panel
#endif /*STAGE_PANEL_PLAY_PANEL_H*/
