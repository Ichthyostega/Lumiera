/*
  Actions.cpp  -  Definition of the main workspace window object
 
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

#include "actions.hpp"
#include "workspace-window.hpp"

#include "../dialogs/render.hpp"
#include "../dialogs/preferences-dialog.hpp"

using namespace Gtk;
using namespace Glib;
using namespace sigc;
using namespace lumiera::gui;

namespace lumiera {
namespace gui {
namespace workspace {

Actions::Actions(WorkspaceWindow &workspace_window) :
  workspaceWindow(workspace_window),
  is_updating_action_state(false)
{
  register_stock_items();

  workspace_window.signal_show ().connect_notify(mem_fun(this, &Actions::update_action_state));

  //----- Create the Action Group -----//
  actionGroup = ActionGroup::create();
  
  // File menu
  actionGroup->add(Action::create("FileMenu", _("_File")));
  actionGroup->add(Action::create("FileNewProject", Stock::NEW, _("_New Project...")),
    sigc::mem_fun(*this, &Actions::on_menu_file_new_project));
  actionGroup->add(Action::create("FileOpenProject", Stock::OPEN, _("_Open Project...")),
    sigc::mem_fun(*this, &Actions::on_menu_file_open_project));
  actionGroup->add(Action::create("FileRender", _("_Render...")),
    Gtk::AccelKey("<shift>R"),
    sigc::mem_fun(*this, &Actions::on_menu_file_render));
  actionGroup->add(Action::create("FileQuit", Stock::QUIT),
    sigc::mem_fun(*this, &Actions::on_menu_file_quit));

  // Edit menu
  actionGroup->add(Action::create("EditMenu", _("_Edit")));
  actionGroup->add(Action::create("EditCopy", Stock::COPY),
    sigc::mem_fun(*this, &Actions::on_menu_others));
  actionGroup->add(Action::create("EditPaste", Stock::PASTE),
    sigc::mem_fun(*this, &Actions::on_menu_others));
  actionGroup->add(Action::create("EditPreferences", Stock::PREFERENCES),
    sigc::mem_fun(*this, &Actions::on_menu_edit_preferences));

  // View Menu
  actionGroup->add(Action::create("ViewMenu", _("_View")));
  
  assetsPanelAction = ToggleAction::create("ViewAssets", Gtk::StockID("assets_panel"));
  assetsPanelAction->signal_toggled().connect(
    sigc::mem_fun(*this, &Actions::on_menu_view_assets));
  actionGroup->add(assetsPanelAction);
  
  timelinePanelAction = ToggleAction::create("ViewTimeline", Gtk::StockID("timeline_panel"));
  timelinePanelAction->signal_toggled().connect(
    sigc::mem_fun(*this, &Actions::on_menu_view_timeline));
  actionGroup->add(timelinePanelAction);

  viewerPanelAction = ToggleAction::create("ViewViewer", Gtk::StockID("viewer_panel"));
  viewerPanelAction->signal_toggled().connect(
    sigc::mem_fun(*this, &Actions::on_menu_view_viewer));
  actionGroup->add(viewerPanelAction);

  // Help Menu
  actionGroup->add(Action::create("HelpMenu", _("_Help")) );
  actionGroup->add(Action::create("HelpAbout", Stock::ABOUT),
  sigc::mem_fun(*this, &Actions::on_menu_help_about) );
}

void
Actions::register_stock_items()
{
  RefPtr<IconFactory> factory = IconFactory::create();
  
  add_stock_item_set(factory, "assets-panel.png", "assets_panel", _("_Assets"));
  add_stock_item_set(factory, "timeline-panel.png", "timeline_panel", _("_Timeline"));
  add_stock_item_set(factory, "viewer-panel.png", "viewer_panel", _("_Viewer"));
  
  add_stock_item_set(factory, "arrow.png", "arrow", _("_Arrow"));
  add_stock_item_set(factory, "i-beam.png", "i_beam", _("_I-Beam"));  

  factory->add_default(); //Add factory to list of factories.
}

bool
Actions::add_stock_item_set(const Glib::RefPtr<IconFactory>& factory,
                            const Glib::ustring& filename,
                            const Glib::ustring& id,
                            const Glib::ustring& label)
{
  Gtk::IconSet icon_set;
  
  add_stock_icon_source(icon_set, 16, filename);
  add_stock_icon_source(icon_set, 22, filename);
  add_stock_icon_source(icon_set, 24, filename);
  add_stock_icon_source(icon_set, 32, filename);
  add_stock_icon_source(icon_set, 48, filename);

  if(!icon_set.get_sizes().empty())
    {
      const Gtk::StockID stock_id(id);
      factory->add(stock_id, icon_set);
      Gtk::Stock::add(Gtk::StockItem(stock_id, label));
      return true;
    }
    
  return false;
}

bool
Actions::add_stock_icon_source(Gtk::IconSet &icon_set,
                        int size, const Glib::ustring& filename)
{
  Gtk::IconSource source;
  
  try
    {
      //This throws an exception if the file is not found:
      source.set_pixbuf( Gdk::Pixbuf::create_from_file(
        Glib::ustring::compose("%1x%1/%2", size, filename) ) );
    }
  catch(const Glib::Exception& ex)
    {
      return false;
    }

  source.set_size(IconSize(size));
  //source.set_size_wildcarded(); // Icon may be scaled.

  icon_set.add_source(source);

  return true;
}

void
Actions::update_action_state()
{
  REQUIRE(workspaceWindow.assets_panel != NULL);
  REQUIRE(workspaceWindow.timeline_panel != NULL);
  REQUIRE(workspaceWindow.viewer_panel != NULL); 
  
  is_updating_action_state = true;
  assetsPanelAction->set_active(workspaceWindow.assets_panel->is_shown());
  timelinePanelAction->set_active(workspaceWindow.timeline_panel->is_shown());
  viewerPanelAction->set_active(workspaceWindow.viewer_panel->is_shown());
  is_updating_action_state = false;
}

/* ===== File Menu Event Handlers ===== */

void
Actions::on_menu_file_new_project()
{
  g_message("A File|New menu item was selecteda.");
}

void
Actions::on_menu_file_open_project()
{
  g_message("A File|Open menu item was selecteda.");
}

void
Actions::on_menu_file_render()
{
  dialogs::Render dialog(workspaceWindow);
  dialog.run();
}

void
Actions::on_menu_file_quit()
{
  workspaceWindow.hide(); // Closes the main window to stop the Gtk::Main::run().
}

/* ===== Edit Menu Event Handlers ===== */

void
Actions::on_menu_edit_preferences()
{
  dialogs::PreferencesDialog dialog(workspaceWindow);
  dialog.run();
}

/* ===== View Menu Event Handlers ===== */

void
Actions::on_menu_view_assets()
{
  if(!is_updating_action_state)
    workspaceWindow.assets_panel->show(assetsPanelAction->get_active());
}

void
Actions::on_menu_view_timeline()
{
  if(!is_updating_action_state)
    workspaceWindow.timeline_panel->show(timelinePanelAction->get_active());
}

void
Actions::on_menu_view_viewer()
{
  if(!is_updating_action_state)
    workspaceWindow.viewer_panel->show(viewerPanelAction->get_active());
}

void
Actions::on_menu_help_about()
{
  // Configure the about dialog
  AboutDialog dialog;
  
  //dialog.set_program_name(AppTitle);
  dialog.set_version(AppVersion);
  //dialog.set_version(Appconfig::get("version"));
  dialog.set_copyright(AppCopyright);
  dialog.set_website(AppWebsite);
  dialog.set_authors(StringArrayHandle(AppAuthors,
    sizeof(AppAuthors) / sizeof(gchar*),
    OWNERSHIP_NONE));

  dialog.set_transient_for(workspaceWindow);
  
  // Show the about dialog
  dialog.run();
}



//----- Temporary junk
void
Actions::on_menu_others()
{
  g_message("A menu item was selected.");
}

}   // namespace workspace
}   // namespace gui
}   // namespace lumiera

