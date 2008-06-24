/*
  NODEWIRING.hpp  -  Implementation of the node network and operation control
 
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
 
*/


#ifndef ENGINE_NODEWIRING_H
#define ENGINE_NODEWIRING_H


#include "proc/engine/procnode.hpp"

#include <cstddef>



namespace engine {


  class WiringFactory;
  
  
  /**
   * Actual implementation of the link between nodes
   * and also the track switch for the execution path
   * while operating the node network for rendering.
   * @param STATE StateAdapter object controlling the
   *        behaviour of callDown() while rendering.
   * @see NodeFactory 
   */
  template<class STATE>
  class NodeWiring
    : public WiringDescriptor
    {
      
      friend class WiringFactory;
      
    protected:
      virtual BufferID
      callDown (State& currentProcess, BufferID requiredOutputNr)  const 
        {
          STATE thisStep (currentProcess, *this);
          return thisStep.retrieve (requiredOutputNr); // fetch or calculate results
        }
      
    };

    
    
  class WiringFactory
    {
    public:
      WiringDescriptor&
      operator() (uint nrOut, uint nrIn, bool cache);
    };
  
  
  
} // namespace engine
#endif
