/*
  WorkForce.hpp  -  actively coordinated pool of workers for rendering

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

/** @file work-force.cpp
 ** Implementation of render worker coordination.
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


#include "vault/gear/work-force.hpp"
//#include "lib/symbol.hpp"
//#include "include/logging.h"
#include "lib/util.hpp"

//#include <string>

//using std::string;
//using util::isnil;


namespace vault{
namespace gear {
  
  namespace { // internal details
    
    size_t MINIMAL_CONCURRENCY = 2;
    
  } // internal details
  
  
  
//  NA::~NA() { }
  
  /** default value for full computing capacity is to use all (virtual) cores */
  const size_t work::Config::COMPUTATION_CAPACITY = util::max (std::thread::hardware_concurrency()
                                                              , MINIMAL_CONCURRENCY);
  
  
  
  /**
   */

}} // namespace vault::gear
