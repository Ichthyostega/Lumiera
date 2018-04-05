/*
  interactionDirector  -  Global UI Manager

  Copyright (C)         Lumiera.org
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

* *****************************************************/


/** @file interaction-director.cpp
 ** Implementation parts of the top-level controller within the UI.
 */


#include "gui/gtk-base.hpp"
#include "gui/ui-bus.hpp"
#include "gui/ctrl/bus-term.hpp"
#include "gui/ctrl/global-ctx.hpp"
#include "gui/interact/interaction-director.hpp"
#include "gui/interact/view-locator.hpp"
#include "gui/interact/spot-locator.hpp"
#include "gui/interact/navigator.hpp"
#include "gui/interact/focus-tracker.hpp"
#include "gui/interact/ui-coord-resolver.hpp"
#include "gui/dialog/preferences-dialog.hpp"
#include "gui/dialog/render.hpp"
#include "gui/workspace/workspace-window.hpp"
#include "gui/ctrl/ui-state.hpp"
#include "gui/setting/asset-controller.hpp"
#include "gui/timeline/timeline-controller.hpp"
#include "proc/mobject/session/root.hpp"
#include "proc/asset/sequence.hpp"                ///////////////////////////////////////////////////////////TICKET #1096 : avoid direct inclusion to reduce compile times
#include "proc/mobject/session/fork.hpp"          ///////////////////////////////////////////////////////////TICKET #1096 : avoid direct inclusion to reduce compile times
#include "proc/cmd.hpp"
#include "backend/real-clock.hpp"
#include "lib/diff/tree-mutator.hpp"
#include "lib/format-obj.hpp"
//#include "gui/ui-bus.hpp"
//#include "lib/util.hpp"

//#include <memory>
//#include <list>

//using util::isnil;
//using std::list;
//using std::shared_ptr;
using lib::idi::EntryID;
using lib::hash::LuidH;
using lib::diff::TreeMutator;
using util::toString;


namespace gui {
namespace interact {
  
  namespace session = proc::mobject::session;
  namespace cmd = proc::cmd;
  
  using ctrl::GlobalCtx;
  using ctrl::UiState;
  using setting::AssetController;
  using timeline::TimelineController;
  
  
  // emit dtors of children here...
  InteractionDirector::~InteractionDirector()
    { }
  
  
  InteractionDirector::InteractionDirector (GlobalCtx& globals)
    : model::Controller(session::Root::getID(), globals.uiBus_.getAccessPoint())
    , globalCtx_(globals)
    , viewLocator_{new ViewLocator{globals.windowLoc_}}
    , spotLocator_{new SpotLocator}
    , navigator_{*spotLocator_, *viewLocator_}    // Service exposed as Depend<LocationQuery>
    , tracker_{new FocusTracker{*navigator_}}
    , uiState_{new UiState{globals.uiBus_.getStateManager(), *tracker_}}
    , assets_{new AssetController{session::Root::getAssetID(), this->uiBus_}}
    , timelines_{}
    {
      
    }
  
  
  namespace { // Temporary Junk
    inline void
    unimplemented (const char* todo)
    {
      WARN (gui, "%s is not yet implemented. So sorry.", todo);
    }
  }
  
  
  
  void
  InteractionDirector::buildMutator (TreeMutator::Handle buffer)
  {
//  using Attrib = std::pair<const string,string>;
//  using lib::diff::collection;
    
    buffer.create (
      TreeMutator::build()
    );
    unimplemented ("create a sensible binding between root-controller and root-model element");
  }
  
  
  /**
   * setup a new editing project, possibly close the current one.
   * This action launches the project setup UI, for the user to define
   * the storage location and further parameters of the new project.
   * A currently opened project will be deactivated asynchronously,
   * without saving a snapshot, while retaining the last actions
   * in the project's command log.
   */
  void
  InteractionDirector::newProject()
  {
    unimplemented ("launch project setup UI");
  }
  
  
  /**
   * Save a snapshot of the current project's contents and the UI state.
   * @note any command issued against a session will always be logged in
   *       the project's log. A snapshot is a marked reference point and
   *       additionally includes a capture of UI state into the project.
   */
  void
  InteractionDirector::saveSnapshot()
  {
    string snapshotID{"snap-" + toString(backend::RealClock::now())};
    invoke (cmd::session_saveSnapshot, snapshotID);
  }
  
  
  /**
   * Continue evolution of the currently active project under a new identity.
   * From a user's point of view, this is the "save as..." action, but due to
   * the nature of Lumiera's projects, it is in fact more complicated. Thus
   * this action launches the project setup UI and preselects the "fork" option
   * there. Optionally this allows to copy the project's history into the forked
   * new project, or alternatively just to start with a snapshot. Another option
   * there is to set up a new storage location, or to replace the existing project
   * under a new name.
   * @note any commands issued since saving the last snapshot will be retained at least
   *       in the existing project's log (unless the old project is replaced / rewritten)
   */
  void
  InteractionDirector::forkProject()
  {
    unimplemented ("launch project setup UI to create a fork of the project under new name");
  }
  
  
  /**
   * Edit global configuration and setup.
   * This action will launch the project setup UI, which allows to adjust configuration
   * - for this installation of Lumiera
   * - for the current project
   * - for this user's session in this project
   * @todo 4/2017 not yet implemented, delegate to the AssetControler, which represents
   *       all global aspects of the application, session and project
   */
  void
  InteractionDirector::editSetup()
  {
    dialog::PreferencesDialog dialog(getWorkspaceWindow());
    dialog.run();
    
    unimplemented ("edit global configuration");
  }
  
  
  /**
   * Select and open a file to perform a suitable operation.
   * This action will launch the fileOpen UI. Depending on the selected file's meaning,
   * the actual operation will be either to integrate the data somehow into the current
   * session, or to deactivate the current session and replace it with an other session
   * persisted on storage.
   */
  void
  InteractionDirector::openFile()
  {
    unimplemented ("open file");
  }
  
  
  /**
   * Start a render process. This action will launch the render setup UI.
   * Depending on the current Spot, a suitable object to render will be preselected,
   * typically the current timeline.
   * @todo 4/2017 not yet implemented, and it is not clear how to access the functionality.
   *       Do we send a command? Or do we access the Play-Out subsystem directly, to create
   *       some kind of dedicated player instance?
   */
  void
  InteractionDirector::render()
  {
    dialog::Render dialog(getWorkspaceWindow());                                   //////global -> InteractionDirector
    dialog.run();
    
    unimplemented ("start render");
  }
  
  
  /**
   * Establish a pristine new sequence within the session.
   * The goal is to create a new _playground_ for the user to add content.
   * Actually, not only a new sequence is created, but also a new fork (track tree)
   * and a new timeline to hold that sequence. And finally, this new timeline is opened for editing.
   * This action invokes a command into the session, which in turn is responsible for figuring out
   * all the contextual details sensibly.
   * @todo 4/2017 using the session-root (=this) as anchor for the moment,
   *       but should actually figure out the current context dynamically...   //////////////////////////////TICKET #1082 : actually access the interaction state to get "current scope"
   */
  void
  InteractionDirector::newSequence()
  {
    LuidH anchor{*this};                    /////////////////////////////////////////////////////////////////TICKET #1082 : actually access the interaction state to get "current scope"
    LuidH newSeqID{EntryID<proc::asset::Sequence>().getHash()};  ////////////////////////////////////////////TICKET #1096 : a better Idea what to send over the wire?
    invoke (cmd::session_newSequence, anchor, newSeqID);
  }
  
  
  /**
   * Establish a empty new track close to the current scope.
   * Like for #newSequence, the goal is to create a new empty workspace.
   * But since a track can be attached anywhere within the fork (track tree), the currently active element
   * is used to establish a current scope, which in turn is used as anchor to attach the new track in a
   * sensible way, with a preference to add the new track as a sibling to the current scope. The actual
   * details of this decision are delegated to the session, but the command invoked by this action does
   * need a current element as argument, and this current element thus needs to be figured out from
   * the context of invocation (current focus and possibly selection)
   * @todo as of 3/2017 this is an initial draft: It is not clear yet, if this lookup of context
   *       will always be treated implicitly, or if it is better to have a public _content discovery operation_
   *       on InteractionDirector and pass the current element explicitly as argument
   * @todo 4/2017 using the session-root (=this) as anchor for the moment,
   *       but should actually figure out the current context dynamically...   //////////////////////////////TICKET #1082 : actually access the interaction state to get "current scope"
   */
  void
  InteractionDirector::newTrack()
  {
    LuidH anchor{*this};                    /////////////////////////////////////////////////////////////////TICKET #1082 : actually access the interaction state to get "current scope"
    LuidH newTrackID{EntryID<proc::mobject::session::Fork>().getHash()};  ///////////////////////////////////TICKET #1096 : a better Idea what to send over the wire?
    invoke (cmd::sequence_newTrack, anchor, newTrackID);
  }
  
  
  workspace::WorkspaceWindow&
  InteractionDirector::getWorkspaceWindow()
  {
    return globalCtx_.windowLoc_.findActiveWindow();
  }

  
  
}}// namespace gui::interact
