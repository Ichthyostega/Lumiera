/*
  BodyCanvasWidget  -  custom drawing canvas to display the timeline body

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

* *****************************************************/


/** @file body-canvas-widget.cpp
 ** Implementation details of timeline custom drawing.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#include "gui/gtk-base.hpp"
#include "gui/timeline/body-canvas-widget.hpp"

//#include "gui/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

//#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace gui {
namespace timeline {
  
  
  
  BodyCanvasWidget::~BodyCanvasWidget() { }
  
  
  BodyCanvasWidget::BodyCanvasWidget ()
    : Gtk::ScrolledWindow{}
    , canvas_{}
    {
      this->set_shadow_type(Gtk::SHADOW_IN);
      this->set_policy (Gtk::POLICY_ALWAYS, Gtk::POLICY_AUTOMATIC);  // always need a horizontal scrollbar
      this->property_expand() = true;                               //  dynamically grab any available additional space
      this->add(canvas_);
      
//    canvas_.adjustSize();
      
      // show everything....
      this->show_all();
    }
  
  
  
  
  
}}// namespace gui::timeline
