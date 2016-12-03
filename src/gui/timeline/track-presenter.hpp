/*
  TRACK-PRESENTER.hpp  -  presentation control element for a track within the timeline

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file track-presenter.hpp
 ** Presentation control element to model and manage a track within the timeline UI.
 ** In the Lumiera timeline UI, we are mixing two different scope of concerns: For one,
 ** we have the globally tangible scope of actual session elements an operations performed
 ** on those. And then there are more local considerations regarding the "mechanics" of the
 ** UI elements, their state and immediate feedback to user interactions. The _Presenter_ --
 ** as known from the [MVP pattern](https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93presenter) --
 ** serves as link between both levels. For the global angle of view, it is a model::Tangible and thus
 ** plays the role of the _View_, while the _Model_ and _Controler_ roles are mediated through the
 ** gui::UiBus, exchanging command, state and mutation messages. On the other hand, for the local
 ** angle of view, the _Presenter_ is a structural model element, kind of a _view model_, and corresponds
 ** to the respective element within the session. In addition, it manages actively the collaborative
 ** part of layout building, delegating to a mostly passive GTK widget for the actual display.
 ** This way it becomes possible to manage the actual UI resources on a global level, avoiding to
 ** represent potentially several thousand individual elements as GTK entities, while at any time
 ** only several can be visible and active as far as user interaction is concerned.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#ifndef GUI_TIMELINE_TRACK_PRESENTER_H
#define GUI_TIMELINE_TRACK_PRESENTER_H

#include "gui/gtk-base.hpp"
#include "gui/model/controller.hpp"
#include "gui/timeline/track-head-widget.hpp"
#include "gui/timeline/track-body.hpp"

//#include "lib/util.hpp"

//#include <memory>
#include <vector>



namespace gui  {
namespace timeline {
  
  using std::vector;
  using std::unique_ptr;
  
  class ClipPresenter;
  class MarkerWidget;
  
  /**
   * @todo WIP-WIP as of 12/2016
   */
  class TrackPresenter
    : public model::Controller
    {
      vector<unique_ptr<TrackPresenter>> subFork_;
      vector<unique_ptr<MarkerWidget>>   markers_;
      vector<unique_ptr<ClipPresenter>>  clips_;
      
      TrackHeadWidget head_;
      TrackBody       body_;
      
      
    public:
      /**
       * @param identity used to refer to a corresponding session::Fork in the Session
       * @param nexus a way to connect this Controller to the UI-Bus.
       */
      TrackPresenter (ID identity, ctrl::BusTerm& nexus);
      
     ~TrackPresenter();
      
      
      /** set up a binding to respond to mutation messages via UiBus */
      virtual void buildMutator (lib::diff::TreeMutator::Handle)  override;
      
      
    private:/* ===== Internals ===== */
    };
  
  
}}// namespace gui::timeline
#endif /*GUI_TIMELINE_TRACK_PRESENTER_H*/
