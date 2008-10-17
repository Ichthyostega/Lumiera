/*
  video-display-widget.cpp  -  Implementation of the video viewer widget
 
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

#include <gdkmm/drawable.h>
#include <gdkmm/general.h>
#include <cairomm-1.0/cairomm/cairomm.h>

#include "../gtk-lumiera.hpp"

#include "../output/xvdisplayer.hpp"
#include "../output/gdkdisplayer.hpp"

#include "video-display-widget.hpp"

namespace gui {
namespace widgets {

VideoDisplayWidget::VideoDisplayWidget() :
  displayer(NULL)
{
}

VideoDisplayWidget::~VideoDisplayWidget()
{
  if(displayer != NULL)
    delete displayer;
}

void
VideoDisplayWidget::on_realize()
{
  // Call base class:
  Gtk::Widget::on_realize();

  // Set colors
  modify_bg(Gtk::STATE_NORMAL, Gdk::Color("black"));

  if(displayer != NULL)
    delete displayer;
  displayer = createDisplayer(this, 320, 240);

  add_events(Gdk::ALL_EVENTS_MASK);
}

bool 
VideoDisplayWidget::on_button_press_event (GdkEventButton* event)
{
  unsigned char buffer[320 * 240 * 4];

  for(int i = 0; i < 320*240*4; i++)
    buffer[i] = rand();

  displayer->put((void*)buffer);

  return true;
}

Displayer*
VideoDisplayWidget::createDisplayer( Gtk::Widget *drawingArea, int width, int height )
{
  REQUIRE(drawingArea != NULL);
  REQUIRE(width > 0 && height > 0);
  
  Displayer *displayer = NULL;
  
  displayer = new XvDisplayer( drawingArea, width, height );
  if ( !displayer->usable() )
    {
      delete displayer;
      displayer = NULL;
    }

  if ( displayer == NULL )
    {
      displayer = new GdkDisplayer( drawingArea, width, height );
    }

  return displayer;
}

}   // namespace widgets
}   // namespace gui
