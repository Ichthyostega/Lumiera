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

#include "timeline/timeline-view-window.hpp"
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
   */
  TimelineWidget(boost::shared_ptr<model::Sequence> source_sequence);

  /**
   * Destructor
   */
  ~TimelineWidget();
  
  /* ===== Data Access ===== */
public:

  /**
   * Gets a reference to the timeline view window object.
   * @return Returns the timeline view window object.
   **/
  timeline::TimelineViewWindow& get_view_window();
  
  /**
   * Gets the time at which the selection begins.
   */
  gavl_time_t get_selection_start() const;
  
  /**
   * Gets the time at which the selection begins.
   */
  gavl_time_t get_selection_end() const;
  
  /**
   * Sets the period of the selection.
   * @param start The start time.
   * @param end The end time.
   * @param reset_playback_period Specifies whether to set the playback
   * period to the same as this new selection.
   */
  void set_selection(gavl_time_t start, gavl_time_t end,
    bool reset_playback_period = true);
  
  /**
   * Gets the time at which the playback period begins.
   */
  gavl_time_t get_playback_period_start() const;
  
  /**
   * Gets the time at which the playback period ends.
   */
  gavl_time_t get_playback_period_end() const;
  
  /**
   * Sets the playback period.
   * @param start The start time.
   * @param end The end time.
   */
  void set_playback_period(gavl_time_t start, gavl_time_t end);
  
  /**
   * Sets the time which is currenty being played back.
   * @param point The time index being played. This value may be
   * GAVL_TIME_UNDEFINED, if there is no playback point.
   */
  void set_playback_point(gavl_time_t point);
  
  /**
   * Gets the current playback point.
   * @return The time index of the playback point. This value may be
   * GAVL_TIME_UNDEFINED, if there is no playback point.
   */
  gavl_time_t get_playback_point() const;
  
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
  sigc::signal<void, gavl_time_t> mouse_hover_signal() const;
  
  sigc::signal<void> playback_period_drag_released_signal() const;
  
  sigc::signal<void, boost::shared_ptr<timeline::Track> >
    hovering_track_changed_signal() const;
  
  /* ===== Events ===== */
protected:
  
  void on_scroll();
  
  void on_size_allocate(Gtk::Allocation& allocation);
  
  void on_view_window_changed();
  
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
    
  /**
   * Looks up a model track in trackMap that corresponds to a
   * given timeline track.
   * @param timeline_track The timeline UI track to look up.
   * @returns The model track found, or an empty shared_ptr if
   * timeline_track has no corresponding timeline UI track (this is an
   * error condition).
   **/
  boost::shared_ptr<model::Track> lookup_model_track(
    const timeline::Track *timeline_track) const;
  
  // ----- Layout Functions ----- //
  
  void update_scroll();
  
  int measure_branch_height(
    boost::shared_ptr<model::Track> model_track);
  
  int get_y_scroll_offset() const;
  
  // ----- Event Handlers -----//
  
  /**
   * An event handler that receives notifications for when the
   * sequence's track tree has been changed.
   **/
  void on_track_list_changed();
    
  void on_playback_period_drag_released();
  
  bool on_motion_in_body_notify_event(GdkEventMotion *event);  
  
  // ----- Other Functions ----- //
  
  void set_hovering_track(
    boost::shared_ptr<timeline::Track> hovering_track);

protected:

  // Model Data
  
  /**
   * A pointer to the sequence object which this timeline_widget will
   * represent.
   * @remarks This pointer is set by the constructor and is constant, so
   * will not change in value during the lifetime of the class.
   **/
  const boost::shared_ptr<model::Sequence> sequence;
  
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
    
  // Helper Classes
  timeline::TimelineLayoutHelper layoutHelper;

  // View State
  timeline::TimelineViewWindow viewWindow;
  
  // Selection State
  gavl_time_t selectionStart;
  gavl_time_t selectionEnd;
  gavl_time_t playbackPeriodStart;
  gavl_time_t playbackPeriodEnd;
  gavl_time_t playbackPoint;
  
  boost::shared_ptr<timeline::Track> hoveringTrack;

  int totalHeight;

  // Child Widgets
  timeline::TimelineHeaderContainer *headerContainer;
  timeline::TimelineBody *body;
  timeline::TimelineRuler *ruler;

  Gtk::Adjustment horizontalAdjustment, verticalAdjustment;
  Gtk::HScrollbar horizontalScroll;
  Gtk::VScrollbar verticalScroll;
  
  // Signals
  sigc::signal<void, gavl_time_t> mouseHoverSignal;
  sigc::signal<void> playbackPeriodDragReleasedSignal;
  sigc::signal<void, boost::shared_ptr<timeline::Track> >
    hoveringTrackChangedSignal;
   
  /* ===== Constants ===== */
public:
  /**
   * The maximum scale for timeline display.
   * @remarks At MaxScale, every pixel on the timeline is equivalent
   * to 30000000 gavl_time_t increments.
   */ 
  static const int64_t MaxScale;
  
protected:
  static const int TrackPadding;
  static const int HeaderWidth;
  static const double ZoomIncrement;

  friend class timeline::TimelineViewWindow;
  friend class timeline::TimelineBody;
  friend class timeline::TimelineHeaderContainer;
  friend class timeline::TimelineLayoutHelper;
  friend class timeline::TimelineRuler;
  friend class timeline::Tool;
  friend class timeline::ArrowTool;
  friend class timeline::IBeamTool;
  friend class timeline::Track;
  friend class timeline::GroupTrack;
};

}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_WIDGET_HPP

