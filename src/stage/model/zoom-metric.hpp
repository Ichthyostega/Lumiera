/*
  ZOOM-METRIC.hpp  -  mix-in to provide a DisplayMetric based on a ZoomWindow

   Copyright (C)
     2022,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file zoom-metric.hpp
 ** Implementation building block to get the \ref DisplayMetric as defined through a
 ** \ref ZoomWindow. The latter is used as mediator to receive the various view manipulation
 ** signals from widgets, window decoration, zoom slider, navigator and zoom gestures, thereby
 ** ensuring consistent behaviour and capturing a history of zoom states. A current display metric
 ** can be derived directly from ZoomWindow settings, and is used by Clip Widgets and similar
 ** ElementBox entities attached to the canvas, to provide a time calibrated display and
 ** to control the visible extension of elements with distinct duration.
 ** 
 ** \par usage
 ** ZoomMetric should be mixed-into an implementation class and on top of some interface
 ** which derives from stage::model::DisplayMetric; a ZoomWindow instance is directly
 ** embedded and should be wired and controlled by the implementation class accordingly.
 ** 
 ** @see \ref ZoomWindow
 ** @see \ref DisplayMetric
 ** @see stage::timeline::TimelineLayout usage example
 ** @see ClipWidget::establishHorizontalExtension()
 ** 
 */


#ifndef STAGE_MODEL_ZOOM_METRIC_H
#define STAGE_MODEL_ZOOM_METRIC_H


#include "lib/time/timevalue.hpp"
#include "lib/meta/trait.hpp"
#include "stage/model/canvas-hook.hpp"
#include "stage/model/zoom-window.hpp"



namespace stage {
namespace model {
  
  using lib::time::TimeValue;
  using lib::time::TimeSpan;
  using lib::time::Offset;
  using lib::time::Time;
  
  
  /**
   * Mix-In to implement the DisplayMetric interface on top of
   * a ZoomWindow component, directly embedded here and thus
   * also accessible downstream.
   * @remark intended to inherit from and implement \ref DisplayMetric
   */
  template<class I>
  class ZoomMetric
    : public I
    {
      static_assert(lib::meta::is_Subclass<I, DisplayMetric>());
      
    protected:
      ZoomWindow zoomWindow_;
      
      ZoomMetric()
        : zoomWindow_{}
        { }
      
      
    public:
      /* ==== Interface : DisplayMetric ==== */
      
      TimeSpan
      coveredTime() const override
        {
          return zoomWindow_.overallSpan();
        }

      int
      translateTimeToPixels (TimeValue startTimePoint)  const override
        {
          return ZoomMetric::translateScreenDelta (Offset{zoomWindow_.overallSpan().start(), startTimePoint});
        }
      
      int
      translateScreenDelta (Offset timeOffset)  const override
        {
          auto pxOffset = rational_cast<int64_t> (zoomWindow_.px_per_sec() * _FSecs(timeOffset));
          //                         // use 64-bit for the division to prevent numeric-wrap...
          REQUIRE (abs (pxOffset) <= std::numeric_limits<int>::max());
          return int(pxOffset);
        }
      
      TimeValue
      applyScreenDelta(Time anchor, double deltaPx)  const override
        {
          return anchor + Offset{int64_t(deltaPx) / zoomWindow_.px_per_sec()};
        }
    };
  
  
}} // namespace stage::model
#endif /*STAGE_MODEL_ZOOM_METRIC_H*/
