/*
  NodeWiring  -  Implementation of the node network and operation control
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "proc/engine/procnode.hpp"
#include "proc/engine/nodewiring.hpp"
#include "proc/engine/nodeoperation.hpp"
#include "proc/engine/nodewiringconfig.hpp"


namespace engine {
  
  
  /** create and configure a concrete wiring descriptor to tie
   *  a ProcNode to its predecessor nodes. This includes selecting
   *  the actual StateProxy type, configuring it out of some operation
   *  control templates (policy classes). Compiling this operator function
   *  actually drives the necessary template instantiations for all cases
   *  encountered while building the node network.
   *  The created WiringDescriptor object is bulk allocated similar to the ProcNode
   *  objects for a given segment of the Timeline. It should be further configured
   *  with the actual predecessor nodes pointers and can then be used to create
   *  the new processing node to be wired up.  
   */
  WiringDescriptor&
  WiringFactory::operator() (uint nrOut, uint nrIn, bool cache)
  {
    UNIMPLEMENTED ("build the actual wiring descriptor based on given operation options");
    
    return selectConfig(cache, process, inplace).fabricate();
  }
  // BlockAlloc<NodeWiring< StateAdapter< Config<cache, process, inplace> > > >::fabricate();

  
} // namespace engine
