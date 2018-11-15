/*
  ViewerPanel  -  Dockable panel to hold the video display widgets and controls

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

* *****************************************************/


/** @file viewer-panel.cpp
 ** Implementation of a dockable panel to hold the video viewers
 */

#include "stage/gtk-base.hpp"
#include "stage/panel/viewer-panel.hpp"

#include "stage/workspace/workspace-window.hpp"
#include "stage/ui-bus.hpp"  ///////////////////////////////////TODO why are we forced to include this after workspace-window.hpp ??  Ambiguity between std::ref and boost::reference_wrapper
#include "stage/display-service.hpp"


using namespace Gtk;                   ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace gui::widget;           ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace gui::controller;       ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace gui {
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
  
  
}}// namespace gui::panel
