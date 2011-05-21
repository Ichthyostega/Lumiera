/*
  timeline-body.hpp  -  Declaration of the timeline body widget

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

/** @file timeline-body.hpp
 ** This file contains the definition of timeline body widget
 */

#ifndef WIDGETS_TIMELINE_BODY_H
#define WIDGETS_TIMELINE_BODY_H

#include "gui/gtk-lumiera.hpp"
#include "gui/widgets/timeline/timeline-tool.hpp"
#include "lib/time/timevalue.hpp"

#include <boost/scoped_ptr.hpp>

namespace gui {

namespace model {
class Track;
}
  
namespace widgets {

class TimelineWidget;

namespace timeline {

using lib::time::TimeVar;


class Track;
class TimelineViewWindow;


/**
 * Implementation of the timeline body subwidget. This widget is
 * displayed in the centre of the timeline widget, and displays the
 * content of all timeline tracks.
 */
class TimelineBody : public Gtk::DrawingArea
{
public:

  /**
   * Constructor
   * @param timeline_widget A reference to the owner widget of this
   * ruler.
   */
  TimelineBody(gui::widgets::TimelineWidget &timeline_widget);
  
  /**
   * Destructor
   */
  ~TimelineBody();
  
  TimelineWidget&
  getTimelineWidget () const;

  /**
   * Returns the type of the currently selected timeline tool.
   */
  ToolType
  get_tool() const;
  
  /**
   * Selects a tool of a specified type.
   * @param tool_type The type of tool to set.
   */
  void
  set_tool(ToolType tool_type);
  
  /* ===== Events ===== */
protected:

  /**
   * An event handler for when the view window of the timeline changes.
   */
  void on_update_view();

  /**
   * An event handler for when the widget is realized.
   */
  void on_realize();
  
  /**
   * An event handler for when the window must be redrawn.
   */
  bool on_expose_event(GdkEventExpose* event);
  
  bool on_scroll_event(GdkEventScroll* event);
  
  /**
   * The event handler for button press events.
   */
  bool on_button_press_event(GdkEventButton* event);
  
  /**
   * The event handler for button release events.
   */
  bool on_button_release_event(GdkEventButton* event);
  
  /**
   * The event handler for mouse move events.
   */
  bool on_motion_notify_event(GdkEventMotion *event);
  
  /**
   * The event handler for when the TimelineWidget's state object is
   * replaced.
   */
  void on_state_changed();
  
  /* ===== Internals ===== */
private:

  /**
   * Draws the timeline tracks.
   * @param cr The cairo context to draw into.
   */
  void draw_tracks(Cairo::RefPtr<Cairo::Context> cr);
  
  void draw_track(Cairo::RefPtr<Cairo::Context> cr,
    boost::shared_ptr<timeline::Track> timeline_track,
    const int view_width) const;
  
  /**
   * Draws the selected timeline period.
   * @param cr The cairo context to draw into.
   */
  void draw_selection(Cairo::RefPtr<Cairo::Context> cr);
  
  /**
   * Draws the current playback point, if any.
   * @param cr The cairo context to draw into.
   */
  void draw_playback_point(Cairo::RefPtr<Cairo::Context> cr);

  void begin_shift_drag();
  
  int get_vertical_offset() const;
  
  void set_vertical_offset(int offset);
  
  /**
   * A helper function to get the view window
   * @remarks This function must not be called unless the TimlineWidget
   * has a valid state.
   */
  TimelineViewWindow& view_window() const;
   
  /**
   * Registers all the styles that this class will respond to.
   */
  void register_styles() const;

  /**
   * Reads styles from the present stylesheet.
   */
  void read_styles();
  
private:

  // Internal structures
  enum DragType
  {
    None,
    Shift
  };
  
  boost::scoped_ptr<timeline::Tool> tool;
  double mouseDownX, mouseDownY;
  
  // Scroll State
  DragType dragType;
  TimeVar beginShiftTimeOffset;
  int beginShiftVerticalOffset; 

  // Style properties
  Cairo::RefPtr<Cairo::SolidPattern> backgroundColour;
  Cairo::RefPtr<Cairo::SolidPattern> selectionColour;
  float selectionAlpha;
  Cairo::RefPtr<Cairo::SolidPattern> playbackPointColour;
  
  gui::widgets::TimelineWidget &timelineWidget;

  friend class Tool;
  friend class ArrowTool;
  friend class IBeamTool;
};

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_BODY_HPP
