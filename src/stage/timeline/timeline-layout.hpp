/*
  TIMELINE-LAYOUT.hpp  -  global timeline layout management and display control

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

*/


/** @file timeline-layout.hpp
 ** A core service of the timeline UI to ensure consistent display and layout
 ** of all components within the timeline. The content of the timeline is organised
 ** into several nested collections, possibly holding several thousand individual elements.
 ** Together with the requirement to show media elements within a precisely defined, regular
 ** time grid, this forces us to control various aspects of the layout and display style
 ** manually, instead of letting the UI toolkit work out the details automatically. Note
 ** especially that the typical UI toolkit is not prepared to handle such a high number of
 ** individual elements smoothly. Even more so, when most of those elements are not even
 ** visible most of the time. Unfortunately, doing a manual display forces us to perform
 ** the task usually serviced by a table grid widget, that is, to keep flexible elements
 ** aligned in columns or (as is the case here) in rows. Basically we split our display
 ** horizontally, where the right part is just a custom drawing canvas. Consequently we
 ** have to ensure all tracks are perfectly aligned between the track header pane and
 ** the scrollable working space in the timeline body display.
 ** 
 ** # Architecture
 ** 
 ** A naive approach would have a global layout manager drill down into some model storage
 ** and reach into the components to manipulate and adjust the layout to fit. Yet however
 ** straight forward and adequate this might seem, following this routine is a recipe for
 ** disaster, since this procedure now ties and links together details scattered all over
 ** the model into a huge global process, carried out at a single code location. Any further
 ** extension or evolution of details of the UI presentation are bound to be worked into this
 ** core global piece of code, which soon becomes brittle, hard to understand and generally
 ** a liability and maintenance burden. We have seen this happen in numerous existing
 ** code bases (and in fact even our own initial approach started to go down that route).
 ** Thus we strive to break up the whole process of controlling the layout into several
 ** local concerns, each of which can be made self contained. The backbone is formed by
 ** a recursive collaboration between two abstractions (interfaces)
 ** - the building blocks of the timeline expose the interface timeline::Element (TODO 12/18 really?)
 ** - the global timeline widget implements a timeline::LayoutManager interface (TODO 12/18 work out the crucial point of that interface!)
 ** 
 ** ## Display evaluation pass
 ** 
 ** Whenever the layout of timeline contents has to be (re)established, we trigger a recursive
 ** evaluation pass, which in fact is a tree walk. The layout manager creates a DisplayEvaluation
 ** record, which is passed to the [Element's allocate function](\ref Element::allocate). The element
 ** in turn has the liability to walk its children and recursively initiate a nested evaluation
 ** by invoking DisplayEvaluation::evaluateChild(Element), which in turn calls back to
 ** LayoutManager::evaluate() to initiate a recursive evaluation pass. Within the recursively
 ** created DisplayEvaluation elements, we are able to transport and aggregate information
 ** necessary to give each element it' screen allocation. And this in turn allows us to
 ** decide upon a suitable display strategy for each individual element, within a local
 ** and self-contained context.
 ** 
 ** For this to work, the _element_ can not be the actual widget, since the result of this whole
 ** process might be to create or retract an actual GTK widget. For this reason, the timeline
 ** layout management relies on _Presenter_ entities, which in turn control a mostly passive
 ** view -- our solution in fact relies on some flavour of the
 ** [MVP pattern](https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93presenter) here.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** 
 */


#ifndef STAGE_TIMELINE_TIMELINE_LAYOUT_H
#define STAGE_TIMELINE_TIMELINE_LAYOUT_H

#include "stage/gtk-base.hpp"
#include "stage/timeline/display-manager.hpp"
#include "stage/timeline/display-evaluation.hpp"
#include "stage/timeline/header-pane-widget.hpp"
#include "stage/timeline/body-canvas-widget.hpp"
#include "stage/model/zoom-metric.hpp"
#include "stage/model/canvas-hook.hpp"
#include "stage/model/view-hook.hpp"
#include "lib/time/timevalue.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace stage  {
namespace timeline {
  
  using lib::time::Time;
  
  class LayoutElement;
  class TrackHeadWidget;
  class TrackBody;
  
  
  /**
   * Top-level anchor point for the timeline display (widgets).
   * The central entity to organise concerns relevant for the presentation of the
   * Timeline as a whole, as opposed to rendering individual tracks as part of the Timeline.
   * @todo WIP-WIP as of 10/2018
   */
  class TimelineLayout
    : public model::ZoomMetric<DisplayManager>
    , protected model::ViewHook<TrackHeadWidget>
    , protected model::ViewHook<TrackBody>
    {
      Glib::PropertyProxy<int> paneSplitPosition_;
      
      BodyCanvasWidget bodyCanvas_;
      HeaderPaneWidget headerPane_;
      
      DisplayEvaluation displayEvaluation_;
      /////////////////////////////////////////////////////////////////////////////////////////////TICKET #1019 : need a "ZoomWindow" here to manage the visible area
      
    public:
      TimelineLayout (Gtk::Paned&);
     ~TimelineLayout();
      
      /** @internal anchor the display of the root track into the two display panes */
      void installRootTrack (TrackHeadWidget&,TrackBody&);
      void wireForkRoot (LayoutElement&);
      
      Gtk::WidgetPath getBodyWidgetPath()  const;
      
      
    protected: /* ==== Interface: LayoutManager===== */
      
      void triggerDisplayEvaluation()  override;
      
      
    protected: /* ==== Interface: DisplayViewHooks===== */
      
      model::ViewHook<TrackHeadWidget>& getHeadHook()  override { return *this;       };
      model::ViewHook<TrackBody>&       getBodyHook()  override { return *this;       };
      model::CanvasHook<Gtk::Widget>&   getClipHook()  override { return bodyCanvas_; };
      
    protected: /* ==== Interface: ViewHook ===== */
      
      void hook   (TrackHeadWidget&)          override;
      void remove (TrackHeadWidget&)          override;
      void rehook (TrackHeadWidget&) noexcept override;
      
      void hook   (TrackBody&)          override;
      void remove (TrackBody&)          override;
      void rehook (TrackBody&) noexcept override;

    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_TIMELINE_LAYOUT_H*/
