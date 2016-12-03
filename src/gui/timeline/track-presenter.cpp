/*
  TrackPresenter  -  presentation control element for a track within the timeline

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


/** @file track-presenter.cpp
 ** Implementation details of track presentation management.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#include "gui/gtk-lumiera.hpp"
#include "gui/timeline/track-presenter.hpp"
#include "gui/timeline/clip-presenter.hpp"

//#include "gui/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

//#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
using lib::diff::TreeMutator;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace gui {
namespace timeline {
  
  
  
  
  
  TrackPresenter::TrackPresenter (ID identity, ctrl::BusTerm& nexus)
    : Controller{identity, nexus}
    , subFork_{}
    , clips_{}
    {
      UNIMPLEMENTED ("how to attach the TrackPresenter into the two relevant GTK display contexts");
    }
  
  
  TrackPresenter::~TrackPresenter()
  {
  }
  
  
  
  
  void
  TrackPresenter::buildMutator (TreeMutator::Handle buffer)
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
    UNIMPLEMENTED ("diff mutation binding for the TrackPresenter");
  }
  
  
  
  
}}// namespace gui::timeline
