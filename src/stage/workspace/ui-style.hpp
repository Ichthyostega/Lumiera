/*
  UI-STYLE.hpp  -  manage coherent UI styling

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file ui-style.hpp
 ** Service for global theming and style related concerns.
 ** The central UiManager operates an instance of this service to set up and configure the UI
 ** globally. Widgets and similar parts of the interface may use it, when taking decisions
 ** regarding the look and layout details.
 ** 
 ** @see ui-manager.hpp
 ** @see ui-bus.hpp
 */


#ifndef GUI_WORKSPACE_UI_STYLE_H
#define GUI_WORKSPACE_UI_STYLE_H

#include "stage/gtk-base.hpp"
#include "lib/nocopy.hpp"

#include <cairomm/cairomm.h>
#include <string>


namespace gui {
  
  namespace model      { class Project; }           ////////////////////////////////////////////////////TICKET #1048 : rectify UI lifecycle
  namespace controller { class Controller; }        ////////////////////////////////////////////////////TICKET #1048 : rectify UI lifecycle

  class UiBus;

namespace workspace {
  
  using std::string;
  
  
  
  /**
   * Manage global concerns regarding a coherent user interface.
   * Offers access to some global UI resources, and establishes
   * further global services to create workspace windows, to bind
   * menu / command actions and to enter the top-level model parts.
   */
  class UIStyle
    : public Gtk::UIManager
    , util::NonCopyable
    {
      
      string iconSearchPath_;
      string resourceSerachPath_;
      
    public:
      
      /** The registered icon size for giant 48x48 px icons.
       * @remarks This value is set to BuiltinIconSize::ICON_SIZE_INVALID
       *          until register_giant_icon_size is called.
       */
      static Gtk::IconSize GiantIconSize;
      
      /** The registered icon size for giant 16x16 px icons.
       * @remarks This value is set to BuiltinIconSize::ICON_SIZE_INVALID
       *          until register_app_icon_sizes is called.
       */
      static Gtk::IconSize MenuIconSize;
      
      
    public:
      /**
       * Set up a coherent theming and styling for the application.
       * Based on the Gtk::UIManager, the UIStyle service allows to access some
       * style related resources, but mostly its task is to configure the GTK toolkit
       * appropriately during startup.
       */
      UIStyle();
      
      /**
       * Sets the theme to use for the Lumiera GUI.
       * @param stylesheetName GTK CSS stylesheet to load from the resourceSearchPath_
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
      readStyleColourProperty (Gtk::Widget &widget, const gchar *property_name,
                               guint16 red, guint16 green, guint16 blue);
      
    private:
      void registerAppIconSizes();
      void registerStockItems();
      
      /**
       * Adds an icon (in different sizes) to the icon factory.
       * @param factory The factory to add the icon to.
       * @param icon_name The file name of the icon to add.
       * @param id The id name of the icon.
       * @param label The user readable icon name for this icon.
       * @return \c true if the icon was successfully loaded,
       *         returns \c false otherwise.
       */
      bool
      addStockIconSet (Glib::RefPtr<Gtk::IconFactory> const& factory
                      ,cuString& icon_name
                      ,cuString& id
                      ,cuString& label);
      
      /**
       * Loads an icon, searching standard icon locations,
       * and adds it to an icon set.
       * @param icon_set The icon set to add the icon to.
       * @param icon_name The file name of the icon to load.
       * @param size The size of the icon to load.
       * @param wildcard \c true if this icon is to be wildcarded.
       * @return \c true if the icon was loaded successfully.
       */
      bool
      addStockIcon (Glib::RefPtr<Gtk::IconSet> const& icon_set
                   ,cuString& icon_name
                   ,Gtk::IconSize size
                   ,bool wildcard);
      
      /**
       * Loads an icon from a the icon theme
       * @param icon_set The icon set to add the icon to.
       * @param icon_name The name of the icon to load.
       * @param size The size of the icon to load.
       * @param wildcard \c true if this icon is to be wildcarded.
       * @return \c true if the icon was loaded successfully.
       */
      bool
      addThemeIconSource (Glib::RefPtr<Gtk::IconSet> const& icon_set
                         ,cuString& icon_name
                         ,Gtk::IconSize size
                         ,bool wildcard);
      
      /**
       * Loads an icon from a non theme set.
       * @param icon_set The icon set to add the icon to.
       * @param base_dir The root icons directory to load from.
       * @param icon_name The file name of the icon to load.
       * @param size The size of the icon to load.
       * @param wildcard \c true if this icon is to be wildcarded.
       * @return \c true if the icon was loaded successfully.
       */
      bool
      addNonThemeIconSource (Glib::RefPtr<Gtk::IconSet> const& icon_set
                            ,cuString& base_dir
                            ,cuString& icon_name
                            ,Gtk::IconSize size
                            ,bool wildcard);
      
      /**
       * Loads an icon from a specific path and adds it to an icon set.
       * @param path The path to load from.
       * @param icon_set The icon set to add the icon to.
       * @param size The size of the icon to load.
       * @param wildcard \c true if this icon is to be wildcarded.
       * @return \c true if the icon was loaded successfully.
       */
      bool
      addStockIconFromPath (string path
                           ,Glib::RefPtr<Gtk::IconSet> const& icon_set
                           ,Gtk::IconSize size
                           ,bool wildcard);
      
    };
  
  
  
}}// namespace gui::workspace
#endif /*GUI_WORKSPACE_UI_STYLE_H*/
