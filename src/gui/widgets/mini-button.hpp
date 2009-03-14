/*
  mini-button.hpp  -  Declaration of the mini button widget
 
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
/** @file mini-button.hpp
 ** This file contains the definition of mini button widget
 */

#ifndef MINI_BUTTON_HPP
#define MINI_BUTTON_HPP

#include <gtkmm.h>

namespace gui {
namespace widgets {

/**
 * A ToolButton-like widget
 **/
class MiniButton : public Gtk::Button
{
public:

  /**
   * Creates a new Button containing the image and text from a stock
   * item. 
   * @remarks Stock ids have identifiers like Gtk::Stock::OK and
   * Gtk::Stock::APPLY.
   **/
  MiniButton(const Gtk::StockID& stock_id,
    const Gtk::IconSize icon_size);
  
private:
  
  Gtk::Image image;
};

} // gui
} // widgets

#endif // MINI_BUTTON_HPP

