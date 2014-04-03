/*
  WINDOW-MANAGER.hpp  -  Global UI Manager class

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


/** @file window-manager.hpp
 ** Manager for all application windows and resources.
 ** This file defines the global UI Manager class. The central WindowManager
 ** instance is owned by the GtkLumiera object and initialised in GTK-main.
 ** The WindowManager has the ability to create new windows integrated with
 ** the application framework, to provide Icons and other resources and
 ** to set and access a general UI theme. 
 ** 
 ** @see gtk-lumiera.hpp
 */


#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include "gui/gtk-base.hpp"

#include <boost/noncopyable.hpp>
#include <cairomm/cairomm.h>
#include <string>
#include <memory>

namespace gui {
  
using std::string;
using std::shared_ptr;


namespace model      { class Project; }
namespace controller { class Controller; }
namespace workspace  { class WorkspaceWindow;}


/**
 * The centralised manager of all the windows,
 * icons and resources within Lumiera's GUI.
 */
class WindowManager 
  : boost::noncopyable
{
  string iconSearchPath_;
  string resourceSerachPath_;
  
  
public:
  /**
   * Initialise the window manager on application start.
   * Register the icon configuration and sizes and lookup 
   * all the icons -- either from the default theme of via
   * the given Lumiera icon search paths (see \c setup.ini ).
   * @see lumiera::Config
   */
  void init (string const& iconPath, string const& resourcePath);
  
  /**
   * Creates a new window connected to a specified project and controller
   * @param source_project The project to connect the window to.
   * @param source_controller The controller to connect the window to.
   */
  void newWindow (gui::model::Project&, gui::controller::Controller&);

  /**
   * Sets the theme to use for the Lumiera GUI.
   * @param stylesheetName GTK stylesheet to load from the resourceSearchPath_
   * @throw error::Config if this stylesheet can't be resolved on the searchpath
   * @see #init
   * @see lumiera::Config
   */
  void setTheme (string const& stylesheetName);

  /**
   * A utility function which reads a colour style from the GTK Style.
   * @param widget The widget to load the style out of.
   * @param property_name The name of the style property to load.
   * @param red The fallback red intensity.
   * @param green The fallback green intensity.
   * @param blue The fallback blue intensity.
   * @return The loaded colour.
   */
  static Cairo::RefPtr<Cairo::SolidPattern>
  read_style_colour_property (Gtk::Widget &widget, const gchar *property_name,
                              guint16 red, guint16 green, guint16 blue);

private:

  /** Event handler for when a window has been closed */
  bool on_window_closed(GdkEventAny* event);
    
private:

  /**
   * On creating and closing of any window, handle enabling or disabling of the
   * Window/Close Window menu option.
   *
   * It should be enabled when there is more than one window and disabled
   * otherwise.
   */
  void update_close_window_in_menus();
  
  
  /** Registers the custom icon sizes. */
  void register_app_icon_sizes();

  /**
   * Registers application stock items: icons and
   * labels associated with IDs
   */
  void register_stock_items();

  /**
   * Adds an icon (in different sizes) to the icon factory.
   * @param factory The factory to add the icon to.
   * @param icon_name The file name of the icon to add.
   * @param id The id name of the icon.
   * @param label The user readable icon name for this icon.
   * @return \c true if the icon was successfully loaded,
   *         returns \c false otherwise.
   */
  bool add_stock_icon_set(
    const Glib::RefPtr<Gtk::IconFactory>& factory,
    cuString& icon_name,
    cuString& id,
    cuString& label);
  
  /**
   * Loads an icon, searching standard icon locations,
   * and adds it to an icon set.
   * @param icon_set The icon set to add the icon to.
   * @param icon_name The file name of the icon to load.
   * @param size The size of the icon to load.
   * @param wildcard \c true if this icon is to be wildcarded.
   * @return \c true if the icon was loaded successfully.
   */
  bool add_stock_icon(Gtk::IconSet &icon_set,
    cuString& icon_name, Gtk::IconSize size, bool wildcard);

  /**
   * Loads an icon from a the icon theme
   * @param icon_set The icon set to add the icon to.
   * @param icon_name The name of the icon to load.
   * @param size The size of the icon to load.
   * @param wildcard \c true if this icon is to be wildcarded.
   * @return \c true if the icon was loaded successfully.
   */
  bool add_theme_icon_source(Gtk::IconSet &icon_set,
    cuString& icon_name, Gtk::IconSize size, bool wildcard);
  
  /**
   * Loads an icon from a non theme set.
   * @param icon_set The icon set to add the icon to.
   * @param base_dir The root icons directory to load from.
   * @param icon_name The file name of the icon to load.
   * @param size The size of the icon to load.
   * @param wildcard \c true if this icon is to be wildcarded.
   * @return \c true if the icon was loaded successfully.
   */
  bool add_non_theme_icon_source(Gtk::IconSet &icon_set,
    cuString& base_dir, cuString& icon_name,
    Gtk::IconSize size, bool wildcard);

  /**
   * Loads an icon from a specific path and adds it to an icon set.
   * @param path The path to load from.
   * @param icon_set The icon set to add the icon to.
   * @param size The size of the icon to load.
   * @param wildcard \c true if this icon is to be wildcarded.
   * @return \c true if the icon was loaded successfully.
   */
  bool add_stock_icon_from_path(string path,
    Gtk::IconSet &icon_set, Gtk::IconSize size, bool wildcard);


private:
  
  std::list<shared_ptr<workspace::WorkspaceWindow> > windowList;
    
public:

  /**
   * The registered icon size for giant 48x48 px icons.
   * @remarks This value is set to BuiltinIconSize::ICON_SIZE_INVALID
   * until register_giant_icon_size is called.
   */
  static Gtk::IconSize GiantIconSize;
  
  /**
   * The registered icon size for giant 16x16 px icons.
   * @remarks This value is set to BuiltinIconSize::ICON_SIZE_INVALID
   * until register_app_icon_sizes is called.
   */
  static Gtk::IconSize MenuIconSize;
};



}// namespace gui
#endif
