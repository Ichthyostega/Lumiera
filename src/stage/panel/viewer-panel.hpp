/*
  VIEWER-PANEL.hpp  -  Dockable panel to hold the video display widgets and controls            

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

/** @file viewer-panel.hpp
 ** Definition of a dockable panel to hold the video viewers
 */


#ifndef STAGE_PANEL_VIEWER_PANEL_H
#define STAGE_PANEL_VIEWER_PANEL_H


#include "stage/panel/panel.hpp"
#include "stage/widget/video-display-widget.hpp"
#include "stage/ctrl/playback-controller.hpp"

namespace gui  {
namespace panel{
  
  /**
   * A panel to display the video output.
   */
  class ViewerPanel
    : public Panel
    {
      ctrl::PlaybackController playbackController_;
      
    public:
      ViewerPanel (workspace::PanelManager&, Gdl::DockItem&);
      
      static const char* getTitle();
      static const gchar* getStockID();
      
      
    protected:
      void on_frame(void *buffer);
      
    protected:
      
      /** widget to display the video content */
      widget::VideoDisplayWidget display_;
    };
  
}}// namespace gui::panel
#endif /*STAGE_PANEL_VIEWER_PANEL_H*/
