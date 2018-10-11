/*
  INTERACTION-DIRECTOR.hpp  -  Global UI Manager

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

*/


/** @file interaction-director.hpp
 ** The top-level controller to connect model and user interaction state.
 ** Within the Lumiera UI, relevant entities from the session model are mapped onto and represented
 ** by corresponding [UI-Elements](\ref Tangible). Consequently, there is a hierarchy of interrelated
 ** UI elements mirroring the hierarchy within the session model. And, while in the latter, there is
 ** a _conceptual root node_ to correspond to the session itself, within the UI there is a top-level
 ** controller to mirror and represent that root element: The InteractionDirector.
 ** 
 ** For one, the InteractionDirector represents and exposes parts of the model as seen from top level.
 ** More specifically, through the InteractionDirector, it is possible to open and enter the UI
 ** to work with the timeline(s), with the assets and with the global session configuration.
 ** Moreover, this top-level controller allows to issue likewise global actions regarding
 ** those top-level entities:
 ** - create / modify / delete timeline(s)
 ** - create / modify / sequences
 ** - save, close, open and create a session
 ** 
 ** And, secondly, beyond those top-level model-related activities, the InteractionDirector serves
 ** as link between model entities, actions to be performed onto them and the transient yet global
 ** user interaction state. The latter means anything related to _the current window_,
 ** _the current focus_, _the current work-site_, the current interface controller technology used etc.
 ** Obviously, the InteractionDirector can not _handle_ all those heavyweight concerns; but it connects
 ** the involved parts and (re)directs the information flow towards the proper recipient to handle it.
 ** 
 ** @see gtk-lumiera.hpp
 ** @see ui-bus.hpp
 */


#ifndef GUI_INTERACT_INTERACTION_DIRECTOR_H
#define GUI_INTERACT_INTERACTION_DIRECTOR_H

#include "gui/model/controller.hpp"
#include "gui/timeline/timeline-gui.hpp"
#include "lib/depend-inject.hpp"

//#include <string>
#include <vector>
#include <memory>


namespace gui {
  
  class UiBus;

namespace ctrl {
  class GlobalCtx;
  class UiState;
}
namespace setting {
  class AssetController;
}
namespace timeline {
  class TimelineController;
}
namespace workspace {
  class WorkspaceWindow;
}
namespace interact {
  
  using lib::diff::GenNode;
//using std::string;
  using std::unique_ptr;
  
//class Actions;
  class LocationQuery;
  
  class Navigator;
  class SpotLocator;
  class FocusTracker;
  class ViewLocator;
  
  using timeline::TimelineGui;
  
  
  
  /**
   * Top-level controller to establish a link between the model
   * and transient user interaction state (focus, current window)
   */
  class InteractionDirector
    : public model::Controller
    {
      ctrl::GlobalCtx& globalCtx_;
      
      // == exposed to Depend on ==
      using Service_LocationQuery = lib::DependInject<LocationQuery>::ServiceInstance<Navigator>;
      
      
      // == global Services ==
      unique_ptr<ViewLocator> viewLocator_;
      unique_ptr<SpotLocator> spotLocator_;
      Service_LocationQuery    navigator_;
      unique_ptr<FocusTracker>  tracker_;
      
      // == Model globals ==
      using Timelines = std::vector<TimelineGui>;
      using Assets = unique_ptr<setting::AssetController>;
      using State = unique_ptr<ctrl::UiState>;
      
      State     uiState_;
      Assets    assets_;
      Timelines timelines_;
      
      
      /** set up a binding to allow some top-level UI state
       *  to be treated as part of the session model
       * @see tree-mutator.hpp
       */
      void buildMutator (lib::diff::TreeMutator::Handle)  override;
      
      /** ask Session to push up structures for presentation */
      void populateContent_afterStart();
      
      static constexpr auto DELAY_AFTER_GUI_START_in_ms = 100;
      
    public:
      InteractionDirector (ctrl::GlobalCtx&);
     ~InteractionDirector ();
      
      // == Menu actions ==
      void newProject();
      void saveSnapshot();
      void forkProject();
      void editSetup();
      void openFile();
      void render();
      
      void newSequence();
      void newTrack();
      
    private:
      workspace::WorkspaceWindow& getWorkspaceWindow();
      TimelineGui injectTimeline (GenNode const&);
    };
  
  
  
}}// namespace gui::workspace
#endif /*GUI_INTERACT_INTERACTION_DIRECTOR_H*/
