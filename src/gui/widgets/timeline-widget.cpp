/*
  timeline-widget.cpp  -  Implementation of the timeline widget
 
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

#include "timeline-widget.hpp"

using namespace Gtk;
using namespace std;
using namespace lumiera::gui::widgets::timeline;

namespace lumiera {
namespace gui {
namespace widgets {

TimelineWidget::TimelineWidget() :
  Table(2, 2),
  totalHeight(0),
  horizontalAdjustment(0, 0, 0),
  verticalAdjustment(0, 0, 0),
  horizontalScroll(horizontalAdjustment),
  verticalScroll(verticalAdjustment),
  ruler("ruler")
  {
    rowHeaderLayout.set_size_request(100, 100);

    body = new TimelineBody(horizontalAdjustment, verticalAdjustment);
 
    verticalAdjustment.signal_value_changed().connect(
      sigc::mem_fun(this, &TimelineWidget::on_scroll) );

    attach(*body, 1, 2, 1, 2, FILL|EXPAND, FILL|EXPAND);
    attach(ruler, 1, 2, 0, 1, FILL|EXPAND, SHRINK);
    attach(rowHeaderLayout, 0, 1, 1, 2, SHRINK, FILL|EXPAND);
    attach(horizontalScroll, 1, 2, 2, 3, FILL|EXPAND, SHRINK);
    attach(verticalScroll, 2, 3, 1, 2, SHRINK, FILL|EXPAND);

    tracks.push_back(&video1);
    tracks.push_back(&video2);

    layout_tracks();
  }

TimelineWidget::~TimelineWidget()
  {
    delete body;
  }

void
TimelineWidget::on_scroll()
  {
    move_headers();
  }

void
TimelineWidget::layout_tracks()
  {
    vector<timeline::Track*>::iterator i;
    for(i = tracks.begin(); i != tracks.end(); i++)
    {
      timeline::Track *track = *i;
      g_assert(track != NULL);    
      rowHeaderLayout.put(track->get_header_widget(), 0, 0);
    }

    move_headers();
  }

void
TimelineWidget::move_headers()
  {
    int offset = 0;
    const int y_scroll_offset = (int)verticalAdjustment.get_value();

    vector<Track*>::iterator i;
    for(i = tracks.begin(); i != tracks.end(); i++)
    {
      timeline::Track *track = *i;
      g_assert(track != NULL);

      const int height = track->get_track_height();
      rowHeaderLayout.move(track->get_header_widget(), 0, offset - y_scroll_offset);
      offset += height;
    }
    totalHeight = offset;
    verticalAdjustment.set_upper(totalHeight);
  }

}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

