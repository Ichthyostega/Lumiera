/*
  TIMELINE-WIDGET.hpp  -  custom widget for timeline display of the project

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


/** @file timeline/timeline-widget.hpp
 ** This file defines the core component of the Lumiera GUI.
 ** Timelines are the top level elements within Lumiera's high-level-model ("the session").
 ** In the UI workspace, there is a timeline pane with several tabs, each of which holds an
 ** instance of the TimelineWidget. Each of these tabs either represents one of the top-level
 ** timelines in the model, or it represents a (focused / slave) view into some timeline.
 ** 
 ** The TimelineWidget is the top level entry point to a significant part of the UI, anything
 ** related to timeline display and editing operations: each instance of this widget is dedicated
 ** to a single session::Timeline, known by its ID. The widget creates a TimelineController
 ** right away, which takes initiative to populate the display with that Timeline's contents.
 ** 
 ** # Lifecycle
 ** The assumption is that any element creation and deletion is triggered through messages over
 ** the [UI-Bus](\ref ui-bus.hpp). So there will be a _parent element,_ corresponding to the
 ** [model root](\ref proc::mobject::session::Root), and this parent, in response to some
 ** mutation message, will create a TimelineWidget, add it into the appropriate GTK display setup
 ** and manage it as child element; the [construction parameters](\ref TimelineWidget::TimelineWidget)
 ** ensure it gets connected to the bus as well. Incidentally, this assumption also implies that
 ** this parent element has set up a _binding for diff mutation,_ typically by implementing
 ** model::Tangible::buildMutator. And further on this means that the parent will also
 ** destroy the TimelineWidget, prompted by a message to that end. All deregistration
 ** and unwinding happens automatically. Widgets, and also our model::Controller
 ** is `sigc::trackable`, which means after destruction any further signals
 ** will be silently ignored.
 ** 
 ** @todo as of 10/2018 a complete rework of the timeline display is underway
 ** 
 */


#ifndef STAGE_TIMELINE_TIMELINE_WIDGET_H
#define STAGE_TIMELINE_TIMELINE_WIDGET_H

#include "stage/gtk-base.hpp"
//#include "stage/timeline/timeline-controller.hpp"   /////TODO possible to push that into the implementation?
//#include "stage/timeline/layout-manager.hpp"
#include "stage/ctrl/bus-term.hpp"

#include "lib/time/timevalue.hpp"
#include "lib/diff/diff-mutable.hpp"
#include "lib/nocopy.hpp"

//#include <memory>
//#include <vector>



namespace steam {
namespace asset{
  class Timeline;
}}

namespace stage  {
namespace timeline {
  
  using ctrl::BusTerm;
  class TimelineController;
  class TimelineLayout;
  
  /**
   * Interface: GUI page holding a timeline display
   */
  class TimelinePage
    : public Gtk::Paned
    {
    public:
      virtual ~TimelinePage() { }           ///< this is an interface
      
      virtual cuString getLabel()  const    =0;
      
      
      TimelinePage()
        : Gtk::Paned{Gtk::ORIENTATION_HORIZONTAL}
        { }
    };
  
  /**
   * Core timeline display (custom widget).
   * Top level entry point to the timeline display component.
   * @todo WIP-WIP-rewrite as of 12/2016
   * @remarks At top level, this widget is split into a header pane (left)
   *     and a scrollable timeline body (right). The layout of both parts is aligned.
   */
  class TimelineWidget
    : public TimelinePage
    , util::NonCopyable
    {
      std::unique_ptr<TimelineLayout>     layout_;
      std::unique_ptr<TimelineController> control_;
      
    public:
      /** build a new timeline display and attach it to the UI-Bus.
       * @param identity used to refer to a corresponding element in the Session
       * @param trackID the mandatory root track used in the associated Sequence
       * @param nexus some established connection to the UI-Bus, will be used
       *          to register the embedded TimelineController as communication
       *          partner to respond under the given ID.
       * @remarks after creation, the widget can just be hooked up and wired like
       *          any ordinary GTK element; it becomes passive and just responds to
       *          signals. The active role is played by the controller, which also
       *          responds to mutation messages; this is the only way to populate
       *          the timeline display. Likewise, a timeline shall be deleted by
       *          sending an respective mutation message to its _parent element,_
       *          the one that created it, typically also in response to a message.
       *          Non the less it is possible just to delete a TimelineWidget, since
       *          it is a Gtk::Widget, and the controller is also `sigc::trackable`
       *          and additionally, as a gui::model::Tangible, it will deregister
       *          automatically from the UI-Bus. After that, any further messages
       *          towards this element will be dropped silently.
       */
      TimelineWidget (BusTerm::ID identity, BusTerm::ID trackID, BusTerm& nexus);
      
     ~TimelineWidget();
      
      
      
    public: /* ===== Control interface ===== */
      
      /** allow for diff mutation (delegated to TimelineController */
      void buildMutator (lib::diff::TreeMutator::Handle);
      
      cuString getLabel()  const override;
      
      
    public: /* ===== Signals ===== */
      
    private:/* ===== Events ===== */
      
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_TIMELINE_WIDGET_H*/
