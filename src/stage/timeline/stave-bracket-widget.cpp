/*
  StaveBracketWidget  -  track body area to show overview and timecode and markers

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file stave-bracket-widget.cpp
 ** Implementation of drawing code to indicate the structure of nested tracks
 ** in the header area of the Timleline UI.
 ** 
 ** @todo WIP-WIP-WIP as of 2/2023
 ** 
 */


#include "stage/timeline/stave-bracket-widget.hpp"
//#include "stage/style-scheme.hpp"  /////////////////////TODO needed?
//#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
//using util::isnil;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace stage {
namespace timeline {
  
  namespace {
    const uint REQUIRED_WIDTH_px = 30;
  }
  
  
  
  
  StaveBracketWidget::~StaveBracketWidget() { }
  
  StaveBracketWidget::StaveBracketWidget ()
    : _Base{}
    {
      this->property_expand() = false;
    }
  
  
  
  /** */
  bool
  StaveBracketWidget::on_draw (CairoC cox)
  {
    // invoke (presumably empty) base implementation....
    bool event_is_handled = _Base::on_draw (cox);
    
    /////////////////////////////////////////////TICKET #1018 : placeholder drawing
    //
    int w = get_width();
    int h = get_allocated_height();
    cox->set_source_rgb(0.8, 0.0, 0.0);
    cox->set_line_width (5.0);
    cox->move_to(0, 0);
    cox->line_to(w, h);
    cox->move_to(w, 0);
    cox->line_to(0, h);
    cox->stroke();
    /////////////////////////////////////////////TICKET #1018 : placeholder drawing
    
    return event_is_handled;
  }
  
  
  /** indicate layout oriented towards vertical extension */
  Gtk::SizeRequestMode
  StaveBracketWidget::get_request_mode_vfunc()  const
  {
    return Gtk::SizeRequestMode::SIZE_REQUEST_WIDTH_FOR_HEIGHT;
  }
  
  /**
   * The structural outline adapts flexible in vertical direction,
   * but requires a fixed horizontal size for proper drawing.
   */
  void
  StaveBracketWidget::get_preferred_width_vfunc (int& minimum_width, int& natural_width)  const
  {
    minimum_width = natural_width = REQUIRED_WIDTH_px;
  }
  
  void
  StaveBracketWidget::get_preferred_width_for_height_vfunc (int, int& minimum_width, int& natural_width)  const
  {
    get_preferred_width (minimum_width, natural_width);
  }
  
  
  
  
}}// namespace stage::timeline
