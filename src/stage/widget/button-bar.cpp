/*
  ButtonBar  -  button bar widget

   Copyright (C)
     2009,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file button-bar.cpp
 ** Implementation of a toolbar with buttons for use in dialogs.
 */


#include "stage/widget/button-bar.hpp"
#include "include/logging.h"


namespace stage {
namespace widget {
  
  ButtonBar::ButtonBar()
  {
    set_orientation (Gtk::ORIENTATION_HORIZONTAL);
    set_has_window (false);
  }
  
  
  void
  ButtonBar::append (Widget& widget)
  {
    pack_start(widget, Gtk::PACK_SHRINK);
  }
  
  
}}// stage::widget
