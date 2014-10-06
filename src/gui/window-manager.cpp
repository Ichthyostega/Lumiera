/*
  WindowManager  -  Global UI Manager class

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


#include "gui/window-manager.hpp"
#include "gui/gtk-lumiera.hpp"
#include "gui/workspace/workspace-window.hpp"
#include "lib/searchpath.hpp"
#include "lib/util.hpp"

#include <boost/filesystem.hpp>
#include <memory>
#include <list>

using util::cStr;
using std::list;
using std::shared_ptr;

using namespace Gtk;
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
  
  register_app_icon_sizes();  
  register_stock_items();
}


void
WindowManager::setTheme (string const& stylesheetName)
{
  //////////////////////////////////////////////////////////////////////////////////////////TICKET #937 : load here the CSS for GTK3
  gtk_rc_parse (cStr(lib::resolveModulePath (stylesheetName, resourceSerachPath_)));
  gtk_rc_reset_styles (gtk_settings_get_default());
}


void
WindowManager::newWindow (gui::model::Project& source_project,
                          gui::controller::Controller &source_controller)
{ 
  shared_ptr<WorkspaceWindow> window(
    new WorkspaceWindow(source_project, source_controller));
  REQUIRE(window);
  
  window->signal_delete_event().connect(sigc::mem_fun(
    this, &WindowManager::on_window_closed));
  
  windowList.push_back(window);
  
  window->show();

  update_close_window_in_menus();
}


bool
WindowManager::on_window_closed (GdkEventAny* event)
{ 
  REQUIRE(event);
  REQUIRE(event->window);
     
  list< shared_ptr<WorkspaceWindow> >::iterator iterator = 
    windowList.begin();
  
  while (iterator != windowList.end())
    {
      shared_ptr<WorkspaceWindow> workspace_window(*iterator);
      REQUIRE(workspace_window);

      Glib::RefPtr<Gdk::Window> window = workspace_window->get_window();
      REQUIRE(window);
      if(window->gobj() == event->window)
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

  update_close_window_in_menus();
      
  // Unless this is false, the window won't close
  return false;
}


void
WindowManager::update_close_window_in_menus()
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
WindowManager::read_style_colour_property (Gtk::Widget& widget,
                                           const gchar *property_name,
                                           guint16 red, guint16 green, guint16 blue)
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
WindowManager::register_app_icon_sizes()
{
  if(GiantIconSize == ICON_SIZE_INVALID)
    GiantIconSize = IconSize::register_new ("giant", 48, 48);
  if(MenuIconSize == ICON_SIZE_INVALID)
    MenuIconSize = IconSize::register_new ("menu", 16, 16);
}


void
WindowManager::register_stock_items()
{
  Glib::RefPtr<IconFactory> factory = IconFactory::create();
  
  add_stock_icon_set(factory, "panel-assets",   "panel_assets",  _("_Assets"));
  add_stock_icon_set(factory, "panel-timeline", "panel_timeline",_("_Timeline"));
  add_stock_icon_set(factory, "panel-viewer",   "panel_viewer",  _("_Viewer"));
  
  add_stock_icon_set(factory, "window-new",     "new_window",    _("New _Window"));
  
  add_stock_icon_set(factory, "tool-arrow",     "tool_arrow",    _("_Arrow"));
  add_stock_icon_set(factory, "tool-i-beam",    "tool_i_beam",   _("_I-Beam"));
  
  add_stock_icon_set(factory, "track-disabled", "track_disabled",_("Track Disabled"));
  add_stock_icon_set(factory, "track-enabled",  "track_enabled", _("Track Enabled"));
  add_stock_icon_set(factory, "track-locked",   "track_locked",  _("Track Locked"));
  add_stock_icon_set(factory, "track-unlocked", "track_unlocked",_("Track Unlocked")); 

  factory->add_default(); //Add factory to list of factories.
}


bool
WindowManager::add_stock_icon_set (Glib::RefPtr<IconFactory> const& factory,
                                   cuString& icon_name,
                                   cuString& id,
                                   cuString& label)
{
  Glib::RefPtr<Gtk::IconSet> icon_set =
                                 Gtk::IconSet::create();
  
  // Load all the sizes, wildcarding the first, largest icon to be
  // loaded
  bool no_icons = true;
  no_icons &= !add_stock_icon(
    icon_set, icon_name, GiantIconSize, no_icons);
  no_icons &= !add_stock_icon(
    icon_set, icon_name, ICON_SIZE_BUTTON, no_icons);
  no_icons &= !add_stock_icon(
    icon_set, icon_name, ICON_SIZE_MENU, no_icons);
  no_icons &= !add_stock_icon(
    icon_set, icon_name, ICON_SIZE_LARGE_TOOLBAR, no_icons);
  no_icons &= !add_stock_icon(
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
WindowManager::add_stock_icon (const Glib::RefPtr<Gtk::IconSet> &icon_set,
                               cuString& icon_name,
                               Gtk::IconSize size,
                               bool wildcard)
{ 
  // Try the icon theme  
  if(add_theme_icon_source(icon_set, icon_name, size, wildcard))
    return true;
  
  // Try to resolve the icon via the configured search path
  lib::SearchPathSplitter iconLocations (iconSearchPath_);
  while (iconLocations)
    if (add_non_theme_icon_source (icon_set
                                  ,iconLocations.next()
                                  ,icon_name
                                  ,size
                                  ,wildcard))
      return true;
  
  return false; // icon not found
}


bool
WindowManager::add_theme_icon_source (const Glib::RefPtr<Gtk::IconSet> &icon_set,
                                      cuString& icon_name,
                                      Gtk::IconSize size,
                                      bool wildcard)
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
  return add_stock_icon_from_path(path, icon_set, size, wildcard);
}


bool
WindowManager::add_non_theme_icon_source (const Glib::RefPtr<Gtk::IconSet> &icon_set,
                                          cuString& base_dir, cuString& icon_name,
                                          Gtk::IconSize size, bool wildcard)
{
  // Get the size
  int width = 0, height = 0;
  if(!IconSize::lookup(size, width, height))
    return false;
  REQUIRE(width > 0);
  
  // Try to load the icon
  cuString path(ustring::compose("%1/%2x%3/%4.png",
    base_dir, width, height, icon_name)); 
  return add_stock_icon_from_path(path, icon_set, size, wildcard);
}


bool
WindowManager::add_stock_icon_from_path (string path,
		const Glib::RefPtr<Gtk::IconSet> &icon_set,
                                         Gtk::IconSize size,
                                         bool wildcard)
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


}   // namespace gui
