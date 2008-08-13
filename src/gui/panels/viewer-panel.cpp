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

using namespace lumiera::gui::widgets;
using namespace Gtk;

namespace lumiera {
namespace gui {
namespace panels {

ViewerPanel::ViewerPanel() :
  Panel("viewer", _("Viewer"), "panel_viewer"),
  previousButton(Stock::MEDIA_PREVIOUS),
  rewindButton(Stock::MEDIA_REWIND),
  playPauseButton(Stock::MEDIA_PLAY),
  forwardButton(Stock::MEDIA_FORWARD),
  nextButton(Stock::MEDIA_NEXT)
  {
    //----- Set up the Tool Bar -----//
    // Add the commands
    toolBar.append(previousButton);
    toolBar.append(rewindButton);
    toolBar.append(playPauseButton);
    toolBar.append(forwardButton);
    toolBar.append(nextButton);
    
    // Configure the toolbar
    toolBar.set_toolbar_style(TOOLBAR_ICONS);
    
    //----- Pack in the Widgets -----//
    pack_start(display, PACK_EXPAND_WIDGET);
    pack_start(toolBar, PACK_SHRINK);
  }

}   // namespace panels
}   // namespace gui
}   // namespace lumiera
