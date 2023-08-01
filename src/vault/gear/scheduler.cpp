/*
  Scheduler  -  coordination of render activities under timing and dependency constraints

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

/** @file scheduler.cpp
 ** Implementation of messaging, dispatch and prioritisation of render activities.
 ** The _scheduling_ functionality is assembled from low-level operational building blocks.
 ** 
 ** @note as of X/2023 this is complete bs
 ** @todo WIP  ///////////////////////TICKET #
 ** 
 ** @see ////TODO_test usage example
 ** @see scheduler.cpp implementation
 ** 
 ** @todo WIP-WIP-WIP 6/2023 »Playback Vertical Slice«
 ** 
 */


#include "vault/gear/scheduler.hpp"
//#include "lib/symbol.hpp"
//#include "include/logging.h"

//#include <string>

//using std::string;
//using util::isnil;


namespace vault{
namespace gear {
  
  namespace { // internal details
    
  } // internal details
  
  namespace activity {
    
    Hook::~Hook() { } // emit VTable here...
  }
  
//  NA::~NA() { }
  
  
  
  
  /**
   */

}} // namespace vault::gear
