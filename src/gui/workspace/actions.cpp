/*
  Actions.cpp  -  Definition of the main workspace window object

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

#include "actions.hpp"
#include "workspace-window.hpp"

#include "gui/window-manager.hpp"

#include "gui/dialogs/render.hpp"
#include "gui/dialogs/preferences-dialog.hpp"
#include "gui/dialogs/name-chooser.hpp"

#include "gui/model/project.hpp"

#include "include/logging.h"

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
}

void
Actions::populate_main_actions(Glib::RefPtr<Gtk::UIManager> uiManager)
{
  REQUIRE(uiManager);
  
  //----- Create the Action Group -----//
  actionGroup = ActionGroup::create();
  
  // File menu
  actionGroup->add(Action::create("FileMenu", _("_File")));
  actionGroup->add(Action::create("FileNewProject", Stock::NEW, _("_New Project...")),
    mem_fun(*this, &Actions::on_menu_file_new_project));
  actionGroup->add(Action::create("FileOpenProject", Stock::OPEN, _("_Open Project...")),
    mem_fun(*this, &Actions::on_menu_file_open_project));
  actionGroup->add(Action::create("FileSaveProject", Stock::SAVE, _("_Save Project")),
    mem_fun(*this, &Actions::on_menu_others));
  actionGroup->add(Action::create("FileSaveProjectAs", Stock::SAVE_AS, _("_Save Project As...")),
    mem_fun(*this, &Actions::on_menu_others));
  actionGroup->add(Action::create("FileRender", _("_Render...")),
    AccelKey("<shift>R"),
    mem_fun(*this, &Actions::on_menu_file_render));
  actionGroup->add(Action::create("FileQuit", Stock::QUIT),
    mem_fun(*this, &Actions::on_menu_file_quit));

  // Edit menu
  actionGroup->add(Action::create("EditMenu", _("_Edit")));
  actionGroup->add(Action::create("EditUndo", Stock::UNDO),
    mem_fun(*this, &Actions::on_menu_others));
  actionGroup->add(Action::create("EditRedo", Stock::REDO),
    mem_fun(*this, &Actions::on_menu_others));
  actionGroup->add(Action::create("EditCut", Stock::CUT),
    mem_fun(*this, &Actions::on_menu_others));
  actionGroup->add(Action::create("EditCopy", Stock::COPY),
    mem_fun(*this, &Actions::on_menu_others));
  actionGroup->add(Action::create("EditPaste", Stock::PASTE),
    mem_fun(*this, &Actions::on_menu_others));
  actionGroup->add(Action::create("EditPreferences", Stock::PREFERENCES),
    mem_fun(*this, &Actions::on_menu_edit_preferences));

  // View Menu
  actionGroup->add(Action::create("ViewMenu", _("_View")));
  
  assetsPanelAction = ToggleAction::create("ViewAssets",
    StockID("panel_assets"));
  assetsPanelAction->signal_toggled().connect(
    mem_fun(*this, &Actions::on_menu_view_assets));
  actionGroup->add(assetsPanelAction);
  
  timelinePanelAction = ToggleAction::create("ViewTimeline",
    StockID("panel_timeline"));
  timelinePanelAction->signal_toggled().connect(
    mem_fun(*this, &Actions::on_menu_view_timeline));
  actionGroup->add(timelinePanelAction);

  viewerPanelAction = ToggleAction::create("ViewViewer",
    StockID("panel_viewer"));
  viewerPanelAction->signal_toggled().connect(
    mem_fun(*this, &Actions::on_menu_view_viewer));
  actionGroup->add(viewerPanelAction);
  
  // Sequence Menu
  actionGroup->add(Action::create("SequenceMenu", _("_Sequence")));
  actionGroup->add(Action::create("SequenceAdd", _("_Add...")),
    mem_fun(*this, &Actions::on_menu_sequence_add));
    
  // Track Menu
  actionGroup->add(Action::create("TrackMenu", _("_Track")));
  actionGroup->add(Action::create("TrackAdd", _("_Add...")),
    mem_fun(*this, &Actions::on_menu_track_add));

  // Window Menu
  actionGroup->add(Action::create("WindowMenu", _("_Window")));
  actionGroup->add(Action::create("WindowNewWindow",
    StockID("new_window")),
    mem_fun(*this, &Actions::on_menu_window_new_window));
  actionGroup->add(Action::create("WindowCloseWindow",
    _("Close Window")),
    mem_fun(*this, &Actions::on_menu_window_close_window));
  actionGroup->add(Action::create("WindowShowPanel", _("_Show Panel")));

  // Help Menu
  actionGroup->add(Action::create("HelpMenu", _("_Help")) );
  actionGroup->add(Action::create("HelpAbout", Stock::ABOUT),
    mem_fun(*this, &Actions::on_menu_help_about) );
  
  uiManager->insert_action_group(actionGroup);
  
  //----- Create the UI layout -----//
  uString ui_info = 
      "<ui>"
      "  <menubar name='MenuBar'>"
      "    <menu action='FileMenu'>"
      "      <menuitem action='FileNewProject'/>"
      "      <menuitem action='FileOpenProject'/>"
      "      <menuitem action='FileSaveProject'/>"
      "      <menuitem action='FileSaveProjectAs'/>"
      "      <separator/>"
      "      <menuitem action='FileRender'/>"
      "      <separator/>"
      "      <menuitem action='FileQuit'/>"
      "    </menu>"
      "    <menu action='EditMenu'>"
      "      <menuitem action='EditUndo'/>"
      "      <menuitem action='EditRedo'/>"
      "      <separator/>"
      "      <menuitem action='EditCut'/>"
      "      <menuitem action='EditCopy'/>"
      "      <menuitem action='EditPaste'/>"
      "      <separator/>"
      "      <menuitem action='EditPreferences'/>"
      "    </menu>"
      "    <menu action='ViewMenu'>"
      "      <menuitem action='ViewAssets'/>"
      "      <menuitem action='ViewTimeline'/>"
      "      <menuitem action='ViewViewer'/>"
      "    </menu>"
      "    <menu action='SequenceMenu'>"
      "      <menuitem action='SequenceAdd'/>"
      "    </menu>"
      "    <menu action='TrackMenu'>"
      "      <menuitem action='TrackAdd'/>"
      "    </menu>"
      "    <menu action='WindowMenu'>"
      "      <menuitem action='WindowNewWindow'/>"
      "      <menuitem action='WindowCloseWindow'/>"
      "      <menu action='WindowShowPanel'/>"
      "    </menu>"
      "    <menu action='HelpMenu'>"
      "      <menuitem action='HelpAbout'/>"
      "    </menu>"
      "  </menubar>"
      "  <toolbar  name='ToolBar'>"
      "    <toolitem action='FileNewProject'/>"
      "    <toolitem action='FileOpenProject'/>"
      "    <toolitem action='FileSaveProject'/>"
      "    <separator/>"
      "    <toolitem action='EditUndo'/>"
      "    <toolitem action='EditRedo'/>"
      "    <separator/>"
      "    <toolitem action='EditCut'/>"
      "    <toolitem action='EditCopy'/>"
      "    <toolitem action='EditPaste'/>"
      "  </toolbar>"
      "</ui>";

  try
    {
      uiManager->add_ui_from_string(ui_info);
    }
  catch(const Glib::Error& ex)
    {
      ERROR(gui, "Building menus failed: %s", ex.what().data());
      return;
    }
    
  //----- Add Extra Actions -----//
  populate_show_panel_actions(uiManager);
}

void
Actions::populate_show_panel_actions(Glib::RefPtr<Gtk::UIManager> uiManager)
{  
  const int count = PanelManager::get_panel_description_count();
  
  Glib::RefPtr<Gtk::ActionGroup> actionGroup = ActionGroup::create();
  for(int i = 0; i < count; i++)
    {
      const gchar *stock_id = PanelManager::get_panel_stock_id(i);
      cuString name = ustring::compose("Panel%1", i);
      actionGroup->add(Action::create(name, StockID(stock_id)),
        bind(mem_fun(*this, &Actions::on_menu_show_panel), i));
    }
    
  uiManager->insert_action_group(actionGroup);
  
  for(int i = 0; i < count; i++)
    {
      cuString name = ustring::compose("Panel%1", i);
      uiManager->add_ui(uiManager->new_merge_id(),
        "/MenuBar/WindowMenu/WindowShowPanel", name, name);
    }
}

void
Actions::update_action_state()
{
  /*REQUIRE(workspaceWindow.assetsPanel != NULL);
  REQUIRE(workspaceWindow.timelinePanel != NULL);
  REQUIRE(workspaceWindow.viewerPanel != NULL); 
  
  is_updating_action_state = true;
  assetsPanelAction->set_active(
    workspaceWindow.assetsPanel->is_shown());
  timelinePanelAction->set_active(
    workspaceWindow.timelinePanel->is_shown());
  viewerPanelAction->set_active(
    workspaceWindow.viewerPanel->is_shown());
  is_updating_action_state = false;*/
}

/* ===== File Menu Event Handlers ===== */

void
Actions::on_menu_file_new_project()
{
  g_message("A File|New menu item was selected.");
}

void
Actions::on_menu_file_open_project()
{
  g_message("A File|Open menu item was selected.");
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
  Main *main = Main::instance();
  REQUIRE(main);
  main->quit();
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
  //if(!is_updating_action_state)
  //  workspaceWindow.assetsPanel->show(
  //    assetsPanelAction->get_active());
}

void
Actions::on_menu_view_timeline()
{
  //if(!is_updating_action_state)
  //  workspaceWindow.timelinePanel->show(timelinePanelAction->get_active());
}

void
Actions::on_menu_view_viewer()
{
  //if(!is_updating_action_state)
  //  workspaceWindow.viewerPanel->show(viewerPanelAction->get_active());
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

/* ===== Window Menu Event Handlers ===== */

void
Actions::on_menu_window_new_window()
{
  GtkLumiera::application().windowManager().newWindow (
    workspaceWindow.get_project(),
    workspaceWindow.get_controller()); 
}

void
Actions::on_menu_window_close_window()
{
  workspaceWindow.hide();
  // delete &workspaceWindow;
}

void
Actions::on_menu_show_panel(int panel_index)
{
  workspaceWindow.get_panel_manager().show_panel(panel_index);
}

/* ===== Help Menu Event Handlers ===== */

void
Actions::on_menu_help_about()
{
  // Configure the about dialog
  AboutDialog dialog;
  
  //dialog.set_program_name(AppTitle);
  dialog.set_version(GtkLumiera::getAppVersion());
  //dialog.set_version(AppState::get("version"));
  dialog.set_copyright(GtkLumiera::getCopyright());
  dialog.set_website(GtkLumiera::getLumieraWebsite());
  dialog.set_authors(GtkLumiera::getLumieraAuthors());

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

