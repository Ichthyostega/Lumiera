/*
  timeline-header-container.cpp  -  Declaration of the timeline
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
 
*/
/** @file timeline-header-container.hpp
 ** This file contains the definition of the header container
 ** widget
 */

#ifndef HEADER_CONTAINER_HPP
#define HEADER_CONTAINER_HPP

#include "../../gtk-lumiera.hpp"
#include <vector>

namespace gui {
  
namespace model {
class Track;
}
  
namespace widgets {

class TimelineWidget;

namespace timeline {

class Track;

/**
 * A helper class for the TimelineWidget. TimelineHeaderContainer
 * is container widget for all the left-hand-side header widgets
 * associated with timeline tracks.
 */
class TimelineHeaderContainer : public Gtk::Container
{
public:
  /**
   * Constructor
   *
   * @param[in] timeline_widget A pointer to the owner timeline widget
   */
  TimelineHeaderContainer(gui::widgets::TimelineWidget* timeline_widget);
  
  /**
   * Attaches the header all the header widgets of root
   * tracks to this control.
   *
   * @note This must be called when the track list changes
   * to synchronise the headers with the timeline body and
   * the backend.
   */
  void update_headers();

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
   **/
  void forall_vfunc(gboolean include_internals, GtkCallback callback,
                    gpointer callback_data);

  /* ===== Events ===== */      
private:

  /**
   * An event handler for when the window must be redrawn.
   */
  bool on_expose_event(GdkEventExpose *event);

  /**
   * This event is called when the scroll bar moves.
   */  
  void on_scroll();
  
  void on_hovering_track_changed(timeline::Track *hovering_track);
    
  /* ===== Internals ===== */
private:

  /**
   * Moves all the header widgets to the correct position given scroll,
   * stacking etc.
   */
  void layout_headers();
  
  /**
   * Recursively lays out all the controls in the header widget.
   * @param track The parent track object which will be recursed into.
   * @param offset A shared value used to accumulate the y-offset of
   * header widgets.
   * @param header_width The width of this widget in pixels.
   * @param depth The depth within the tree of track.
   **/
  void layout_headers_recursive(boost::shared_ptr<model::Track> track,
    int &offset, const int header_width, const int depth,
    bool parent_expanded);
  
  /**
   * Recursively sets all the track header widgets to be child widgets
   * of this widget.
   * @param track The parent track object which will be recursed into.
   **/
  void set_parent_recursive(boost::shared_ptr<model::Track> const
    model_track);
  
  /**
   * Recursively causes all the visible track header widgets to call
   * size_request( ).
   **/
  void size_request_recursive(
    boost::shared_ptr<model::Track> model_track);
  
  void forall_vfunc_recursive(
    boost::shared_ptr<model::Track> model_track,
    GtkCallback callback, gpointer callback_data);
  
  /**
   * Draws the border decoration around the track header.
   * @param model_track The track to draw the decoration for.
   * @param clip_rect The clip to drawing to.
   * @param depth The depth within the tree of this track. This is used
   * to control the amount of indention.
   * @param offset The vertical offset of the headers in pixels.
   **/
  void draw_header_decoration(
    boost::shared_ptr<model::Track> model_track,
    const Gdk::Rectangle &clip_rect);
  
  Track* expander_button_from_point(const Gdk::Point &point);
  
  const Gdk::Rectangle get_expander_button_rectangle(
    timeline::Track* track);
  
  /**
   * A helper function which calls lookup_timeline_track within the
   * parent timeline widget, but also applies lots of data consistency
   * checks in the process.
   * @param model_track The model track to look up in the parent widget.
   * @return Returns the track found, or returns NULL if no matching
   * track was found.
   * @remarks If the return value is going to be NULL, an ENSURE will
   * fail.
   **/
  timeline::Track* lookup_timeline_track(
    boost::shared_ptr<model::Track> model_track);

/**
   * A helper function which calls get_tracks within the sequence of the
   * parent timeline widget, but also applies lots of data consistency
   * checks in the process.
   * @param model_track The model track to look up in the parent widget.
   * @return Returns the track found, or returns NULL if no matching
   * track was found.
   **/  
  const std::list< boost::shared_ptr<model::Track> >&
    get_tracks() const;
  
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
   * The owner TimelineWidget of which this class is a helper
   */
  gui::widgets::TimelineWidget* const timelineWidget;
  
  /**
   * The widget's window object. 
   * 
   * @note This is needed for the sake of clipping when
   * widgets are scrolled.
   */
  Glib::RefPtr<Gdk::Window> gdkWindow;
  
  std::map<timeline::Track*, Gdk::Rectangle> headerBoxes;
  
  //----- User Interaction State -----//
  timeline::Track *hoveringExpander;
  
  timeline::Track *clickedExpander;

  //----- Style Values -----//
  
  /**
   * The style value which indicates the amount of padding around each
   * header pixels.
   **/
  int margin;
  
  int expand_button_size;
};

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // HEADER_CONTAINER_HPP

