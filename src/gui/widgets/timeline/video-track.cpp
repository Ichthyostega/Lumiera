/*
  video-track.cpp  -  Implementation of the timeline video track object
 
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

#include "video-track.hpp"

using namespace Gtk;

namespace lumiera {
namespace gui {
namespace widgets {
namespace timeline {

VideoTrack::VideoTrack() :
  headerWidget("HeaderTest")
  {
    headerFrame.add(headerWidget);
    headerFrame.set_shadow_type (Gtk::SHADOW_ETCHED_OUT);
    headerFrame.set_name ("TimelineHeaderBaseUnselected");
  }

Gtk::Widget&
VideoTrack::get_header_widget()
  {
    return headerFrame;
  }

/*int
VideoTrack::get_height()
  {
    return 100;
  }*/

void
VideoTrack::draw_track(Cairo::RefPtr<Cairo::Context> cairo)
  {
    
  }

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

