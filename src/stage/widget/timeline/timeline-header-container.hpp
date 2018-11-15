/*
  TIMELINE-HEADER-CONTAINER.hpp  -  header container widget

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


/** @file timeline-header-container.hpp
 ** A container to hold the header area of the timeline display.
 ** Most notably this was used to create a first version of the patchbay
 ** and control areas for the tracks. Very likely to be completely rewritten. 
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */


#ifndef STAGE_WIDGET_TIMELINE_HEADER_CONTAINER_H
#define STAGE_WIDGET_TIMELINE_HEADER_CONTAINER_H

#include "stage/gtk-base.hpp"
#include "timeline-layout-helper.hpp"

namespace stage {
  namespace model {
    class Track;
  }
  namespace widget {
    class TimelineWidget;
  
  
  namespace timeline {
  
  class Track;
  
  /**
   * A helper class for the TimelineWidget. TimelineHeaderContainer
   * is container widget for all the left-hand-side header widgets
   * associated with timeline tracks.
   */
  class TimelineHeaderContainer
    : public Gtk::Container
    {
    public:
      TimelineHeaderContainer(stage::widget::TimelineWidget &timeline_widget);
      
      /**
       * Attaches the header all the header widgets of root
       * tracks to this control.
       *
       * @note This must be called when the track list changes
       * to synchronise the headers with the timeline body and
       * the core data.
       */
      void update_headers();
      
      void clear_headers();
      
      
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
       * An event handler that is called to notify this widget to allocate
       * a given area for itself.
       * @param allocation The area to allocate for this widget.
       */
      void on_size_allocate (Gtk::Allocation& allocation);
      
      /**
       * An event handler that is called to offer an allocation to this
       * widget.
       * @param requisition The area offered for this widget.
       */
      void on_size_request (Gtk::Requisition* requisition);
      
      /**
       * Applies a given function to all the widgets in the container.
       */
      void forall_vfunc(gboolean include_internals, GtkCallback callback,
                        gpointer callback_data);
      
      /**
       * An event handler that is called when a widget is removed from the
       * container.
       */
      void on_remove(Widget* widget);
      
      
      /* ===== Events ===== */
    private:
      
      void on_layout_changed();
      
      /**
       * This event is called when the scroll bar moves.
       */  
      void on_scroll();
      
      void on_hovering_track_changed(shared_ptr<timeline::Track> hovering_track);
      
      
    private:
      /* ===== Internal Event Handlers ===== */
      
      /**
       * An internal event handler, which is called when the scroll slide
       * timer calls it.
       */
      bool on_scroll_slide_timer();
      
      
      /* ===== Internals ===== */
    private:
      
      /**
       * Moves all the header widgets to the correct position given scroll,
       * stacking etc.
       */
      void layout_headers();
      
      /**
       * Draws the border decoration around the track header.
       * @param modelTrack The track to draw the decoration for.
       * @param clip_rect The clip to drawing to.
       * @param depth The depth within the tree of this track. This is used
       * to control the amount of indention.
       * @param offset The vertical offset of the headers in pixels.
       */
      void draw_header_decoration(shared_ptr<model::Track> modelTrack,
                                  const Gdk::Rectangle &clip_rect);
      
      /**
       * A helper function which calls lookup_timeline_track within the
       * parent timeline widget, but also applies lots of data consistency
       * checks in the process.
       * @param modelTrack The model track to look up in the parent widget.
       * @return Returns the track found, or returns NULL if no matching
       * track was found.
       * @remarks If the return value is going to be NULL, an ENSURE will
       * fail.
       * @deprecated for #955
       */
      shared_ptr<timeline::Track>
      lookup_timeline_track (shared_ptr<model::Track> modelTrack);
      
      
      void begin_drag();
      void end_drag(bool apply = true);
      
      
      /**
       * Recursively raises all the header widget windows in a branch to the 
       * top of the Z-order.
       * @param node The window of node's track header will be raised, as
       * well as all it's descendant nodes.
       */
      void raise_recursive(
        TimelineLayoutHelper::TrackTree::iterator_base node);
        
      /**
       * Begins, or continues a scroll slide at a given rate
       * @param scroll_slide_rate The distance to slide every timer event
       *        in units of 1/256th of the view height.
       */
      void begin_scroll_slide(int scroll_slide_rate);
      
      /**
       * Ends a scroll slide, and disconnects the slide timer
       */
      void end_scroll_slide();
      
    private:
      
      /**
       * The owner TimelineWidget of which this class is a helper
       */
      stage::widget::TimelineWidget &timelineWidget;
      
      /**
       * The widget's window object. 
       * 
       * @note This is needed for the sake of clipping when
       * widgets are scrolled.
       */
      Glib::RefPtr<Gdk::Window> gdkWindow;
      
      /**
       * The Menu object which will be displayed as the context menu when
       * the user right clicks on the header container.
       * @remarks The context menu will be displayed when the user's right
       * click is not processed by track headers.
       */
      Gtk::Menu contextMenu;
      
      /**
       * This connection is used to represent the timer which causes scroll
       * sliding to occur.
       * @remarks Scroll sliding is an animated scroll which occurs when
       * the user drags a header outside the area of the timeline body.
       */
      sigc::connection scrollSlideEvent;
      
      /**
       * Specifies the rate at which scroll sliding is currently taking
       * place.
       */
      int scrollSlideRate;
        
      //----- User Interaction State -----//  
      shared_ptr<timeline::Track> hoveringTrack;
      
      Gdk::Point mousePoint;
      
      
      /* ===== Constants ===== */  
      /**
       * The amount to divide the mouse overshoot by to produce the slide
       * scroll rate.
       * @remarks Smaller values cause faster scrolling.
       */
      static const int ScrollSlideRateDivisor;
      
      /**
       * The interval between scroll slide events in ms.
       */
      static const int ScrollSlideEventInterval;
      
      friend class stage::widget::TimelineWidget;
      friend class timeline::Track;
    };
  
  
}}}// namespace stage::widget::timeline
#endif /*STAGE_WIDGET_TIMELINE_HEADER_CONTAINER_H*/

