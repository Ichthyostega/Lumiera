/*
  TIMELINE-GUI.hpp  -  top-level anchor point and placeholder for "the timeline"

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file timeline/timeline-gui.hpp
 ** Anchor point and placeholder for the UI representation of a Timeline from the session.
 ** TimelineGui elements are what is managed by the InteractionDirector as children reflecting
 ** the top-level collection of Timelines within the session. However, these elements are proxies,
 ** implemented as [smart-handle for a widget](\ref model::WLink) -- because the fact _that_ a given
 ** timeline _is indeed presented_ needs to remain at the UI's discretion, retaining the latter's total
 ** authority in question of interaction materiality. Moreover, the actual TimelineWidget is not at all
 ** attached within the InteractionDirector as a managing entity, it is rather installed as a child widget
 ** within a notebook control at some [timeline docking panel](\ref TimelinePanel). There could be several
 ** such timeline panels in various top-level windows, and it is a question within the user's workspace
 ** arrangement where exactly it will be placed, if at all. So TimelineGui acts as smart-handle to access
 ** such a widget, and it stands in as a proxy to receive diff messages for this given timeline. Yet when
 ** this timeline is not present in the UI, such diff messages and other UI-Bus messages addressed for
 ** this entity are to be silently ignored.
 ** 
 ** \par how the timeline presentation structure is built
 ** The actual UI representation of a timeline tends to be a highly complex tree of interconnected widgets.
 ** There is no "master plan" knowledge within the UI how to build such a structure, rather this structure
 ** evolves in response to _population diff messages_ from the core. Which in turn are either produced
 ** initially, or in answer to a population request originating from within the UI, prompted by the
 ** need to expose some timeline at a given place within the interface.
 ** 
 ** @see gui::interact::InteractionDirector::buildMutator
 ** @see gui::timeline::TimelineWidget
 ** 
 */


#ifndef STAGE_TIMELINE_TIMELINE_GUI_H
#define STAGE_TIMELINE_TIMELINE_GUI_H

#include "stage/ctrl/bus-term.hpp"
#include "stage/model/w-link.hpp"
#include "lib/diff/diff-mutable.hpp"
#include "lib/nocopy.hpp"

#include <memory>


namespace gui  {
namespace timeline {
  
  class TimelinePage;
  class TimelineWidget;
  using ctrl::BusTerm;
  
  /**
   * smart-Handle as anchor point for "the UI representation" of a timeline.
   * A collection of TimelineGui elements is managed as children of the InteractionDirector.
   * The corresponding TimelineWidget may or may not be present within the UI currently.
   */
  class TimelineGui
    : public model::WLink<TimelineWidget>
    , public lib::diff::DiffMutable
    , util::Cloneable
    {
      using ID = BusTerm::ID;
      
      ID timelineID_;
      ID rootTrackID_;
      
    public:
      TimelineGui (ID identity, ID trackID);
      virtual ~TimelineGui();
      
      TimelineGui (TimelineGui const&);
      TimelineGui (TimelineGui &&);
      
      operator ID()  const { return timelineID_; }
      
      
      /** actually build a TimelineWidget to enact the role
       *  represented by this smart-handle */
      std::unique_ptr<TimelinePage> buildTimelineWidget (BusTerm&);
      
      /** @internal this method is invoked by the UI-Bus when dispatching a MutationMessage... */
      void buildMutator (lib::diff::TreeMutator::Handle buffer)  override;

    };
  
  
}}// namespace gui::timeline
#endif /*STAGE_TIMELINE_TIMELINE_GUI_H*/
