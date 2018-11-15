/*
  BUTTON-BAR.hpp  -  button bar widget

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
 ** A toolbar widget for use in dialogs.
 */


#ifndef GUI_WIDGET_BUTTON_BAR_H
#define GUI_WIDGET_BUTTON_BAR_H

#include "stage/widget/mini-button.hpp"

namespace gui {
namespace widget {
  
  /**
   * A modified toolbar widget for use in dialogs.
   */
  class ButtonBar
    : public Gtk::Box
    {
    public:
      ButtonBar();
      
      /** Append a widget to the button bar */
      void append (Widget&);
      
      /**
       * Append a button to the button bar,
       * and connect a click event.
       */
      template<class T>
      void
      append (MiniWrapper<T>& button, sigc::slot<void> const& clicked_slot)
      {
        button.signal_clicked().connect (clicked_slot);
        append (button);
      }
    };
  
  
}}// gui::widget
#endif /*GUI_WIDGET_BUTTON_BAR_H*/
