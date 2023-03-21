/*
  MENU-BUTTON.hpp  -  button widget to invoke a menu

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

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


/** @file menu-button.hpp
 ** A button widget that displays a menu when clicked.
 ** @todo leftover from the old GTK-2 Lumiera UI as of 3/23,
 **       but should be retained and integrated into the reworked GTK-3 UI 
 */


#ifndef STAGE_WIDGET_MENU_BUTTON_H
#define STAGE_WIDGET_MENU_BUTTON_H

#include "stage/gtk-base.hpp"



namespace stage {
namespace widget {
  
  using namespace Gtk;       ////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
  
  /**
   * A button that displays a menu when clicked on.
   */
  class MenuButton
    : public ToggleButton
    {
    public:
      /**
       * Create an empty button.
       * @remarks With an empty button, you can Button::add() a widget
       *    such as a Pixmap or Box. If you just wish to add a
       *    Label, you may want to use the
       *    \code MenuButton(cuString& label) \endcode
       *    ctor directly instead.
       */
      MenuButton();
      
      /**
       * Creates a new Button containing the image and text from a stock item. 
       * @remarks Stock IDs have identifiers like \c Stock::OK and \c Stock::APPLY.
       */
      MenuButton (StockID const& stock_id);
      
      /**
       * Creates a simple Push Button with label.
       * @remarks Create a button with the given label inside.
       * You won't be able to add a widget in this button
       * since it already has a Label in it
       */
      MenuButton (cuString& label, bool mnemonic=false);
      
      /**
       * Gets the menu which will be displayed when the button is clicked on.
       * @return Returns a reference to the menu that will be clicked on.
       *    This reference can be used to populate the menu with items.
       */
      Menu& get_menu();
      
      /**
       * Append a Menu Item to the Menu
       * @param slug Unique identifier in the UI Manager
       * @param title The title of the item
       * @param callback The signal handler when clicked
       * @param toggle
       */
      void append (uString& slug, uString& title, sigc::slot<void>& callback, bool toggle=false);
      
      
      /**
       * Append a Menu Item to the Menu
       * @param slug Unique identifier in the UI Manager
       * @param title The title of the item
       * @param callback The signal handler when clicked
       * @param toggle
       */
      void append (const char *slug, const char* title, sigc::slot<void>& callback, bool toggle=false);
      
      
      /** Append a Gtk::SeparatorMenuItem to the Menu */
      void appendSeparator();
      
      
      /**
       * Get an object from the Menu
       *
       * @param slug The slug that created the object
       * @return Widget* Returns a valid Gtk::Widget or NULL if not found
       * @remarks Typically you'll 'get' a MenuItem of sorts.
       *        Use \c MenuButton::get_menu() to get the menu as a whole
       */
      Gtk::Widget* get (uString slug);
      
      
      /** Pops up the menu.*/
      void popup();
      
      
    protected:
      /**
       * An internal method which sets up the button at create time.
       */
      void setupButton();
      
      /**
       * An event handler for when the button is pressed.
       */
      void on_pressed();
      
      /**
       * An event handler for when the menu is closed.
       */
      void on_menu_deactivated();
      
      
    private:
      /**
       * callback function used to determine the correct position for the popup menu.
       * @param x The x-coordinate to display the menu in root window coordinates.
       * @param y The y-coordinate to display the menu in root window coordinates.
       * @param push_in This value is set to true if the menu should be
       *          pushed in if it collides with the edge of the screen.
       */
      void on_menu_position (int& x, int& y, bool& push_in);
      
    private:
      /** hBox for the layout of an image, caption and arrow. */
      HBox hBox;
      
      /** The image that will optionally display an icon. */
      Image image;
      
      /** caption text label to show on the button. */
      Label caption;
      
      /**
       * The arrow widget that will be displayed to hint the user that this
       * button is a drop-down.
       */
      Arrow arrow;
      
      /**
       * The internal menu object which is the popup menu of this widget.
       */
      Menu menu;
      
      Glib::RefPtr<UIManager> uimanager;
      Glib::RefPtr<ActionGroup> actions;
    
    };
  
  
}}// stage::widget
#endif /*STAGE_WIDGET_MENU_BUTTON_H*/
