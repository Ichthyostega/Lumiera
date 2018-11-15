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
 ** we have the globally tangible scope of actual session elements and operations performed
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
 ** only a small number of elements can be visible and active as far as user interaction is concerned.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** 
 */


#ifndef STAGE_TIMELINE_TRACK_PRESENTER_H
#define STAGE_TIMELINE_TRACK_PRESENTER_H

#include "stage/gtk-base.hpp"
#include "stage/model/controller.hpp"
#include "stage/timeline/marker-widget.hpp"
#include "stage/timeline/clip-presenter.hpp"
#include "stage/timeline/track-head-widget.hpp"
#include "stage/timeline/track-body.hpp"

#include "lib/nocopy.hpp"
//#include "lib/util.hpp"

//#include <memory>
#include <utility>
#include <vector>



namespace stage  {
namespace timeline {
  
  using std::vector;
  using std::unique_ptr;
  
  
  /**
   * Reference frame to organise the display related to a specific Track in the Timeline-GUI.
   */
  struct DisplayFrame
    : util::NonCopyable
    {
      TrackHeadWidget head;
      TrackBody       body;
      
      template<class FUN>
      DisplayFrame (FUN anchorDisplay)
        : head{}
        , body{}
        {
          anchorDisplay (head, body);
        }
      
     ~DisplayFrame()
        {
          TODO ("cause the managed presentation elements to detach from their parents");
        }
      
      void
      setTrackName (cuString& name)
        {
          head.setTrackName (name);
          body.setTrackName (name);  ///////////////////////////////////TICKET #1017 -- TODO 11/18 : not clear yet if TrackBody needs to know its name
        }

      void
      injectSubTrack (TrackHeadWidget& head, TrackBody& body)
        {
          UNIMPLEMENTED ("inject the widgets to represent a nested sub-track within this timeline track display frame");
        }
    };
  
  
  /**
   * @todo WIP-WIP as of 12/2016
   */
  class TrackPresenter
    : public model::Controller
    {
      DisplayFrame display_;
      
      vector<unique_ptr<TrackPresenter>> subFork_;
      vector<unique_ptr<MarkerWidget>>   markers_;
      vector<unique_ptr<ClipPresenter>>  clips_;
      
      
    public:
     ~TrackPresenter();
      
      /**
       * @param identity used to refer to a corresponding session::Fork in the Session
       * @param nexus a way to connect this Controller to the UI-Bus.
       */
      template<class FUN>
      TrackPresenter (ID id, ctrl::BusTerm& nexus, FUN anchorDisplay)
        : Controller{id, nexus}
        , display_{anchorDisplay}
        , subFork_{}
        , markers_{}
        , clips_{}
        { 
          setTrackName (id.getSym());  // fallback initialise track-name from human-readable ID symbol 
        }
      
      
      /** set up a binding to respond to mutation messages via UiBus */
      virtual void buildMutator (lib::diff::TreeMutator::Handle)  override;
      
      
    private:/* ===== Internals ===== */
      
      /** invoked via diff to show a (changed) track name */
      void
      setTrackName (string name)
        {
          display_.setTrackName (name);
        }
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_TRACK_PRESENTER_H*/
