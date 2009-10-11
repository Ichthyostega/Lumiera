/*
  timeline-state.hpp  -  Declaration of the timeline state object
 
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
/** @file widgets/timeline/timeline-state.hpp
 ** This file contains the definition of the timeline state object
 */

#ifndef TIMELINE_STATE_HPP
#define TIMELINE_STATE_HPP

#include "timeline-view-window.hpp"

namespace gui {
  
namespace model {
class Sequence;
}  
  
namespace widgets { 
namespace timeline {

/**
 * TimelineState is a container for the state data for TimelineWidget.
 * @remarks TimelineState s can be swapped out so that TimelineWidget
 * can flip between different views.
 **/
class TimelineState
{
public:

  /**
   * Constructor
   * @param source_sequence The sequence on which the TimelineWidget
   * will operate when this TimelineState is attached.
   **/
  TimelineState(boost::shared_ptr<model::Sequence> source_sequence);
  
public:

  /**
   * Gets the sequence that is attached to this timeline state object.
   * @return Returns a shared_ptr to the sequence object.
   **/
  boost::shared_ptr<model::Sequence> get_sequence() const;

  /**
   * Gets a reference to the timeline view window object.
   * @return Returns the timeline view window object.
   **/
  timeline::TimelineViewWindow& get_view_window();
  
  /**
   * Gets the time at which the selection begins.
   */
  lumiera::Time get_selection_start() const;
  
  /**
   * Gets the time at which the selection begins.
   */
  lumiera::Time get_selection_end() const;
  
  /**
   * Sets the period of the selection.
   * @param start The start time.
   * @param end The end time.
   * @param reset_playback_period Specifies whether to set the playback
   * period to the same as this new selection.
   */
  void set_selection(lumiera::Time start, lumiera::Time end,
    bool reset_playback_period = true);
  
  /**
   * Gets the time at which the playback period begins.
   */
  lumiera::Time get_playback_period_start() const;
  
  /**
   * Gets the time at which the playback period ends.
   */
  lumiera::Time get_playback_period_end() const;
  
  /**
   * Sets the playback period.
   * @param start The start time.
   * @param end The end time.
   */
  void set_playback_period(lumiera::Time start, lumiera::Time end);
  
  /**
   * Sets the time which is currenty being played back.
   * @param point The time index being played. This value may be
   * GAVL_TIME_UNDEFINED, if there is no playback point.
   */
  void set_playback_point(lumiera::Time point);
  
  /**
   * Gets the current playback point.
   * @return The time index of the playback point. This value may be
   * GAVL_TIME_UNDEFINED, if there is no playback point.
   */
  lumiera::Time get_playback_point() const;
  
  /**
   * A signal to notify when the selected period has changed.
   **/
  sigc::signal<void> selection_changed_signal() const; 
  
  /**
   * A signal to notify when the playback point or playback periods have
   * changed.
   **/
  sigc::signal<void> playback_changed_signal() const; 
  
private:

  /**
   * A pointer to the sequence object which this timeline_widget will
   * represent.
   * @remarks This pointer is set by the constructor and is constant, so
   * will not change in value during the lifetime of the class.
   **/
  boost::shared_ptr<model::Sequence> sequence;
  
  // View State
  /**
   * The ViewWindow for the TimelineWidget display with.
   **/
  timeline::TimelineViewWindow viewWindow;
  
  // Selection State
  
  /**
   * The start time of the selection period.
   **/
  lumiera::Time selectionStart;
  
  /**
   * The end time of the selection period.
   **/
  lumiera::Time selectionEnd;
  
  /**
   * The start time of the playback period.
   **/
  lumiera::Time playbackPeriodStart;
  
  /**
   * The end time of the playback period.
   **/
  lumiera::Time playbackPeriodEnd;
  
  /**
   * The time of the playback point.
   **/
  lumiera::Time playbackPoint;
  
  // Signals
  
  /**
   * A signal to notify when the selected period has changed.
   **/
  sigc::signal<void> selectionChangedSignal;
  
  /**
   * A signal to notify when the playback point or playback periods have
   * changed.
   **/
  sigc::signal<void> playbackChangedSignal;
};

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_STATE_HPP
