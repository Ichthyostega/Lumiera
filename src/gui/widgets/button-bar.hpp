/*
  button-bar.hpp  -  Declaration of the button bar widget
 
  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
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
/** @file button-bar.hpp
 ** This file contains the definition of the button bar widget
 */

#ifndef BUTTON_BAR_HPP
#define BUTTON_BAR_HPP

#include <gtkmm.h>

namespace gui {
namespace widgets {

/**
 * A modified toolbar widget for use in dialogs.
 **/
class ButtonBar : public Gtk::Toolbar
{
public:
  /**
   * Constructor
   **/
  ButtonBar();

private:
  /**
   * The expose event handler.
   * @remarks This event handler bypasses the Toolbar expose handler
   * so that the toolbar background is not drawn.
   **/
  bool on_expose_event(GdkEventExpose* event);
  
  /**
   * A helper function that calles Container::propagate_expose on a
   * widget.
   * @param widget The widget to call propagate_expose on.
   * @remarks This function should be used with Container::foreach.
   **/
  void expose_each(Gtk::Widget& widget);
  
  /**
   * A pointer to the expose event received in on_expose_event.
   * @remarks This value is used by expose_each, and is only value
   * during on_expose_event's foreach call.
   **/
  GdkEventExpose* exposeEvent;
};

} // gui
} // widgets

#endif // BUTTON_BAR_HPP

