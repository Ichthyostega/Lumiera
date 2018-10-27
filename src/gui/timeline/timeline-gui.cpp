/*
  TimelineGui  -  top-level anchor point and placeholder for "the timeline"

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

* *****************************************************/


/** @file timeline-gui.cpp
 ** Implementation details a delegating anchor proxy for a TimelineWidget.
 ** To reduce header include impact of global-ctx.hpp, all operations involving
 ** actual object construction are defined within this dedicated translation unit.
 ** 
 ** @see TimelineWidget
 ** @see gui::interact::InteractionDirector::buildMutator
 ** @see gui::timeline::TimelineController::buildMutator
 ** 
 */


#include "gui/gtk-base.hpp"
#include "include/ui-protocol.hpp"
#include "gui/timeline/timeline-gui.hpp"
#include "gui/timeline/timeline-widget.hpp"

#include <utility>

using lib::diff::TreeMutator;
using std::unique_ptr;


namespace gui {
namespace timeline {
  
  
  /**
   * Initially, TimelineGui is just an empty placeholder handle.
   * Yet knowledge of the represented timeline's ID is required
   */
  TimelineGui::TimelineGui (ID identity, ID trackID)
    : timelineID_{identity}
    , rootTrackID_{trackID}
    { }
  
  /**
   * @remark basically the default copy ctor, just redefined here explicitly
   *  to allow using TimelineGui without the need to include timeline-widget.hpp
   */
  TimelineGui::TimelineGui (TimelineGui const& r)
    : WLink{r}, util::Cloneable{}
    , timelineID_{r.timelineID_}
    , rootTrackID_{r.rootTrackID_}
    { }
  
  TimelineGui::TimelineGui (TimelineGui && rr)
    : WLink{std::move (rr)}
    , timelineID_{rr.timelineID_}
    , rootTrackID_{rr.rootTrackID_}
    { }
  
  TimelineGui::~TimelineGui()
    {
      if (this->isActive())
        TODO ("cascading destruction of the TimelineWidget");   /////////////////////////////////////////////TICKET 1016 : implement sane unwinding
    }
  
  
  /**
   * actually build a TimelineWidget to enact the role represented by this smart-handle.
   * @remark some implementation twist involved here, since TimelinePanel manages a collection
   *    of `unique_ptr<TimelinePage>`, in order to be able to hold an empty placeholder page.
   *    Since C++ unfortunately does not support _Covariance_ proper, we need to fabricate a
   *    unique_ptr<TimelinePage> right here, which can than be handed over to TimelinePanel,
   *    yet still we _do inherit_ from WLink<TimelineWidget>, i.e. expose the subclass.
   *    The `unque_ptr` owns and manages the TimelineWidget, which is itself non-copyable
   *    and stays at a fixed location in heap memory, as is required by gui::ctrl::Nexus
   *    to maintain a registration of the UI-Bus connection. WLink to the contrary
   *    just connects to the widget, and is automatically disconnected when it dies.
   */
  unique_ptr<TimelinePage>
  TimelineGui::buildTimelineWidget (BusTerm& nexus)
  {
    TimelineWidget* widget;
    unique_ptr<TimelinePage> pageHandle {widget = new TimelineWidget{timelineID_, rootTrackID_, nexus}};
    this->connect (*widget);
    return pageHandle;
  }
  
  
  /**
   * forwarding implementation of the DiffMutable interface,
   * delegating through the referred TimelineWidget if present,
   * yet silently ignoring any diff messages otherwise.
   * @see tree-mutator.hpp
   */
  void
  TimelineGui::buildMutator (lib::diff::TreeMutator::Handle buffer)
  {
    if (this->isActive())
      operator*().buildMutator (buffer);  // delegate to TimelineController
    else                                 //  else when no widget exists...
      buffer.create(
        TreeMutator::build()
          .ignoreAllChanges());       //     ...consume and ignore diff
  }
  
  
}}// namespace gui::timeline
