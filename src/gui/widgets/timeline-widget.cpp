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

#include <boost/foreach.hpp>

using namespace Gtk;
using namespace std;
using namespace lumiera::gui::widgets::timeline;

namespace lumiera {
namespace gui {
namespace widgets {

const int TimelineWidget::TrackPadding = 1;
const int TimelineWidget::HeaderWidth = 100;
const double TimelineWidget::ZoomIncrement = 1.25;
const int64_t TimelineWidget::MaxScale = 30000000;

TimelineWidget::TimelineWidget() :
  Table(2, 2),
  timeOffset(0),
  timeScale(1),
  totalHeight(0),
  horizontalAdjustment(0, 0, 0),
  verticalAdjustment(0, 0, 0),
  selectionStart(0),
  selectionEnd(0),
  playbackPeriodStart(0),
  playbackPeriodEnd(0),
  horizontalScroll(horizontalAdjustment),
  verticalScroll(verticalAdjustment)
{
  body = new TimelineBody(this);
  ENSURE(body != NULL);
  headerContainer = new HeaderContainer(this);
  ENSURE(headerContainer != NULL);
  ruler = new TimelineRuler(this);
  ENSURE(ruler != NULL);

  horizontalAdjustment.signal_value_changed().connect( sigc::mem_fun(
    this, &TimelineWidget::on_scroll) );
  verticalAdjustment.signal_value_changed().connect( sigc::mem_fun(
    this, &TimelineWidget::on_scroll) );
  body->signal_motion_notify_event().connect( sigc::mem_fun(
    this, &TimelineWidget::on_motion_in_body_notify_event) );
  
  set_time_scale(GAVL_TIME_SCALE / 200);
  set_selection(2000000, 4000000);

  attach(*body, 1, 2, 1, 2, FILL|EXPAND, FILL|EXPAND);
  attach(*ruler, 1, 2, 0, 1, FILL|EXPAND, SHRINK);
  attach(*headerContainer, 0, 1, 1, 2, SHRINK, FILL|EXPAND);
  attach(horizontalScroll, 1, 2, 2, 3, FILL|EXPAND, SHRINK);
  attach(verticalScroll, 2, 3, 1, 2, SHRINK, FILL|EXPAND);

  tracks.push_back(&video1);
  tracks.push_back(&video2);
  
  update_tracks();
  
  set_tool(timeline::Arrow);
}

TimelineWidget::~TimelineWidget()
{
  REQUIRE(body != NULL);
  if(body != NULL)
    body->unreference();
    
  REQUIRE(headerContainer != NULL);
  if(headerContainer != NULL)
    headerContainer->unreference();
    
  REQUIRE(ruler != NULL);
  if(ruler != NULL)
    ruler->unreference();
}

gavl_time_t
TimelineWidget::get_time_offset() const
{
  return timeOffset;
}

void
TimelineWidget::set_time_offset(gavl_time_t time_offset)
{
  REQUIRE(ruler != NULL);
  
  timeOffset = time_offset;
  horizontalAdjustment.set_value(time_offset);
  ruler->update_view();
}

int64_t
TimelineWidget::get_time_scale() const
{
  return timeScale;
}

void
TimelineWidget::set_time_scale(int64_t time_scale)
{
  REQUIRE(ruler != NULL);
  
  timeScale = time_scale;
  
  const int view_width = body->get_allocation().get_width();
  horizontalAdjustment.set_page_size(timeScale * view_width);
  
  ruler->update_view();
}

void
TimelineWidget::zoom_view(int zoom_size)
{
  const Allocation allocation = body->get_allocation();
  zoom_view(allocation.get_width() / 2, zoom_size);
}

void
TimelineWidget::zoom_view(int point, int zoom_size)
{ 
  int64_t new_time_scale = (double)timeScale * pow(1.25, -zoom_size);
  
  // Limit zooming in too close
  if(new_time_scale < 1) new_time_scale = 1;
  
  // Nudge zoom problems caused by integer rounding
  if(new_time_scale == timeScale && zoom_size < 0)
    new_time_scale++;
    
  // Limit zooming out too far
  if(new_time_scale > MaxScale)
    new_time_scale = MaxScale;
  
  // The view must be shifted so that the zoom is centred on the cursor
  set_time_offset(get_time_offset() +
    (timeScale - new_time_scale) * point);
    
  // Apply the new scale
  set_time_scale(new_time_scale);
}

void
TimelineWidget::shift_view(int shift_size)
{
  const int view_width = body->get_allocation().get_width();
  
  set_time_offset(get_time_offset() +
    shift_size * timeScale * view_width / 256);
}

gavl_time_t
TimelineWidget::get_selection_start() const
{
  return selectionStart;
}

gavl_time_t
TimelineWidget::get_selection_end() const
{
  return selectionEnd;
}

void
TimelineWidget::set_selection(gavl_time_t start, gavl_time_t end,
  bool reset_playback_period)
{
  REQUIRE(ruler != NULL);
  REQUIRE(body != NULL);
    
  if(start < end)
    {
      selectionStart = start;
      selectionEnd = end;
    }
  else
    {
      // The selection is back-to-front, flip it round
      selectionStart = end;
      selectionEnd = start;
    }
    
  if(reset_playback_period)
    {
      playbackPeriodStart = selectionStart;
      playbackPeriodEnd = selectionEnd;
    }

  ruler->queue_draw();
  body->queue_draw();
}

gavl_time_t
TimelineWidget::get_playback_period_start() const
{
  return playbackPeriodStart;
}
  
gavl_time_t
TimelineWidget::get_playback_period_end() const
{
  return playbackPeriodEnd;
}
  
void
TimelineWidget::set_playback_period(gavl_time_t start, gavl_time_t end)
{
  REQUIRE(ruler != NULL);
  REQUIRE(body != NULL);
  
  if(start < end)
    {
      playbackPeriodStart = start;
      playbackPeriodEnd = end;
    }
  else
    {
      // The period is back-to-front, flip it round
      playbackPeriodStart = end;
      playbackPeriodEnd = start;
    }

  ruler->queue_draw();
  body->queue_draw();
}

ToolType
TimelineWidget::get_tool() const
{
  REQUIRE(body != NULL);
  return body->get_tool();
}
  
void
TimelineWidget::set_tool(ToolType tool_type)
{
  REQUIRE(body != NULL);
  body->set_tool(tool_type);
}

sigc::signal<void, gavl_time_t>
TimelineWidget::mouse_hover_signal() const
{
  return mouseHoverSignal;
}

void
TimelineWidget::on_scroll()
{
  timeOffset = horizontalAdjustment.get_value();
  ruler->update_view();
}
  
void
TimelineWidget::on_size_allocate(Allocation& allocation)
{
  Widget::on_size_allocate(allocation);
  
  update_scroll();
}

int
TimelineWidget::time_to_x(gavl_time_t time) const
{
  return (int)((time - timeOffset) / timeScale);
}

gavl_time_t
TimelineWidget::x_to_time(int x) const
{
  return (gavl_time_t)((int64_t)x * timeScale + timeOffset);
}

void
TimelineWidget::update_tracks()
{
  ASSERT(headerContainer != NULL);
  headerContainer->update_headers();
  
  // Recalculate the total height of the timeline scrolled area
  totalHeight = 0;
  BOOST_FOREACH( Track* track, tracks )
    {
      ASSERT(track != NULL);
      totalHeight += track->get_height() + TrackPadding;
    }    
}
  
void
TimelineWidget::update_scroll()
{
  ASSERT(body != NULL);
  const Allocation body_allocation = body->get_allocation();
  
  //----- Horizontal Scroll ------//
  
  // TEST CODE
  horizontalAdjustment.set_upper(1000 * GAVL_TIME_SCALE / 200);
  horizontalAdjustment.set_lower(-1000 * GAVL_TIME_SCALE / 200);
  
  // Set the page size
  horizontalAdjustment.set_page_size(
    timeScale * body_allocation.get_width());
  
  //----- Vertical Scroll -----//
  
  // Calculate the vertical length that can be scrolled:
  // the total height of all the tracks minus one screenful 
  int y_scroll_length = totalHeight - body_allocation.get_height();
  if(y_scroll_length < 0) y_scroll_length = 0;    
  
  // If by resizing we're now over-scrolled, scroll back to
  // maximum distance
  if((int)verticalAdjustment.get_value() > y_scroll_length)
      verticalAdjustment.set_value(y_scroll_length);
  
  verticalAdjustment.set_upper(y_scroll_length);
  
  // Hide the scrollbar if no scrolling is possible
#if 0
  // Having this code included seems to cause a layout loop as the
  // window is shrunk
  if(y_scroll_length <= 0 && verticalScroll.is_visible())
    verticalScroll.hide();
  else if(y_scroll_length > 0 && !verticalScroll.is_visible())
    verticalScroll.show();
#endif
}

int
TimelineWidget::get_y_scroll_offset() const
{
  return (int)verticalAdjustment.get_value();
}

bool
TimelineWidget::on_motion_in_body_notify_event(GdkEventMotion *event)
{
  REQUIRE(event != NULL);
  ruler->set_mouse_chevron_offset(event->x);
  mouseHoverSignal.emit(x_to_time(event->x));
  return true;
}

}   // namespace widgets
}   // namespace gui
}   // namespace lumiera
