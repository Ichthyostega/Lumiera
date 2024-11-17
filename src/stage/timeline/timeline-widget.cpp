/*
  TimelineWidget  -  custom widget for timeline display of the project

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file timeline/timeline-widget.cpp
 ** Implementation details of Lumiera's timeline display widget.
 ** 
 ** @todo as of 10/2018 a complete rework of the timeline display is underway
 ** @see timeline-controller.cpp
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/timeline-widget.hpp"
#include "stage/timeline/timeline-controller.hpp"
#include "stage/timeline/timeline-layout.hpp"
#include "stage/style-scheme.hpp"

//#include "stage/workspace/workspace-window.hpp"
//#include "stage/ui-bus.hpp"
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


namespace stage {
namespace timeline {
  
//const int TimelineWidget::TrackPadding = 1;
//const int TimelineWidget::HeaderWidth = 150;
//const int TimelineWidget::HeaderIndentWidth = 10;
  
  
  
  
  TimelineWidget::TimelineWidget (BusTerm::ID identity, BusTerm::ID trackID, BusTerm& nexus)
    : TimelinePage{}
    , layout_{new TimelineLayout{*this}}
    , control_{new TimelineController{identity, trackID, nexus, *layout_}}
    {
      get_style_context()->add_class(CLASS_timeline);
      get_style_context()->add_class(CLASS_timeline_page);
      show_all();
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
  
  Gtk::WidgetPath
  TimelineWidget::getBodyWidgetPath()  const
  {
    return layout_->getBodyWidgetPath();
  }

  
  
  
}}// namespace stage::timeline
