/*
  TimelineHeaderContainer  -  header container widget

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

* *****************************************************/


/** @file timeline-header-container.cpp
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */


#include "stage/widget/timeline/timeline-header-container.hpp"
#include "stage/widget/timeline/timeline-track.hpp"
#include "stage/widget/timeline-widget.hpp"
#include "stage/draw/rectangle.hpp"

#include <boost/foreach.hpp>
#include <memory>

using std::pair;
using std::shared_ptr;

using namespace Gtk;         ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace util;        ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace stage {
namespace widget {
namespace timeline {
  
  
  // ===== Constants ===== //
  
  const int TimelineHeaderContainer::ScrollSlideRateDivisor = 4;
  const int TimelineHeaderContainer::ScrollSlideEventInterval = 40;
  
  
  // ===== Implementation ===== //
  
  TimelineHeaderContainer::TimelineHeaderContainer(gui::widget::TimelineWidget &timeline_widget)
    : Glib::ObjectBase("TimelineHeaderContainer")
    , timelineWidget(timeline_widget)
    {
      // This widget will not have a window at first
      set_has_window (false);
      
      set_redraw_on_allocate(false);
      
      // Connect to the timeline widget's vertical scroll event,
      // so that we get notified when the view shifts
      timelineWidget.verticalAdjustment->signal_value_changed().connect(
        sigc::mem_fun(this, &TimelineHeaderContainer::on_scroll) );
      
      // Connect to the timeline widget's hover event,
      // so that we get notified when tracks are hovered on
      timelineWidget.hovering_track_changed_signal().connect(
        sigc::mem_fun(this,
        &TimelineHeaderContainer::on_hovering_track_changed) );
      
#if false
      // Create the context menu
      Menu::MenuList& menu_list = contextMenu.items();
      menu_list.push_back( Menu_Helpers::MenuElem(_("_Add Track"),
        sigc::mem_fun(timelineWidget,
        &TimelineWidget::on_add_track_command) ) );
#endif
    }
  
  
  void
  TimelineHeaderContainer::update_headers()
  {
    // Ensure headers are parented correctly
    pair<shared_ptr<model::Track>, shared_ptr<timeline::Track> > pair;
    BOOST_FOREACH( pair, timelineWidget.trackMap )
      {
        REQUIRE(pair.second);
        Widget &widget = pair.second->get_header_widget();
        if(widget.get_parent() == NULL)  // Is the header unparented?
          widget.set_parent(*this);
        ENSURE(widget.get_parent() == this);
      }
  }
  
  void
  TimelineHeaderContainer::clear_headers()
  {
    // Unparent all the headers
    pair<shared_ptr<model::Track>, shared_ptr<timeline::Track> > pair;
    BOOST_FOREACH( pair, timelineWidget.trackMap )
      {
        REQUIRE(pair.second);
        Widget &widget = pair.second->get_header_widget();
        if(widget.get_parent() != NULL)  // Is the header unparented?
          widget.unparent();
      }
  }
  
  void
  TimelineHeaderContainer::on_realize()
  {
    set_has_window (false);
    
    // Call base class:
    Gtk::Container::on_realize();
    
    // Create the GdkWindow:
    GdkWindowAttr attributes;
    memset(&attributes, 0, sizeof (attributes));
    
    Allocation allocation = get_allocation();
    
    // Set initial position and size of the Gdk::Window:
    attributes.x = allocation.get_x();
    attributes.y = allocation.get_y();
    attributes.width = allocation.get_width();
    attributes.height = allocation.get_height();
    
    attributes.event_mask = get_events () | Gdk::EXPOSURE_MASK; 
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.wclass = GDK_INPUT_OUTPUT;
    
    gdkWindow = Gdk::Window::create(get_window(), &attributes,
            GDK_WA_X | GDK_WA_Y);
    //unset_flags(Gtk::NO_WINDOW);
    set_window(gdkWindow);
    
    // Unset the background so as to make the colour match the parent
    // window
    //unset_bg(STATE_NORMAL);
    
    // Make the widget receive expose events
    gdkWindow->set_user_data(gobj());
    
    // Make the widget sensitive to mouse events
    add_events(
      Gdk::POINTER_MOTION_MASK |
      Gdk::BUTTON_PRESS_MASK |
      Gdk::BUTTON_RELEASE_MASK);
  }
  
  void
  TimelineHeaderContainer::on_unrealize()
  {
    // Unreference any window we may have created
    gdkWindow.clear();
    
    // Call base class:
    Container::on_unrealize();
  }
  
  
  bool
  TimelineHeaderContainer::on_button_press_event (GdkEventButton* event)
  {
    REQUIRE(event != NULL);
    switch(event->button)
      {
      case 1: // Left Click
        break;
        
      case 3: // Right Click
        {
          // Popup the context menu
          shared_ptr<Track> header(
            timelineWidget.layoutHelper.header_from_point(
              Gdk::Point(event->x, event->y)));
          
          // Are we hovering on a header?
          if(header)
            {
              // Yes - show the header's context menu
              header->show_header_context_menu(
                event->button, event->time);
            }
          else
            {
              // No - show the default context menu
              contextMenu.popup(event->button, event->time);
            }
          break;
        }
      }
    
    return true;
  }
  
  
  bool
  TimelineHeaderContainer::on_button_release_event (GdkEventButton* event)
  {
    end_drag();
    return Container::on_button_release_event(event);
  }
  
  
  bool
  TimelineHeaderContainer::on_motion_notify_event (GdkEventMotion* event)
  {
    REQUIRE(event != NULL);
    REQUIRE(gdkWindow);
    
    const bool result = Container::on_motion_notify_event(event);
    
    TimelineLayoutHelper &layout = timelineWidget.layoutHelper;
    
    // Get the mouse point
    int window_x = 0, window_y = 0;
    gdkWindow->get_origin(window_x, window_y);
    mousePoint = Gdk::Point(event->x_root - window_x,
                            event->y_root - window_y);
    
    // Are we beginning to drag a header?
    if ((event->state & GDK_BUTTON1_MASK) && hoveringTrack &&
       !layout.is_dragging_track())
      {
        begin_drag();      
        return result;
      }
    
    // Are we currently dragging?
    if (layout.is_dragging_track())
      {
        // Forward the message to the layout manager
        layout.drag_to_point(mousePoint);
        
        // Is the mouse out of bounds? if so we must begin scrolling
        const int height = get_allocation().get_height();
        const int y = mousePoint.get_y();
        
        if(y < 0)
          begin_scroll_slide(y / ScrollSlideRateDivisor);
        else if(y > height)
          begin_scroll_slide((y - height) / ScrollSlideRateDivisor);
        else end_scroll_slide();
        
        return result;
      }
    
    // Hit test the rectangle
    hoveringTrack = layout.header_from_point(mousePoint);
    
    return result;
  }
  
  
  void
  TimelineHeaderContainer::on_size_request (Requisition* requisition)
  {
    // We don't care about the size of all the child widgets, but if we
    // don't send the size request down the tree, some widgets fail to
    // calculate their text layout correctly. 
    
    const TimelineLayoutHelper::TrackTree &layout_tree =
      timelineWidget.layoutHelper.get_layout_tree();
    
    // Send a size request to all the children
    if (!layout_tree.empty())
      {
        TimelineLayoutHelper::TrackTree::pre_order_iterator iterator;
        for(iterator = ++layout_tree.begin(); // ++ so that we skip the sequence root
          iterator != layout_tree.end();
          iterator++)
          {
            Widget &widget =
              lookup_timeline_track(*iterator)->get_header_widget();
            if(widget.get_visible())
              { /* widget.size_request(); */ }
          }
      }
    
    // Initialize the output parameter:
    *requisition = Gtk::Requisition();
    requisition->width = TimelineWidget::HeaderWidth;
    requisition->height = 0;
  }
  
  
  void
  TimelineHeaderContainer::on_size_allocate (Allocation& allocation)
  {
    // Use the offered allocation for this container:
    set_allocation(allocation);
    
    // Resize the widget's window
    if (gdkWindow)
      {
        gdkWindow->move_resize(allocation.get_x(), allocation.get_y(),
                               allocation.get_width(), allocation.get_height());
      }
    
    // Relayout the child widgets of the headers
    layout_headers();
  }
  
  
  void
  TimelineHeaderContainer::forall_vfunc (gboolean /* include_internals */,
                                         GtkCallback callback,
                                         gpointer callback_data)
  {
    REQUIRE(callback != NULL);
    
    pair<shared_ptr<model::Track>, shared_ptr<timeline::Track> > pair;
    BOOST_FOREACH( pair, timelineWidget.trackMap )
      {
        REQUIRE(pair.second);
        GtkWidget *widget = pair.second->get_header_widget().gobj();
        
        REQUIRE(widget);
        callback(widget, callback_data);
      }
  }
  
  
  void
  TimelineHeaderContainer::on_remove (Widget* widget)
  {
    REQUIRE(widget);
    
    // Ensure headers are parented correctly
    pair<shared_ptr<model::Track>, shared_ptr<timeline::Track> > pair;
    BOOST_FOREACH( pair, timelineWidget.trackMap )
      {
        REQUIRE(pair.second);
        Widget &this_widget = pair.second->get_header_widget();
        if(&this_widget == widget)
          this_widget.unparent();
      }
  }
  
  
  void
  TimelineHeaderContainer::on_layout_changed()
  {
    layout_headers();
  }
  
  void
  TimelineHeaderContainer::on_scroll()
  {
    // If the scroll has changed, we will have to shift
    // all the header widgets
    layout_headers();
  }
  
  void
  TimelineHeaderContainer::on_hovering_track_changed(shared_ptr<timeline::Track>)
  {
    /* do nothing */
  }
  
  
  bool
  TimelineHeaderContainer::on_scroll_slide_timer()
  {
    // Shift the view
    const int view_height = get_allocation().get_height();
    timelineWidget.set_y_scroll_offset(
      timelineWidget.get_y_scroll_offset() +
      scrollSlideRate * view_height / 256);
    
    // Keep the layout manager updated
    timelineWidget.layoutHelper.drag_to_point(mousePoint);
    
    // Return true to keep the timer going
    return true;
  }
  
  
  void
  TimelineHeaderContainer::layout_headers()
  { 
    // We can't layout before the widget has been set up
    if(!gdkWindow)
      return;
    
    bool headers_shown = false;
    
    TimelineLayoutHelper &layout_helper =
      timelineWidget.layoutHelper;
    const TimelineLayoutHelper::TrackTree &layout_tree =
      layout_helper.get_layout_tree();
    
    if (!layout_tree.empty())
      {
        TimelineLayoutHelper::TrackTree::pre_order_iterator iterator;
        for(iterator = ++layout_tree.begin(); // ++ so that we skip the sequence root
          iterator != layout_tree.end();
          iterator++)
          {
            const shared_ptr<timeline::Track> timeline_track =
              lookup_timeline_track(*iterator);
            
            Widget &widget = timeline_track->get_header_widget();
            
            boost::optional<Gdk::Rectangle> header_rect =
              layout_helper.get_track_header_rect(timeline_track);
            
            if(header_rect)
              {
                REQUIRE(header_rect->get_width() >= 0);
                REQUIRE(header_rect->get_height() >= 0);
                
                // Apply the allocation to the header
                widget.size_allocate (*header_rect);
                if(!widget.get_visible())
                  {
                    widget.show();
                    headers_shown = true;
                  }
              }
            else // No header rect, so the track must be hidden
              if(widget.get_visible())
                widget.hide();
          }
        
        // If headers have been shown while we're dragging, the dragging
        // branch headers have to be brought back to the top again
        if(headers_shown && layout_helper.is_dragging_track())
          raise_recursive(layout_helper.get_dragging_track_iter());
        
        // Repaint the background of our parenting
        queue_draw();
      }
  }
  
  
  shared_ptr<timeline::Track>
  TimelineHeaderContainer::lookup_timeline_track (shared_ptr<model::Track> modelTrack)
  {
    REQUIRE(modelTrack != NULL);
    
    shared_ptr<timeline::Track> timeline_track =
      timelineWidget.lookup_timeline_track(modelTrack);
    ENSURE(timeline_track);
    
    return timeline_track;
  }
  
  
  void
  TimelineHeaderContainer::begin_drag()
  {
    TimelineLayoutHelper &layout = timelineWidget.layoutHelper;
    layout.begin_dragging_track(mousePoint);
    
    // Raise all the header widgets so they float above the widgets not
    // being dragged
    raise_recursive(layout.get_dragging_track_iter());
    
    // Set the cursor to a hand
    REQUIRE(gdkWindow);
    //gdkWindow->set_cursor(Gdk::Cursor(Gdk::FLEUR));
  }
  
  
  void
  TimelineHeaderContainer::end_drag (bool apply)
  {
    TimelineLayoutHelper &layout = timelineWidget.layoutHelper;
     
    // Has the user been dragging?
    if(layout.is_dragging_track())
      layout.end_dragging_track(apply);
    
    // End the scroll slide
    end_scroll_slide();
    
    // Reset the arrow as a cursor
    REQUIRE(gdkWindow);
    //gdkWindow->set_cursor(Gdk::Cursor(Gdk::LEFT_PTR));
  }
  
  
  void
  TimelineHeaderContainer::raise_recursive (TimelineLayoutHelper::TrackTree::iterator_base node)
  {
    TimelineLayoutHelper::TrackTree::pre_order_iterator iter;
    
    const TimelineLayoutHelper::TrackTree &layout_tree =
      timelineWidget.layoutHelper.get_layout_tree();
      
    shared_ptr<timeline::Track> timeline_track =
      lookup_timeline_track(*node);
    REQUIRE(timeline_track);
    
    Glib::RefPtr<Gdk::Window> window =
      timeline_track->get_header_widget().get_window();
    ENSURE(window); // Something strange has happened if there was no
                    // window
    window->raise();
    
    for(iter = layout_tree.begin(node);
      iter != layout_tree.end(node);
      iter++)
      {
        raise_recursive(iter);
      }
  }
  
  
  void
  TimelineHeaderContainer::begin_scroll_slide (int scroll_slide_rate)
  {
    scrollSlideRate = scroll_slide_rate;
    if(!scrollSlideEvent.connected())
      scrollSlideEvent = Glib::signal_timeout().connect(sigc::mem_fun(
        this, &TimelineHeaderContainer::on_scroll_slide_timer),
        ScrollSlideEventInterval);
  }
  
  
  void
  TimelineHeaderContainer::end_scroll_slide()
  {
    scrollSlideRate = 0;
    if(scrollSlideEvent.connected())
      scrollSlideEvent.disconnect();
  }
  
  
}}}// namespace stage::widget::timeline
