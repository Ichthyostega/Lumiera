/*
  Actions  -  Definition of a helper class for user actions / menu

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


#include "gui/workspace/actions.hpp"
#include "gui/config-keys.hpp"
#include "gui/workspace/workspace-window.hpp"

#include "gui/workspace/ui-manager.hpp"

#include "gui/dialog/render.hpp"
#include "gui/dialog/preferences-dialog.hpp"
#include "gui/dialog/name-chooser.hpp"

#include "gui/model/project.hpp"

#include "lib/format-string.hpp"

#include "include/logging.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <vector>



namespace gui {
namespace workspace {
  
  using boost::algorithm::is_any_of;
  using boost::algorithm::split;
  using Gtk::Action;
  using Gtk::ActionGroup;
  using Gtk::ToggleAction;
  using Gtk::AboutDialog;
  using Gtk::AccelKey;
  using Gtk::StockID;
  using Gtk::Main;       /////////////////////////////////////////////////////////////////////////////////////TICKET #1032 replace Main -> Application
  using sigc::mem_fun;
  using Glib::ustring;
  using ::util::_Fmt;
  using std::vector;
  
  namespace Stock = Gtk::Stock;
  
  
  Actions::Actions (GetWindow how_to_access_current_window)
    : getWorkspaceWindow(how_to_access_current_window)
    , is_updating_action_state(false)
    {
    }
  
  
  void
  Actions::populateMainActions (Gtk::UIManager& uiManager)
  {
    //----- Create the Action Group -----//
    actionGroup = ActionGroup::create();
    
    // File menu
    actionGroup->add(Action::create("FileMenu", _("_File")));
    actionGroup->add(Action::create("FileNewProject",   Stock::NEW, _("_New Project...")),
      mem_fun(*this, &Actions::onMenu_file_new_project));
    actionGroup->add(Action::create("FileOpenProject",  Stock::OPEN, _("_Open Project...")),
      mem_fun(*this, &Actions::onMenu_file_open_project));
    actionGroup->add(Action::create("FileSaveProject",  Stock::SAVE, _("_Save Project")),
      mem_fun(*this, &Actions::onMenu_others));
    actionGroup->add(Action::create("FileSaveProjectAs",Stock::SAVE_AS, _("_Save Project As...")),
      mem_fun(*this, &Actions::onMenu_others));
    actionGroup->add(Action::create("FileRender", _("_Render...")),
      AccelKey("<shift>R"),
      mem_fun(*this, &Actions::onMenu_file_render));
    actionGroup->add(Action::create("FileQuit", Stock::QUIT),
      mem_fun(*this, &Actions::onMenu_file_quit));
    
    // Edit menu
    actionGroup->add(Action::create("EditMenu", _("_Edit")));
    actionGroup->add(Action::create("EditUndo", Stock::UNDO),
      mem_fun(*this, &Actions::onMenu_others));
    actionGroup->add(Action::create("EditRedo", Stock::REDO),
      mem_fun(*this, &Actions::onMenu_others));
    actionGroup->add(Action::create("EditCut",  Stock::CUT),
      mem_fun(*this, &Actions::onMenu_others));
    actionGroup->add(Action::create("EditCopy", Stock::COPY),
      mem_fun(*this, &Actions::onMenu_others));
    actionGroup->add(Action::create("EditPaste",Stock::PASTE),
      mem_fun(*this, &Actions::onMenu_others));
    actionGroup->add(Action::create("EditPreferences", Stock::PREFERENCES),
      mem_fun(*this, &Actions::onMenu_edit_preferences));
    
    // View Menu
    actionGroup->add(Action::create("ViewMenu", _("_View")));
    
    assetsPanelAction = ToggleAction::create("ViewAssets",
      StockID("panel_assets"));
    assetsPanelAction->signal_toggled().connect(
      mem_fun(*this, &Actions::onMenu_view_assets));
    actionGroup->add(assetsPanelAction);
    
    timelinePanelAction = ToggleAction::create("ViewTimeline",
      StockID("panel_timeline"));
    timelinePanelAction->signal_toggled().connect(
      mem_fun(*this, &Actions::onMenu_view_timeline));
    actionGroup->add(timelinePanelAction);
    
    viewerPanelAction = ToggleAction::create("ViewViewer",
      StockID("panel_viewer"));
    viewerPanelAction->signal_toggled().connect(
      mem_fun(*this, &Actions::onMenu_view_viewer));
    actionGroup->add(viewerPanelAction);
    
    // Sequence Menu
    actionGroup->add(Action::create("SequenceMenu", _("_Sequence")));
    actionGroup->add(Action::create("SequenceAdd", _("_Add...")),
      mem_fun(*this, &Actions::onMenu_sequence_add));
    
    // Track Menu
    actionGroup->add(Action::create("TrackMenu", _("_Track")));
    actionGroup->add(Action::create("TrackAdd", _("_Add...")),
      mem_fun(*this, &Actions::onMenu_track_add));
    
    // Window Menu
    actionGroup->add(Action::create("WindowMenu", _("_Window")));
    actionGroup->add(Action::create("WindowNewWindow",
      StockID("new_window")),
      mem_fun(*this, &Actions::onMenu_window_new_window));
    actionGroup->add(Action::create("WindowCloseWindow",
      _("Close Window")),
      mem_fun(*this, &Actions::onMenu_window_close_window));
    actionGroup->add(Action::create("WindowShowPanel", _("_Show Panel")));
    
    // Help Menu
    actionGroup->add(Action::create("HelpMenu", _("_Help")) );
    actionGroup->add(Action::create("HelpAbout", Stock::ABOUT),
      mem_fun(*this, &Actions::onMenu_help_about) );
    
    uiManager.insert_action_group(actionGroup);
    
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
        uiManager.add_ui_from_string (ui_info);
      }
    catch(const Glib::Error& ex)
      {
        ERROR (gui, "Building menus failed: %s", ex.what().data());
        return;
      }
      
    //----- Add Extra Actions -----//
    populateShowPanelActions (uiManager);
  }
  
  
  
  void
  Actions::populateShowPanelActions (Gtk::UIManager& uiManager)
  {  
    const uint count = PanelManager::getPanelDescriptionCount();
    
    Glib::RefPtr<Gtk::ActionGroup> actionGroup = ActionGroup::create();
    for (uint i = 0; i < count; i++)
      {
        const gchar *stock_id = PanelManager::getPanelStockID(i);
        cuString name = ustring::compose("Panel%1", i);
        actionGroup->add(Action::create(name, StockID(stock_id)),
          bind(mem_fun(*this, &Actions::onMenu_show_panel), i));
      }
      
    uiManager.insert_action_group (actionGroup);
    
    for (uint i = 0; i < count; i++)
      {
        cuString name = ustring::compose("Panel%1", i);
        uiManager.add_ui (uiManager.new_merge_id(),
                          "/MenuBar/WindowMenu/WindowShowPanel",
                          name, name);
      }
  }
  
  
  void
  Actions::updateActionState()
  {
       ///////////////////////////////////////////////////////////////////////////////////////TICKET #1076  find out how to handle this properly
    /*
    WorkspaceWindow& currentWindow = getWorkspaceWindow();
    
    REQUIRE(currentWindow.assetsPanel != NULL);
    REQUIRE(currentWindow.timelinePanel != NULL);
    REQUIRE(currentWindow.viewerPanel != NULL); 
    
    is_updating_action_state = true;
    assetsPanelAction->set_active  (currentWindow.assetsPanel->is_shown());
    timelinePanelAction->set_active(currentWindow.timelinePanel->is_shown());
    viewerPanelAction->set_active  (currentWindow.viewerPanel->is_shown());
    is_updating_action_state = false;
    */
  }
  
  
  /* ===== File Menu Event Handlers ===== */
  
  void
  Actions::onMenu_file_new_project()
  {
    g_message("A File|New menu item was selected.");
  }
  
  
  void
  Actions::onMenu_file_open_project()
  {
    g_message("A File|Open menu item was selected.");
  }
  
  
  void
  Actions::onMenu_file_render()
  {
    dialog::Render dialog(getWorkspaceWindow());
    dialog.run();
  }
  
  
  void
  Actions::onMenu_file_quit()
  {
    Main *main = Main::instance();
    REQUIRE(main);
    main->quit();
  }
  
  
  /* ===== Edit Menu Event Handlers ===== */
  
  void
  Actions::onMenu_edit_preferences()
  {
    dialog::PreferencesDialog dialog(getWorkspaceWindow());
    dialog.run();
  }
  
  
  /* ===== View Menu Event Handlers ===== */
  
  void
  Actions::onMenu_view_assets()
  {
    /////////////////////////////////////////////////////////////////////////////////////TODO defunct since GTK-3 transition
    //if(!is_updating_action_state)
    //  workspaceWindow.assetsPanel->show(
    //    assetsPanelAction->get_active());
  }
  
  
  void
  Actions::onMenu_view_timeline()
  {
    /////////////////////////////////////////////////////////////////////////////////////TODO defunct since GTK-3 transition
    //if(!is_updating_action_state)
    //  workspaceWindow.timelinePanel->show(timelinePanActionselAction->get_active());
  }
  
  
  void
  Actions::onMenu_view_viewer()
  {
    /////////////////////////////////////////////////////////////////////////////////////TODO defunct since GTK-3 transition
    //if(!is_updating_action_state)
    //  workspaceWindow.viewerPanel->show(viewerPanelAction->get_active());
  }
  
  
  /* ===== Sequence Menu Event Handlers ===== */
  
  void
  Actions::onMenu_sequence_add()
  {
    dialog::NameChooser dialog(getWorkspaceWindow(),
      _("Add Sequence"), _("New Sequence"));
/////////////////////////////////////////////////////////////////////////////////////////TICKET #1070 need a way how to issue session commands    
//  if(dialog.run() == RESPONSE_OK)
//    workspaceWindow().getProject().add_new_sequence(dialog.getName());
  }
  
  
  /* ===== Track Menu Event Handlers ===== */
  
  void
  Actions::onMenu_track_add()
  {
    g_message("Hello");
  }
  
  
  /* ===== Window Menu Event Handlers ===== */
  
  void
  Actions::onMenu_window_new_window()
  {
//  windowList_.newWindow();   //////////////////////////////////TODO move into UiManager 
  }
  
  
  void
  Actions::onMenu_window_close_window()
  {
    getWorkspaceWindow().hide();
    // delete &workspaceWindow;
  }
  
  
  void
  Actions::onMenu_show_panel(int panel_index)
  {
    getWorkspaceWindow().getPanelManager().showPanel(panel_index);
  }
  
  
  /* ===== Help Menu Event Handlers ===== */
  
  void
  Actions::onMenu_help_about()
  {
    // Configure the about dialog
    AboutDialog dialog;
    
    cuString copyrightNotice {_Fmt(_("© %s the original Authors\n"
                                     "-- Lumiera Team --\n"
                                     "Lumiera is Free Software (GPL)"))
                                     % Config::get (KEY_COPYRIGHT)};
    
    string authors = Config::get (KEY_AUTHORS);
    vector<uString> authorsList;
    split (authorsList, authors, is_any_of (",|"));
    
    
    dialog.set_program_name(Config::get (KEY_TITLE));
    dialog.set_version(Config::get (KEY_VERSION));
    dialog.set_copyright(copyrightNotice);
    dialog.set_website(Config::get (KEY_WEBSITE));
    dialog.set_authors(authorsList);
    
    dialog.set_transient_for(getWorkspaceWindow());
    
    // Show the about dialog
    dialog.run();
  }
  
  
  
  //----- Temporary junk
  void
  Actions::onMenu_others()                                      /////////////////////////TICKET #1070 need a concept how to bind global actions
  {
    g_message("A menu item was selected.");
  }
  
  
}}// namespace gui::workspace
