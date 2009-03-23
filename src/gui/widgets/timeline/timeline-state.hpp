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

class TimelineState
{
public:

  TimelineState(boost::shared_ptr<model::Sequence> source_sequence);
  
public:

  boost::shared_ptr<model::Sequence> get_sequence() const;

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
  

  sigc::signal<void> selection_changed_signal() const; 
  
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
  timeline::TimelineViewWindow viewWindow;
  
  // Selection State
  gavl_time_t selectionStart;
  gavl_time_t selectionEnd;
  gavl_time_t playbackPeriodStart;
  gavl_time_t playbackPeriodEnd;
  gavl_time_t playbackPoint;
  
  // Signals
  sigc::signal<void> selectionChangedSignal;
  sigc::signal<void> playbackChangedSignal;
};

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_STATE_HPP
