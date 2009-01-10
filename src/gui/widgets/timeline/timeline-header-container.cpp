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
#include "timeline-track.hpp"
#include "../timeline-widget.hpp"

using namespace Gtk;
using namespace std;
using namespace boost;
using namespace util;

namespace gui {
namespace widgets {
namespace timeline {

TimelineHeaderContainer::TimelineHeaderContainer(
  gui::widgets::TimelineWidget &timeline_widget) :
    Glib::ObjectBase("TimelineHeaderContainer"),
    timelineWidget(timeline_widget),
    margin(-1),
    expand_button_size(12)
{
  // This widget will not have a window at first
  set_flags(Gtk::NO_WINDOW);
  
  set_redraw_on_allocate(false);
  
  // Connect to the timeline widget's vertical scroll event,
  // so that we get notified when the view shifts
  timelineWidget.verticalAdjustment.signal_value_changed().connect(
    sigc::mem_fun(this, &TimelineHeaderContainer::on_scroll) );
    
  // Connect to the timeline widget's hover event,
  // so that we get notified when tracks are hovered on
  timelineWidget.hovering_track_changed_signal().connect(
    sigc::mem_fun(this,
    &TimelineHeaderContainer::on_hovering_track_changed) );
    
  // Create the context menu
  Menu::MenuList& menu_list = contextMenu.items();
  menu_list.push_back( Menu_Helpers::MenuElem(_("_Add Track"),
    sigc::mem_fun(timelineWidget,
    &TimelineWidget::on_add_track_command) ) );
    
  // Install style properties
  register_styles();
  
  // Load the styles up
  read_styles();
}
 
void
TimelineHeaderContainer::update_headers()
{    
  // Ensure headers are parented correctly
  pair<shared_ptr<model::Track>, shared_ptr<timeline::Track> > pair; 
  BOOST_FOREACH( pair, timelineWidget.trackMap )
    {
      REQUIRE(pair.first);
      
      // Set the header's parent widget
      Widget &widget = lookup_timeline_track(pair.first)->
        get_header_widget();
      
      const Container *parent = widget.get_parent();
      if(parent == NULL)  // Is the header unparented?
        widget.set_parent(*this);
      ENSURE(widget.get_parent() == this);
    }
  
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
  REQUIRE(event != NULL);
  
  switch(event->button)
    {
    case 1: // Left Click
      // Did the user press the button on an expander?
      if(hoveringExpander != NULL)
        {
          // Yes? The prime for a release event
          clickedExpander = hoveringExpander;
          queue_draw();
        }
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

bool TimelineHeaderContainer::on_button_release_event (
  GdkEventButton* event)
{
  // Did the user release the button on an expander?
  if(clickedExpander != NULL)
    {
      // Yes? The toggle the expanding
      clickedExpander->expand_collapse(
        clickedExpander->get_expanded() ? Track::Collapse : Track::Expand);
      clickedExpander.reset();
      
      timelineWidget.layoutHelper.update_layout();
    }

  return Container::on_button_release_event(event);    
}

bool TimelineHeaderContainer::on_motion_notify_event (
  GdkEventMotion* event)
{
  REQUIRE(event != NULL);
 
  // Are we hovering on an expander?
  shared_ptr<timeline::Track> expander = expander_button_from_point(
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
    
  const TimelineLayoutHelper::TrackTree &layout_tree =
    timelineWidget.layoutHelper.get_layout_tree();
    
  TimelineLayoutHelper::TrackTree::pre_order_iterator iterator;
  for(iterator = ++layout_tree.begin(); // ++ so that we skip the sequence root
    iterator != layout_tree.end();
    iterator++)
    {
      Widget &widget =
        lookup_timeline_track(*iterator)->get_header_widget();
      if(widget.is_visible())
        widget.size_request();
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
  if(gdkWindow)
    {
      gdkWindow->move(allocation.get_x(), allocation.get_y());
      gdkWindow->resize(
        allocation.get_width(), allocation.get_height());
    }
  
  // Relayout the child widgets of the headers
  layout_headers();
}
  
void
TimelineHeaderContainer::forall_vfunc(gboolean /* include_internals */,
        GtkCallback callback, gpointer callback_data)
{ 
  REQUIRE(callback != NULL);
  
  BOOST_FOREACH( shared_ptr<model::Track> track, get_tracks() )
    {
      REQUIRE(track);
      forall_vfunc_recursive(track, callback, callback_data);
    }
}

void
TimelineHeaderContainer::on_remove(Widget*)
{
  // Do nothing - this is just to keep Gtk::Container happy
}

void
TimelineHeaderContainer::on_layout_changed()
{
  layout_headers();
}

bool
TimelineHeaderContainer::on_expose_event(GdkEventExpose *event)
{ 
  if(gdkWindow)
    {
      const Allocation container_allocation = get_allocation();
       
      // Paint a border underneath all the headers
      const TimelineLayoutHelper::TrackTree &layout_tree =
        timelineWidget.layoutHelper.get_layout_tree();
      
      TimelineLayoutHelper::TrackTree::pre_order_iterator iterator;
      for(iterator = ++layout_tree.begin(); // ++ so that we skip the sequence root
        iterator != layout_tree.end();
        iterator++)
        {
          shared_ptr<model::Track> model_track = *iterator;
          REQUIRE(model_track);
          
          draw_header_decoration(model_track,
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
  boost::shared_ptr<timeline::Track> hovering_track)
{
  (void)hovering_track;
  
  // The hovering track has changed, redraw so we can light the header
  queue_draw();
}
  
void
TimelineHeaderContainer::layout_headers()
{ 
  REQUIRE(margin >= 0); // read_styles must have been called before now

  // We can't layout before the widget has been set up
  if(!gdkWindow)
    return;
    
  TimelineLayoutHelper &layout_helper =
    timelineWidget.layoutHelper;
  const TimelineLayoutHelper::TrackTree &layout_tree =
    layout_helper.get_layout_tree();
  
  TimelineLayoutHelper::TrackTree::pre_order_iterator iterator;
  for(iterator = ++layout_tree.begin(); // ++ so that we skip the sequence root
    iterator != layout_tree.end();
    iterator++)
    {      
      const shared_ptr<timeline::Track> timeline_track =
        lookup_timeline_track(*iterator);
      
      Widget &widget = timeline_track->get_header_widget();
      
      optional<Gdk::Rectangle> header_rect =
        layout_helper.get_track_header_rect(timeline_track);
      
      if(header_rect)
        {
          REQUIRE(header_rect->get_width() >= 0);
          REQUIRE(header_rect->get_height() >= 0);
            
          // Calculate the allocation of the header widget
          Allocation header_allocation(
            header_rect->get_x() + margin + expand_button_size,   // x
            header_rect->get_y() + margin,                        // y
            max( header_rect->get_width() - expand_button_size -
              margin * 2, 0 ),                                    // width
            header_rect->get_height() - margin * 2);              // height
          
          // Apply the allocation to the header
          widget.size_allocate (header_allocation);
          if(!widget.is_visible())
            widget.show();
        }
      else // No header rect, so the track must be hidden
        if(widget.is_visible())
          widget.hide();
    }
    
  // Repaint the background of our parenting
  queue_draw ();
}

void
TimelineHeaderContainer::forall_vfunc_recursive(
  shared_ptr<model::Track> model_track, GtkCallback callback,
  gpointer callback_data)
{
  REQUIRE(callback != NULL);
    
  callback( lookup_timeline_track(model_track)->
    get_header_widget().gobj(), callback_data) ;
  
  // Recurse through all the children
  BOOST_FOREACH( shared_ptr<model::Track> child,
    model_track->get_child_tracks() )
    forall_vfunc_recursive(child, callback, callback_data);
}

void
TimelineHeaderContainer::draw_header_decoration(
    shared_ptr<model::Track> model_track,
    const Gdk::Rectangle &clip_rect)
{
  REQUIRE(model_track != NULL);
  REQUIRE(clip_rect.get_width() > 0);
  REQUIRE(clip_rect.get_height() > 0);
  
  Glib::RefPtr<Style> style = get_style();
  REQUIRE(style);
  
  shared_ptr<timeline::Track> timeline_track =
    lookup_timeline_track(model_track);
  
  // Get the header box  
  const optional<Gdk::Rectangle> &optional_box = 
    timelineWidget.layoutHelper.get_track_header_rect(timeline_track);
  if(!optional_box)
    return;
  const Gdk::Rectangle box = *optional_box;
  
  // Don't paint the box, if it won't be visible
  if(box.get_x() >= clip_rect.get_width() ||
    box.get_height() <= 0 ||
    box.get_y() + box.get_height() <= clip_rect.get_y()  ||
    box.get_y() >= clip_rect.get_y() + clip_rect.get_height())
    return;

  // Use paint_box to draw a themed bevel around the header
  style->paint_box(gdkWindow, STATE_NORMAL,
    SHADOW_OUT, clip_rect, *this, "",
    box.get_x(), box.get_y(),
    box.get_width(), box.get_height());
    
  // Paint the expander if there are child tracks
  StateType state_type = STATE_NORMAL;
  if(clickedExpander == timeline_track)
    state_type = STATE_SELECTED;
  else if(hoveringExpander == timeline_track)
    state_type = STATE_PRELIGHT;
  
  if(!model_track->get_child_tracks().empty())
    style->paint_expander (gdkWindow,
      state_type, 
      clip_rect, *this, "",
      box.get_x() + expand_button_size / 2 + margin,
      box.get_y() + box.get_height() / 2,
      timeline_track->get_expander_style());
}

shared_ptr<timeline::Track>
TimelineHeaderContainer::expander_button_from_point(
  const Gdk::Point &point)
{ 
  const TimelineLayoutHelper::TrackTree &layout_tree =
    timelineWidget.layoutHelper.get_layout_tree();
  
  TimelineLayoutHelper::TrackTree::pre_order_iterator iterator;
  for(iterator = ++layout_tree.begin(); // ++ so we skip the sequence root
    iterator != layout_tree.end();
    iterator++)
    {
      const shared_ptr<timeline::Track> timeline_track =
        lookup_timeline_track(*iterator);
      
      // Hit test the rectangle
      const optional<Gdk::Rectangle> rect =
        get_expander_button_rectangle(timeline_track);
      
      if(rect)
        {
          if(point.get_x() >= rect->get_x() &&
            point.get_x() < rect->get_x() + rect->get_width() &&
            point.get_y() >= rect->get_y() &&
            point.get_y() < rect->get_y() + rect->get_height())
            return timeline_track;
        }
    }
    
  return shared_ptr<timeline::Track>();
}

const optional<Gdk::Rectangle>
TimelineHeaderContainer::get_expander_button_rectangle(
  shared_ptr<Track> track)
{
  REQUIRE(track != NULL);
  
  optional<Gdk::Rectangle> box =
    timelineWidget.layoutHelper.get_track_header_rect(track);
  if(box)
    {
      return optional<Gdk::Rectangle>(Gdk::Rectangle(
        margin + box->get_x(), margin + box->get_y(),
        expand_button_size, box->get_height() - margin * 2));
    }
    
  return optional<Gdk::Rectangle>();
}

shared_ptr<timeline::Track>
TimelineHeaderContainer::lookup_timeline_track(
  shared_ptr<model::Track> model_track)
{
  REQUIRE(model_track != NULL);
  
  shared_ptr<timeline::Track> timeline_track =
    timelineWidget.lookup_timeline_track(model_track);
  ENSURE(timeline_track);
  
  return timeline_track;
}

const std::list< boost::shared_ptr<model::Track> >
TimelineHeaderContainer::get_tracks() const
{
  REQUIRE(timelineWidget.sequence);
  return timelineWidget.sequence->get_child_tracks();
}

void
TimelineHeaderContainer::register_styles() const
{
  static bool registered = false;
  if(registered) return;
  registered = true;
  
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
    {
      get_style_property("heading_margin", margin);
      margin = max(margin, 0);
    }
  else 
    WARN(gui, "TimelineHeaderContainer::read_styles()"
      " should only be called once");
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
