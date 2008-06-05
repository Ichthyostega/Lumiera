/*
  timeline.cpp  -  Implementation of the timeline widget
 
  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/

#include <cairomm-1.0/cairomm/cairomm.h>

#include "timeline-body.hpp"
#include "../timeline-widget.hpp"

using namespace Gtk;
using namespace std;
using namespace lumiera::gui::widgets;
using namespace lumiera::gui::widgets::timeline;

namespace lumiera {
namespace gui {
namespace widgets {
namespace timeline {

TimelineBody::TimelineBody(lumiera::gui::widgets::TimelineWidget *timeline_widget) :
    Glib::ObjectBase("TimelineBody"),
    timelineWidget(timeline_widget)
  {
    g_assert(timelineWidget != NULL);
        
    // Connect up some events  
    timelineWidget->horizontalAdjustment.signal_value_changed().connect(
      sigc::mem_fun(this, &TimelineBody::on_scroll) );
    timelineWidget->verticalAdjustment.signal_value_changed().connect(
      sigc::mem_fun(this, &TimelineBody::on_scroll) );
    
    // Install style properties
    gtk_widget_class_install_style_property(
      GTK_WIDGET_CLASS(G_OBJECT_GET_CLASS(gobj())), 
      g_param_spec_boxed("track_background",
        "Track Background",
        "The background colour of timeline tracks",
        GDK_TYPE_COLOR, G_PARAM_READABLE));

  }

void
TimelineBody::on_scroll()
  {
    queue_draw();
  }

bool
TimelineBody::on_expose_event(GdkEventExpose* event)
  {
    // This is where we draw on the window
    Glib::RefPtr<Gdk::Window> window = get_window();
    if(!window)
      return false;
    
    // Makes sure the widget styles have been loaded
    read_styles();
    
    // Prepare to render via cairo      
    Gtk::Allocation allocation = get_allocation();
    Cairo::RefPtr<Cairo::Context> cairo = window->create_cairo_context();

    // Translate the view by the scroll distance
    cairo->translate(
      -(int)timelineWidget->horizontalAdjustment.get_value(),
      -(int)timelineWidget->verticalAdjustment.get_value());
    
    // Interate drawing each track
    vector<timeline::Track*>::iterator i;
    for(i = timelineWidget->tracks.begin();
      i != timelineWidget->tracks.end(); i++)
    {
      timeline::Track *track = *i;
      g_assert(track != NULL);   

      const int track_height = track->get_height();
    
      // Draw the track background
      cairo->rectangle(0, 0, allocation.get_width(), track_height);
      gdk_cairo_set_source_color(cairo->cobj(), &track_background);
      cairo->fill();
    
      // Render the track
      cairo->save();
      track->draw_track(cairo);
      cairo->restore();
      
      // Shift for the next track
      cairo->translate(0, track_height + TimelineWidget::TrackPadding);
    }   

    return true;
  }
  
void
TimelineBody::read_styles()
  {
    GdkColor *colour;

    gtk_widget_style_get(Widget::gobj(), "track_background", &colour, NULL);

    // Did the color load successfully?
    if(colour != NULL)
      track_background = *colour;
    else
    {
      g_warning("track_background style value failed to load");
      track_background.red   = 0x0000;
      track_background.green = 0x0000;
      track_background.blue  = 0x0000;
      track_background.pixel = 0x00000000;
    }
  }

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

