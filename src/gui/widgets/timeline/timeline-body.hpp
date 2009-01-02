/*
  timeline-body.hpp  -  Declaration of the timeline body widget
 
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
/** @file timeline-body.hpp
 ** This file contains the definition of timeline body widget
 */

#ifndef TIMELINE_BODY_HPP
#define TIMELINE_BODY_HPP

#include "../../gtk-lumiera.hpp"
#include "timeline-tool.hpp"

namespace gui {

namespace model {
class Track;
}
  
namespace widgets {

class TimelineWidget;

namespace timeline {

class Track;

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
   * @param timeline_widget The owner widget of this ruler.
   */
  TimelineBody(gui::widgets::TimelineWidget *timeline_widget);
  
  /**
   * Destructor
   */
  ~TimelineBody();
  
  /**
   * Returns the type of the currently selected timeline tool.
   */
  ToolType get_tool() const;
  
  /**
   * Selects a tool of a specified type.
   * @param tool_type The type of tool to set.
   */
  void set_tool(ToolType tool_type);

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
  
  /* ===== Internals ===== */
private:

  /**
   * Draws the timeline tracks.
   * @param cr The cairo context to draw into.
   */
  void draw_tracks(Cairo::RefPtr<Cairo::Context> cr);
  
  void draw_track(Cairo::RefPtr<Cairo::Context> cr,
    boost::shared_ptr<model::Track> track, const int view_width) const;
  
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
  
  boost::shared_ptr<timeline::Track> track_from_point(const int y)
    const;
  
  boost::shared_ptr<timeline::Track> track_from_branch(
    boost::shared_ptr<model::Track> model_track,
    const int y, int &offset) const;
  
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
  
  timeline::Tool *tool;
  double mouseDownX, mouseDownY;
  
  // Scroll State
  DragType dragType;
  gavl_time_t beginShiftTimeOffset;
  int beginShiftVerticalOffset; 

  // Style properties
  GdkColor backgroundColour;
  GdkColor selectionColour;
  float selectionAlpha;
  GdkColor playbackPointColour;
  
  gui::widgets::TimelineWidget* const timelineWidget;

  friend class Tool;
  friend class ArrowTool;
  friend class IBeamTool;
};

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_BODY_HPP
