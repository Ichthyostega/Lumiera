/*
  WindowManager  -  Global UI Manager

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

* *****************************************************/


/** @file §§§
 ** TODO §§§
 */


#include "gui/window-manager.hpp"
#include "gui/gtk-lumiera.hpp"
#include "gui/workspace/workspace-window.hpp"
#include "lib/searchpath.hpp"
#include "lib/util.hpp"

#include <gtkmm/stylecontext.h>
#include <boost/filesystem.hpp>
#include <memory>
#include <list>

using util::cStr;
using std::list;
using std::shared_ptr;

using namespace Gtk;  ///////////////////////////TODO explicit using directives please!
using namespace Glib;
using namespace gui::workspace;

namespace fsys = boost::filesystem;

namespace gui {
  
  IconSize WindowManager::GiantIconSize = ICON_SIZE_INVALID;
  IconSize WindowManager::MenuIconSize = ICON_SIZE_INVALID;
  
  
  void
  WindowManager::init (string const& iconPath, string const& resourcePath)
  {
    this->iconSearchPath_ = iconPath;
    this->resourceSerachPath_ = resourcePath;
    
    registerAppIconSizes();
    registerStockItems();
  }
  
  
  void
  WindowManager::setTheme (string const& stylesheetName)
  {
    auto screen = Gdk::Screen::get_default();
    auto css_provider = CssProvider::create();
    try
      {
        css_provider->load_from_path (lib::resolveModulePath (stylesheetName, resourceSerachPath_));
                                                                 /////////////////////////////////TICKET #953 should detect and notify CSS parsing errors. CssProvider offers a signal for this purpose
                                                                 /////////////////////////////////TICKET #953 ...seems to be supported properly starting with gtkmm 3.18 (Debian/Jessie has 3.14)
      }
    catch(Glib::Error const& failure)
      {
        WARN(gui, "Failure while loading stylesheet '%s': %s", cStr(stylesheetName), cStr(failure.what()));
      }
    
    StyleContext::add_provider_for_screen (screen, css_provider,
                                           GTK_STYLE_PROVIDER_PRIORITY_USER);
  }
  
  
  void
  WindowManager::newWindow (gui::model::Project& source_project, gui::controller::Controller& source_controller)
  { 
    shared_ptr<WorkspaceWindow> window(new WorkspaceWindow(source_project, source_controller));
    REQUIRE(window);
    
    window->signal_delete_event().connect(sigc::mem_fun(
      this, &WindowManager::on_window_closed));
    
    windowList.push_back(window);
    
    window->show();
  
    updateCloseWindowInMenus();
  }
  
  
  bool
  WindowManager::on_window_closed (GdkEventAny* event)
  {
    REQUIRE(event);
    REQUIRE(event->window);
    
    list< shared_ptr<WorkspaceWindow> >::iterator iterator = windowList.begin();
    
    while (iterator != windowList.end())
      {
        shared_ptr<WorkspaceWindow> workspace_window(*iterator);
        REQUIRE(workspace_window);
        
        Glib::RefPtr<Gdk::Window> window = workspace_window->get_window();
        REQUIRE(window);
        if (window->gobj() == event->window)
          {
            // This window has been closed
            iterator = windowList.erase(iterator);
          }
        else
          iterator++;
      }
    
    if (windowList.empty())
      {
        // All windows have been closed - we should exit
        Main *main = Main::instance();
        REQUIRE(main);
        main->quit();
      }
    
    updateCloseWindowInMenus();
    
    // Unless this is false, the window won't close
    return false;
  }
  
  
  void
  WindowManager::updateCloseWindowInMenus()
  {
    bool enable = windowList.size() > 1;
    
    list< shared_ptr<WorkspaceWindow> >::iterator iterator = 
      windowList.begin();
    
    while (iterator != windowList.end())
      {
        shared_ptr<WorkspaceWindow> workspace_window(*iterator);
        REQUIRE(workspace_window);
        
        workspace_window->set_close_window_sensitive(enable);
        iterator++;
      }
  }
  
  
  Cairo::RefPtr<Cairo::SolidPattern>
  WindowManager::readStyleColourProperty (Gtk::Widget& widget
                                         ,const gchar * property_name
                                         ,guint16 red, guint16 green, guint16 blue)
  {
    REQUIRE (property_name);
    
    // TODO: Can we get rid of the GdkColor completely here?
    GdkColor *color;
    gtk_widget_style_get(widget.gobj(), property_name, &color, NULL);
    
    Cairo::RefPtr<Cairo::SolidPattern> pattern;
    // Did the color load successfully?
    if (color != NULL)
      {
        pattern = Cairo::SolidPattern::create_rgb ( (double)color->red   / 0xFFFF,
                                                    (double)color->green / 0xFFFF,
                                                    (double)color->blue  / 0xFFFF);
      }
    else
      {
        WARN(gui, "%s style value failed to load", property_name);
        
        pattern = Cairo::SolidPattern::create_rgb ( red, green, blue );
      }
    
    return pattern;
  }
  
  
  void
  WindowManager::registerAppIconSizes()
  {
    if(GiantIconSize == ICON_SIZE_INVALID)
      GiantIconSize = IconSize::register_new ("giant", 48, 48);
    if(MenuIconSize == ICON_SIZE_INVALID)
      MenuIconSize = IconSize::register_new ("menu", 16, 16);
  }
  
  
  void
  WindowManager::registerStockItems()
  {
    Glib::RefPtr<IconFactory> factory = IconFactory::create();
    
    addStockIconSet(factory, "panel-assets",   "panel_assets",  _("_Assets"));
    addStockIconSet(factory, "panel-timeline", "panel_timeline",_("_Timeline"));
    addStockIconSet(factory, "panel-viewer",   "panel_viewer",  _("_Viewer"));
    
    addStockIconSet(factory, "window-new",     "new_window",    _("New _Window"));
    
    addStockIconSet(factory, "tool-arrow",     "tool_arrow",    _("_Arrow"));
    addStockIconSet(factory, "tool-i-beam",    "tool_i_beam",   _("_I-Beam"));
    
    addStockIconSet(factory, "track-disabled", "track_disabled",_("Track Disabled"));
    addStockIconSet(factory, "track-enabled",  "track_enabled", _("Track Enabled"));
    addStockIconSet(factory, "track-locked",   "track_locked",  _("Track Locked"));
    addStockIconSet(factory, "track-unlocked", "track_unlocked",_("Track Unlocked"));
    
    factory->add_default(); //Add factory to list of factories.
  }
  
  
  bool
  WindowManager::addStockIconSet (Glib::RefPtr<IconFactory> const& factory
                                 ,cuString& icon_name
                                 ,cuString& id
                                 ,cuString& label)
  {
    Glib::RefPtr<Gtk::IconSet> icon_set = Gtk::IconSet::create();
    
    // Load all the sizes, wildcarding the first, largest icon to be
    // loaded
    bool no_icons = true;
    no_icons &= !addStockIcon(
      icon_set, icon_name, GiantIconSize, no_icons);
    no_icons &= !addStockIcon(
      icon_set, icon_name, ICON_SIZE_BUTTON, no_icons);
    no_icons &= !addStockIcon(
      icon_set, icon_name, ICON_SIZE_MENU, no_icons);
    no_icons &= !addStockIcon(
      icon_set, icon_name, ICON_SIZE_LARGE_TOOLBAR, no_icons);
    no_icons &= !addStockIcon(
      icon_set, icon_name, MenuIconSize, no_icons);
    
    if(no_icons)
      {
        // No icons were loaded
        ERROR (gui, "Unable to load icon '%s'", cStr(icon_name));
        return false;
      }
    
    // Add the icon set to the icon factory
    const Gtk::StockID stock_id(id);
    factory->add(stock_id, icon_set);
    Gtk::Stock::add(Gtk::StockItem(stock_id, label));
    return true;
  }
  
  
  bool
  WindowManager::addStockIcon (Glib::RefPtr<Gtk::IconSet> const& icon_set
                              ,cuString& icon_name
                              ,Gtk::IconSize size
                              ,bool wildcard)
  {
    // Try the icon theme  
    if(addThemeIconSource(icon_set, icon_name, size, wildcard))
      return true;
    
    // Try to resolve the icon via the configured search path
    lib::SearchPathSplitter iconLocations (iconSearchPath_);
    while (iconLocations)
      if (addNonThemeIconSource (icon_set
                                ,iconLocations.next()
                                ,icon_name
                                ,size
                                ,wildcard))
        return true;
    
    return false; // icon not found
  }
  
  
  bool
  WindowManager::addThemeIconSource (Glib::RefPtr<Gtk::IconSet> const& icon_set
                                    ,cuString& icon_name
                                    ,Gtk::IconSize size
                                    ,bool wildcard)
  {
    // Get the size
    int width = 0, height = 0;
    if(!IconSize::lookup(size, width, height))
      return false;
    REQUIRE(width > 0);
    
    // Try to load the icon
    Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();
    REQUIRE(theme);
    
    TODO ("find out how IconInfo could be made const. For example, GTKmm 2.10.10 is missing the const on operator bool() in iconinfo.h");
    IconInfo info = theme->lookup_icon(icon_name, width, (IconLookupFlags)0);
    
    if (!info) return false; // unable to resolve Icon
    
    cuString path(info.get_filename());
    return addStockIconFromPath(path, icon_set, size, wildcard);
  }
  
  
  bool
  WindowManager::addNonThemeIconSource (Glib::RefPtr<Gtk::IconSet> const& icon_set
                                       ,cuString& base_dir
                                       ,cuString& icon_name
                                       ,Gtk::IconSize size
                                       ,bool wildcard)
  {
    // Get the size
    int width = 0, height = 0;
    if(!IconSize::lookup(size, width, height))
      return false;
    REQUIRE(width > 0);
    
    // Try to load the icon
    cuString path(ustring::compose("%1/%2x%3/%4.png",
      base_dir, width, height, icon_name)); 
    return addStockIconFromPath(path, icon_set, size, wildcard);
  }
  
  
  bool
  WindowManager::addStockIconFromPath (string path
                                      ,Glib::RefPtr<Gtk::IconSet> const& icon_set
                                      ,Gtk::IconSize size
                                      ,bool wildcard)
  {
    if (!fsys::exists (path)) return false;
    
    try {
        Gtk::IconSource source;
        source.set_pixbuf(Gdk::Pixbuf::create_from_file(path));
        source.set_size_wildcarded(wildcard);
        source.set_size(size);
        
        icon_set->add_source(source);
        
        return true;
      }
    
    catch(Glib::Exception const& ex)
      {
        WARN (gui, "Failure when accessing icon '%s'. Problem: %s", cStr(path), cStr(ex.what()));
        return false;
      }
  }
  
  
}// namespace gui
