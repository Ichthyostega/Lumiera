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
 ** @todo the old `Gtk::Action` and `Gtk::ActionGroup` are planned to be deprecated.
 **       Recommendation is to rely on `Gtk::Builder` and `Gtk::SimpleAction` instead.
 **       As of 5/2017, it is not clear to what extent this might force us into additional
 **       "desktop integration" we do not need nor want (like automatically connecting to
 **       D-Bus). For that reason, we'll postpone this transition for the time being    /////////////////////TICKET #1068 
 ** 
 ** @see ui-manager.hpp
 ** @see gtk-lumiera.cpp
 */


#ifndef STAGE_CTRL_ACTIONS_H
#define STAGE_CTRL_ACTIONS_H

#include "stage/gtk-base.hpp"
#include "stage/ctrl/global-ctx.hpp"
#include "stage/ctrl/window-locator.hpp"
#include "stage/workspace/workspace-window.hpp"
#include "stage/workspace/panel-manager.hpp"
#include "lib/format-string.hpp"
#include "lib/nocopy.hpp"

#include <string>


namespace stage {
namespace ctrl {
  namespace error = lumiera::error;
  
  using Gtk::Action;
  using Gtk::ActionGroup;
  using Gtk::ToggleAction;
  using Gtk::AccelKey;
  using Gtk::StockID;
  using Glib::ustring;
  using ::util::_Fmt;
  using std::string;
  
  namespace Stock = Gtk::Stock;
  
  
  
  /**
  * A helper class which registers global user actions
  * and populates the main menu and toolbar.
  */
  class Actions
    : util::NonCopyable
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
          auto menu  = [&](auto id, auto menuName)     {actionGroup->add (Action::create(id, menuName)); };
          auto entry = [&](auto closure, auto ...args) {actionGroup->add (Action::create(args...), closure);};
          
          
          menu("FileMenu", _("_File"));
          entry ([&]() { globalCtx_.director_.newProject();   } , "FileNewProject",  Stock::NEW,     _("_New Project..."));
          entry ([&]() { globalCtx_.director_.saveSnapshot(); } , "FileSave",        Stock::SAVE,    _("_Save Project"));
          entry ([&]() { globalCtx_.director_.forkProject();  } , "FileSaveAs",      Stock::SAVE_AS, _("_Save Project As..."));
          entry ([&]() { globalCtx_.director_.openFile();     } , "FileOpen",        Stock::OPEN,    _("_Open..."));
          entry ([&]() { globalCtx_.director_.render();       } , "FileRender",                      _("_Render...")),  AccelKey("<shift>R");
          entry ([&]() { globalCtx_.uiManager_.terminateUI(); } , "FileQuit",        Stock::QUIT);
          
          
          menu("EditMenu", _("_Edit"));
          entry ([&]() { unimplemented ("Edit/Undo");         } , "EditUndo",        Stock::UNDO);
          entry ([&]() { unimplemented ("Edit/Redo");         } , "EditRedo",        Stock::REDO);
          entry ([&]() { unimplemented ("Edit/Cut");          } , "EditCut",         Stock::CUT);
          entry ([&]() { unimplemented ("Edit/Copy");         } , "EditCopy",        Stock::COPY);
          entry ([&]() { unimplemented ("Edit/Pasta");        } , "EditPaste",       Stock::PASTE);
          entry ([&]() { globalCtx_.director_.editSetup();    } , "EditPreferences", Stock::PREFERENCES);
          
          
          menu("SequenceMenu", _("_Sequence"));
          entry ([&]() { globalCtx_.director_.newSequence();  } , "SequenceAdd", _("_Add..."));
          
          
          menu("TrackMenu", _("_Track"));
          entry ([&]() { globalCtx_.director_.newTrack();     } , "TrackAdd", _("_Add..."));
          
          
          menu("HelpMenu", _("_Help"));
          entry ([&]() { globalCtx_.wizard_.show_HelpAbout(); } , "HelpAbout", Stock::ABOUT);
          entry ([&]() { globalCtx_.wizard_.launchTestCtrl(); } , "HelpTest", _("Self _Tests..."));
          
          
          menu("WindowMenu", _("_Window"));
          entry ([&]() { globalCtx_.windowLoc_.newWindow(); }   , "WindowNewWindow", StockID("new_window"));
          entry ([&]() { globalCtx_.windowLoc_.closeWindow();}  , "WindowCloseWindow", _("Close Window"));
          actionGroup->add(Action::create("WindowShowPanel", _("_Show Panel")));

          
          menu("ViewMenu", _("_View"));
          assetsPanelAction = ToggleAction::create("ViewAssets", StockID("panel_assets"));
          assetsPanelAction->signal_toggled().connect (                              [&]() { onMenu_view_assets(); });
          actionGroup->add(assetsPanelAction);
          
          infoboxPanelAction = ToggleAction::create("ViewInfoBox", StockID("panel_infobox"));
          infoboxPanelAction->signal_toggled().connect (                             [&]() { onMenu_view_infobox(); });
          actionGroup->add(infoboxPanelAction);
          
          timelinePanelAction = ToggleAction::create("ViewTimeline", StockID("panel_timeline"));
          timelinePanelAction->signal_toggled().connect(                             [&]() { onMenu_view_timeline(); });
          actionGroup->add(timelinePanelAction);
          
          viewerPanelAction = ToggleAction::create("ViewViewer", StockID("panel_viewer"));
          viewerPanelAction->signal_toggled().connect(                               [&]() { onMenu_view_viewer(); });
          actionGroup->add(viewerPanelAction);
          
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
                    <menuitem action='ViewInfoBox'/>
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
                    <menuitem action='HelpTest'/>
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
              ERROR (stage, "Building menus failed: %s", ex.what().data());
              throw error::Config(_Fmt("global menu definition rejected: %s") % ex.what());
            }
          
          //----- Add Extra Actions -----//
          populateShowPanelActions (uiManager);
        }
      
      
      
      /**
       * Updates the state of the menu/toolbar actions
       * to reflect the current state of the workspace
       * @note the actual functionality was disabled by Joel with changeset 6151415 (from 4.4.2009),
       *       which introduced a PanelManager. This whole concept was seemingly never finished.
       * @todo as of 2017, the relation of panels to top level windows is unclear and likely
       *       needs to be replaced by a focus + perspective concept (--> see topic "Interaction Control")
       */
      void
      updateActionState (workspace::WorkspaceWindow& currentWindow)
        {
             ///////////////////////////////////////////////////////////////////////////////////////TICKET #1076  find out how to handle this properly
             ///////////////////////////////////////////////////////////////////////////////////////TICKET #1097  clarify the role and behaviour of Panels
          /* unfinished, disabled by Joel with changeset 6151415 (from 4.4.2009)
           * 
          REQUIRE(currentWindow.assetsPanel != NULL);
          REQUIRE(currentWindow.timelinePanel != NULL);
          REQUIRE(currentWindow.viewerPanel != NULL);
          
//        is_updating_action_state = true;
          assetsPanelAction->set_active  (currentWindow.assetsPanel->is_shown());
          timelinePanelAction->set_active(currentWindow.timelinePanel->is_shown());
          viewerPanelAction->set_active  (currentWindow.viewerPanel->is_shown());
//        is_updating_action_state = false;
          */
        }
      
      
      
    private: /* ===== Internals ===== */
      
      
      /**
       * Populates the menu entries to show specific panels
       * within the current window.
       * @todo 4/2017 this can only be a preliminary solution.
       *       What we actually want is perspectives, and we want
       *       a specific UI to be anchored somewhere in UI space,
       *       so we'll be rather be navigating _towards it_, instead
       *       of "showing it here".
       */
      void
      populateShowPanelActions (Gtk::UIManager& uiManager)
        {
          const uint count = workspace::PanelManager::getPanelDescriptionCount();
          
          Glib::RefPtr<Gtk::ActionGroup> actionGroup = ActionGroup::create();          ////////////////////////TICKET #1068 not clear if it is necessary to give a *name* to that action group
          for (uint i = 0; i < count; i++)
            {
              const gchar *stock_id = workspace::PanelManager::getPanelStockID(i);
              cuString panelName = ustring::compose("Panel%1", i);
              actionGroup->add(Action::create(panelName, StockID(stock_id)),
                               [i,this]() {
                                            globalCtx_.windowLoc_.findActiveWindow()
                                                                 .getPanelManager()
                                                                 .showPanel (i);
                                          });
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
      
      
      
      
      /* ============ View Actions ========== */
      
      void
      onMenu_view_assets()
        {
          /////////////////////////////////////////////////////////////////////////////////////TODO defunct since GTK-3 transition
          //if(!is_updating_action_state)
          //  workspaceWindow.assetsPanel->show(
          //    assetsPanelAction->get_active());                                        //////global -> InteractionDirector
          unimplemented ("view assets");
        }
      
      void
      onMenu_view_infobox()
        {
          /////////////////////////////////////////////////////////////////////////////////////TODO defunct since GTK-3 transition
          //if(!is_updating_action_state)
          //  workspaceWindow.infoboxPanel->show(
          //    infoboxPanelAction->get_active());                                        //////global -> InteractionDirector
          unimplemented ("view infobox");
        }
      
      void
      onMenu_view_timeline()
        {
          /////////////////////////////////////////////////////////////////////////////////////TODO defunct since GTK-3 transition
          //if(!is_updating_action_state)
          //  workspaceWindow.timelinePanel->show(timelinePanActionselAction->get_active());
          unimplemented ("view timeline");
        }
      
      void
      onMenu_view_viewer()
        {
          /////////////////////////////////////////////////////////////////////////////////////TODO defunct since GTK-3 transition
          //if(!is_updating_action_state)
          //  workspaceWindow.viewerPanel->show(viewerPanelAction->get_active());        //////global -> InteractionDirector
          unimplemented ("view viewer");
        }
      
      
      // Temporary Junk
      void
      unimplemented (const char* todo)
        {
          WARN (stage, "%s is not yet implemented. So sorry.", todo);
        }
      
      
      
    private: /* ===== Actions ===== */
      
      Glib::RefPtr<Gtk::ActionGroup> actionGroup;
      
      Glib::RefPtr<Gtk::ToggleAction> assetsPanelAction;
      Glib::RefPtr<Gtk::ToggleAction> infoboxPanelAction;
      Glib::RefPtr<Gtk::ToggleAction> timelinePanelAction;
      Glib::RefPtr<Gtk::ToggleAction> viewerPanelAction;
      
      
    private: /* ===== Internals ===== */
      bool is_updating_action_state;
    };
  
  
}}// namespace stage::ctrl
#endif /*STAGE_CTRL_ACTIONS_H*/
