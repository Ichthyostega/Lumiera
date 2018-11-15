/*
  TIMELINE-RULER.hpp  -  time axis ruler widget

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


/** @file timeline-ruler.hpp
 ** The ruler at the top of the timeline display to represent the time axis.
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */


#ifndef STAGE_WIDGET_TIMELINE_RULER_H
#define STAGE_WIDGET_TIMELINE_RULER_H

#include "stage/gtk-base.hpp"
#include "lib/time/timevalue.hpp"
#include "stage/widget/timeline/timeline-state.hpp"


namespace stage {
namespace widget {
    class TimelineWidget;
  
    
  namespace timeline {
  
  class TimelineViewWindow;
  
  
  /**
   * A sub-widget of the TimelineWidget.
   * This class implements a ruler which is placed
   * along the top edge of the timeline.
   */
  class TimelineRuler
    : public Gtk::DrawingArea
    {
    public:
      TimelineRuler (gui::widget::TimelineWidget& timeline_widget);
      
      /**
       * Sets offset of the mouse chevron
       * @param offset displacement of the mouse chevron in pixels from the
       *               left edge of the widget. If offset is less than 0 or greater than
       *               the width, the chevron will not be visible.
       */
      void set_mouse_chevron_offset (int offset);
      
      
      
    private: /* ===== Events ===== */
      /**
       * An event handler for when the view window of the timeline changes.
       * @remarks Causes the ruler to be redrawn from scratch. The cached
       * ruler backdrop is destroyed and redrawn.
       */
      void on_update_view();
      
      /**
       * An event handler for when the widget is realized.
       */
      void on_realize();
      
      /**
       * An event handler for when the window must be redrawn.
       */
      bool on_expose_event (GdkEventExpose *event);
      
      /**
       * The event handler for button press events.
       */
      bool on_button_press_event (GdkEventButton* event);
      
      /**
       * The event handler for button release events.
       */
      bool on_button_release_event (GdkEventButton* event);
      
      /**
       * The event handler for mouse move events.
       */
      bool on_motion_notify_event (GdkEventMotion *event);
      
      /**
       * The handler for when the widget must calculate it's new shape.
       */
      void on_size_request (Gtk::Requisition *requisition);
      
      /**
       * The handler for when the widget must take the size of a given
       * area.
       */
      void on_size_allocate (Gtk::Allocation& allocation);
      
      /**
       * The event handler for when the TimelineWidget's state is switched.
       */
      void on_state_changed (shared_ptr<TimelineState> newState);
      
      
      
    private: /* ===== Internal Methods ===== */
      
      /**
       * As the user drags, this function is called to update the position
       * of the moving end of the playback period.
       */
      void set_leading_x (const int x);
      
      /**
       * Draws the ruler graduations.
       * @param cr Cairo context to draw the ruler into.
       * @param ruler_rect The area of the ruler widget.
       */
      void draw_ruler (Cairo::RefPtr<Cairo::Context> cr, const Gdk::Rectangle ruler_rect);
      
      /**
       * Overlays the mouse chevron.
       * @param cr Cairo context to draw the chevron into.
       * @param ruler_rect The area of the ruler widget.
       */
      void draw_mouse_chevron (Cairo::RefPtr<Cairo::Context> cr, const Gdk::Rectangle ruler_rect);
        
      /**
       * Overlays the currently selected period.
       * @param cr The Cairo context to draw the selection into.
       * @param ruler_rect The area of the ruler widget.
       */
      void draw_selection (Cairo::RefPtr<Cairo::Context> cr, const Gdk::Rectangle ruler_rect);
      
      /**
       * Overlays the currently selected playback period.
       * @param cr The Cairo context to draw the period into.
       * @param ruler_rect The area of the ruler widget.
       */
      void draw_playback_period (Cairo::RefPtr<Cairo::Context> cr, const Gdk::Rectangle ruler_rect);
      
      /**
       * Overlays the current playback point.
       * @param cr The Cairo context to draw the period into.
       * @param ruler_rect The area of the ruler widget.
       */
      void draw_playback_point (Cairo::RefPtr<Cairo::Context> cr, const Gdk::Rectangle ruler_rect);
      
      /**
       * After notification of a timeline state switch
       * do any local adjustments to adapt to the new state 
       */
      void propagateStateChange();
      
      /**
       * Given the current zoom, this function calculates the preiod
       * between major graduations on the ruler scale.
       * @return The period as a gavl_time_t
       */
      gavl_time_t calculate_major_spacing()  const;
      
      /**
       * Access current timeline view window
       */
      TimelineViewWindow& viewWindow()  const;
      
      /**
       * Registers all the styles that this class will respond to.
       */
      void register_styles()  const;
      
      /**
       * Reads styles from the present stylesheet.
       */
      void read_styles();
      
    private:
      
      // State values
      
      /**
       * This value is set to true if the user is dragging with the left
       * mouse button.
       */
      bool isDragging;
      
      /**
       * During a selection drag, one end of the selection is moving with
       * the mouse, the other is pinned. pinnedDragTime specifies the time
       * of that point.
       */
      lib::time::TimeVar pinnedDragTime;
      
      // Indicated values
      /**
       * The offset from the left of the control in pixels to draw the
       * mouse chevron. If offset is less than 0 or greater than
       * the width, the chevron will not be visible.
       */
      int mouseChevronOffset;
      
      // Style values
      int annotationHorzMargin;
      int annotationVertMargin;
      int majorTickHeight;
      int minorLongTickHeight;
      int minorShortTickHeight;
      int minDivisionWidth;
      int mouseChevronSize;
      int selectionChevronSize;
      
      Cairo::RefPtr<Cairo::SolidPattern> playbackPointColour;
      float playbackPointAlpha;
      int playbackPointSize;
        
      Cairo::RefPtr<Cairo::SolidPattern> playbackPeriodArrowColour;
      float playbackPeriodArrowAlpha;
      int playbackPeriodArrowSize;
      int playbackPeriodArrowStemSize;
      
      /**
       * The owner widget
       */
      gui::widget::TimelineWidget &timelineWidget;
      
      /**
       * the currently active timeline state object 
       */
      shared_ptr<TimelineState> timelineState;
      
      /**
       * The caches image of the ruler, over which the chevrons etc. will
       * be drawn.
       * @remarks This backdrop is cached because it changes relatively
       * infrequently in comparison to the overlays, thus improving
       * performance somewhat.
       */
      Cairo::RefPtr<Cairo::ImageSurface> rulerImage;
    };
  
  
}}}// namespace stage::widget::timeline
#endif /*STAGE_WIDGET_TIMELINE_RULER_H*/

