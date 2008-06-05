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

const int TimelineWidget::TrackPadding = 1;
const int TimelineWidget::HeaderWidth = 100;

TimelineWidget::TimelineWidget() :
  Table(2, 2),
  totalHeight(0),
  horizontalAdjustment(0, 0, 0),
  verticalAdjustment(0, 0, 0),
  horizontalScroll(horizontalAdjustment),
  verticalScroll(verticalAdjustment),
  ruler("ruler")
  {
    body = new TimelineBody(this);
    g_assert(body != NULL);
    headerContainer = new HeaderContainer(this);
    g_assert(headerContainer != NULL);
  
    verticalAdjustment.signal_value_changed().connect(
      sigc::mem_fun(this, &TimelineWidget::on_scroll) );

    attach(*body, 1, 2, 1, 2, FILL|EXPAND, FILL|EXPAND);
    attach(ruler, 1, 2, 0, 1, FILL|EXPAND, SHRINK);
    attach(*headerContainer, 0, 1, 1, 2, SHRINK, FILL|EXPAND);
    attach(horizontalScroll, 1, 2, 2, 3, FILL|EXPAND, SHRINK);
    attach(verticalScroll, 2, 3, 1, 2, SHRINK, FILL|EXPAND);

    tracks.push_back(&video1);
    tracks.push_back(&video2);
    
    update_tracks();
  }

TimelineWidget::~TimelineWidget()
  {
    g_assert(body != NULL);
    delete body;
    g_assert(headerContainer != NULL);
    delete headerContainer;
  }

void
TimelineWidget::on_scroll()
  {
    
  }
  
void
TimelineWidget::on_size_allocate(Allocation& allocation)
  {
    Widget::on_size_allocate(allocation);
    
    update_scroll();
  }
  
void
TimelineWidget::update_tracks()
  {
    g_assert(headerContainer != NULL);
    headerContainer->update_headers();
    
    // Recalculate the total height of the timeline scrolled area
    totalHeight = 0;
    vector<timeline::Track*>::iterator i;
    for(i = tracks.begin(); i != tracks.end(); i++)
      {
        timeline::Track *track = *i;
        g_assert(track != NULL);
        totalHeight += track->get_height() + TrackPadding;
      }    
  }
  
void
TimelineWidget::update_scroll()
  {
    g_assert(body != NULL);
    const Allocation body_allocation = body->get_allocation();
    
    // Calculate the length that can be scrolled:
    // the total height of all the tracks minus one screenful 
    int y_scroll_length = totalHeight - body_allocation.get_height();
    if(y_scroll_length < 0) y_scroll_length = 0;    
    
    // If by resizing we're now over-scrolled, scroll back to
    // maximum distance
    if((int)verticalAdjustment.get_value() > y_scroll_length)
        verticalAdjustment.set_value(y_scroll_length);
    
    verticalAdjustment.set_upper(y_scroll_length);
    
    // Hide the scrollbar if no scrolling is possible
    if(y_scroll_length == 0 && verticalScroll.is_visible())
      verticalScroll.hide();
    else if(y_scroll_length != 0 && !verticalScroll.is_visible())
      verticalScroll.show();
  }

int
TimelineWidget::get_y_scroll_offset() const
  {
    return (int)verticalAdjustment.get_value();
  }

}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

