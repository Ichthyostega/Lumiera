/*
  TimelineWidget  -  custom widget for timeline display of the project

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


/** @file timeline/timeline-widget.cpp
 ** Implementation details of Lumiera's timeline display widget.
 ** 
 ** @todo as of 10/2018 a complete rework of the timeline display is underway
 ** @see timeline-controller.cpp
 ** 
 */


#include "gui/gtk-base.hpp"
#include "gui/timeline/timeline-widget.hpp"
#include "gui/timeline/timeline-controller.hpp"
#include "gui/timeline/layout-manager.hpp"

//#include "gui/workspace/workspace-window.hpp"
//#include "gui/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

//#include "lib/util.hpp"
//#include <algorithm>
//#include <cstdlib>



//using util::_Fmt;
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
  
  
  
  
  TimelineWidget::TimelineWidget (BusTerm::ID identity, BusTerm::ID trackID, BusTerm& nexus)
    : TimelinePage{}
    , layout_{new LayoutManager}
    , control_{new TimelineController{identity, trackID, nexus, *layout_}}
    {
      UNIMPLEMENTED ("build the timeline UI");
    }
  
  TimelineWidget::~TimelineWidget() { }
  
  
  void
  TimelineWidget::buildMutator (lib::diff::TreeMutator::Handle buff)
  {
    control_->buildMutator (buff);
  }
  
  
  cuString
  TimelineWidget::getLabel()  const
  {
    return control_->getName();
  }

  
  
  
}}// namespace gui::timeline
