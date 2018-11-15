/*
  TimelineHeaderWidget  -  widget to render the timeline header

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


/** @file timeline-header-widget.cpp
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */


#include "stage/widget/timeline-widget.hpp"
#include "stage/draw/rectangle.hpp"

#include <boost/foreach.hpp>

using namespace Gtk;         ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace std;         ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace util;        ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

using std::shared_ptr;

namespace stage {
namespace widget {
namespace timeline {
  
  
  TimelineHeaderWidget::TimelineHeaderWidget (timeline::Track &timeline_track)
    : Glib::ObjectBase("TimelineHeaderWidget")
    , track(timeline_track)
    , widget(NULL)
    , hoveringExpander(false)
    , clickedExpander(false)
    , margin(-1)
    , expand_button_size(12)
    {
      set_has_window(false);
      set_redraw_on_allocate(false);
      
      // Install style properties
      register_styles();
      
      // Load the styles up
      read_styles();
    }
  
  
  void
  TimelineHeaderWidget::on_realize()
  {
    set_has_window(false);
    Container::on_realize();
    
    // Create the GdkWindow:
    GdkWindowAttr attributes;
    memset(&attributes, 0, sizeof(attributes));
    
    Allocation allocation = get_allocation();
    
    // Set initial position and size of the Gdk::Window:
    attributes.x = allocation.get_x();
    attributes.y = allocation.get_y();
    attributes.width = allocation.get_width();
    attributes.height = allocation.get_height();
    
    attributes.event_mask = get_events () |
      Gdk::EXPOSURE_MASK |
      Gdk::POINTER_MOTION_MASK |
      Gdk::BUTTON_PRESS_MASK |
      Gdk::BUTTON_RELEASE_MASK; 
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.wclass = GDK_INPUT_OUTPUT;
    
    gdkWindow = Gdk::Window::create(get_window(), &attributes,
            GDK_WA_X | GDK_WA_Y);
    set_has_window(true);
    set_window(gdkWindow);
    
    // Unset the background so as to make the colour match the parent
    // window
    //unset_bg(STATE_NORMAL);
    
    // Make the widget receive expose events
    gdkWindow->set_user_data(gobj());
  }
  
  
  void
  TimelineHeaderWidget::on_unrealize()
  {
    // Un-reference any window we may have created
    gdkWindow.clear();
    
    // Call base implementation
    Container::on_unrealize();
  }
  
  
  void
  TimelineHeaderWidget::set_child_widget (Widget& child)
  {
    widget = &child;
    widget->set_parent(*this);
  }
  
  
  void
  TimelineHeaderWidget::on_size_request (Requisition* requisition)
  {
    REQUIRE(requisition); 
    Requisition child_requisition = {0, 0};
    
#if false     //////////////////////////////////////////////////////////////TODO GTK-3 porting
    if(widget && widget->get_visible())
      child_requisition = widget->size_request();
#endif
    requisition->width = child_requisition.width,
    requisition->height = child_requisition.height;
    
    ENSURE(requisition->width >= 0);
    ENSURE(requisition->height >= 0);
  }
  
  
  void
  TimelineHeaderWidget::on_size_allocate (Gtk::Allocation& allocation)
  {
    // Use the offered allocation for this container:
    set_allocation(allocation);
    
    // Resize the widget's window
    if (gdkWindow)
      {
        gdkWindow->move_resize(allocation.get_x(), allocation.get_y(),
          allocation.get_width(), allocation.get_height());
      }
    
    //Assign sign space to the child:
    Gtk::Allocation child_allocation(
      margin + expand_button_size,
      margin,
      max(allocation.get_width() - expand_button_size - margin*2, 0),
          allocation.get_height() - margin*2);
    
    if (widget && widget->get_visible())
      widget->size_allocate(child_allocation);
  }
  
  
  bool
  TimelineHeaderWidget::on_expose_event(GdkEventExpose *event)
  {
#if false //////////////////////////////////////////////////////////////////TODO unfinished code
    Glib::RefPtr<Style> style = get_style();
    REQUIRE(style);
    REQUIRE(gdkWindow);
    
    shared_ptr<model::Track> modelTrack = track.getModelTrack();
    REQUIRE(modelTrack);
    
    // Get the header box  
    const Gdk::Rectangle allocation = get_allocation();
    const Gdk::Rectangle box(0, 0,
      allocation.get_width(), allocation.get_height());
    
    // Use paint_box to draw a themed bevel around the header
    style->paint_box(gdkWindow, STATE_NORMAL,
      SHADOW_OUT, box, *this, "",
      box.get_x(), box.get_y(),
      box.get_width(), box.get_height());
    
    // Paint the expander if there are child tracks
    StateType state_type = STATE_NORMAL;
    if(clickedExpander)
      state_type = STATE_SELECTED;
    else if(hoveringExpander)
      state_type = STATE_PRELIGHT;
    
    if(!modelTrack->get_child_tracks().empty())
      style->paint_expander (gdkWindow,
        state_type, 
        box, *this, "",
        box.get_x() + expand_button_size / 2 + margin,
        box.get_y() + box.get_height() / 2,
        track.get_expander_style());
#endif
    return true; //Container::on_expose_event(event);
  }
  
  
  bool
  TimelineHeaderWidget::on_button_press_event (GdkEventButton* event)
  {
    clickedExpander = hoveringExpander;
    return Container::on_button_press_event(event);
  }
  
  
  bool
  TimelineHeaderWidget::on_button_release_event (GdkEventButton* event)
  {
    TimelineLayoutHelper &layout = track.timelineWidget.layoutHelper;
    
    // Did the user release the button on an expander?
    if (clickedExpander && !layout.is_dragging_track())
      {
        // Yes? The toggle the expanding
        track.expand_collapse(track.get_expanded() ? Track::Collapse
                                                   : Track::Expand);
        clickedExpander = false;
        
        layout.update_layout();
      }
    
    return Container::on_button_release_event(event);
  }
  
  
  bool
  TimelineHeaderWidget::on_motion_notify_event (GdkEventMotion* event)
  {
    bool hovering = false;
    
    const Gdk::Rectangle box = get_allocation();
    if (gui::draw::pt_in_rect (Gdk::Point(event->x, event->y),
                               Gdk::Rectangle(margin, margin,
                                              expand_button_size,
                                              box.get_height() - margin * 2)))
      hovering = true;
    
    if(hovering != hoveringExpander)
      {
        hoveringExpander = hovering;
        queue_draw();
      }
    
    return Container::on_motion_notify_event(event);
  }
  
  
  void
  TimelineHeaderWidget::forall_vfunc(gboolean /* include_internals */
                                    ,GtkCallback callback
                                    ,gpointer callback_data)
  {
    if (widget)
      callback (widget->gobj(), callback_data);
  }
  
  
  void
  TimelineHeaderWidget::on_add (Gtk::Widget* child)
  {
    REQUIRE(child);
    if (!widget)
      {
        widget = child;
        widget->set_parent(*this);
      }
  }
  
  
  void
  TimelineHeaderWidget::on_remove (Gtk::Widget* child)
  {
    REQUIRE(child);
    if(child)
      {
        const bool visible = child->get_visible();
        
        if(child == widget)
          {
            widget = NULL;
            child->unparent();
            
            if(visible)
              queue_resize();
          }
      }
  }
  
  
  GType
  TimelineHeaderWidget::child_type_vfunc()  const
  {
    // If there is still space for one widget,
    // then report the type of widget that may be added.
    if (!widget)
      return Gtk::Widget::get_type();
    else
      {
        //No more widgets may be added.
        return G_TYPE_NONE;
      }
  }
  
  
  void
  TimelineHeaderWidget::register_styles()  const
  {
    static bool registered = false;
    if(registered) return;
    registered = true;
    
    GtkWidgetClass *klass = GTK_WIDGET_CLASS(G_OBJECT_GET_CLASS(gobj()));
    
    gtk_widget_class_install_style_property(klass, 
      g_param_spec_int("margin",
      "Heading Margin",
      "The amount of padding around each header pixels.",
      0, G_MAXINT, 4, G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property(klass, 
      g_param_spec_int("expand_button_size",
      "Expand Button Size",
      "The width of the expander button in pixels.",
      0, G_MAXINT, 12, G_PARAM_READABLE));
  }
  
  
  void
  TimelineHeaderWidget::read_styles()
  {
    get_style_property("margin", margin);
    get_style_property("expand_button_size", expand_button_size);
  }
  
  
}}}// namespace stage::widget::timeline
