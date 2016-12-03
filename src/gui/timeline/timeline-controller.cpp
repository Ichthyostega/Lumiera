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
    {
      UNIMPLEMENTED ("how to make the controller operative...");
    }
  
  
  TimelineController::~TimelineController()
  {
  }
  
  
  void
  TimelineController::buildMutator (TreeMutator::Handle buffer)
  {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1039
    using Attrib = std::pair<const string,string>;
    using lib::diff::collection;
    
    buffer.create (
      TreeMutator::build()
        .attach (collection(scope)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {
                    return spec.data.isNested();                  // »Selector« : require object-like sub scope
                  })
               .matchElement ([&](GenNode const& spec, PMockElm const& elm) -> bool
                  {
                    return spec.idi == elm->getID();
                  })
               .constructFrom ([&](GenNode const& spec) -> PMockElm
                  {
                    PMockElm child = std::make_unique<MockElm>(spec.idi, this->uiBus_);
                    return child;
                  })
               .buildChildMutator ([&](PMockElm& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                  {
                    if (target->getID() != subID) return false;   //require match on already existing child object
                    target->buildMutator (buff);                  // delegate to child to build nested TreeMutator
                    return true;
                  }))
        .attach (collection(attrib)
               .isApplicableIf ([&](GenNode const& spec) -> bool
                  {
                    return spec.isNamed()                         // »Selector« : accept attribute-like values
                       and not spec.data.isNested();              //              but no nested objects
                  })
               .matchElement ([&](GenNode const& spec, Attrib const& elm) -> bool
                  {
                    return elm.first == spec.idi.getSym();
                  })
               .constructFrom ([&](GenNode const& spec) -> Attrib
                  {
                    string key{spec.idi.getSym()},
                           val{render(spec.data)};
                    return {key, val};
                  })
               .assignElement ([&](Attrib& target, GenNode const& spec) -> bool
                  {
                    string key{spec.idi.getSym()},
                           newVal{render (spec.data)};
                    target.second = newVal;
                    return true;
                  })));
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1039
    UNIMPLEMENTED ("diff mutation binding for the TimelineController");
  }
  
  
  
}}// namespace gui::timeline
