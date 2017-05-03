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
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */

#ifndef GUI_WIDGET_TIMELINE_BODY_H
#define GUI_WIDGET_TIMELINE_BODY_H

#include "gui/gtk-base.hpp"
#include "gui/widget/timeline/timeline-tool.hpp"
#include "lib/time/timevalue.hpp"

#include <memory>

namespace gui {

namespace model {
class Track;
}
  
namespace widget {

class TimelineWidget;

namespace timeline {

using lib::time::TimeVar;
using lib::time::TimeSpan;


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
  TimelineBody(gui::widget::TimelineWidget &timeline_widget);
  
  virtual ~TimelineBody();
  
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
  set_tool(ToolType tool_type, bool force=false);
  
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
   * @todo looks like this was the main drawing hook, and happens to be discontinued in GTK3.   ///////////////////TICKET #937 : custom timeline widget draw function not invoked anymore
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
   * The event handler for when the TimelineWidget's state is switched.
   * @deprecated needs to be rewritten from scratch for GTK-3
   */
  void on_state_changed (shared_ptr<TimelineState> newState);
  
  /* ===== Internals ===== */
private:
  /**
   * Access the current timeline view window
   * @warning must not be called unless the TimlineWidget
   *          has a valid state.
   */
  TimelineViewWindow& viewWindow() const;

  /**
   * Draws the timeline tracks.
   * @param cr The cairo context to draw into.
   */
  void draw_tracks(Cairo::RefPtr<Cairo::Context> cr);
  
  void draw_track(Cairo::RefPtr<Cairo::Context> cr,
                  shared_ptr<timeline::Track> timeline_track,
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
  
  /** adjust to the new timeline state */
  void propagateStateChange();

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
  
  std::unique_ptr<timeline::Tool> tool;
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
  
  gui::widget::TimelineWidget &timelineWidget;
  shared_ptr<TimelineState> timelineState;
  

  friend class Tool;
  friend class ArrowTool;
  friend class IBeamTool;
};
  
  
}}}// namespace gui::widget::timeline
#endif /*GUI_WIDGET_TIMELINE_BODY_H*/
