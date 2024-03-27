/*
  NODEWIRING.hpp  -  Implementation of the node network and operation control

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

*/


/** @file nodewiring.hpp
 ** Mechanism to wire ProcNode instances for a render network
 ** @todo unfinished draft from 2009 regarding the render process
 */


#ifndef ENGINE_NODEWIRING_H
#define ENGINE_NODEWIRING_H


#include "steam/engine/proc-node.hpp"
#include "lib/allocation-cluster.hpp"
#include "steam/engine/nodewiring-def.hpp"

#include <memory>



namespace steam {
namespace engine {
  
  
  class WiringFactory;
  
  namespace config { class WiringFactoryImpl; }
  
  using lib::RefArray;
  
  
  
  
  /**
   * Actual implementation of the link between nodes,
   * also acting as "track switch" for the execution path
   * chosen while operating the node network for rendering.
   * @param STATE Invocation state object controlling the
   *        behaviour of callDown() while rendering.
   * @see StateAdapter
   * @see NodeFactory 
   */
  template<class STATE>
  class NodeWiring
    : public WiringDescriptor
    {
      
    public:
      NodeWiring(WiringSituation const& setup)
        : WiringDescriptor(setup.makeOutDescriptor(), 
                           setup.makeInDescriptor(),
                           setup.resolveProcessingFunction(),
                           setup.createNodeID())
        { }
      
    private:
      virtual BuffHandle
      callDown (State& currentProcess, uint requestedOutputNr)  const 
        {
          STATE thisStep (currentProcess, *this, requestedOutputNr);
          return thisStep.retrieve (); // fetch or calculate results
        }
      
    };
  
  
  
  class WiringFactory
    {
      lib::AllocationCluster& alloc_;
      std::unique_ptr<config::WiringFactoryImpl> pImpl_;
      
    public:
      WiringFactory (lib::AllocationCluster& a);
     ~WiringFactory ();
      
      WiringDescriptor&
      operator() (WiringSituation const& setup);
    };
  
  
  
}} // namespace steam::engine
#endif
