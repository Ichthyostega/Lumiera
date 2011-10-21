/*
  timeline-tool.hpp  -  Declaration of the Tool class

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

*/
/** @file timeline-tool.hpp
 ** This file contains the definition of base class for timeline
 ** tool objects
 */

#ifndef TIMELINE_TOOL_HPP
#define TIMELINE_TOOL_HPP

#include "gui/gtk-lumiera.hpp"

namespace gui {
namespace widgets {

class TimelineWidget;
  
namespace timeline {
  
class TimelineBody;
class TimelineState;
class TimelineViewWindow;

/**
 * Specifies the types of different timeline tool classes.
 */
enum ToolType
{
  None,
  Arrow,
  IBeam
};

/**
 * The base class of all timeline tools.
 */
class Tool
{
protected:
  /**
   * Constructor
   * @param timelineBody The owner timeline body object
   */
  Tool(TimelineBody &timelineBody);
  
public:
  /**
   * Destructor to be overridden by derived classes.
   * @remarks If this were not present, derrived class destructors
   * would not be called.
   */
  virtual ~Tool() {};

  /**
   * Gets the type of tool represented by this class.
   * @remarks This must be implemented by all timeline tool classes.
   */
  virtual ToolType get_type() const = 0;
  
  /**
   * Reaplies the cursor for the current tool at the current moment.
   */
  bool apply_cursor();
  
public:
  /* ===== Event Handlers ===== */
  /**
   * The event handler for button press events.
   * @remarks This can be overridden by the derived classes, but
   * Tool::on_button_press_event must be called <b>at the start</b>
   * of the derived class's override.
   */
  virtual void on_button_press_event(GdkEventButton* event);
  
  /**
   * The event handler for button release events.
   * @remarks This can be overridden by the derived classes, but
   * Tool::on_button_release_event must be called <b>at the end</b> of
   * the derived class's override.
   */
  virtual void on_button_release_event(GdkEventButton* event);
  
  /**
   * The event handler for mouse move events.
   * @remarks This can be overridden by the derived classes, but
   * Tool::on_motion_notify_event must be called <b>at the start</b> of
   * the derived class's override.
   */
  virtual void on_motion_notify_event(GdkEventMotion *event);
  
protected:
  /* ===== Internal Overrides ===== */
  /**
   * Gets the cursor to display for this tool at this moment.
   * @remarks This must be implemented by all timeline tool classes.
   */
  virtual Gdk::Cursor get_cursor() const = 0;

protected:
  /* ===== Utilities ===== */
  
  /**
   * Helper function which retrieves the pointer to owner timeline
   * widget object, which is the owner of the timeline body.
   */
  gui::widgets::TimelineWidget &get_timeline_widget() const;
  
  /**
   * Helper function which retrieves the rectangle of the timeline
   * body.
   */
  Gdk::Rectangle get_body_rectangle() const;
  
  /**
   * A helper function to get the state
   */
  std::tr1::shared_ptr<TimelineState> get_state() const;
  
  /**
   * A helper function to get the view window
   */
  TimelineViewWindow& view_window() const;

protected:
  TimelineBody &timelineBody;

  bool isDragging;
  Gdk::Point mousePoint;
};

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_TOOL_HPP
