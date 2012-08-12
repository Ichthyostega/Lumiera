/*
  button-bar.hpp  -  Declaration of the button bar widget

  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>

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
/** @file button-bar.hpp
 ** This file contains the definition of the button bar widget
 */

#ifndef BUTTON_BAR_HPP
#define BUTTON_BAR_HPP

#include "mini-button.hpp"

namespace gui {
namespace widgets {

/**
 * A modified toolbar widget for use in dialogs.
 */
class ButtonBar : public Gtk::Box
{
public:
  /**
   * Constructor
   */
  ButtonBar();
  
  /**
   * Append a widget to the button bar.
   * @param widget The button to append.
   */
  void append(Widget &widget);

  /**
   * Append a button to the button bar, and connect a click event.
   * @param button The button to append.
   * @param clicked_slot The slot to connect.
   */
  template<class T> void append(MiniWrapper<T>& button,
    const sigc::slot<void>& clicked_slot)
  {
    button.signal_clicked().connect(clicked_slot);
    append(button);
  }
  
private:
  /* ===== Overrides ===== */

  /**
   * An event handler that is called to offer an allocation to this
   * widget.
   * @param requisition The area offered for this widget.
   */
  void on_size_request(Gtk::Requisition* requisition);
  
  /**
   * An event handler that is called to notify this widget to allocate
   * a given area for itself.
   * @param allocation The area to allocate for this widget.
   */
  virtual void on_size_allocate(Gtk::Allocation& allocation);
};

} // gui
} // widgets

#endif // BUTTON_BAR_HPP

