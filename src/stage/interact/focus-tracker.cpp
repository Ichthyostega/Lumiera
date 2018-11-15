/*
  FocusTracker  -  track focus and activation changes to move the activity spot

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file focus-tracker.cpp
 ** Implementation helper functionality to link the global activity spot
 ** to typical UI focus and activation changes.
 ** 
 ** @todo WIP 2/2017 early draft / foundations of "interaction control"
 */


#include "stage/interact/focus-tracker.hpp"
#include "stage/interact/navigator.hpp"
//#include "stage/ctrl/global-ctx.hpp"
//#include "lib/util.hpp"

//using util::cStr;
//using util::isnil;


namespace stage {
namespace interact {
  
  
  
  // dtors via smart-ptr invoked from here...
  FocusTracker::~FocusTracker()
    { }
  
  
  FocusTracker::FocusTracker (Navigator& navigator)
    : navigator_{navigator}
    { }
  
  
  /** */
  
  
  
}}// namespace stage::interact
