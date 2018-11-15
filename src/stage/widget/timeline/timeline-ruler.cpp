/*
  TimelineRuler  -  time axis ruler widget

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

* *****************************************************/


/** @file timeline-ruler.cpp
 ** The ruler at the top of the timeline display to represent the time axis.
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */


#include "stage/widget/timeline/timeline-ruler.hpp"
#include "stage/widget/timeline-widget.hpp"
#include "stage/workspace/ui-style.hpp"
#include "stage/draw/cairo-util.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time.h"

#include <cairomm/cairomm.h>

using namespace Gtk;     ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace Cairo;     ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace stage;     ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace stage::widget;     ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace stage::widget::timeline;     ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

using std::shared_ptr;
using gui::draw::CairoUtil;
using lib::time::Time;
using lib::time::TimeVar;


namespace stage {
namespace widget {
namespace timeline {
  
  
  TimelineRuler::TimelineRuler (TimelineWidget &timeline_widget)
    : Glib::ObjectBase("TimelineRuler")
    , isDragging(false)
    , pinnedDragTime(Time::ZERO)
    , mouseChevronOffset(0)
    , annotationHorzMargin(0)
    , annotationVertMargin(0)
    , majorTickHeight(0)
    , minorLongTickHeight(0)
    , minorShortTickHeight(0)
    , minDivisionWidth(100)
    , mouseChevronSize(5)
    , selectionChevronSize(5)
    , playbackPointAlpha(0.5f)
    , playbackPointSize(12)
    , playbackPeriodArrowAlpha(0.5f)
    , playbackPeriodArrowSize(10)
    , playbackPeriodArrowStemSize(3)
    , timelineWidget(timeline_widget)
    {
      // Connect up some events
      timeline_widget.state_changed_signal().connect(
        sigc::mem_fun(this, &TimelineRuler::on_state_changed) );
      
      // Install style properties
      register_styles();
    }
  
  
  TimelineViewWindow&
  TimelineRuler::viewWindow() const
  {
    REQUIRE(timelineState);
    return timelineState->getViewWindow();
  }
  
  
  void
  TimelineRuler::set_mouse_chevron_offset(int offset)
  {
    mouseChevronOffset = offset;
    queue_draw();
  }
  
  
  void
  TimelineRuler::on_update_view()
  {
    rulerImage.clear();
    queue_draw();
  }
  
  
  void
  TimelineRuler::on_realize()
  {
    Widget::on_realize();
    
    // Set event notifications
    add_events(Gdk::POINTER_MOTION_MASK |
      Gdk::SCROLL_MASK |
      Gdk::BUTTON_PRESS_MASK |
      Gdk::BUTTON_RELEASE_MASK);
    
    // Load styles
    read_styles();
  }
  
  
  bool
  TimelineRuler::on_expose_event (GdkEventExpose* event)
  {
    REQUIRE(event != NULL);
    
    // This is where we draw on the window
    Glib::RefPtr<Gdk::Window> window = get_window();
    if(!window)
      return false;
    
    if (timelineState)
      {
        // Prepare to render via cairo      
        const Allocation allocation = get_allocation();
        
        Cairo::RefPtr<Context> cr = window->create_cairo_context();
        REQUIRE(cr);
        
        // Draw the ruler
        if(!rulerImage)
          {
            // We have no cached rendering - it must be redrawn
            // but do we need ro allocate a new image?
            if(!rulerImage ||
              rulerImage->get_width() != allocation.get_width() ||
              rulerImage->get_height() != allocation.get_height())
              rulerImage = ImageSurface::create(FORMAT_RGB24,
                allocation.get_width(), allocation.get_height());
                
            ENSURE(rulerImage);
              
            Cairo::RefPtr<Context> image_cairo = Context::create(rulerImage);
            ENSURE(image_cairo);
            
            draw_ruler(image_cairo, allocation);
          }
        
        // Draw the cached ruler image
        cr->set_source(rulerImage, 0, 0);
        cr->paint();
        
        // Draw the overlays
        draw_mouse_chevron(cr, allocation);
        draw_selection(cr, allocation);
        draw_playback_period(cr, allocation);
        draw_playback_point(cr, allocation);
      }
    
    return true;
  }
  
  
  bool
  TimelineRuler::on_button_press_event (GdkEventButton* event)
  {
    REQUIRE(event != NULL);
    
    if (timelineState)
      {
        if(event->button == 1)
        {
          pinnedDragTime = viewWindow().x_to_time(event->x);
          isDragging = true;
        }
      }
    
    return true;
  }
  
  
  bool
  TimelineRuler::on_button_release_event (GdkEventButton* event)
  {
    REQUIRE(event != NULL);
    
    if(event->button == 1)
    {
      isDragging = false;
      timelineWidget.on_playback_period_drag_released();
    }
    
    return true;
  }
  
  
  bool
  TimelineRuler::on_motion_notify_event (GdkEventMotion *event)
  {
    REQUIRE(event != NULL);
    
    set_mouse_chevron_offset(event->x);
    
    if(isDragging)
      set_leading_x(event->x);
    
    return true;
  }
  
  
  void
  TimelineRuler::on_size_request (Gtk::Requisition *requisition)
  {
    REQUIRE(requisition != NULL);
    
    // Initialise the output parameter
    *requisition = Gtk::Requisition();
    
    requisition->width = 0; 
    get_style_property("height", requisition->height);
  }
  
  
  void
  TimelineRuler::on_size_allocate(Gtk::Allocation& allocation)
  {
    Widget::on_size_allocate(allocation);
    rulerImage.clear(); // The widget has changed size - redraw
  }
  
  
  void
  TimelineRuler::on_state_changed (shared_ptr<TimelineState> newState)
  {
    REQUIRE (newState);
    timelineState = newState;
    
    propagateStateChange();
  }
  
  
  void
  TimelineRuler::propagateStateChange()
  {
    // Connect up some events
    viewWindow().changed_signal().connect(
      sigc::mem_fun(this, &TimelineRuler::on_update_view) );
    
    // Redraw
    on_update_view();
  }
  
  
  
  void
  TimelineRuler::set_leading_x (const int x)
  {
    if (timelineState)
      {
        TimeVar newStartPoint (viewWindow().x_to_time(x));
        Offset selectionLength (pinnedDragTime, newStartPoint);
        
        if (newStartPoint > pinnedDragTime)
          newStartPoint=pinnedDragTime; // use the smaller one as selection start
        
        timelineState->setPlaybackPeriod (Mutation::changeTime(newStartPoint)      );
        timelineState->setPlaybackPeriod (Mutation::changeDuration(selectionLength));
                                     //////////////////////////////////////////////////////TICKET #797 : this looks cheesy. Should provide a single Mutation to change all at once
                                                                       ////////////////////TODO        : code duplication with timeline-ibeam-tool 205      
     }
  }
  
  
  void
  TimelineRuler::draw_ruler (Cairo::RefPtr<Cairo::Context> cr, const Gdk::Rectangle ruler_rect)
  {
    REQUIRE(cr);
    REQUIRE(ruler_rect.get_width() > 0);
    REQUIRE(ruler_rect.get_height() > 0);
    
                                     /////////////////////////////TICKET #861 : what part of these calculation could be centralised within TimelineViewWindow?
    const TimelineViewWindow &window = viewWindow();
    const gavl_time_t left_offset = _raw(window.get_time_offset());
    const int64_t time_scale = window.get_time_scale();
    
    // Preparation steps
    const int height = ruler_rect.get_height();
    Glib::RefPtr<Pango::Layout> pango_layout = create_pango_layout("");
#if false ///////////////////////////////////////////////////////////////////TODO needs to be ported to GTK-3 (or implemented in a different way)
    Glib::RefPtr<Style> style = get_style();
    
    // Render the background, and clip inside the area
    Gdk::Cairo::set_source_color(cr, style->get_bg(STATE_NORMAL));
#endif
    cr->rectangle(0, 0, 
      ruler_rect.get_width(), ruler_rect.get_height());
    cr->fill_preserve();
    cr->clip();
    
    // Make sure we don't have impossible zoom
    if(time_scale <= 0)
      return;
#if false
    // Render ruler annotations
    Gdk::Cairo::set_source_color(cr, style->get_fg(STATE_NORMAL));
#endif
    
    const gavl_time_t major_spacing = calculate_major_spacing();
    const gavl_time_t minor_spacing = major_spacing / 10;
    
    int64_t time_offset = left_offset - left_offset % major_spacing;
    if(left_offset < 0)
      time_offset -= major_spacing;
    
    int x = 0;
    const int64_t x_offset = left_offset / time_scale;
    
    do
      {
        x = (int)(time_offset / time_scale - x_offset);
        
        cr->set_line_width(1);
        
        if(time_offset % major_spacing == 0)
          {
            // Draw the major grid-line
            cr->move_to(x + 0.5, height - majorTickHeight);
            cr->line_to(x + 0.5, height);
            cr->stroke();
            
            // Draw the text
            pango_layout->set_text(lumiera_tmpbuf_print_time(time_offset));           ///////////////TICKET #750 : should delegate to a Timecode format here
            cr->move_to(annotationHorzMargin + x, annotationVertMargin);
            pango_layout->add_to_cairo_context(cr);
            cr->fill();
          }
        else
          {
            // Draw the long or short minor grid-line
            if(time_offset % (minor_spacing * 2) == 0)           
              cr->move_to(x + 0.5, height - minorLongTickHeight);
            else
              cr->move_to(x + 0.5, height - minorShortTickHeight);
            
            cr->line_to(x + 0.5, height);
            cr->stroke();
          }
          
        time_offset += minor_spacing;
      }
    while(x < ruler_rect.get_width());
  }
  
  
  void
  TimelineRuler::draw_mouse_chevron (Cairo::RefPtr<Cairo::Context> cr, const Gdk::Rectangle ruler_rect)
  {
    REQUIRE(cr);
    REQUIRE(ruler_rect.get_width() > 0);
    REQUIRE(ruler_rect.get_height() > 0);
    
    // Is the mouse chevron in view?
    if(mouseChevronOffset < 0 ||
      mouseChevronOffset >= ruler_rect.get_width())
      return;
#if false
    // Set the source colour
    Glib::RefPtr<Style> style = get_style();
    Gdk::Cairo::set_source_color(cr, style->get_fg(STATE_NORMAL));
#endif
    cr->move_to(mouseChevronOffset + 0.5,
      ruler_rect.get_height());
    cr->rel_line_to(-mouseChevronSize, -mouseChevronSize);
    cr->rel_line_to(2 * mouseChevronSize, 0);
      
    cr->fill();
  }
  
  
  void
  TimelineRuler::draw_selection (Cairo::RefPtr<Cairo::Context> cr, const Gdk::Rectangle ruler_rect)
  {
    REQUIRE(cr);
    REQUIRE(ruler_rect.get_width() > 0);
    REQUIRE(ruler_rect.get_height() > 0);
    REQUIRE(timelineState);
    
    const TimelineViewWindow &window = timelineState->getViewWindow();
    
#if false
    Glib::RefPtr<Style> style = get_style();
    Gdk::Cairo::set_source_color(cr, style->get_fg(STATE_NORMAL));
#endif
    
    // Draw the selection start chevron
    const int a = 1 + window.time_to_x(timelineState->getSelectionStart());
    if(a >= 0 && a < ruler_rect.get_width())
      {
        cr->move_to(a, ruler_rect.get_height());
        cr->rel_line_to(0, -selectionChevronSize);
        cr->rel_line_to(-selectionChevronSize, 0);
        cr->fill();
      }
    
    // Draw the selection end chevron
    const int b = window.time_to_x(timelineState->getSelectionEnd());
    if(b >= 0 && b < ruler_rect.get_width())
      {
        cr->move_to(b, ruler_rect.get_height());
        cr->rel_line_to(0, -selectionChevronSize);
        cr->rel_line_to(selectionChevronSize, 0);
        cr->fill();
      }
  }
  
  
  void
  TimelineRuler::draw_playback_period(Cairo::RefPtr<Cairo::Context> cr, const Gdk::Rectangle ruler_rect)
  {
    REQUIRE(cr);
    REQUIRE(ruler_rect.get_width() > 0);
    REQUIRE(ruler_rect.get_height() > 0);
    REQUIRE(timelineState);
    
    const TimelineViewWindow &window = timelineState->getViewWindow();
    
    // Calculate coordinates
    const float halfSize = playbackPeriodArrowSize / 2;
    
    const float a = 1.5f + window.time_to_x(timelineState->getPlaybackPeriodStart());
    
    const float b = a + halfSize;
    const float d = 0.5f + window.time_to_x(timelineState->getPlaybackPeriodEnd());
    
    const float c = d - halfSize;
    
    const float e = ruler_rect.get_height() - playbackPeriodArrowSize - 0.5f;
    const float f = e + (playbackPeriodArrowSize - playbackPeriodArrowStemSize) / 2;
    const float g = ruler_rect.get_height() - playbackPeriodArrowSize / 2  - 0.5f;
    const float i = ruler_rect.get_height() - 0.5f;
    const float h = i - (playbackPeriodArrowSize - playbackPeriodArrowStemSize) / 2;
    
    // Contruct the path
    if(d - a >= playbackPeriodArrowSize)
      {
        // Draw an arrow: <===>
        cr->move_to(a, g);
        cr->line_to(b, e);
        cr->line_to(b, f);
        cr->line_to(c, f);
        cr->line_to(c, e);
        cr->line_to(d, g);
        cr->line_to(c, i);
        cr->line_to(c, h);  
        cr->line_to(b, h);
        cr->line_to(b, i);
        cr->close_path();
      }
    else
      { 
        // The space is too narrow for an arrow, so draw calipers: > < 
        cr->move_to(a, g);
        cr->rel_line_to(-halfSize, -halfSize);
        cr->rel_line_to(0, playbackPeriodArrowSize);
        cr->close_path();
        
        cr->move_to(d, g);
        cr->rel_line_to(halfSize, -halfSize);
        cr->rel_line_to(0, playbackPeriodArrowSize);
        cr->close_path();
      }
    
    // Fill
    cr->set_source (CairoUtil::pattern_set_alpha (playbackPeriodArrowColour, playbackPeriodArrowAlpha));
    cr->fill_preserve();
    
    // Stroke
    cr->set_source(playbackPeriodArrowColour);
    cr->set_line_width(1);
    cr->stroke();
  }
  
  
  void
  TimelineRuler::draw_playback_point (Cairo::RefPtr<Cairo::Context> cr, const Gdk::Rectangle ruler_rect)
  {
    REQUIRE(cr);
    REQUIRE(ruler_rect.get_width() > 0);
    REQUIRE(ruler_rect.get_height() > 0);
    REQUIRE(timelineState);
    
    if (!timelineState->isPlaying()) return;
    
    Time point = timelineState->getPlaybackPoint();
    const int x = viewWindow().time_to_x(point);
    
    cr->move_to(x + 0.5, ruler_rect.get_height());
    cr->rel_line_to(0, -playbackPointSize);
    cr->rel_line_to(playbackPointSize / 2.0, playbackPointSize / 2.0);
    cr->close_path();  
    
    // Fill
    cr->set_source (CairoUtil::pattern_set_alpha (playbackPeriodArrowColour, playbackPeriodArrowAlpha));
    cr->fill_preserve();
    
    // Stroke
    cr->set_source(playbackPointColour);
    cr->set_line_width(1);
    cr->stroke();
  }
  
  
  gavl_time_t 
  TimelineRuler::calculate_major_spacing()  const
  {
    unsigned int i;
    
                                     /////////////////////////////TICKET #861 : couldn't that be a library function in TimelineViewWindow?
    const gavl_time_t major_spacings[] = {
        GAVL_TIME_SCALE / 1000,    
        GAVL_TIME_SCALE / 400,
        GAVL_TIME_SCALE / 200,
        GAVL_TIME_SCALE / 100,    
        GAVL_TIME_SCALE / 40,
        GAVL_TIME_SCALE / 20,
        GAVL_TIME_SCALE / 10,    
        GAVL_TIME_SCALE / 4,
        GAVL_TIME_SCALE / 2,
        GAVL_TIME_SCALE,
        2l * GAVL_TIME_SCALE,
        5l * GAVL_TIME_SCALE,
        10l * GAVL_TIME_SCALE,
        15l * GAVL_TIME_SCALE,
        30l * GAVL_TIME_SCALE,
        60l * GAVL_TIME_SCALE,
        2ll  * 60ll * GAVL_TIME_SCALE,
        5ll  * 60ll * GAVL_TIME_SCALE,
        10ll * 60ll * GAVL_TIME_SCALE,
        15ll * 60ll * GAVL_TIME_SCALE,
        30ll * 60ll * GAVL_TIME_SCALE,
        60ll * 60ll * GAVL_TIME_SCALE
      };     
    
    const int64_t time_scale = viewWindow().get_time_scale();
    
    for(i = 0; i < sizeof(major_spacings) / sizeof(gavl_time_t); i++)
      {
        const int64_t division_width = major_spacings[i] / time_scale;
        
        if(division_width > minDivisionWidth)
          break;
      }
    
    return major_spacings[i];
  }
  
  
  void
  TimelineRuler::register_styles()  const
  {
    static bool registered = false;
    if(registered) return;
    registered = true;
    
    GtkWidgetClass *klass = GTK_WIDGET_CLASS(G_OBJECT_GET_CLASS(gobj()));
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_int("height",
      "Height of the Ruler Widget",
      "The height of the ruler widget in pixels.",
      0, G_MAXINT, 18, G_PARAM_READABLE));
          
    gtk_widget_class_install_style_property(klass,
      g_param_spec_int("major_tick_height",
      "Height of Major Ticks",
      "The length of major ticks in pixels.",
      0, G_MAXINT, 18, G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_int("minor_long_tick_height",
      "Height of Long Minor Ticks",
      "The length of long minor ticks in pixels.",
      0, G_MAXINT, 6, G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_int("minor_short_tick_height",
      "Height of Short Minor Ticks",
      "The length of short minor ticks in pixels.",
      0, G_MAXINT, 3, G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_int("annotation_horz_margin",
      "Horizontal margin around annotation text",
      "The horizontal margin around the annotation text in pixels.",
      0, G_MAXINT, 3,
      G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_int("annotation_vert_margin",
      "Vertical margin around annotation text",
      "The vertical margin around the annotation text in pixels.",
      0, G_MAXINT, 0, G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_int("min_division_width",
      "Minimum Division Width",
      "The minimum distance in pixels that two major division may approach.",
      0, G_MAXINT, 100, G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_int("mouse_chevron_size",
      "Mouse Chevron Size",
      "The height of the mouse chevron in pixels.",
      0, G_MAXINT, 5, G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_int("selection_chevron_size",
      "Selection Chevron Size",
      "The height of the selection chevrons in pixels.",
      0, G_MAXINT, 5, G_PARAM_READABLE));
    
    // ----- Playback Marker Styling ----- //
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_boxed("playback_point_colour",
        "Playback Marker Colour",
        "The colour of playback marker",
        GDK_TYPE_COLOR, G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_float("playback_point_alpha", "Playback Arrow Alpha",
      "The transparency of the playback marker.",
      0, 1.0, 0.5, G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_int("playback_point_size",
      "Playback Marker Size",
      "The height of the playback marker in pixels.",
      0, G_MAXINT, 12, G_PARAM_READABLE));
    
    // ----- Playback Period Arrow Styling ----- //
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_boxed("playback_period_arrow_colour",
        "Playback Period Arrow Colour",
        "The colour of the playback period arrow",
        GDK_TYPE_COLOR, G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_float("playback_period_arrow_alpha",
      "Playback Period Arrow Alpha",
      "The transparency of the playback period arrow.",
      0, 1.0, 0.5, G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_int("playback_period_arrow_size",
      "Playback Arrow Head Size",
      "The height of the playback arrow head in pixels.",
      0, G_MAXINT, 10, G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property(klass,
      g_param_spec_int("playback_period_arrow_stem_size",
      "Playback Arrow Stem Size",
      "The height of the playback arrow head in pixels.",
      0, G_MAXINT, 3, G_PARAM_READABLE));
  }
  
  
  void
  TimelineRuler::read_styles()
  {
    get_style_property("annotation_horz_margin", annotationHorzMargin);
    get_style_property("annotation_vert_margin", annotationVertMargin);
    get_style_property("major_tick_height", majorTickHeight);
    get_style_property("minor_long_tick_height", minorLongTickHeight);
    get_style_property("minor_short_tick_height", minorShortTickHeight);
    get_style_property("min_division_width", minDivisionWidth);
    get_style_property("mouse_chevron_size", mouseChevronSize);
    get_style_property("selection_chevron_size", selectionChevronSize);
    
    playbackPointColour = workspace::UIStyle::readStyleColourProperty(
      *this, "playback_point_colour", 0, 0, 0);
    get_style_property("playback_point_alpha", playbackPointAlpha);
    get_style_property("playback_point_size", playbackPointSize);
    
    playbackPeriodArrowColour =workspace::UIStyle::readStyleColourProperty(
      *this, "playback_period_arrow_colour", 0, 0, 0);
    get_style_property("playback_period_arrow_alpha",
      playbackPeriodArrowAlpha);
    get_style_property("playback_period_arrow_size",
      playbackPeriodArrowSize);
    get_style_property("playback_period_arrow_stem_size",
      playbackPeriodArrowStemSize);
  }
  
  
}}}// namespace stage::widget::timeline
