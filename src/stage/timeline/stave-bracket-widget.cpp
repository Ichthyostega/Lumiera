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
  }
  
  
  
  
  StaveBracketWidget::StaveBracketWidget ()
    : DrawingArea{}
    { }
  
  
  StaveBracketWidget::~StaveBracketWidget() { }

  
  /** */
  
  
  
}}// namespace stage::timeline
