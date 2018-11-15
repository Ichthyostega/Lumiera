/*
  TimelineWidget  -  custom widget for timeline display of the project

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


/** @file widget/timeline-widget.cpp
 ** 
 ** @deprecated as of 11/2016, a complete rework of the timeline display is underway
 ** @see gui::timeline::TimelineWidget new timeline display
 ** 
 */


#include "stage/widget/timeline-widget.hpp"

#include <boost/foreach.hpp>
#include <typeinfo>
#include <memory>

using lib::time::Time;
using lib::time::TimeValue;
using std::dynamic_pointer_cast;

using namespace Gtk;         ////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

using namespace util;        ////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace stage::widget::timeline;       ////////////TODO OK this one looks more sensible, but why do we need to refer to such a nested space so much?

namespace stage {
namespace widget {
  
  const int TimelineWidget::TrackPadding = 1;
  const int TimelineWidget::HeaderWidth = 150;
  const int TimelineWidget::HeaderIndentWidth = 10;
  
  
  
  
  TimelineWidget::TimelineWidget (shared_ptr<timeline::TimelineState> source_state)
    : Table(2, 2)
    , layoutHelper(*this)
    , headerContainer(NULL)
    , body(NULL)
    , ruler(NULL)
    , horizontalAdjustment(Gtk::Adjustment::create(0, 0, 0))
    , verticalAdjustment(Gtk::Adjustment::create(0, 0, 0))
    , horizontalScroll(horizontalAdjustment)
    , verticalScroll(verticalAdjustment)
    , update_tracks_frozen(false)
    {
      body = manage(new TimelineBody(*this));
      ENSURE(body != NULL);
      headerContainer = manage(new TimelineHeaderContainer(*this));
      ENSURE(headerContainer != NULL);
      ruler = manage(new TimelineRuler(*this));
      ENSURE(ruler != NULL);
      
      horizontalAdjustment->signal_value_changed().connect( sigc::mem_fun(
        this, &TimelineWidget::on_scroll) );
      verticalAdjustment->signal_value_changed().connect( sigc::mem_fun(
        this, &TimelineWidget::on_scroll) );
      body->signal_motion_notify_event().connect( sigc::mem_fun(
        this, &TimelineWidget::on_motion_in_body_notify_event) );
        
      update_tracks();
      
      attach(*body, 1, 2, 1, 2, FILL|EXPAND, FILL|EXPAND);
      attach(*ruler, 1, 2, 0, 1, FILL|EXPAND, SHRINK);
      attach(*headerContainer, 0, 1, 1, 2, SHRINK, FILL|EXPAND);
      attach(horizontalScroll, 1, 2, 2, 3, FILL|EXPAND, SHRINK);
      attach(verticalScroll, 2, 3, 1, 2, SHRINK, FILL|EXPAND);
      
      set_state(source_state);
      
      set_tool(timeline::Arrow);
    }
  
  
  TimelineWidget::~TimelineWidget()
  {
    REQUIRE(headerContainer);
    headerContainer->clear_headers();
    
    trackMap.clear();
  }
  
  
  
  /* ===== Data Access ===== */
  
  /** @deprecated for #955 */
  shared_ptr<timeline::TimelineState>
  TimelineWidget::get_state()
  {
    return state;
  }
  
  /** @deprecated for #955 */
  void
  TimelineWidget::set_state (shared_ptr<timeline::TimelineState> new_state)
  { 
  
    state = new_state;
    
    // Clear the track tree
    trackMap.clear();
    
    if (state)
      {
        // Hook up event handlers
        state->getViewWindow().changed_signal().connect( sigc::mem_fun(
          this, &TimelineWidget::on_view_window_changed) );
        state->getSequence()->get_child_track_list().signal_changed().
          connect(sigc::mem_fun(
            this, &TimelineWidget::on_track_list_changed ) );
        
        state->selectionChangedSignal().connect(mem_fun(*this,
          &TimelineWidget::on_body_changed));
        state->playbackChangedSignal().connect(mem_fun(*this,
          &TimelineWidget::on_body_changed));
      }
    
    update_tracks();
    
    // Send the state changed signal
    stateChangedSignal.emit (state);
  }
  
  void
  TimelineWidget::zoom_view (double timescale_ratio)
  {
    if(state)
    {
      const int view_width = body->get_allocation().get_width();
      state->getViewWindow().zoom_view(view_width / 2, timescale_ratio);
    }
  }
  
  ToolType
  TimelineWidget::get_tool()  const
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
  
  sigc::signal<void, Time>
  TimelineWidget::mouse_hover_signal() const
  {
    return mouseHoverSignal;
  }
  
  sigc::signal<void>
  TimelineWidget::playback_period_drag_released_signal() const
  {
    return playbackPeriodDragReleasedSignal;
  }
  
  sigc::signal<void, shared_ptr<timeline::Track>>
  TimelineWidget::hovering_track_changed_signal() const
  {
    return hoveringTrackChangedSignal;
  }
  
  TimelineWidget::TimelineStateChangeSignal
  TimelineWidget::state_changed_signal() const
  {
    return stateChangedSignal;
  }
  
  
  
  /* ===== Events ===== */
  
  void
  TimelineWidget::on_scroll()
  {
    if(state)
      {
        TimeValue newStartOffset ((gavl_time_t)horizontalAdjustment->get_value());
        state->getViewWindow().set_time_offset(Time(newStartOffset));
      }
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
    
    if(state)
      { 
        timeline::TimelineViewWindow &window = state->getViewWindow();
        
        const int view_width = body->get_allocation().get_width();
        
        horizontalAdjustment->set_page_size(
          window.get_time_scale() * view_width);
        
        horizontalAdjustment->set_value(_raw(window.get_time_offset()));
      }
  }
  
  
  void
  TimelineWidget::on_body_changed()
  {
    REQUIRE(ruler != NULL);
    REQUIRE(body != NULL);
    ruler->queue_draw();
    body->queue_draw();
  }
  
  
  void
  TimelineWidget::on_add_track_command()
  {
    // # TEST CODE
    if(sequence())
      sequence()->get_child_track_list().push_back(
        shared_ptr<model::Track>(new model::ClipTrack()));
  }
  
  
  
  
  /* ===== Internals ===== */
  
  void
  TimelineWidget::update_tracks()
  {
    if(update_tracks_frozen)
      return;
    
    if(state)
      {
        // Remove any tracks which are no longer present in the model
        remove_orphaned_tracks();
        
        // Create timeline tracks from all the model tracks
        create_timeline_tracks();
        
        // Update the layout helper
        layoutHelper.clone_tree_from_sequence();
        layoutHelper.update_layout();
      }
    else
      trackMap.clear();
  }
  
  
  void
  TimelineWidget::freeze_update_tracks()
  {
    update_tracks_frozen = true;
  }
  
  
  void
  TimelineWidget::thaw_update_tracks()
  {
    update_tracks_frozen = false;
  }
  
  
  void
  TimelineWidget::create_timeline_tracks()
  {
    REQUIRE(state);
    
    BOOST_FOREACH(shared_ptr<model::Track> child,
      sequence()->get_child_tracks())
      create_timeline_tracks_from_branch(child);
      
    // Update the header container
    REQUIRE(headerContainer != NULL);
    headerContainer->update_headers();
  }
  
  
  /** @deprecated for #955 */
  void
  TimelineWidget::create_timeline_tracks_from_branch(
    shared_ptr<model::Track> modelTrack)
  {
    REQUIRE(modelTrack);
    
    // Is a timeline UI track present in the map already?
    if (!contains(trackMap, modelTrack))
      {
        // The timeline UI track is not present
        // We will need to create one
        trackMap[modelTrack] = 
          create_timeline_track_from_modelTrack(modelTrack);
      }
    
    // Recurse to child tracks
    BOOST_FOREACH(shared_ptr<model::Track> child,
      modelTrack->get_child_tracks())
      create_timeline_tracks_from_branch(child);
  }
  
  
  /** @deprecated for #955 */
  shared_ptr<timeline::Track>
  TimelineWidget::create_timeline_track_from_modelTrack(
    shared_ptr<model::Track> modelTrack)
  {
    REQUIRE(modelTrack);
    
    // Choose a corresponding timeline track class from the model track's
    // class
    if(typeid(*modelTrack) == typeid(model::ClipTrack))
      return shared_ptr<timeline::Track>(new timeline::ClipTrack(
        *this, dynamic_pointer_cast<model::ClipTrack>(modelTrack)));
    else if(typeid(*modelTrack) == typeid(model::GroupTrack))
      return shared_ptr<timeline::Track>(new timeline::GroupTrack(
        *this, dynamic_pointer_cast<model::GroupTrack>(modelTrack)));
    
    ASSERT(NULL); // Unknown track type;
    return shared_ptr<timeline::Track>();
  }
  
  
  /** @deprecated for #955 */
  void
  TimelineWidget::remove_orphaned_tracks()
  {
    std::map<shared_ptr<model::Track>,
      shared_ptr<timeline::Track>>
      orphan_track_map(trackMap);
    
    // Remove all tracks which are still present in the sequence
    BOOST_FOREACH(shared_ptr<model::Track> child,
      sequence()->get_child_tracks())
      search_orphaned_tracks_in_branch(child, orphan_track_map);
    
    // orphan_track_map now contains all the orphaned tracks
    // Remove them
    std::pair<shared_ptr<model::Track>, shared_ptr<timeline::Track>>
      pair; 
    BOOST_FOREACH( pair, orphan_track_map )
      {
        ENSURE(pair.first);
        trackMap.erase(pair.first);
      }
  }
  
  
  /** @deprecated for #955 */
  void
  TimelineWidget::search_orphaned_tracks_in_branch(
      shared_ptr<model::Track> modelTrack,
      std::map<shared_ptr<model::Track>,
      shared_ptr<timeline::Track>>& orphan_track_map)
  {
    REQUIRE(modelTrack);
    
    // Is the timeline UI still present?
    if(contains(orphan_track_map, modelTrack))
      orphan_track_map.erase(modelTrack);
    
    // Recurse to child tracks
    BOOST_FOREACH(shared_ptr<model::Track> child,
      modelTrack->get_child_tracks())
      search_orphaned_tracks_in_branch(child, orphan_track_map);
  }
  
  
  /** @deprecated for #955 */
  shared_ptr<timeline::Track>
  TimelineWidget::lookup_timeline_track(
    shared_ptr<model::Track> modelTrack) const
  {
    REQUIRE(modelTrack);
    REQUIRE(modelTrack != sequence()); // The sequence isn't
                                        // really a track
    
    std::map<shared_ptr<model::Track>, shared_ptr<timeline::Track>>::
      const_iterator iterator = trackMap.find(modelTrack);
    if (iterator == trackMap.end())
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
  TimelineWidget::on_layout_changed()
  {
    REQUIRE(headerContainer != NULL);
    REQUIRE(body != NULL);
    
    headerContainer->on_layout_changed();
    body->queue_draw();
    update_scroll();
  }
  
  
  void
  TimelineWidget::update_scroll()
  {
    REQUIRE(body != NULL);
    const Allocation body_allocation = body->get_allocation();
    
    if(state)
      {
                                              ///////////////////////////////////////////////TICKET #861 shoudln't that be performed by TimelineViewWindow, instead of manipulating values from the outside?
        timeline::TimelineViewWindow &window = state->getViewWindow();
        
        //----- Horizontal Scroll ------//
        
        // TEST CODE
        horizontalAdjustment->set_upper( 1000 * GAVL_TIME_SCALE / 200);
        horizontalAdjustment->set_lower(-1000 * GAVL_TIME_SCALE / 200);
        
        // Set the page size
        horizontalAdjustment->set_page_size(
          window.get_time_scale() * body_allocation.get_width());
        
        //----- Vertical Scroll -----//
        
        // Calculate the vertical length that can be scrolled:
        // the total height of all the tracks minus one screenful 
        int y_scroll_length = layoutHelper.get_total_height() -
          body_allocation.get_height();
        if(y_scroll_length < 0) y_scroll_length = 0;    
        
        // If by resizing we're now over-scrolled, scroll back to
        // maximum distance
        if((int)verticalAdjustment->get_value() > y_scroll_length)
            verticalAdjustment->set_value(y_scroll_length);
        
        verticalAdjustment->set_upper(y_scroll_length);
        
        // Hide the scrollbar if no scrolling is possible
        // Having this code included seems to cause a layout loop as the
        // window is shrunk
        if(y_scroll_length <= 0 && verticalScroll.get_visible())
          verticalScroll.hide();
        else if(y_scroll_length > 0 && !verticalScroll.get_visible())
          verticalScroll.show();
      }
  }
  
  
  int
  TimelineWidget::get_y_scroll_offset() const
  {
    return (int)verticalAdjustment->get_value();
  }
  
  
  void
  TimelineWidget::set_y_scroll_offset(const int offset)
  {
    verticalAdjustment->set_value(offset);
  }
  
  
  bool
  TimelineWidget::on_motion_in_body_notify_event(GdkEventMotion *event)
  {
    REQUIRE(event != NULL);
    ruler->set_mouse_chevron_offset(event->x);
    
    if(state)
      {
        timeline::TimelineViewWindow &window = state->getViewWindow();
        mouseHoverSignal.emit(window.x_to_time(event->x));
      }
    
    return true;
  }
  
  
  /** @deprecated for #955 */
  shared_ptr<model::Sequence>
  TimelineWidget::sequence()  const
  {
    if(!state)
      return shared_ptr<model::Sequence>();
    shared_ptr<model::Sequence> sequence = state->getSequence();
    ENSURE(sequence);
    return sequence;
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
  TimelineWidget::set_hovering_track(shared_ptr<timeline::Track> hovering_track)
  {
    hoveringTrack = hovering_track;
    hoveringTrackChangedSignal.emit(hovering_track);
  }
  
  
}}// gui::widget
