/*
  UI-MANAGER.hpp  -  Global UI Manager

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


/** @file ui-manager.hpp
 ** Manager for global user interface concerns and global state.
 ** The central UiManager instance is owned by the GtkLumiera object and initialised in GTK-main.
 ** It establishes and wires the top-level entities of the UI-Layer and thus, indirectly offers
 ** services to provide Icons and other resources, to open and manage workspace windows, to
 ** form and issue (global) actions and to delve into the UI representation of top-level parts
 ** of the session model. Notable connections established herein:
 ** - connection to the [UI-Bus](\ref ui-bus.hpp)
 ** - the global Actions available though the menu
 ** - the WindowList
 ** - the InteractionDirector (top-level controller)
 ** 
 ** @see gtk-lumiera.hpp
 ** @see ui-bus.hpp
 */


#ifndef GUI_WORKSPACE_UI_MANAGER_H
#define GUI_WORKSPACE_UI_MANAGER_H

#include "gui/gtk-base.hpp"

#include <boost/noncopyable.hpp>
#include <cairomm/cairomm.h>
#include <string>
#include <memory>


namespace gui {
  
  namespace model      { class Project; }           ////////////////////////////////////////////////////TICKET #1048 : rectify UI lifecycle
  namespace controller { class Controller; }        ////////////////////////////////////////////////////TICKET #1048 : rectify UI lifecycle

  class UiBus;

namespace workspace {
  
  using std::unique_ptr;
  using std::string;
  
  class GlobalCtx;
  class Actions;
  
  
  
  /**
   * Manage global concerns regarding a coherent user interface.
   * Offers access to some global UI resources, and establishes
   * further global services to create workspace windows, to bind
   * menu / command actions and to enter the top-level model parts.
   */
  class UiManager
    : public Gtk::UIManager
    , boost::noncopyable
    {
      
      unique_ptr<GlobalCtx> globals_;
      unique_ptr<Actions>   actions_;
      
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
       * There is one global UiManager instance,
       * which is created by [the Application](\ref GtkLumiera)
       * and allows access to the UI-Bus backbone. The UiManager itself
       * is _not a ctrl::Controller,_ and thus not directly connected to the Bus.
       * Rather, supports the top-level windows for creating a consistent interface.
       */
      UiManager (UiBus& bus);
     ~UiManager ();
      
      /**
       * Set up the first top-level application window.
       * This triggers the build-up of the user interface widgets.
       */
      void createApplicationWindow();
      
      /** @todo find a solution how to enable/disable menu entries according to focus
       *                                               /////////////////////////////////////////////////TICKET #1076  find out how to handle this properly
       */
      void updateWindowFocusRelatedActions();
      
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
      
      void allowCloseWindow (bool yes);
      
    private:
      void initGlobalUI ();
      
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
      
      
    private:
      
    };
  
  
  
}}// namespace gui::workspace
#endif /*GUI_WORKSPACE_UI_MANAGER_H*/
