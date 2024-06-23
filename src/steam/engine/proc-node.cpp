/*
  ProcNode  -  Implementation of render node processing

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file proc-node.cpp
 ** Translation unit to hold the actual implementation of node processing operations.
 ** 
 ** @todo WIP-WIP-WIP 6/2024 not clear yet what goes here and what goes into turnout-system.cpp
 */


#include "steam/engine/proc-node.hpp"

namespace steam {
namespace engine {
  
  
  namespace { // Details...

    
  } // (END) Details...
  
  
//  using mobject::Placement;
  
  Port::~Port() { }  ///< @remark VTables for the Port-Turnout hierarchy emitted from \ref proc-node.cpp
  
  
  /** @internal */
  
  
}} // namespace steam::engine
