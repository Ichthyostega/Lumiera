/*
  menu-button.hpp  -  Declaration of the menu button widget
 
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
/** @file menu-button.hpp
 ** This file contains the definition of menu button widget
 */

#ifndef MENU_BUTTON_HPP
#define MENU_BUTTON_HPP

#include <gtkmm.h>

namespace gui {
namespace widgets {

/**
 * A button that display a menu when clicked on.
 **/
class MenuButton : public Gtk::Button
{
public:

  /**
   * Create an empty button.
   * @remarks With an empty button, you can Gtk::Button::add() a widget
   * such as a Gtk::Pixmap or Gtk::Box. If you just wish to add a
   * Gtk::Label, you may want to use the
   * Gtk::MenuButton(const Glib::ustring& label) ctor directly instead.
   **/
  MenuButton();

  /**
   * Creates a new Button containing the image and text from a stock
   * item. 
   * @remarks Stock ids have identifiers like Gtk::Stock::OK and
   * Gtk::Stock::APPLY.
   **/
  MenuButton(const Gtk::StockID& stock_id);
  
  /**
   * Creates a simple Push Button with label.
   * @remarks Create a button with the given label inside. You won't be
   * able to add a widget in this button since it already has a
   * Gtk::Label in it
   **/
  MenuButton(const Glib::ustring& label, bool mnemonic=false);
  
  /**
   * Gets the menu which will be displayed when the button is clicked
   * on.
   * @return Returns a reference to the menu that will be clicked on.
   * This reference can be used to populate the menu with items.
   **/
  Gtk::Menu& get_menu();
  
  /**
   * Pops up the menu.
   **/
  void popup();

protected:

  /**
   * An internal method which sets up the button at creat time.
   **/
  void setup_button();

  /**
   * An event handler for when the button is pressed.
   **/
  virtual void on_pressed();

private:

  /**
   * A callback function used to determine the correct position for the
   * popup menu.
   * @param x The x-coordinate to display the menu in root window
   * coordinates.
   * @param y The y-coordinate to display the menu in root window
   * coordinates.
   * @param push_in This value is set to true if the menu should be
   * pushed in if it collides with the edge of the screen.
   **/
  void on_menu_position(int& x, int& y, bool& push_in);

private:

  Gtk::HBox hBox;
  
  Gtk::Image image;
  
  Gtk::Label caption;

  /**
   * The internal menu object which is the popup menu of this widget.
   **/
  Gtk::Menu menu;
  
  /**
   * The arrow widget that will be displayed to hint the user that this
   * button is a drop-down.
   **/
  Gtk::Arrow arrow;
};

} // gui
} // widgets

#endif // MENU_BUTTON_HPP

