/*
  viewer-panel.cpp  -  Implementation of the viewer panel
 
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
 
* *****************************************************/

#include "../gtk-lumiera.hpp"
#include "viewer-panel.hpp"

#include "../workspace/workspace-window.hpp"
#include "../controller/controller.hpp"
#include "../controller/playback-controller.hpp"

#include "gui/display-service.hpp"

using namespace Gtk;
using namespace gui::widgets;
using namespace gui::controller;

namespace gui {
namespace panels {

ViewerPanel::ViewerPanel(workspace::WorkspaceWindow &workspace_window) :
  Panel(workspace_window, "viewer", _("Viewer"), "panel_viewer")
{    
  //----- Pack in the Widgets -----//
  pack_start(display, PACK_EXPAND_WIDGET);
  
  PlaybackController &playback =
    workspace_window.get_controller().get_playback_controller();
  
  FrameDestination outputDestination (sigc::mem_fun(this, &ViewerPanel::on_frame));
  playback.use_display (DisplayService::setUp (outputDestination));
}

void
ViewerPanel::on_frame(void *buffer)
{
  Displayer *displayer = display.get_displayer();
  REQUIRE(displayer);
  
  displayer->put(buffer);
}

}   // namespace panels
}   // namespace gui
