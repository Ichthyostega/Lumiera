/*
  window-manager.hpp  -  Defines the global UI Manager class
 
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
/** @file window-manager.hpp
 ** This file contains the defintion of global UI Manager class.
 ** @see window-manager.cpp
 ** @see gtk-lumiera.hpp
 */

#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include "gtk-lumiera.hpp"

namespace gui {

/**
 * The centralised manager of all lumiera-gui's windows.
 **/
class WindowManager
{
public:
  /**
   * Default constructor
   **/
  WindowManager();

  /**
   * Sets the theme of the lumiera-gui's.
   * @param path This string must specify a path where a GTK stylesheet
   * will be found.
   **/
  bool set_theme(Glib::ustring path);
  
  /**
   * A utility function which reads a colour style from the GTK Style.
   * @param widget The widget to load the style out of.
   * @param property_name The name of the style property to load.
   * @param red The fallback red intensity.
   * @param green The fallback green intensity.
   * @param blue The fallback blue intensity.
   * @return The loaded colour.
   **/
  static GdkColor read_style_colour_property(
    Gtk::Widget &widget, const gchar *property_name,
    guint16 red, guint16 green, guint16 blue);
    
private:

  /**
   * Registers the extra large icon size.
   **/
  static void register_giant_icon_size();

  /**
   * Registers application stock items: icons and
   * labels associated with IDs
   **/
  static void register_stock_items();

  /**
   * Adds an icon (in different sizes) to the icon factory.
   * @param factory The factory to add the icon to.
   * @param icon_name The file name of the icon to add.
   * @param id The id name of the icon.
   * @param label The user readable icon name for this icon.
   * @return Returns true if the icon was successfully loaded, returns
   * false otherwise.
   **/
  static bool add_stock_icon_set(
    const Glib::RefPtr<Gtk::IconFactory>& factory,
    const Glib::ustring& icon_name,
    const Glib::ustring& id,
    const Glib::ustring& label);
  
  /**
   * Loads an icon, searching standard icon locations,
   * and adds it to an icon set.
   * @param icon_set The icon set to add the icon to.
   * @param icon_name The file name of the icon to load.
   * @param size The size of the icon to load.
   * @param wildcard This value is set to true if this icon is
   * wildcarded.
   * @return Returns true if the icon was loaded successfully.
   **/
  static bool add_stock_icon(Gtk::IconSet &icon_set,
    const Glib::ustring& icon_name, Gtk::IconSize size, bool wildcard);
  
  /**
   * Loads an icon from a specific path and adds it to an icon set.
   * @param icon_set The icon set to add the icon to.
   * @param base_dir The root icons directory to load from.
   * @param icon_name The file name of the icon to load.
   * @param size The size of the icon to load.
   * @param wildcard This value is set to true if this icon is
   * wildcarded.
   * @return Returns true if the icon was loaded successfully.
   **/
  static bool add_stock_icon_source(Gtk::IconSet &icon_set,
    const Glib::ustring& base_dir, const Glib::ustring& icon_name,
    Gtk::IconSize size, bool wildcard);
    
private:

  /**
   * The registered icon size for giant 48x48 px icons.
   * @remarks This value is set to BuiltinIconSize::ICON_SIZE_INVALID
   * until register_giant_icon_size is called.
   **/
  static Gtk::IconSize giantIconSize;
};

}   // namespace gui

#endif // WINDOW_MANAGER_HPP
