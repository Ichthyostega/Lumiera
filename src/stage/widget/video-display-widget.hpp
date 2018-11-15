/*
  VIDEO-DISPLAY-WIDGET.hpp  -  GUI widget for displaying video

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


/** @file video-display-widget.hpp
 ** Widget to create a video display embedded into the UI
 ** @deprecated defunct since the transition to GTK-3
 */




#ifndef STAGE_WIDGET_VIDEO_DISPLAY_WIDGET_H
#define STAGE_WIDGET_VIDEO_DISPLAY_WIDGET_H

#include "stage/gtk-base.hpp"
#include "stage/output/displayer.hpp"


using namespace gui::output;  ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace gui {
namespace widget {

  /**
   * @todo the first UI draft included a video displayer widget library implementation,
   *       Unfortunately, this became defunct with the switch to GTK-3. And a fun fact is,
   *       even while Lumiera is a video editing application, we did not yet reach the state
   *       as to care for video display ourselves. Someone (TM) need to care for this!
   */
  class VideoDisplayWidget
    : public Gtk::DrawingArea
    {
      Displayer* displayer_;
      
    public:
      VideoDisplayWidget();
     ~VideoDisplayWidget();
      
      Displayer* getDisplayer() const;
      
      
    private: /* ===== Overrides ===== */
      virtual void on_realize()  override;
      
      
    private: /* ===== Internals ===== */
      static Displayer*
      createDisplayer (Gtk::Widget* drawingArea, int width, int height);
    };
  
  
}}// gui::widget
#endif /*STAGE_WIDGET_VIDEO_DISPLAY_WIDGET_H*/
