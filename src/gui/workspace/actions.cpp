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
#include "../dialogs/name-chooser.hpp"

#include "../model/project.hpp"

using namespace Gtk;
using namespace Glib;
using namespace sigc;
using namespace gui;

namespace gui {
namespace workspace {

Actions::Actions(WorkspaceWindow &workspace_window) :
  workspaceWindow(workspace_window),
  is_updating_action_state(false)
{
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
  
  assetsPanelAction = ToggleAction::create("ViewAssets",
    Gtk::StockID("panel_assets"));
  assetsPanelAction->signal_toggled().connect(
    sigc::mem_fun(*this, &Actions::on_menu_view_assets));
  actionGroup->add(assetsPanelAction);
  
  timelinePanelAction = ToggleAction::create("ViewTimeline",
    Gtk::StockID("panel_timeline"));
  timelinePanelAction->signal_toggled().connect(
    sigc::mem_fun(*this, &Actions::on_menu_view_timeline));
  actionGroup->add(timelinePanelAction);

  viewerPanelAction = ToggleAction::create("ViewViewer",
    Gtk::StockID("panel_viewer"));
  viewerPanelAction->signal_toggled().connect(
    sigc::mem_fun(*this, &Actions::on_menu_view_viewer));
  actionGroup->add(viewerPanelAction);
  
  // Sequence Menu
  actionGroup->add(Action::create("SequenceMenu", _("_Sequence")));
  actionGroup->add(Action::create("SequenceAdd", _("_Add...")),
    sigc::mem_fun(*this, &Actions::on_menu_sequence_add));
    
  // Track Menu
  actionGroup->add(Action::create("TrackMenu", _("_Track")));
  actionGroup->add(Action::create("TrackAdd", _("_Add...")),
    sigc::mem_fun(*this, &Actions::on_menu_track_add));

  // Help Menu
  actionGroup->add(Action::create("HelpMenu", _("_Help")) );
  actionGroup->add(Action::create("HelpAbout", Stock::ABOUT),
  sigc::mem_fun(*this, &Actions::on_menu_help_about) );
}

void
Actions::update_action_state()
{
  REQUIRE(workspaceWindow.resourcesPanel != NULL);
  REQUIRE(workspaceWindow.timelinePanel != NULL);
  REQUIRE(workspaceWindow.viewerPanel != NULL); 
  
  is_updating_action_state = true;
  assetsPanelAction->set_active(
    workspaceWindow.resourcesPanel->is_shown());
  timelinePanelAction->set_active(
    workspaceWindow.timelinePanel->is_shown());
  viewerPanelAction->set_active(
    workspaceWindow.viewerPanel->is_shown());
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
    workspaceWindow.resourcesPanel->show(
      assetsPanelAction->get_active());
}

void
Actions::on_menu_view_timeline()
{
  if(!is_updating_action_state)
    workspaceWindow.timelinePanel->show(timelinePanelAction->get_active());
}

void
Actions::on_menu_view_viewer()
{
  if(!is_updating_action_state)
    workspaceWindow.viewerPanel->show(viewerPanelAction->get_active());
}

/* ===== Sequence Menu Event Handlers ===== */

void
Actions::on_menu_sequence_add()
{
  dialogs::NameChooser dialog(workspaceWindow,
    _("Add Sequence"), _("New Sequence"));
  if(dialog.run() == RESPONSE_OK)
    workspaceWindow.get_project().add_new_sequence(dialog.get_name());
}

/* ===== Track Menu Event Handlers ===== */

void
Actions::on_menu_track_add()
{
  g_message("Hello");
}

/* ===== View Menu Event Handlers ===== */

void
Actions::on_menu_help_about()
{
  // Configure the about dialog
  AboutDialog dialog;
  
  //dialog.set_program_name(AppTitle);
  dialog.set_version(GtkLumiera::get_app_version());
  //dialog.set_version(AppState::get("version"));
  dialog.set_copyright(GtkLumiera::get_app_copyright());
  dialog.set_website(GtkLumiera::get_app_website());
  dialog.set_authors(GtkLumiera::get_app_authors());

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

