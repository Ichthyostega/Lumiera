/*
  TimelineController  -  coordinate operation of timeline display

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

* *****************************************************/


/** @file timeline-controller.cpp
 ** Implementation details of timeline operation management and control.
 ** - we build a binding to allow TimelineController to handle mutation messages
 **   on behalf of "the timeline". While the setup of a Timeline is quite flexible
 **   at the session level, here, when it comes to UI presentation, it can be
 **   boiled down to
 **   + a name
 **   + a single mandatory root track (which in turn could hold nested tracks)
 ** - thus we get a rather simple mapping, with some fixed attributes and no
 **   flexible child collection. The root track is implemented as TrackPresenter.
 ** 
 ** @todo as of 12/2016 a complete rework of the timeline display is underway
 ** @see TimelineWidget
 ** 
 */


#include "gui/gtk-lumiera.hpp"
#include "gui/timeline/timeline-controller.hpp"
#include "gui/timeline/track-presenter.hpp"
#include "gui/timeline/marker-widget.hpp"

//#include "gui/workspace/workspace-window.hpp"
//#include "gui/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

//#include "lib/util.hpp"
//#include <algorithm>
//#include <cstdlib>



//using util::_Fmt;
using lib::diff::TreeMutator;
using lib::diff::collection;
using std::make_unique;
//using std::shared_ptr;
//using std::weak_ptr;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace gui {
namespace timeline {
  
//const int TimelineWidget::TrackPadding = 1;
//const int TimelineWidget::HeaderWidth = 150;
//const int TimelineWidget::HeaderIndentWidth = 10;
  
  
  
  
  TimelineController::TimelineController (ID identity, ID trackID, ctrl::BusTerm& nexus)
    : Controller{identity, nexus}
    , markers_{}
    , fork_{new TrackPresenter{trackID, nexus}}
    , name_{identity.getSym()}              // fallback initialise name from human-readable ID symbol 
    {
      UNIMPLEMENTED ("how to make the controller operative...");
    }
  
  
  TimelineController::~TimelineController()
  {
  }
  
  
  void
  TimelineController::buildMutator (TreeMutator::Handle buffer)
  {
    using PMarker = unique_ptr<MarkerWidget>;
    
    buffer.create (
      TreeMutator::build()
        .attach (collection(markers_)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {
                    return spec.data.isNested();                  // »Selector« : require object-like sub scope
                  })
               .matchElement ([&](GenNode const& spec, PMarker const& elm) -> bool
                  {
                    return spec.idi == ID(elm);
                  })
               .constructFrom ([&](GenNode const& spec) -> PMarker
                  {
                    return make_unique<MarkerWidget>(spec.idi, this->uiBus_);
                  })
               .buildChildMutator ([&](PMarker& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                  {
                    if (ID(target) != subID) return false;         //require match on already existing child object
                    target->buildMutator (buff);                  // delegate to child to build nested TreeMutator
                    return true;
                  }))
        .change("name", [&](string val)
            {
              name_ = val;
            })
        .mutateAttrib("fork", [&](TreeMutator::Handle buff)
            {
              REQUIRE (fork_);
              fork_->buildMutator(buff);
            }));
  }
  
  
  
}}// namespace gui::timeline
