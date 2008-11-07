/*
  timeline-header-container.cpp  -  Implementation of the timeline
  header container widget
 
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
 
* *****************************************************/

#include <boost/foreach.hpp>

#include "timeline-header-container.hpp"
#include "track.hpp"
#include "../timeline-widget.hpp"

using namespace Gtk;
using namespace std;

namespace gui {
namespace widgets {
namespace timeline {

TimelineHeaderContainer::TimelineHeaderContainer(gui::widgets::TimelineWidget
    *timeline_widget) :
    Glib::ObjectBase("TimelineHeaderContainer"),
    timelineWidget(timeline_widget),
    margin(-1)
{
  REQUIRE(timeline_widget != NULL);

  // This widget will not have a window at first
  set_flags(Gtk::NO_WINDOW);
  
  set_redraw_on_allocate(false);
  
  // Connect to the timeline widget's vertical scroll event,
  // so that we get notified when the view shifts
  timelineWidget->verticalAdjustment.signal_value_changed().connect(
    sigc::mem_fun(this, &TimelineHeaderContainer::on_scroll) );
    
  // Install style properties
  register_styles();
}
 
void
TimelineHeaderContainer::update_headers()
{
  REQUIRE(timelineWidget != NULL);
    
  // Add fresh headers  
  BOOST_FOREACH( Track* track, timelineWidget->tracks )
    set_parent_recursive(track);
}
  
void
TimelineHeaderContainer::on_realize()
{
  set_flags(Gtk::NO_WINDOW);
  
  // Call base class:
  Gtk::Container::on_realize();
  
  // Create the GdkWindow:
  GdkWindowAttr attributes;
  memset(&attributes, 0, sizeof(attributes));

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
  unset_flags(Gtk::NO_WINDOW);
  set_window(gdkWindow);
  
  // Unset the background so as to make the colour match the parent window
  unset_bg(STATE_NORMAL);

  // Make the widget receive expose events
  gdkWindow->set_user_data(gobj());
}
  
void
TimelineHeaderContainer::on_unrealize()
{
  // Unreference any window we may have created
  gdkWindow.clear();

  // Call base class:
  Gtk::Widget::on_unrealize();
}

void
TimelineHeaderContainer::on_size_request (Requisition* requisition)
{ 
  // We don't care about the size of all the child widgets, but if we
  // don't send the size request down the tree, some widgets fail to
  // calculate their text layout correctly. 
  BOOST_FOREACH( Track* track, timelineWidget->tracks )
    size_request_recursive(track);
    
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
  if(gdkWindow)
    gdkWindow->resize(allocation.get_width(), allocation.get_height());
  
  // Relayout the child widgets of the headers
  layout_headers();
}
  
void
TimelineHeaderContainer::forall_vfunc(gboolean /* include_internals */,
        GtkCallback callback, gpointer callback_data)
{ 
  REQUIRE(callback != NULL);
  
  BOOST_FOREACH( Track* track, timelineWidget->tracks )
    {
      ASSERT(track != NULL);
      forall_vfunc_recursive(track, callback, callback_data);
    }
}

bool
TimelineHeaderContainer::on_expose_event(GdkEventExpose *event)
{
  if(gdkWindow)
    {
      int offset = 0;
      const int y_scroll_offset = timelineWidget->get_y_scroll_offset();
      
      Glib::RefPtr<Style> style = get_style();
      const Allocation container_allocation = get_allocation();
      
      read_styles();
       
      // Paint a border underneath all the root headers
      BOOST_FOREACH( Track* track, timelineWidget->tracks )
        {
          ASSERT(track != NULL);
          
          const int height = TimelineWidget::measure_branch_height(
            track);
          ASSERT(height >= 0);
               
          style->paint_box(
            gdkWindow, STATE_NORMAL, SHADOW_OUT,
            Gdk::Rectangle(
              0, 0,
              container_allocation.get_width(),
              container_allocation.get_height()),
            *this, "", 0, offset - y_scroll_offset,
            container_allocation.get_width(), height);
          
          offset += height + TimelineWidget::TrackPadding;
        }
    }

  return Container::on_expose_event(event);
}
  
void
TimelineHeaderContainer::on_scroll()
{
  // If the scroll has changed, we will have to shift all the
  // header widgets
  layout_headers();
}
  
void
TimelineHeaderContainer::layout_headers()
{
  ASSERT(timelineWidget != NULL);
  
  // We can't layout before the widget has been set up
  if(!gdkWindow)
    return;

  int offset = 0;
  const int y_scroll_offset = timelineWidget->get_y_scroll_offset();
  
  read_styles();
  
  const Allocation container_allocation = get_allocation();
  const int header_width = container_allocation.get_width ()
     - margin * 2;
  
  BOOST_FOREACH( Track* track, timelineWidget->tracks )
    {
      ASSERT(track != NULL);
      layout_headers_recursive(track, y_scroll_offset, offset,
        header_width, 0);
    }
    
  // Repaint the background of our parenting
  queue_draw ();
}

void
TimelineHeaderContainer::layout_headers_recursive(Track *track,
  const int y_scroll_offset, int &offset,
  const int header_width, int depth) const
{
  const int height = track->get_height();
  ASSERT(height >= 0);
  
  const int indent = depth * 10;
       
  Allocation header_allocation;
  header_allocation.set_x (margin + indent);
  header_allocation.set_y (offset - y_scroll_offset + margin);
  header_allocation.set_width (header_width - indent);
  header_allocation.set_height (height - margin * 2);
  
  Widget &widget = track->get_header_widget();
  if(widget.is_visible())
    widget.size_allocate (header_allocation);
  
  offset += height + TimelineWidget::TrackPadding;
  
  // Recurse through all the children
  BOOST_FOREACH( Track* child, track->get_child_tracks() )
    layout_headers_recursive(child, y_scroll_offset, offset,
      header_width, depth + 1);
}

void
TimelineHeaderContainer::set_parent_recursive(Track *track)
{
  REQUIRE(track != NULL);
  track->get_header_widget().set_parent(*this);
  
  // Recurse through all the children
  BOOST_FOREACH( Track* child, track->get_child_tracks() )
    set_parent_recursive(child);
}

void
TimelineHeaderContainer::size_request_recursive(Track *track)
{
  REQUIRE(track != NULL);
  if(track->get_header_widget().is_visible())
    track->get_header_widget().size_request();
  
  // Recurse through all the children
  BOOST_FOREACH( Track* child, track->get_child_tracks() )
    size_request_recursive(child);
}

void
TimelineHeaderContainer::forall_vfunc_recursive(Track* track,
  GtkCallback callback, gpointer callback_data)
{
  REQUIRE(track != NULL);
  REQUIRE(callback != NULL);
  
  callback(track->get_header_widget().gobj(), callback_data);
  
  // Recurse through all the children
  BOOST_FOREACH( Track* child, track->get_child_tracks() )
    forall_vfunc_recursive(child, callback, callback_data);
}

void
TimelineHeaderContainer::register_styles() const
{
  GtkWidgetClass *klass = GTK_WIDGET_CLASS(G_OBJECT_GET_CLASS(gobj()));

  gtk_widget_class_install_style_property(klass, 
    g_param_spec_int("heading_margin",
    "Heading Margin",
    "The amount of padding around each header pixels.",
    0, G_MAXINT, 4, G_PARAM_READABLE));
}

void
TimelineHeaderContainer::read_styles()
{
  if(margin <= 0)
    get_style_property("heading_margin", margin);
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
