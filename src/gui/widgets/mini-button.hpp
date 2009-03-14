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
 * A wrapper for ToolButton-like Button widgets
 **/
template<class T>
class MiniWrapper : public T
{
public:

  /**
   * Creates a new Button containing the image and text from a stock
   * item. 
   * @param stock_id The stock_id of the image.
   * @param icon_size The size of the image to show.
   * @remarks Stock ids have identifiers like Gtk::Stock::OK and
   * Gtk::Stock::APPLY.
   **/
  MiniWrapper(const Gtk::StockID& stock_id,
    const Gtk::IconSize icon_size = Gtk::ICON_SIZE_LARGE_TOOLBAR) :
    image(stock_id, icon_size)
  {
    T::add(image);
    T::set_relief(Gtk::RELIEF_NONE);
    T::set_focus_on_click(false);
  }
    
  /**
   * Sets a new image from a stock-id for this button.
   * @param stock_id The stock_id of the image.
   * @param icon_size The size of the image to show.
   **/
  void set_stock_id(const Gtk::StockID& stock_id,
    const Gtk::IconSize icon_size = Gtk::ICON_SIZE_LARGE_TOOLBAR)
  {
    image.set(stock_id, icon_size);
  }
  
private:
  
  /**
   * The image widget for the button.
   **/
  Gtk::Image image;
};

/**
 * A ToolButton-like widget
 **/
typedef MiniWrapper<Gtk::Button> MiniButton;

/**
 * A ToggleToolButton-like widget
 **/
typedef MiniWrapper<Gtk::ToggleButton> MiniToggleButton;

} // gui
} // widgets

#endif // MINI_BUTTON_HPP

