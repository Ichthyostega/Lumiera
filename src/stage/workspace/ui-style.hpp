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


#ifndef STAGE_WORKSPACE_UI_STYLE_H
#define STAGE_WORKSPACE_UI_STYLE_H

#include "stage/gtk-base.hpp"
#include "common/advice.hpp"
#include "lib/symbol.hpp"
#include "lib/nocopy.hpp"

#include <cairomm/cairomm.h>
#include <string>


namespace stage {
  
  namespace model      { class Project; }           ////////////////////////////////////////////////////TICKET #1048 : rectify UI lifecycle
  namespace controller { class Controller; }        ////////////////////////////////////////////////////TICKET #1048 : rectify UI lifecycle
  namespace timeline { class TimelineWidget; }

  class UiBus;

namespace workspace {

  using lib::Literal;
  using std::string;
  
  using StyleAdvice = lumiera::advice::Provision<PStyleContext>;

  
  
  
  /**
   * Manage global concerns regarding a coherent user interface.
   * Offers access to some global UI resources, and establishes
   * further global services to create workspace windows, to bind
   * menu / command actions and to enter the top-level model parts.
   */
  class UiStyle
    : public Gtk::UIManager
    , util::NonCopyable
    {
      
      string iconSearchPath_;
      string resourceSerachPath_;
      
      StyleAdvice styleAdviceTrackBody_;
      StyleAdvice styleAdviceTrackRuler_;
      
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
       * Based on the Gtk::UIManager, the UiStyle service allows to access some
       * style related resources, but mostly its task is to configure the GTK toolkit
       * appropriately during startup.
       */
      UiStyle();
      
      /**
       * Sets the theme to use for the Lumiera GUI.
       * @param stylesheetName GTK CSS stylesheet to load from the resourceSearchPath_
       * @throw error::Config if this stylesheet can't be resolved on the searchpath
       * @see lumiera::Config
       */
      void setTheme (string const& stylesheetName);
      
      /**
       * Use the existing TimelineWidget's GTK-WidgetPath to establish a systematic
       * CSS styling context, which can be used for theming and styling of Lumiera's
       * custom UI elements within the timeline display. Especially, this allows us
       * to anchor those custom elements at a dedicated point in the hierarchy, the
       * way it is visible for CSS selectors. Thus, either standard CSS rules and
       * inheritance take effect on our custom elements, or the GTK stylesheet
       * may add dedicated style rules for elements within the timeline display,
       * most notably the system of nested scopes in the track fork and the
       * display of clips and overlays.
       * 
       * Especially the following selector path can be used
       * - `paned.timeline__page > timeline__body > fork.timeline__fork`
       */
      void prepareStyleContext (timeline::TimelineWidget const&);
      
      
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
                      ,Literal iconName
                      ,Literal id
                      ,Literal label);
      
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
  
  
  
}}// namespace stage::workspace
#endif /*STAGE_WORKSPACE_UI_STYLE_H*/
