/*
  timeline-track.cpp  -  Implementation of the timeline track object
 
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

#warning This header must soon be removed when we drop Etch compatibility
#include <gtk/gtktoolbar.h>

#include "timeline-track.hpp"
#include "../../window-manager.hpp"

using namespace Gtk;

namespace gui {
namespace widgets {
namespace timeline {
  
Track::Track() :
  expanded(true),
  enableButton(Gtk::StockID("track_enabled")),
  lockButton(Gtk::StockID("track_unlocked"))
{
  buttonBar.append(enableButton);
  buttonBar.append(lockButton);
  
  buttonBar.set_toolbar_style(TOOLBAR_ICONS);
  
#if 0
  buttonBar.set_icon_size(WindowManager::MenuIconSize);
#else
#warning This code soon be removed when we drop Etch compatibility

  // Temporary bodge for etch compatibility - will be removed soon
  gtk_toolbar_set_icon_size (buttonBar.gobj(),
    (GtkIconSize)(int)WindowManager::MenuIconSize);
#endif

  headerWidget.pack_start(titleBox, PACK_SHRINK);
  headerWidget.pack_start(buttonBar, PACK_SHRINK);
}

Gtk::Widget&
Track::get_header_widget()
{
  return headerWidget;
}

int
Track::get_height() const
{
  return 100;
}

bool
Track::get_expanded() const
{
  return expanded;
}

void
Track::set_expanded(bool expanded)
{
  this->expanded = expanded;
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

