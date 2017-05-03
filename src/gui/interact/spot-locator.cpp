/*
  SpotLocator  -  conduct and direct a global centre of activity

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


/** @file spot-locator.cpp
 ** Implementation details of a machinery to move the global activity "spot".
 ** 
 ** @todo WIP 2/2017 early draft / foundations of "interaction control"
 */


#include "gui/interact/spot-locator.hpp"
//#include "gui/ctrl/global-ctx.hpp"
//#include "lib/util.hpp"

//using util::cStr;
//using util::isnil;


namespace gui {
namespace interact {
  
  
  
  // dtors via smart-ptr invoked from here...
  SpotLocator::~SpotLocator()
    { }
  
  
  SpotLocator::SpotLocator()
    { }
  
  
  /** */
  
  
  
}}// namespace gui::interact
