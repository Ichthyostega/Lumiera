/*
  VIEWER-PANEL.hpp  -  Dockable panel to hold the video display widgets and controls            

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file viewer-panel.hpp
 ** Definition of a dockable panel to hold a video viewer
 */


#ifndef STAGE_PANEL_VIEWER_PANEL_H
#define STAGE_PANEL_VIEWER_PANEL_H


#include "stage/panel/panel.hpp"
#include "stage/widget/video-display-widget.hpp"
#include "stage/ctrl/demo-controller.hpp"

namespace stage {
namespace panel {
  
  /**
   * A panel to display the video output.
   */
  class ViewerPanel
    : public Panel
    {
      widget::VideoDisplayWidget display_;
      
    public:
      ViewerPanel (workspace::PanelManager&, Gdl::DockItem&);
      
      static const char* getTitle();
      static const gchar* getStockID();
      
      ctrl::DemoController demoPlayback_;
    };
  
}}// namespace stage::panel
#endif /*STAGE_PANEL_VIEWER_PANEL_H*/
