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
#include <typeinfo>

using namespace Gtk;
using namespace std;
using namespace boost;
using namespace util;
using namespace gui::widgets::timeline;

namespace gui {
namespace widgets {

const int TimelineWidget::TrackPadding = 1;
const int TimelineWidget::HeaderWidth = 150;
const double TimelineWidget::ZoomIncrement = 1.25;
const int64_t TimelineWidget::MaxScale = 30000000;

TimelineWidget::TimelineWidget(
  shared_ptr<model::Sequence> source_sequence) :
  Table(2, 2),
  sequence(source_sequence),
  viewWindow(this, 0, 1),
  totalHeight(0),
  horizontalAdjustment(0, 0, 0),
  verticalAdjustment(0, 0, 0),
  selectionStart(0),
  selectionEnd(0),
  playbackPeriodStart(0),
  playbackPeriodEnd(0),
  playbackPoint(GAVL_TIME_UNDEFINED),
  horizontalScroll(horizontalAdjustment),
  verticalScroll(verticalAdjustment)
{
  REQUIRE(sequence);
  
  body = new TimelineBody(this);
  ENSURE(body != NULL);
  headerContainer = new TimelineHeaderContainer(this);
  ENSURE(headerContainer != NULL);
  ruler = new TimelineRuler(this);
  ENSURE(ruler != NULL);

  horizontalAdjustment.signal_value_changed().connect( sigc::mem_fun(
    this, &TimelineWidget::on_scroll) );
  verticalAdjustment.signal_value_changed().connect( sigc::mem_fun(
    this, &TimelineWidget::on_scroll) );
  body->signal_motion_notify_event().connect( sigc::mem_fun(
    this, &TimelineWidget::on_motion_in_body_notify_event) );
  viewWindow.changed_signal().connect( sigc::mem_fun(
    this, &TimelineWidget::on_view_window_changed) );
  
  viewWindow.set_time_scale(GAVL_TIME_SCALE / 200);
  set_selection(2000000, 4000000);
  
  update_tracks();
  
  attach(*body, 1, 2, 1, 2, FILL|EXPAND, FILL|EXPAND);
  attach(*ruler, 1, 2, 0, 1, FILL|EXPAND, SHRINK);
  attach(*headerContainer, 0, 1, 1, 2, SHRINK, FILL|EXPAND);
  attach(horizontalScroll, 1, 2, 2, 3, FILL|EXPAND, SHRINK);
  attach(verticalScroll, 2, 3, 1, 2, SHRINK, FILL|EXPAND);
  
  set_tool(timeline::Arrow);
  
  // Receive notifications of changes to the tracks
  sequence->get_child_track_list().signal_changed().connect(
    sigc::mem_fun( this, &TimelineWidget::on_track_list_changed ) );
}

TimelineWidget::~TimelineWidget()
{
  // Destroy child widgets
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

/* ===== Data Access ===== */

TimelineViewWindow&
TimelineWidget::get_view_window()
{
  return viewWindow;
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

void
TimelineWidget::set_playback_point(gavl_time_t point)
{
  playbackPoint = point;
  ruler->queue_draw();
  body->queue_draw();
}

gavl_time_t
TimelineWidget::get_playback_point() const
{
  return playbackPoint;
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

shared_ptr<timeline::Track>
TimelineWidget::get_hovering_track() const
{
  return hoveringTrack;
}

/* ===== Signals ===== */

sigc::signal<void, gavl_time_t>
TimelineWidget::mouse_hover_signal() const
{
  return mouseHoverSignal;
}

sigc::signal<void>
TimelineWidget::playback_period_drag_released_signal() const
{
  return playbackPeriodDragReleasedSignal;
}

sigc::signal<void, shared_ptr<timeline::Track> >
TimelineWidget::hovering_track_changed_signal() const
{
  return hoveringTrackChangedSignal;
}

/* ===== Events ===== */

void
TimelineWidget::on_scroll()
{
  viewWindow.set_time_offset(horizontalAdjustment.get_value());
}
  
void
TimelineWidget::on_size_allocate(Allocation& allocation)
{
  Widget::on_size_allocate(allocation);
  
  update_scroll();
}

void
TimelineWidget::on_view_window_changed()
{
  REQUIRE(ruler != NULL);
   
  const int view_width = body->get_allocation().get_width();
  horizontalAdjustment.set_page_size(
    viewWindow.get_time_scale() * view_width);
  horizontalAdjustment.set_value(
    viewWindow.get_time_offset());
}

void
TimelineWidget::on_add_track_command()
{
  REQUIRE(sequence);
  
  // # TEST CODE
  sequence->get_child_track_list().push_back(
    shared_ptr<model::Track>(new model::ClipTrack()));
}

/* ===== Internals ===== */

void
TimelineWidget::update_tracks()
{ 
  REQUIRE(sequence);
  
  // Create timeline tracks from all the model tracks
  create_timeline_tracks();
  
  // Update the header container
  ASSERT(headerContainer != NULL);
  headerContainer->update_headers();
  
  // Recalculate the total height of the timeline scrolled area
  totalHeight = 0;
  BOOST_FOREACH(shared_ptr<model::Track> track,
    sequence->get_child_tracks())
    {
      ASSERT(track);
      totalHeight += measure_branch_height(track);
    }    
}

void
TimelineWidget::create_timeline_tracks()
{
  REQUIRE(sequence);
  REQUIRE(headerContainer != NULL);
  REQUIRE(body != NULL);
  
  BOOST_FOREACH(shared_ptr<model::Track> child,
    sequence->get_child_tracks())
    create_timeline_tracks_from_branch(child);
    
  headerContainer->show_all_children();
  body->queue_draw();
}

void
TimelineWidget::create_timeline_tracks_from_branch(
  shared_ptr<model::Track> model_track)
{
  REQUIRE(model_track);
  
  // Is a timeline UI track present in the map already?
  const model::Track *track = model_track.get();
  if(!contains(trackMap, track))
    {
      // The timeline UI track is not present
      // We will need to create one
      trackMap[model_track.get()] = 
        create_timeline_track_from_model_track(model_track);
    }
  
  // Recurse to child tracks
  BOOST_FOREACH(shared_ptr<model::Track> child,
    model_track->get_child_tracks())
    create_timeline_tracks_from_branch(child);
}

shared_ptr<timeline::Track>
TimelineWidget::create_timeline_track_from_model_track(
  shared_ptr<model::Track> model_track)
{
  REQUIRE(model_track);
  
  // Choose a corresponding timeline track class from the model track's
  // class
  if(typeid(*model_track) == typeid(model::ClipTrack))
    return shared_ptr<timeline::Track>(new timeline::ClipTrack());
  else if(typeid(*model_track) == typeid(model::GroupTrack))
    return shared_ptr<timeline::Track>(new timeline::GroupTrack());
  
  ASSERT(NULL); // Unknown track type;
  return shared_ptr<timeline::Track>();
}

shared_ptr<timeline::Track>
TimelineWidget::lookup_timeline_track(
  shared_ptr<model::Track> model_track)
{
  REQUIRE(sequence);  
  std::map<const model::Track*, shared_ptr<timeline::Track> >::
    const_iterator iterator = trackMap.find(model_track.get());
  if(iterator == trackMap.end())
    {
      // The track is not present in the map
      // We are in an error condition if the timeline track is not found
      // - the timeline tracks must always be synchronous with the model
      // tracks.
      ENSURE(0);
      return shared_ptr<timeline::Track>();
    }
  ENSURE(iterator->second != NULL);
  return iterator->second;
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
    viewWindow.get_time_scale() * body_allocation.get_width());
  
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
TimelineWidget::measure_branch_height(
  shared_ptr<model::Track> model_track)
{
  REQUIRE(model_track);
  
  const shared_ptr<timeline::Track> timeline_track =
    lookup_timeline_track(model_track);
  ENSURE(timeline_track);
  
  int height = timeline_track->get_height() + TrackPadding;
  
  // Recurse through all the children  
  BOOST_FOREACH( shared_ptr<model::Track> child,
    model_track->get_child_tracks() )
    {
      ASSERT(child != NULL);
      height += measure_branch_height(child);
    }
    
  return height;
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
  mouseHoverSignal.emit(viewWindow.x_to_time(event->x));
  
  return true;
}

void
TimelineWidget::on_track_list_changed()
{
  update_tracks();
}

void
TimelineWidget::on_playback_period_drag_released()
{
  playbackPeriodDragReleasedSignal.emit();
}

void
TimelineWidget::set_hovering_track(
  shared_ptr<timeline::Track> hovering_track)
{
  hoveringTrack = hovering_track;
  hoveringTrackChangedSignal.emit(hovering_track);
}

}   // namespace widgets
}   // namespace gui
