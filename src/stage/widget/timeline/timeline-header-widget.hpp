/*
  TIMELINE-HEADER-WIDGET.hpp  -  widget to render the timeline header

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


/** @file timeline-header-widget.hpp
 ** Widget to generate the timeline header
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */


#ifndef STAGE_WIDGET_HEADER_WIDGET_H
#define STAGE_WIDGET_HEADER_WIDGET_H

#include "stage/gtk-base.hpp"

namespace gui {  
namespace widget {
namespace timeline {
  
  class Track;
  
  /**
   * TimelineHeaderWidget is the base implementation of all header widgets
   * and acts as a containers for the header controls.
   */
  class TimelineHeaderWidget
    : public Gtk::Container
    {
    public:
      /** @param timeline_track track that owns this header widget */
      TimelineHeaderWidget (timeline::Track& timeline_track);
      
      void set_child_widget (Widget& child);
      
      
    private:
      /* ===== Overrides ===== */
      
      /**
       * An event handler for the window realized signal.
       */
      void on_realize();
      
      /**
       * An event handler for the window unrealized signal.
       */
      void on_unrealize();
      
      /**
       * An event handler that is called to offer
       * an allocation to this widget.
       * @param requisition The area offered for this widget.
       */
      void on_size_request (Gtk::Requisition* requisition);
      
      /**
       * An event handler that is called to notify this widget
       * to allocate a given area for itself.
       * @param allocation The area to allocate for this widget.
       */
      void on_size_allocate (Gtk::Allocation& allocation);
      
      /**
       * An event handler for when the window must be redrawn.
       */
      bool on_expose_event (GdkEventExpose *event);
      
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
      
      /**
       * Applies a given function to all the widgets in the container.
       */
      void forall_vfunc (gboolean include_internals, GtkCallback callback, gpointer callback_data);
      
      /**
       * A notification of when a widget is added to this container.
       */
      void on_add (Gtk::Widget* child);
      
      /**
       * A notification of when a widget is removed to this container.
       */
      void on_remove (Gtk::Widget* child);
      
      /**
       * An implementation of the a container function that specifies the
       * types of child widget that this widget will accept.
       */
      GType child_type_vfunc() const;
      
      /**
       * Registers all the styles that this class will respond to.
       */
      void register_styles() const;
      
      /**
       * Reads styles from the present stylesheet.
       */
      void read_styles();
      
    private:
      
      /**
       * A reference to the timeline track that owns this widget.
       */
      timeline::Track &track;
      
      /**
       * The widget placed inside this container.
       * @remarks This value is set to NULL if the container is empty
       */
      Gtk::Widget* widget;
      
      /**
       * This value is true if the mouse hovering over the expander.
       * @remarks This value is updated by on_motion_notify_event
       */
      bool hoveringExpander;
      
      /**
       * This value is true if the mouse button
       * is depressed over the expander.
       * @remarks This value is updated both by
       *          on_button_press_event and on_button_release_event
       */
      bool clickedExpander;
      
      /**
       * The widget's window object. 
       */
      Glib::RefPtr<Gdk::Window> gdkWindow;
      
      //----- Style Values -----//
      
      /**
       * The style value which indicates the amount of padding around each
       * header pixels.
       */
      int margin;
      
      /**
       * The style value which indicates the size to draw the expand button
       * in pixels.
       */
      int expand_button_size;
    };
  
  
}}}// namespace gui::widget::timeline
#endif /*STAGE_WIDGET_HEADER_WIDGET_H*/
