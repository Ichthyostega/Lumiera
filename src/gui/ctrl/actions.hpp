/*
  ACTIONS.hpp  -  Definition of a helper class for user actions / menu

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

*/


/** @file actions.hpp
 ** Setup of global actions for the main menu.
 ** The Actions helper defines the structure and contents of the workspace window main menu.
 ** The individual menu options are bound to functors (closures), which use a _global UI context_
 ** to access the target objects or invoke the signals.
 ** 
 ** @see ui-manager.hpp
 ** @see gtk-lumiera.cpp
 */


#ifndef GUI_CTRL_ACTIONS_H
#define GUI_CTRL_ACTIONS_H

#include "gui/gtk-lumiera.hpp"
#include "gui/config-keys.hpp"
#include "gui/ctrl/global-ctx.hpp"
#include "gui/workspace/workspace-window.hpp"
#include "gui/dialog/render.hpp"
#include "gui/dialog/preferences-dialog.hpp"
#include "gui/dialog/name-chooser.hpp"
#include "lib/format-string.hpp"
#include "include/logging.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/noncopyable.hpp>
#include <functional>
#include <vector>
#include <string>


namespace gui {
namespace ctrl {
  namespace error = lumiera::error;
  
  using std::function;
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
  using std::string;
  
  namespace Stock = Gtk::Stock;
  
  
  
  /**
  * A helper class which registers and handles
  * user action events.
  */
  class Actions
    : boost::noncopyable
    {
      GlobalCtx& globalCtx_;
      
    public:
      Actions (GlobalCtx& globals)
        : globalCtx_{globals}
        , is_updating_action_state(false)
        { }
      
      
      /**
       * Populates the uiManager with the main set of global actions.
       * @remarks we define the menu bindings with the help of stock language lambdas.
       *          This does not make the resulting functors `sigc::trackable`, yet this
       *          is not necessary either, since Actions, together with all the other
       *          top level UI backbone entities, is created and maintained by GtkLumiera,
       *          and thus ensured to exist as long as the GTK event loop is running.
       */
      void
      populateMainActions (Gtk::UIManager& uiManager)
        {
          //----- Create the Action Group -----//
          actionGroup = ActionGroup::create();                     ////////////////////////////////////////////TICKET #1068 not clear if it is necessary to give a *name* to that action group
          
          // File menu
          actionGroup->add(Action::create("FileMenu", _("_File")));
          actionGroup->add(Action::create("FileNewProject", Stock::NEW, _("_New Project...")),    [&]() { onMenu_file_newProject(); });
          actionGroup->add(Action::create("FileSave",   Stock::SAVE,    _("_Save Project")),      [&]() { onMenu_file_save();   });
          actionGroup->add(Action::create("FileSaveAs", Stock::SAVE_AS, _("_Save Project As...")),[&]() { onMenu_file_saveAs(); });
          actionGroup->add(Action::create("FileOpen",   Stock::OPEN,    _("_Open...")),           [&]() { onMenu_file_open();   });
          actionGroup->add(Action::create("FileRender", _("_Render...")), AccelKey("<shift>R"),   [&]() { onMenu_file_render(); });
          actionGroup->add(Action::create("FileQuit", Stock::QUIT),                               [&]() { onMenu_file_quit();   });
          
          // Edit menu
          actionGroup->add(Action::create("EditMenu", _("_Edit")));
          actionGroup->add(Action::create("EditUndo", Stock::UNDO),                  [&]() { onMenu_others(); });
          actionGroup->add(Action::create("EditRedo", Stock::REDO),                  [&]() { onMenu_others(); });
          actionGroup->add(Action::create("EditCut",  Stock::CUT),                   [&]() { onMenu_others(); });
          actionGroup->add(Action::create("EditCopy", Stock::COPY),                  [&]() { onMenu_others(); });
          actionGroup->add(Action::create("EditPaste",Stock::PASTE),                 [&]() { onMenu_others(); });
          actionGroup->add(Action::create("EditPreferences", Stock::PREFERENCES),    [&]() { onMenu_edit_preferences(); });
          
          // View Menu
          actionGroup->add(Action::create("ViewMenu", _("_View")));
          
          assetsPanelAction = ToggleAction::create("ViewAssets", StockID("panel_assets"));
          assetsPanelAction->signal_toggled().connect (                              [&]() { onMenu_view_assets(); });
          actionGroup->add(assetsPanelAction);
          
          timelinePanelAction = ToggleAction::create("ViewTimeline", StockID("panel_timeline"));
          timelinePanelAction->signal_toggled().connect(                             [&]() { onMenu_view_timeline(); });
          actionGroup->add(timelinePanelAction);
          
          viewerPanelAction = ToggleAction::create("ViewViewer", StockID("panel_viewer"));
          viewerPanelAction->signal_toggled().connect(                               [&]() { onMenu_view_viewer(); });
          actionGroup->add(viewerPanelAction);
          
          // Sequence Menu
          actionGroup->add(Action::create("SequenceMenu", _("_Sequence")));
          actionGroup->add(Action::create("SequenceAdd", _("_Add...")),              [&]() { onMenu_sequence_add(); });
          
          // Track Menu
          actionGroup->add(Action::create("TrackMenu", _("_Track")));
          actionGroup->add(Action::create("TrackAdd", _("_Add...")),                 [&]() { onMenu_track_add(); });
          
          // Window Menu
          actionGroup->add(Action::create("WindowMenu", _("_Window")));
          actionGroup->add(Action::create("WindowNewWindow", StockID("new_window")), [&]() { onMenu_window_new_window(); });
          actionGroup->add(Action::create("WindowCloseWindow", _("Close Window")),   [&]() { onMenu_window_close_window(); });
          actionGroup->add(Action::create("WindowShowPanel", _("_Show Panel")));
          
          // Help Menu
          actionGroup->add(Action::create("HelpMenu", _("_Help")) );
          actionGroup->add(Action::create("HelpAbout", Stock::ABOUT),                [&]() { onMenu_help_about(); });
          
          uiManager.insert_action_group(actionGroup);
          
          //----- Create the UI layout -----//
          string ui_info = R"***(
              <ui>
                <menubar name='MenuBar'>
                  <menu action='FileMenu'>
                    <menuitem action='FileNewProject'/>
                    <menuitem action='FileSave'/>
                    <menuitem action='FileSaveAs'/>
                    <menuitem action='FileOpen'/>
                    <separator/>
                    <menuitem action='FileRender'/>
                    <separator/>
                    <menuitem action='FileQuit'/>
                  </menu>
                  <menu action='EditMenu'>
                    <menuitem action='EditUndo'/>
                    <menuitem action='EditRedo'/>
                    <separator/>
                    <menuitem action='EditCut'/>
                    <menuitem action='EditCopy'/>
                    <menuitem action='EditPaste'/>
                    <separator/>
                    <menuitem action='EditPreferences'/>
                  </menu>
                  <menu action='ViewMenu'>
                    <menuitem action='ViewAssets'/>
                    <menuitem action='ViewTimeline'/>
                    <menuitem action='ViewViewer'/>
                  </menu>
                  <menu action='SequenceMenu'>
                    <menuitem action='SequenceAdd'/>
                  </menu>
                  <menu action='TrackMenu'>
                    <menuitem action='TrackAdd'/>
                  </menu>
                  <menu action='WindowMenu'>
                    <menuitem action='WindowNewWindow'/>
                    <menuitem action='WindowCloseWindow'/>
                    <menu action='WindowShowPanel'/>
                  </menu>
                  <menu action='HelpMenu'>
                    <menuitem action='HelpAbout'/>
                  </menu>
                </menubar>
                <toolbar  name='ToolBar'>
                  <toolitem action='FileNewProject'/>
                  <toolitem action='FileOpen'/>
                  <toolitem action='FileSave'/>
                  <separator/>
                  <toolitem action='EditUndo'/>
                  <toolitem action='EditRedo'/>
                  <separator/>
                  <toolitem action='EditCut'/>
                  <toolitem action='EditCopy'/>
                  <toolitem action='EditPaste'/>
                </toolbar>
              </ui>
          )***";
          try
            {
              uiManager.add_ui_from_string (ui_info);
            }
          catch(Glib::Error& ex)
            {
              ERROR (gui, "Building menus failed: %s", ex.what().data());
              throw error::Config(_Fmt("global menu definition rejected: %s") % ex.what());
            }
          
          //----- Add Extra Actions -----//
          populateShowPanelActions (uiManager);
        }
      
      
      
    private: /* ===== Internals ===== */
      workspace::WorkspaceWindow&
      getWorkspaceWindow()
        {
          return globalCtx_.windowList_.findActiveWindow();
        }
      
      
      /**
       * Populates a uiManager with actions for the Show Panel menu.
       */
      void
      populateShowPanelActions (Gtk::UIManager& uiManager)
        {
          const uint count = workspace::PanelManager::getPanelDescriptionCount();
          
          Glib::RefPtr<Gtk::ActionGroup> actionGroup = ActionGroup::create();          ////////////////////////TICKET #1068 not clear if it is necessary to give a *name* to that action group
          for (uint i = 0; i < count; i++)
            {
              const gchar *stock_id = workspace::PanelManager::getPanelStockID(i);
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
      
      
      
      /**
       * Updates the state of the menu/toolbar actions
       * to reflect the current state of the workspace */
      void
      updateActionState()
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
      
      
      
      
      
    private: /* ====== Actions =========== */
      
      /* ============ File Menu ========== */
      
      void
      onMenu_file_newProject()
        {
          globalCtx_.director_.newProject();
        }
      
      void
      onMenu_file_save()
        {
          globalCtx_.director_.saveSnapshot();
        }
      
      void
      onMenu_file_saveAs()
        {
          globalCtx_.director_.forkProject();
        }
      
      void
      onMenu_file_open()
        {
          globalCtx_.director_.openFile();
        }
      
      void
      onMenu_file_render()
        {
          dialog::Render dialog(getWorkspaceWindow());                                   //////global -> InteractionDirector
          dialog.run();
        }
      
      void
      onMenu_file_quit()
        {
          globalCtx_.uiManager_.terminateUI();
        }
      
      
      
      /* ============ Edit Menu ========== */
      
      void
      onMenu_edit_preferences()
        {
          dialog::PreferencesDialog dialog(getWorkspaceWindow());                        //////global -> InteractionDirector
          dialog.run();
        }
      
      
      
      /* ============ View Menu ========== */
      
      void
      onMenu_view_assets()
        {
          /////////////////////////////////////////////////////////////////////////////////////TODO defunct since GTK-3 transition
          //if(!is_updating_action_state)
          //  workspaceWindow.assetsPanel->show(
          //    assetsPanelAction->get_active());                                        //////global -> InteractionDirector
        }
      
      void
      onMenu_view_timeline()
        {
          /////////////////////////////////////////////////////////////////////////////////////TODO defunct since GTK-3 transition
          //if(!is_updating_action_state)
          //  workspaceWindow.timelinePanel->show(timelinePanActionselAction->get_active());
        }
      
      void
      onMenu_view_viewer()
        {
          /////////////////////////////////////////////////////////////////////////////////////TODO defunct since GTK-3 transition
          //if(!is_updating_action_state)
          //  workspaceWindow.viewerPanel->show(viewerPanelAction->get_active());        //////global -> InteractionDirector
        }
      
      
      
      /* ============ Sequence Menu ====== */
      
      void
      onMenu_sequence_add()
        {
          ///////////////////////////////////////////////////////TODO this is bad user interaction design. Just create the sequence! The user is free to change the default name afterwards
          dialog::NameChooser dialog(getWorkspaceWindow(),
            _("Add Sequence"), _("New Sequence"));
///////////////////////////////////////////////////////////////////////////////////////////////TICKET #1070 need a way how to issue session commands    
//        if(dialog.run() == RESPONSE_OK)
//          workspaceWindow().getProject().add_new_sequence(dialog.getName());           //////global -> InteractionDirector
        }
      
      
      
      /* ============ Track Menu ========= */
      
      void
      onMenu_track_add()
        {
          g_message("Hello");                                                            //////TODO Problem is how to find the context!!!!
        }
      
      
      
      /* ============ Window Menu ======== */
      
      void
      onMenu_window_new_window()
        {
          globalCtx_.windowList_.newWindow();
        }
      
      void
      onMenu_window_close_window()
        {
          globalCtx_.windowList_.closeWindow();
        }
      
      void
      onMenu_show_panel(int panel_index)
        {
          getWorkspaceWindow().getPanelManager().showPanel (panel_index);
        }
      
      
      
      /* ============ Help Menu ========== */
      
      void
      onMenu_help_about()
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
      
      
      
      // Temporary Junk
      void
      onMenu_others()                                      /////////////////////////TICKET #1070 need a concept how to bind global actions
        {
          g_message("A menu item was selected.");
        }
      
      
      
    private: /* ===== Actions ===== */
      
      Glib::RefPtr<Gtk::ActionGroup> actionGroup;
      
      Glib::RefPtr<Gtk::ToggleAction> assetsPanelAction;
      Glib::RefPtr<Gtk::ToggleAction> timelinePanelAction;
      Glib::RefPtr<Gtk::ToggleAction> viewerPanelAction;
      
      
    private: /* ===== Internals ===== */
      bool is_updating_action_state;
    };
  
  
}}// namespace gui::ctrl
#endif /*GUI_CTRL_ACTIONS_H*/
