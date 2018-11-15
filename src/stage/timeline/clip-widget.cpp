/*
  ClipWidget  -  display of a clip in timeline or media bin view

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


/** @file clip-widget.cpp
 ** Implementation details of _clip display_ within the Lumiera UI.
 ** 
 ** @note a "clip" can be a media clip proper, or just some part
 **       of the former, a channel, or even an effect attached to
 **       a clip. Moreover, this clip display can be used both
 **       within the context of the timeline or as element in a
 **       media bin in the asset management section.
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/clip-widget.hpp"

//#include "stage/ui-bus.hpp"
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


namespace stage {
namespace timeline {
  
  
  
  
  
  ClipWidget::ClipWidget ()
    {
    }
  
  
  ClipWidget::~ClipWidget()
  {
  }
  
  
  
  
}}// namespace stage::timeline
