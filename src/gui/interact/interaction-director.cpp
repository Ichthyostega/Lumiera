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
#include "gui/interact/spot-locator.hpp"
#include "gui/interact/navigator.hpp"
#include "gui/interact/focus-tracker.hpp"
#include "gui/ctrl/ui-state.hpp"
#include "gui/setting/asset-controller.hpp"
#include "gui/timeline/timeline-controller.hpp"
#include "proc/mobject/session/root.hpp"
#include "lib/diff/tree-mutator.hpp"
//#include "gui/ui-bus.hpp"
//#include "lib/util.hpp"

//#include <memory>
//#include <list>

//using util::isnil;
//using std::list;
//using std::shared_ptr;
using lib::diff::TreeMutator;


namespace gui {
namespace interact {
  
  namespace session = proc::mobject::session;
  
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
    , spotLocator_{new SpotLocator}
    , navigator_{new Navigator{*spotLocator_}}
    , tracker_{new FocusTracker{*navigator_}}
    , uiState_{new UiState{globals.uiBus_.getStateManager(), *tracker_}}
    , assets_{new AssetController{session::Root::getAssetID(), this->uiBus_}}
    , timelines_{}
    {
      
    }
  
  
  void
  InteractionDirector::buildMutator (TreeMutator::Handle buffer)
  {
//  using Attrib = std::pair<const string,string>;
//  using lib::diff::collection;
    
    buffer.create (
      TreeMutator::build()
    );
    UNIMPLEMENTED ("create a sensible binding between root-controller and root-model element");
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
    UNIMPLEMENTED ("launch project setup UI");
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
    UNIMPLEMENTED ("issue save snapshot command towards current session");
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
    UNIMPLEMENTED ("launch project setup UI to create a fork of the project under new name");
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
    UNIMPLEMENTED ("open file");
  }
  
  
  
}}// namespace gui::interact