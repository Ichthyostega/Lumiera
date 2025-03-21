/*
  TIMELINE-CONTROLLER.hpp  -  coordinate operation of timeline display

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file timeline-controller.hpp
 ** Controller to supervise operation of timeline display in the UI.
 ** While the TimelineWidget is the top level entry point to any facility dealing with
 ** timeline display and editing operations, the widget in turn creates a TimelineController
 ** right away, which then takes initiative to populate the display with the corresponding
 ** session::Timeline contents. The controller thus serves as the model connection through
 ** the UI-Bus and becomes the active part of running the timeline display, delegating
 ** display activities to the widget, which in turn uses the timeline::LayoutManager to
 ** work out the details of presentation in collaboration with the local UI model elements.
 ** Incidentally, those UI model elements, which are actually Presenters, are managed as
 ** children of the TimelineController; this makes sense from an architectural point of view:
 ** In Lumiera, we separate between the core concerns and the questions of UI mechanics. For
 ** the former, the global angle of view, the roles of _controller_ and _model_ are mediated
 ** by the control::UiBus, while the [tangible elements](\ref model::Tangible), play the
 ** role of the _view._ But at the same time, following the local UI centric angle, the latter
 ** entities act more like presenters, forming a dedicated _view model,_ while controlling
 ** mostly passive view components (widgets).
 ** 
 ** To sum up
 ** - TimelineController is a Tangible, connected to the UiBus, representing "the timeline"
 ** - it directly manages a set of TrackPresenter entities, to correspond to the session::Fork
 ** - these in turn manage a set of ClipPresenter entities
 ** - and those presenters care for injecting suitable widgets into the TimelineWidget's parts.
 ** 
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** 
 */


#ifndef STAGE_TIMELINE_TIMELINE_CONTROLLER_H
#define STAGE_TIMELINE_TIMELINE_CONTROLLER_H

#include "stage/gtk-base.hpp"
#include "stage/model/controller.hpp"

#include "lib/time/timevalue.hpp"

#include <memory>
#include <vector>
#include <string>



namespace stage  {
namespace timeline {
  
  using std::unique_ptr;
  using std::vector;
  using std::string;
  
  class TrackPresenter;
  class TimelineLayout;
  class MarkerWidget;
  
  
  /**
   * Controller to supervise the timeline display.
   * As a [tangible element](\ref model::Tangible), it is attached to the UI-Bus.
   * @todo WIP-WIP-rewrite as of 12/2016
   * @remarks a Timeline always has an attached Sequence, which in turn has
   *     a single mandatory root track. This in turn might hold further child tracks,
   *     thus forming a fork of nested scopes.
   */
  class TimelineController
    : public model::Controller
    {
      string name_;
      
      vector<unique_ptr<MarkerWidget>> markers_;
      std::unique_ptr<TrackPresenter>  fork_;
      
      
    public:
      /**
       * @param identity used to refer to a corresponding timeline element in the Session
       * @param trackID the mandatory root track used in the associated Sequence
       * @param nexus some established connection to the UI-Bus, used for registration.
       */
      TimelineController (ID identity, ID trackID, ctrl::BusTerm& nexus, TimelineLayout&);
      
     ~TimelineController();
      
      
      /** set up a binding to respond to mutation messages via UiBus */
      virtual void buildMutator (lib::diff::TreeMutator::Handle)  override;
      
      
    public: /* ===== Control interface ===== */
      
      string const&
      getName()  const
        {
          return name_;
        }
      
    public: /* ===== Signals ===== */
      
    private:/* ===== Events ===== */
      
    private:/* ===== Internals ===== */
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_TIMELINE_CONTROLLER_H*/
