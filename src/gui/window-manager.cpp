/*
  window-manager.cpp  -  Defines the global UI Manager class
 
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
 
* *****************************************************/

#include "window-manager.hpp"

using namespace Gtk;
using namespace Glib;

namespace gui {
  
IconSize WindowManager::GiantIconSize = ICON_SIZE_INVALID;
IconSize WindowManager::MenuIconSize = ICON_SIZE_INVALID;

WindowManager::WindowManager()
{
  register_app_icon_sizes();  
  register_stock_items();
}

bool
WindowManager::set_theme(Glib::ustring path)
{
  if(access(path.c_str(), R_OK))
    {
      ERROR(gui, "WindowManger: Unable to load rc file \"%s\"",
        path.c_str());
      return false;        
    }
  
  gtk_rc_parse(path.c_str());
  gtk_rc_reset_styles (gtk_settings_get_default());

  return true;
}
    
GdkColor
WindowManager::read_style_colour_property(
  Gtk::Widget &widget, const gchar *property_name,
  guint16 red, guint16 green, guint16 blue)
{
  GdkColor *colour;

  gtk_widget_style_get(widget.gobj(), property_name, &colour, NULL);

  // Did the color load successfully?
  if(colour != NULL)
    return *colour;
  else
  {
    WARN(gui, "%s style value failed to load", property_name);
    
    const GdkColor default_colour = {0, red, green, blue};  
    return default_colour;
  }
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
  RefPtr<IconFactory> factory = IconFactory::create();
  
  add_stock_icon_set(factory, "panel-assets", "panel_assets", _("_Assets"));
  add_stock_icon_set(factory, "panel-timeline", "panel_timeline", _("_Timeline"));
  add_stock_icon_set(factory, "panel-viewer", "panel_viewer", _("_Viewer"));
  
  add_stock_icon_set(factory, "tool-arrow", "tool_arrow", _("_Arrow"));
  add_stock_icon_set(factory, "tool-i-beam", "tool_i_beam", _("_I-Beam"));
  
  add_stock_icon_set(factory, "track-disabled", "track_disabled", _("Track Disabled"));
  add_stock_icon_set(factory, "track-enabled", "track_enabled", _("Track Enabled"));
  add_stock_icon_set(factory, "track-locked", "track_locked", _("Track Locked"));
  add_stock_icon_set(factory, "track-unlocked", "track_unlocked", _("Track Unlocked")); 

  factory->add_default(); //Add factory to list of factories.
}

bool
WindowManager::add_stock_icon_set(
  const Glib::RefPtr<IconFactory>& factory,
  const Glib::ustring& icon_name,
  const Glib::ustring& id,
  const Glib::ustring& label)
{
  Gtk::IconSet icon_set;
  
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
      ERROR(gui, "Unable to load icon \"%s\"", icon_name.c_str());
      return false;
    }
  
  // Add the icon set to the icon factory
  const Gtk::StockID stock_id(id);
  factory->add(stock_id, icon_set);
  Gtk::Stock::add(Gtk::StockItem(stock_id, label));
  return true;
}

bool
WindowManager::add_stock_icon(Gtk::IconSet &icon_set,
  const Glib::ustring& icon_name, Gtk::IconSize size, bool wildcard)
{
  // Try the ~/.lumiera/icons folder
  if(add_stock_icon_source(icon_set, ustring::compose("%1/%2",
    GtkLumiera::get_home_data_path(), ustring("icons")),
    icon_name, size, wildcard))
    return true;
  
  if(add_stock_icon_source(
    icon_set, get_current_dir(), icon_name, size, wildcard))
    return true;
    
  return false;
}

bool
WindowManager::add_stock_icon_source(Gtk::IconSet &icon_set,
  const Glib::ustring& base_dir, const Glib::ustring& icon_name,
  Gtk::IconSize size, bool wildcard)
{
  ustring path;
  Gtk::IconSource source;
  
  int width = 0, height = 0;
  if(!IconSize::lookup(size, width, height))
    return false;
  
  try
    {  
      ustring path = ustring::compose("%1/%2x%3/%4.png",
        base_dir, width, height, icon_name);
        
      INFO(gui, "Attempting to load icon: %s", path.c_str());
      
      // This throws an exception if the file is not found:
      source.set_pixbuf(Gdk::Pixbuf::create_from_file(path));
    }
  catch(const Glib::Exception& ex)
    {
      INFO(gui, "Failed to load icon: %s", path.c_str());
      return false;
    }

  source.set_size(size);
  source.set_size_wildcarded(wildcard);

  icon_set.add_source(source);

  return true;
}

}   // namespace gui
