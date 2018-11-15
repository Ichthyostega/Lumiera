/*
  TIMELINE-IBEAM-TOOL.hpp  -  Selection marking tool

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

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


/** @file timeline-ibeam-tool.hpp
 ** A tool to select ranges in the timeline
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */

#ifndef STAGE_WIDGET_TIMELINE_IBEAM_TOOL_H
#define STAGE_WIDGET_TIMELINE_IBEAM_TOOL_H

#include "stage/widget/timeline/timeline-tool.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time/control.hpp"


namespace stage {
namespace widget {
namespace timeline {
  
  using lib::time::TimeVar;
  using lib::time::TimeSpan;
  using lib::time::Control;
  
  
  /**
   * A helper class to implement the timeline i-beam tool
   */
  class IBeamTool
    : public Tool
    {
    public:
      /**
       * @param timeline_body owner timeline body object
       */
      IBeamTool (TimelineBody& timeline_body);
      
      
      ToolType get_type()  const;
      
    protected:
     ~IBeamTool();
      
      
      Glib::RefPtr<Gdk::Cursor> get_cursor()  const;
      
      
    protected:
      /**
       * The event handler for button press events.
       */
      void on_button_press_event (GdkEventButton* event);
      
      /**
       * The event handler for button release events.
       */
      void on_button_release_event (GdkEventButton* event);
      
      /**
       * The event handler for mouse move events.
       */
      void on_motion_notify_event (GdkEventMotion *event);
      
      
    private: /* ===== Internal Event Handlers ===== */
      
      /**
       * An internal event handler, which is called
       * when the scroll slide timer calls it.
       */
      bool on_scroll_slide_timer();
      
      
    private: /* ===== Internal Methods ===== */
      
      /**
       * As the user drags, this function is called
       * to update the position of the moving end of the selection.
       */
      void set_leading_x (const int x);
      
      /**
       * Begins, or continues a scroll slide at a given rate
       * @param scroll_slide_rate The distance to slide every timer event
       *        in units of 1/256th of the view width.
       */
      void begin_scroll_slide (int scroll_slide_rate);
      
      /**
       * Ends a scroll slide, and disconnects the slide timer
       */
      void end_scroll_slide();
      
      /**
       * Determines if the cursor is hovering
       * over the start of the selection.
       */
      bool is_mouse_in_start_drag_zone()  const;
      
      /**
       * Determines if the cursor is hovering
       * over the end of the selection.
       */
      bool is_mouse_in_end_drag_zone()  const;
      
      
    private: /* ==== Enums ===== */
      
      /**
       * An enum used to represent the type of drag currently take place.
       */
      enum DragType
        {
          /**
           * No drag is occurring
           */
          None,
          
          /**
           * A selection drag is occurring.
           * @remarks The position of one end of the selection was set at
           * mouse-down of the drag, and the other end is set by
           * drag-release.
           */
          Selection,
          
          /**
           * The start of the selection is being dragged.
           */
          GrabStart,
          
          /**
           * The end of the selection is being dragged.
           */
          GrabEnd
        };
      
      /* ==== Internals ===== */
      Control<TimeSpan> selectionControl;
      
      /**
       * Specifies the type of drag currently taking place.
       */
      DragType dragType;
      
      /**
       * During a selection drag, one end of the selection is moving with
       * the mouse, the other is pinned. pinnedDragTime specifies the time
       * of that point.
       */
      TimeVar pinnedDragTime;
      
      /**
       * This connection is used to represent the timer which causes scroll
       * sliding to occur.
       * @remarks Scroll sliding is an animated scroll which occurs when
       * the user drags a selection outside the area of the timeline body.
       */
      sigc::connection scrollSlideEvent;
      
      /**
       * Specifies the rate at which scroll sliding is currently taking
       * place.
       */
      int scrollSlideRate;
      
      /* ===== Constants ===== */
      /**
       * DragZoneSize defines the width of the zone near to the end of the
       * selection in which dragging will cause the selection to resize.
       * @remarks The selection marque can be resized by dragging the ends
       * of it. Special cursors are shown when the mouse is in this region.
       */
      static const int DragZoneWidth;
      
      /**
       * The amount to divide the mouse overshoot by to produce the slide
       * scroll rate.
       * @remarks Smaller values cause faster scrolling.
       */
      static const int ScrollSlideRateDivisor;
      
      /**
       * The interval between scroll slide events in ms.
       */
      static const int ScrollSlideEventInterval;
    };
  
  
}}}// namespace stage::widget::timeline
#endif /*STAGE_WIDGET_TIMELINE_IBEAM_TOOL_H*/
