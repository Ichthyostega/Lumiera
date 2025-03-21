/*
  gtk-canvas-main.cpp  -  demo application to explore the abilities of Gtk::Canvas

   Copyright (C)
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.
*/


/** @file gtk-canvas-main.cpp
 ** Simple GTK Application frame to explore canvas handling.
 ** This demo application was extracted in 10/2018 to preserve
 ** [exploration experiments](\ref demo::CanvasDemoPanel), carried
 ** out in 2016 to decide upon the technology to support Lumiera's
 ** stage::timeline::TimelineWidget.
 */


#include "stage/gtk-base.hpp"
#include "gtk-canvas-experiment.hpp"
#include "lib/error.hpp"


namespace demo {
  
  class Window
    : public Gtk::Window
    {
      CanvasDemoPanel demoPanel_;
      
    public:
      Window()
        : demoPanel_{}
        {
          this->add (demoPanel_);
        }
    };
}//namespace



int
main (int argc, char* argv[])
{
  NOTICE (main, "*** GTK-Canvas Experiment ***");
  
  auto guiApp = Gtk::Application::create (argc, argv);
  
  demo::Window window;
  return guiApp->run(window);
}
