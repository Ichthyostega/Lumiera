/*
  timeline-header-widget.hpp  -  Declaration of the timeline
  header widget
 
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
/** @file timeline-header-widget.hpp
 ** This file contains the definition of the header widget
 */

#ifndef HEADER_WIDGET_HPP
#define HEADER_WIDGET_HPP

#include "../../gtk-lumiera.hpp"

namespace gui {  
namespace widgets {
namespace timeline {

class Track;

class TimelineHeaderWidget : public Gtk::Container
{
public:
  TimelineHeaderWidget(timeline::Track &timeline_track);

  void set_child_widget(Widget& child);

private:
  /* ===== Overrides ===== */
private:

  /**
   * An event handler for the window realized signal.
   */
  void on_realize();
  
  /**
   * An event handler for the window unrealized signal.
   */
  void on_unrealize();

  //Overrides:
  void on_size_request(Gtk::Requisition* requisition);
  void on_size_allocate(Gtk::Allocation& allocation);
  
  /**
   * An event handler for when the window must be redrawn.
   */
  bool on_expose_event(GdkEventExpose *event);
  
  /**
   * An event handler for mouse button press events.
   * @param event The GDK event containing details of the event.
   */
  bool on_button_press_event (GdkEventButton* event);
  
  /**
   * An event handler for mouse button release events.
   * @param event The GDK event containing details of the event.
   */
  bool on_button_release_event (GdkEventButton* event);
  
  /**
   * An event handler for mouse movement events.
   * @param event The GDK event containing details of the event.
   */
  bool on_motion_notify_event (GdkEventMotion* event);

  void forall_vfunc(gboolean include_internals, GtkCallback callback, gpointer callback_data);

  void on_add(Gtk::Widget* child);
  void on_remove(Gtk::Widget* child);
  GtkType child_type_vfunc() const;

  /**
   * Registers all the styles that this class will respond to.
   */
  void register_styles() const;
  
  /**
   * Reads styles from the present stylesheet.
   */
  void read_styles();

private:

  timeline::Track &track;

  Gtk::Widget* widget;
  
  bool hoveringExpander;
  
  bool clickedExpander;
  
  /**
   * The widget's window object. 
   **/
  Glib::RefPtr<Gdk::Window> gdkWindow;
  
  //----- Style Values -----//
  
  /**
   * The style value which indicates the amount of padding around each
   * header pixels.
   **/
  int margin;
  
  /**
   * The style value which indicates the size to draw the expand button
   * in pixels.
   **/
  int expand_button_size;
};

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // HEADER_WIDGET_HPP


