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
    hoveringExpander(NULL),
    clickedExpander(NULL),
    margin(-1),
    expand_button_size(12)
{
  REQUIRE(timeline_widget != NULL);

  // This widget will not have a window at first
  set_flags(Gtk::NO_WINDOW);
  
  set_redraw_on_allocate(false);
  
  // Connect to the timeline widget's vertical scroll event,
  // so that we get notified when the view shifts
  timelineWidget->verticalAdjustment.signal_value_changed().connect(
    sigc::mem_fun(this, &TimelineHeaderContainer::on_scroll) );
    
  // Connect to the timeline widget's hover event,
  // so that we get notified when tracks are hovered on
  timelineWidget->hovering_track_changed_signal().connect(
    sigc::mem_fun(this,
    &TimelineHeaderContainer::on_hovering_track_changed) );
    
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
  
  // Unset the background so as to make the colour match the parent
  // window
  unset_bg(STATE_NORMAL);

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

bool TimelineHeaderContainer::on_button_press_event (
  GdkEventButton* event)
{
  // Did the user press the button on an expander?
  if(hoveringExpander != NULL)
    {
      // Yes? The prime for a release event
      clickedExpander = hoveringExpander;
      queue_draw();
    }
  
  return Container::on_button_press_event(event);
}

bool TimelineHeaderContainer::on_button_release_event (
  GdkEventButton* event)
{
  // Did the user release the button on an expander?
  if(clickedExpander != NULL)
    {
      // Yes? The toggle the expanding
      clickedExpander->set_expanded(!clickedExpander->get_expanded());
      clickedExpander = NULL;
      layout_headers();
    }

  return Container::on_button_release_event(event);    
}

bool TimelineHeaderContainer::on_motion_notify_event (
  GdkEventMotion* event)
{
  REQUIRE(event != NULL);
 
  // Are we hovering on an expander?
  Track *expander = expander_button_from_point(
    Gdk::Point(event->x, event->y));
  
  if(expander != hoveringExpander)
    {
      hoveringExpander = expander;
      queue_draw();
    }
    
  return Container::on_motion_notify_event(event);
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
      // Start at an offset from the scroll offset
      int offset = -timelineWidget->get_y_scroll_offset();

      const Allocation container_allocation = get_allocation();
      
      read_styles();
       
      // Paint a border underneath all the root headers
      BOOST_FOREACH( Track* track, timelineWidget->tracks )
        {
          ASSERT(track != NULL);
          
          draw_header_decoration(track,
            Gdk::Rectangle(0, 0,
              container_allocation.get_width(),
              container_allocation.get_height()));
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
TimelineHeaderContainer::on_hovering_track_changed(
  timeline::Track *hovering_track)
{
  // The hovering track has changed, redraw so we can light the header
  queue_draw();
}
  
void
TimelineHeaderContainer::layout_headers()
{
  ASSERT(timelineWidget != NULL);
  
  // We can't layout before the widget has been set up
  if(!gdkWindow)
    return;

  // Make sure the style are loaded
  read_styles();
    
  // Clear previously cached layout
  headerBoxes.clear();

  // Start at minus-the-scroll offset
  int offset = -timelineWidget->get_y_scroll_offset();
    
  const Allocation container_allocation = get_allocation();
  const int header_width = container_allocation.get_width();
  
  BOOST_FOREACH( Track* track, timelineWidget->tracks )
    {
      ASSERT(track != NULL);
      layout_headers_recursive(track, offset, header_width, 0, true);
    }
    
  // Repaint the background of our parenting
  queue_draw ();
}

void
TimelineHeaderContainer::layout_headers_recursive(Track *track,
  int &offset, const int header_width, const int depth,
  bool parent_expanded)
{
  REQUIRE(depth >= 0);
  
  const int indent = depth * 10;
  Widget &widget = track->get_header_widget();
  
  if(parent_expanded)
    {
      const int track_height = track->get_height();
      
      // Calculate the box of the header
      Gdk::Rectangle header_box(
        indent,                                               // x
        offset,                                               // y
        max( header_width - indent, 0 ),                      // width
        track_height - TimelineWidget::TrackPadding);  // height
      ASSERT(header_box.get_height() >= 0);
      
      // Cache the bounding box
      headerBoxes[track] = header_box;
        
      // Calculate the allocation of the header widget
      Allocation header_allocation(
        header_box.get_x() + margin + expand_button_size,     // x
        header_box.get_y() + margin,                          // y
        max( header_box.get_width() - expand_button_size -
          margin * 2, 0 ),                                    // width
        header_box.get_height() - margin * 2);                // height
      
      // Apply the allocation to the header
      widget.size_allocate (header_allocation);
      if(!widget.is_visible())
        widget.show();
      
      // Offset for the next header
      offset += track_height;
    }
  else
    if(widget.is_visible())
      widget.hide();
    
  // Recurse through all the children
  BOOST_FOREACH( Track* child, track->get_child_tracks() ) 
    layout_headers_recursive(child, offset, header_width, depth + 1,
      track->get_expanded() && parent_expanded);
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
TimelineHeaderContainer::draw_header_decoration(Track* track,
    const Gdk::Rectangle &clip_rect)
{
  REQUIRE(track != NULL);
  REQUIRE(clip_rect.get_width() > 0);
  REQUIRE(clip_rect.get_height() > 0);
  
  Glib::RefPtr<Style> style = get_style();
  ASSERT(style);
  
  // Get the cached header box
  ASSERT(headerBoxes.find(track) != headerBoxes.end());  
  const Gdk::Rectangle &box = headerBoxes[track];
  
  // Paint the box, if it will be visible
  if(box.get_x() < clip_rect.get_width() &&
    box.get_height() > 0 &&
    box.get_y() + box.get_height() > clip_rect.get_y()  &&
    box.get_y() < clip_rect.get_y() + clip_rect.get_height())
    {
      // Use paint_box to draw a themed bevel around the header
      style->paint_box(gdkWindow, STATE_NORMAL,
        SHADOW_OUT, clip_rect, *this, "",
        box.get_x(), box.get_y(),
        box.get_width(), box.get_height());
        
      // Paint the expander if there are child tracks
      StateType state_type = STATE_NORMAL;
      if(clickedExpander == track) state_type = STATE_SELECTED;
      else if(hoveringExpander == track) state_type = STATE_PRELIGHT;
      
      const ExpanderStyle expander_style = 
        track->get_expanded() ? EXPANDER_EXPANDED : EXPANDER_COLLAPSED;
      
      if(!track->get_child_tracks().empty())
        style->paint_expander (gdkWindow,
          state_type, 
          clip_rect, *this, "",
          box.get_x() + expand_button_size / 2 + margin,
          box.get_y() + box.get_height() / 2,
          expander_style);
    }
  
  // Recurse through all the children
  if(track->get_expanded())
    BOOST_FOREACH( Track* child, track->get_child_tracks() )
      draw_header_decoration(child, clip_rect);
}

Track*
TimelineHeaderContainer::expander_button_from_point(
  const Gdk::Point &point)
{
  std::pair<Track*, Gdk::Rectangle> pair; 
  BOOST_FOREACH( pair, headerBoxes )
    {
      // Hit test the rectangle
      const Gdk::Rectangle rect =
        get_expander_button_rectangle(pair.first);
      
      if(point.get_x() >= rect.get_x() &&
        point.get_x() < rect.get_x() + rect.get_width() &&
        point.get_y() >= rect.get_y() &&
        point.get_y() < rect.get_y() + rect.get_height())
        return pair.first;
    }
    
  return NULL;
}

const Gdk::Rectangle
TimelineHeaderContainer::get_expander_button_rectangle(
  Track* track)
{
  REQUIRE(track != NULL);
  ASSERT(headerBoxes.find(track) != headerBoxes.end());  
  
  const Gdk::Rectangle &box = headerBoxes[track];
  const int y_centre = box.get_y() + box.get_height() / 2;  
  return Gdk::Rectangle(
    margin + box.get_x(), y_centre - expand_button_size / 2,
    expand_button_size, expand_button_size);
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
