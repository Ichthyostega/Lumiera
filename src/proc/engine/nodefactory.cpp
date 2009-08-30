/*
  NodeFactory  -  Interface for creating processing nodes of various kinds
 
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


#include "proc/engine/nodefactory.hpp"
#include "proc/mobject/session/effect.hpp"
#include "lib/allocationcluster.hpp"

#include "proc/engine/nodewiring.hpp"

namespace engine {
  
  namespace { // Details of node fabrication

    using lib::AllocationCluster;
    
    template<class NODE>
    NODE &
    makeNode ()
    {
      ////TODO: this is a draft how it /might/ work
      ////TODO: of course this can't be hard wired....
      
      ProcNode * predecessor1, predecessor2;
      
      WiringSituation setup;
      setup.defineInput (4, predecessor1, 2);
      setup.defineInput (2, predecessor2);
      
      WiringFactory* wFac;
      
      AllocationCluster allocator;
      bool doCache = (2 == 2); // find out Cache should be used for this node
      
      WiringDescriptor & wDesc = (*wFac)(setup,doCache);
      
      NODE* newNode = allocator.create<NODE> (wDesc);
      
      return *newNode;
      
      /*
       * Problems to be solved:                                                                        /////////////TODO: see Ticket #247
       * - write a generic allocator, which later on can be augmented to do block wise bulk allocation /////////////DONE: AllocationCluster
       * - the allocator needs to keep track of the objects; actually he owns the objects              /////////////DONE
       * - we need to access the wiring descriptor of the predecessor nodes. Which means, the code
       *   must be in the body of NodeFactory (because the latter is friend of ProcNode and can access the wiring descriptor)
       * - btw reconsider if this level of protection is necessary, or if the const WiringDescriptor cant be just on the public interface of ProcNode
       * - find out how the Caching/ReadInput/InPlace detection can work
       * - think about a sensible Ctor for NodeFactory. This one must create the WiringFactory and pass the Allocator.
       * - all of this will be installed into the ToolFactory, i.e. it is part of the build process
       */
    }
    
  } // (END) Details of node fabrication
  
  
  using mobject::Placement;
  using mobject::session::Effect;
  
  
  /** create a processing node able to render an effect */
  PTrafo
  NodeFactory::operator() (Placement<Effect> const&)
  {
    UNIMPLEMENTED ("create proc node for Effect/Plugin");
  }


} // namespace engine
