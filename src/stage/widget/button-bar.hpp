/*
  BUTTON-BAR.hpp  -  button bar widget

   Copyright (C)
     2009,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file button-bar.hpp
 ** A toolbar widget for use in dialogs.
 */


#ifndef STAGE_WIDGET_BUTTON_BAR_H
#define STAGE_WIDGET_BUTTON_BAR_H

#include "stage/widget/mini-button.hpp"

namespace stage {
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
  
  
}}// stage::widget
#endif /*STAGE_WIDGET_BUTTON_BAR_H*/
