/*
  TimelineLayout  -  global timeline layout management and display control

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


/** @file timeline-layout.cpp
 ** Implementation details of global timeline layout management.
 ** 
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** @todo WIP-WIP-WIP as of 12/2019
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/timeline-layout.hpp"
#include "stage/timeline/track-body.hpp"

//#include "stage/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

#include "common/advice.hpp"
//#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace stage {
namespace timeline {
  
  
  
  
  TimelineLayout::~TimelineLayout() { }
  
  TimelineLayout::TimelineLayout (Gtk::Paned& topLevelContainer)
    : paneSplitPosition_{topLevelContainer.property_position()}
    , bodyCanvas_{*this}                           // inject (as interface DisplayManager)
    , headerPane_{bodyCanvas_.get_vadjustment()}  //  wire the patchbay (Gtk::Viewport) to follow the body vertical scroll movement
    , displayEvaluation_{}
    {
      topLevelContainer.add1 (headerPane_);
      topLevelContainer.add2 (bodyCanvas_);
      displayEvaluation_.wireCanvas (bodyCanvas_);
    }
  
  
  /**
   * This function is invoked once for each new TimelineWidget, in order to build the
   * starting point for the track widget structure, which then can be extended recursively
   * to add further nested tracks. The central problem for this widget hierarchy is that
   * we have to build two matching structures in parallel...
   * - the track header area ("patchbay")
   * - the corresponding track body with actual content (clips)
   */
  void
  TimelineLayout::installRootTrack(TrackHeadWidget& head, TrackBody& body)
  {
    headerPane_.installForkRoot (head);
    bodyCanvas_.installForkRoot (body);
    
    // detect changes of the track structure
    body.signalStructureChange_ = signalStructureChange_;
    signalStructureChange_(); // this _is_ such a change
  }
  
  void
  TimelineLayout::wireForkRoot (LayoutElement& forkRoot)
  {
    displayEvaluation_.wireForkRoot (forkRoot);
  }

  Gtk::WidgetPath
  TimelineLayout::getBodyWidgetPath()  const
  {
    return bodyCanvas_.get_path();
  }
  
  
  
  /* ==== Interface: LayoutManager===== */
  
  PixSpan
  TimelineLayout::getPixSpan()
  {
                                           ////////////////////////////////////////////////////////TICKET #1019 : need a "ZoomWindow" here to manage the visible area
                                           ////////////////////////////////////////////////////////TICKET #1039 : "somehow" wire with the TimelineController to find out the covered span
    return PixSpan {0, 248}; ////////////////Lalala Lalü
  }
  
  
  /**
   * Perform a complete recursive pass over all elements relevant for layout,
   * reestablish size allocation and negotiate distribution of available space.
   * @remark this is the entry point to the so called *display evaluation pass*.
   */
  void
  TimelineLayout::triggerDisplayEvaluation()
  {
    displayEvaluation_.perform();
  }

  
  
  /* ==== Interface: ViewHook ===== */
  
  void
  TimelineLayout::hook (TrackHeadWidget& head)
  {
    headerPane_.installForkRoot (head);
  }

  void
  TimelineLayout::hook (TrackBody& body)
  {
    bodyCanvas_.installForkRoot (body);
    
    // detect changes of the track structure
    body.signalStructureChange_ = signalStructureChange_;
    signalStructureChange_(); // this _is_ such a change
  }
  
  
  void
  TimelineLayout::remove (TrackHeadWidget&)
  {
    headerPane_.disable();
  }

  void
  TimelineLayout::remove (TrackBody&)
  {
    bodyCanvas_.disable();
  }
  
  
  void
  TimelineLayout::rehook (TrackHeadWidget&)  noexcept
  {
    NOTREACHED ("TimelineLayout: top-Level must not be re-ordered");
  }
  
  void
  TimelineLayout::rehook (TrackBody&)  noexcept
  {
    NOTREACHED ("TimelineLayout: top-Level must not be re-ordered");
  }
  
  
  
}}// namespace stage::timeline
