/*
  TimelineLayout  -  global timeline layout management and display control

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file timeline-layout.cpp
 ** Implementation details of global timeline layout management.
 ** 
 ** @see track-body.cpp for mapping individual tracks onto the common canvas
 ** @see body-canvas-widget.cpp for painting track background and overlays
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/timeline-layout.hpp"
#include "stage/timeline/track-body.hpp"

#include "common/advice.hpp"
#include "lib/time/timevalue.hpp"

using lib::time::Time;
using lib::time::FSecs;
using lib::time::TimeSpan;


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
      /////////////////////////////////////////////////////////////////////////////////////////////TICKET #1264 : how to pick up initial zoom settings
      zoomWindow_.attachChangeNotification (signalStructureChange_);
      // make the ZoomWindow react on changes to the horizontal scrollbar pos
      bodyCanvas_.get_hadjustment()->property_value().signal_changed().connect(
          sigc::bind(sigc::mem_fun(*this, &TimelineLayout::syncZoomWindow)
                    ,bodyCanvas_.get_hadjustment()));
      // make the ZoomWindow react on changes to the window geometry
      bodyCanvas_.signal_size_allocate().connect(
          sigc::mem_fun(*this, &TimelineLayout::sizeZoomWindow));
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
  
  /**
   * @remark wiring of the Layout/Control structure for DisplayEvaluation can be done
   *         only after the TimelineCtonroller ctor also initialised the model root
   */
  void
  TimelineLayout::setupStructure (LayoutElement& forkRoot)
  {
    displayEvaluation_.attach (*this);
    displayEvaluation_.attach (forkRoot);
    displayEvaluation_.attach (bodyCanvas_);
  }

  Gtk::WidgetPath
  TimelineLayout::getBodyWidgetPath()  const
  {
    return bodyCanvas_.get_path();
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
  
  /**
   * TimelineLayout also participates itself in the DisplayEvaluation,
   * notably to set up the basic parameters for Zoom management 
   */
  void
  TimelineLayout::establishLayout (DisplayEvaluation&)
  {
      Time windowStart = zoomWindow_.visible().start();
      int pxOffset = translateTimeToPixels (windowStart);
      bodyCanvas_.get_hadjustment()->set_value(pxOffset);
  }
  
  void
  TimelineLayout::completeLayout (DisplayEvaluation&)
  {
    /* nothing to do for the collect-phase */
  }
  
  /**
   * Signal receiver (slot) to react on scrollbar changes.
   * Changes the logical window position in the ZoomWindow to reflect
   * the given #scrollPos, which is interpreted relative to the implicitly
   * known size of the timeline canvas in pixels.
   * @remark changes to ZoomWindow parameters cause notification of the listener,
   *         thereby triggering a new DisplayEvaluation; this in turn will invoke
   *         TimelineLayout::establishLayout() eventually, accommodating changes. 
   */
  void
  TimelineLayout::syncZoomWindow (PAdjustment hadj)
  {
    double pos = hadj->get_value();
    TimeValue windowStart = applyScreenDelta(zoomWindow_.overallSpan().start(), pos);
    zoomWindow_.setVisibleStart (windowStart);
  }
  
  /**
   * Signal receiver (slot) to react on changes of the window screen space allocation.
   * Whenever an actual change to the usable window width in pixels is detected,
   * the ZoomWindow will be re-calibrated, in turn leading to a DisplayEvaluation. 
   */
  void
  TimelineLayout::sizeZoomWindow (Gtk::Allocation& alloc)
  {
    int contentWidthPx = alloc.get_width();
    if (abs(contentWidthPx) != zoomWindow_.pxWidth())
      zoomWindow_.calibrateExtension (contentWidthPx);
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
