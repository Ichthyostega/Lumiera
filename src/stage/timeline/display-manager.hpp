/*
  DISPLAY-MANAGER.hpp  -  abstraction to translate model structure and extension into screen layout

   Copyright (C)
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file display-manager.hpp
 ** Abstraction: service for the widgets to translate themselves into screen layout.
 ** 
 ** # Architecture
 ** 
 ** We strive to break up the whole process of controlling the layout into several
 ** local concerns, each of which can be made self contained. The backbone is formed
 ** by a recursive collaboration between two abstractions (interfaces)
 ** - the building blocks of the timeline expose the interface timeline::Element
 ** - the global timeline widget implements a timeline::LayoutManager interface
 ** 
 ** ## Display evaluation pass
 ** 
 ** Whenever the layout of timeline contents has to be (re)established, we trigger a recursive
 ** evaluation pass, which in fact is a tree walk. The layout manager maintains a DisplayEvaluation
 ** record, which is passed to the involved layout elements within the timeline. Each element in turn
 ** has the liability to walk its children and recursively initiate a nested evaluation. During that
 ** pass, we are able to transport and aggregate information necessary to give each element the
 ** adequate required amount of screen real estate.
 ** 
 ** ## Abstracted relative coordinate system
 ** 
 ** When treated naively, there would be serious risk for the DisplayManager to turn into some kind
 ** of GUI God class, which hooks deep into the innards of various widgets to effect a coordinated
 ** UI response. To defeat this threat, we are rather reluctant to expose the DisplayManager itself.
 ** Rather, essential functionality is decomposed into self contained sub interfaces, each targeted
 ** at some specific aspect of drawing or layout management. Especially, there is the system of
 ** interwoven DisplayHook / CanvasHook incantations.
 ** 
 ** Widgets are wrapped and decorated as \ref ViewHooked, which is tightly linked to the corresponding
 ** ViewHook interface. This collaboration allows to attach or "hook" the widget into some abstracted
 ** display and layout management framework, without exposing the internals of said layout management
 ** to the widget, allowing just do detach or to re-hook those widgets in a different order. And
 ** in a similar vein, the sub-interfaces CanvasHooked / CanvasHook expand the same principle
 ** to an attachment at some coordinate point `(x,y)`. In fact, CanvasHook allows to introduce
 ** a [relative coordinate system](\ref stage::timeline::RelativeCanvasHook), without the necessity
 ** for the actual widgets and their managing entities to be aware of these coordinate adjustments.
 ** This patterning serves as foundation for implementing UI structures recursively, leading to code
 ** based on local structural relations, which hopefully is easier to understand and maintain and
 ** extend in the long run.
 ** 
 ** \par Avoiding the double-indirect calls?
 ** In theory, it would be possible to avoid the double forwarding indirect calls in all the
 ** operations of RelativeCanvasHook: We'd have to provide a concrete mix-in to be used already
 ** within body-canvas-widget.cpp, and this mixin would hold a _direct reference_ to the embedded
 ** Gtk::Layout and provide an additional API to adjust the offset. However, the savings through
 ** such an implementation scheme would be dwarfed by the effort for actually rendering a widget.
 ** 
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** @todo WIP-WIP-WIP - drafting the DisplayEvaluation as of 3/2020
 ** @todo WIP as of 1/2021 about to build a first preliminary Clip representation
 ** 
 */


#ifndef STAGE_TIMELINE_DISPLAY_MANAGER_H
#define STAGE_TIMELINE_DISPLAY_MANAGER_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "stage/model/view-hook.hpp"
#include "stage/model/canvas-hook.hpp"
#include "lib/util.hpp"

#include <sigc++/signal.h>



namespace stage  {
namespace timeline {
  
  using util::max;
  
  using lib::time::TimeValue;
  class TrackHeadWidget;
  class TrackBody;
  
  
  /**
   * Interface: a compound of anchoring facilities.
   * With the help of view-hooking, some detail presentation component
   * or widget can attach itself into the overarching view context or canvas
   * of the Timeline, while remaining agnostic about actual structure or implementation
   * of this »display umbrella«. The local presentation component itself is then model::ViewHooked,
   * thereby managing its attachment to the global context automatically. As it turns out, within
   * the timeline display, we typically need a specific combination of such model::ViewHook, and
   * we need them recursively: the actual timeline::DisplayFrame, while attaching below such
   * DisplayViewHooks, is in turn itself again such a sub-anchor, allowing to attach
   * child display frames recursively, which is required to display sub-tracks.
   */
  class DisplayViewHooks
    {
    public:
      virtual ~DisplayViewHooks() { }    ///< this is an interface
      
      virtual model::ViewHook<TrackHeadWidget>& getHeadHook()  =0;
      virtual model::ViewHook<TrackBody>&       getBodyHook()  =0;
      virtual model::CanvasHook<Gtk::Widget>&   getClipHook()  =0;
    };
  
  
  
  /** @todo quick-n-dirty hack. Should consider the Range TS (C++20) */
  struct PixSpan
    {
      int b = 0;
      int e = 0;
      
      PixSpan() { }
      PixSpan(int begin, int end)
        : b{begin}, e{max (begin,end)}
        { }
      
      bool
      empty()  const
        {
          return e <= b;
        }
      
      int
      delta()  const
        {
          return e - b;
        }
    };
  
  /**
   * Interface for coordination of the overall timeline display.
   * @todo WIP-WIP as of 12/2019
   */
  class DisplayManager
    : util::NonCopyable
    , public DisplayViewHooks
    , public model::DisplayMetric
    {
      
      
    public:
      virtual ~DisplayManager();    ///< this is an interface
      
      /** cause a re-allocation of the complete layout */
      virtual void triggerDisplayEvaluation() =0;
      
      using SignalStructureChange = sigc::signal<void>;
      
      /**
       * signal to be invoked whenever the virtual structure of the
       * corresponding timeline changes, thus necessitating a new
       * arrangement of the timeline layout.
       * @remark connected to BodyCanvasWidget::slotStructureChange,
       *         which causes the drawing code to #triggerDisplayEvaluation()
       */
      SignalStructureChange signalStructureChange_;


      /** the overall horizontal pixel span to cover by this timeline */
      PixSpan
      getPixSpan()
        {
          return {translateTimeToPixels (coveredTime().start())
                 ,translateTimeToPixels (coveredTime().end())
                 };
        }
      
      
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_DISPLAY_MANAGER_H*/
