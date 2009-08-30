/*
  timeline-widget.hpp  -  Declaration of the timeline widget
 
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
 
*/
/** @file timeline-widget.hpp
 ** This file contains the definition of timeline widget
 */

#ifndef TIMELINE_WIDGET_HPP
#define TIMELINE_WIDGET_HPP

#include "timeline/timeline-state.hpp"
#include "timeline/timeline-header-container.hpp"
#include "timeline/timeline-body.hpp"
#include "timeline/timeline-ruler.hpp"
#include "timeline/timeline-tool.hpp"
#include "timeline/timeline-arrow-tool.hpp"
#include "timeline/timeline-ibeam-tool.hpp"
#include "timeline/timeline-group-track.hpp"
#include "timeline/timeline-clip-track.hpp"
#include "timeline/timeline-layout-helper.hpp"

#include "../model/sequence.hpp"

namespace gui {
namespace widgets {
  
/**
 * The namespace of all timeline widget helper classes.
 */
namespace timeline {}

/**
 * The timeline widget class.
 * @remarks This widget is a composite of several widgets contained
 * within the timeline namespace.
 */
class TimelineWidget : public Gtk::Table
{
public:
  /**
   * Constructor
   * @param source_state The state that will be used as the data source
   * for this timeline widget.
   */
  TimelineWidget(
    boost::shared_ptr<timeline::TimelineState> source_state);

  /**
   * Destructor
   */
  ~TimelineWidget();
  
  /* ===== Data Access ===== */
public:

  /**
   * Gets a pointer to the current state object.
   * @return The state object that the timeline widget is currently
   * working with.
   **/
  boost::shared_ptr<timeline::TimelineState> get_state();
  
  /**
   * Replaces the current TimelineState object with another.
   * @param new_state The new state to swap in.
   **/
  void set_state(boost::shared_ptr<timeline::TimelineState> new_state);

  /**
   * Zooms the view in or out as by a number of steps while keeping a 
   * given point on the timeline still.
   * @param zoom_size The number of steps to zoom by. The scale factor
   * is 1.25^(-zoom_size).
   **/
  void zoom_view(int zoom_size);
  
  /**
   * Gets the type of the tool currently active.
   */
  timeline::ToolType get_tool() const;
  
  /**
   * Sets the type of the tool currently active.
   */
  void set_tool(timeline::ToolType tool_type);
  
  boost::shared_ptr<timeline::Track> get_hovering_track() const;
  
public:
  /* ===== Signals ===== */
  sigc::signal<void, lumiera::Time> mouse_hover_signal() const;
  
  sigc::signal<void> playback_period_drag_released_signal() const;
  
  sigc::signal<void, boost::shared_ptr<timeline::Track> >
    hovering_track_changed_signal() const;
    
  sigc::signal<void> state_changed_signal() const;
  
  /* ===== Events ===== */
protected:
  
  void on_scroll();
  
  void on_size_allocate(Gtk::Allocation& allocation);
  
  void on_view_window_changed();
  
  void on_body_changed();
  
  void on_add_track_command();
  
  /* ===== Utilities ===== */
protected:


  /* ===== Internals ===== */
private:

  // ----- Track Mapping Functions ----- //

  /**
   * Updates the timeline widget to match the state of the track tree.
   **/
  void update_tracks();
  
  void freeze_update_tracks();
  
  void thaw_update_tracks();
  
  /**
   * Ensures timeline UI tracks have been created for every model track
   * present in sequence.
   **/
  void create_timeline_tracks();
  
  /**
   * Iterates through a branch of tracks, recursing into each sub-branch
   * creating UI timeline tracks for each model track if they don't
   * already exist in trackMap.
   * @param list The parent track of the branch.
   **/
  void create_timeline_tracks_from_branch(
    boost::shared_ptr<model::Track> model_track);
  
  /**
   * Creates a timeline UI track to correspond to a model track.
   * @param model_track The model track to create a timeline track from.
   * @return The timeline track created, or an empty shared_ptr if
   * model_track has an unreckognised type (this is an error condition).
   **/
  boost::shared_ptr<timeline::Track>
    create_timeline_track_from_model_track(
    boost::shared_ptr<model::Track> model_track);
  
  /**
   * Removes any UI tracks which no longer have corresponding model
   * tracks present in the sequence.
   **/
  void remove_orphaned_tracks();
  
  void search_orphaned_tracks_in_branch(
    boost::shared_ptr<model::Track> model_track,
    std::map<boost::shared_ptr<model::Track>,
    boost::shared_ptr<timeline::Track> > &orphan_track_map);
  
  /**
   * Looks up a timeline UI track in trackMap that corresponds to a
   * given model_track.
   * @param model_track The model track to look up.
   * @returns The timeline UI track found, or an empty shared_ptr if
   * model_track has no corresponding timeline UI track (this is an
   * error condition).
   **/
  boost::shared_ptr<timeline::Track> lookup_timeline_track(
    boost::shared_ptr<model::Track> model_track) const;
  
  // ----- Layout Functions ----- //
  
  void on_layout_changed();
  
  void update_scroll();
  
  int get_y_scroll_offset() const;
  
  void set_y_scroll_offset(const int offset);
  
  // ----- Event Handlers -----//
  
  /**
   * An event handler that receives notifications for when the
   * sequence's track tree has been changed.
   **/
  void on_track_list_changed();
    
  void on_playback_period_drag_released();
  
  bool on_motion_in_body_notify_event(GdkEventMotion *event); 
  
  // ----- Helper Functions ----- //
  
  /**
   * Helper to get the sequence object from the state.
   * @return Returns a shared pointer to the sequence.
   **/  
  boost::shared_ptr<model::Sequence> sequence() const;
  
  // ----- Other Functions ----- //
  
  void set_hovering_track(
    boost::shared_ptr<timeline::Track> hovering_track);

protected:

  /**
   * The state that will be used as the data source for this timeline
   * widget.
   **/
  boost::shared_ptr<timeline::TimelineState> state;

  // Model Data
   
  /**
   * The trackMap maps model tracks to timeline widget tracks which are
   * responsible for the UI representation of a track.
   * @remarks The tree structure is maintianed by the model, and as the
   * widget is updated with update_tracks, timeline tracks are added and
   * removed from the map in correspondance with the tree.
   **/
  std::map<boost::shared_ptr<model::Track>,
    boost::shared_ptr<timeline::Track> >
    trackMap;
    
  boost::shared_ptr<timeline::Track> hoveringTrack;
    
  // Helper Classes
  timeline::TimelineLayoutHelper layoutHelper;

  // Child Widgets
  timeline::TimelineHeaderContainer *headerContainer;
  timeline::TimelineBody *body;
  timeline::TimelineRuler *ruler;

  Gtk::Adjustment horizontalAdjustment, verticalAdjustment;
  Gtk::HScrollbar horizontalScroll;
  Gtk::VScrollbar verticalScroll;
  
  // Signals
  sigc::signal<void, lumiera::Time> mouseHoverSignal;
  sigc::signal<void> playbackPeriodDragReleasedSignal;
  sigc::signal<void, boost::shared_ptr<timeline::Track> >
    hoveringTrackChangedSignal;
  sigc::signal<void> stateChangedSignal;
    
  bool update_tracks_frozen;
   
  /* ===== Constants ===== */
public:
  /**
   * The maximum scale for timeline display.
   * @remarks At MaxScale, every pixel on the timeline is equivalent
   * to 30000000 lumiera::Time increments.
   */ 
  static const int64_t MaxScale;
  
protected:
  static const int TrackPadding;
  static const int HeaderWidth;
  static const int HeaderIndentWidth;
  static const double ZoomIncrement;

  friend class timeline::TimelineBody;
  friend class timeline::TimelineHeaderContainer;
  friend class timeline::TimelineHeaderWidget;
  friend class timeline::TimelineLayoutHelper;
  friend class timeline::TimelineRuler;
  friend class timeline::GroupTrack;
};

}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_WIDGET_HPP

