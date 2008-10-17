/*
  header-container.hpp  -  Declaration of the header container widget
 
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
/** @file header-container.hpp
 ** This file contains the definition of the header container
 ** widget
 */

#ifndef HEADER_CONTAINER_HPP
#define HEADER_CONTAINER_HPP

#include "../../gtk-lumiera.hpp"
#include <vector>

namespace lumiera {
namespace gui {
namespace widgets {

class TimelineWidget;

namespace timeline {

class Track;

/**
 * A helper class for the TimelineWidget. HeaderContainer is
 * container widget for all the left-hand-side header widgets
 * associated with timeline tracks.
 */
class HeaderContainer : public Gtk::Container
{
public:
  /**
   * Constructor
   *
   * @param[in] timeline_widget A pointer to the owner timeline widget
   */
  HeaderContainer(lumiera::gui::widgets::TimelineWidget* 
    timeline_widget);
  
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
    
  /* ===== Internals ===== */
private:

  /**
   * Moves all the header widgets to the correct position
   * given scroll, stacking etc.
   */
  void layout_headers();
  
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
  lumiera::gui::widgets::TimelineWidget *timelineWidget;
  
  /**
   * The widget's window object. 
   * 
   * @note This is needed for the sake of clipping when
   * widgets are scrolled.
   */
  Glib::RefPtr<Gdk::Window> gdkWindow;
  
  //----- Header List -----//

  /**
   * A structure to represent a header widget and it's
   * associated track
   */
  struct RootHeader
  {
    Gtk::Widget *widget;
    Track *track;
  };
  
  /**
   * Contains a list of the root currently present on
   * the timeline view
   */
  std::vector< RootHeader > rootHeaders;

  //----- Style Values -----//
  
  /**
   * The style value which indicates the amount of padding around each
   * header pixels.
   **/
  int margin;
};

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

#endif // HEADER_CONTAINER_HPP

