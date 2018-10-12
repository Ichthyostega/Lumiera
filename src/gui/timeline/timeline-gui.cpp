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


using lib::diff::TreeMutator;
using std::make_unique;


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
  
  TimelineGui::~TimelineGui() { }
  
  
  /**
   * actually build a TimelineWidget to enact the role
   * represented by this smart-handle
   */
  std::unique_ptr<TimelineWidget>
  TimelineGui::buildTimelineWidget (BusTerm& nexus)
  {
    auto widget = std::make_unique<TimelineWidget> (timelineID_, rootTrackID_, nexus);
    this->connect (*widget);
    return widget;
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
