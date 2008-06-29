/*
  NODEOPERATION.hpp  -  Specify how the nodes call each other and how processing is organized
 
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


#ifndef ENGINE_NODEOPERATION_H
#define ENGINE_NODEOPERATION_H


#include "proc/engine/procnode.hpp"




namespace engine {
  
  
  
  /**
   * Adapter to shield the ProcNode from the actual buffer management,
   * allowing the processing function within ProcNode to use logical
   * buffer IDs. StateAdapter is created on the stack for each pull()
   * call, using setup/wiring data preconfigured by the builder.
   * Its job is to provide the actual implementation of the Cache
   * push / fetch and recursive downcall to render the source frames.
   */
  class StateAdapter
    : public State
    {
      State& parent_;
      State& current_;
      WiringDescriptor const& wiring_;
      
    protected:
      StateAdapter (State& callingProcess, WiringDescriptor const&) 
        : parent_ (callingProcess),
          current_(callingProcess.getCurrentImplementation())
        { }
      
      friend class NodeWiring<StateAdapter>; // both are sharing implementation details...
      
      
      virtual State& getCurrentImplementation () { return current_; }
      
      /** contains the details of Cache query and recursive calls
       *  to the predecessor node(s), eventually followed by the 
       *  ProcNode::process() callback
       */
      BuffHandle retrieve (uint requiredOutputNr)
        {
          return retrieveResult (requiredOutputNr);
        }
    };
    
  struct Caching
    {
      void retrieveResult (uint requiredOutputNr)
        {
          BuffHandle fetched = current_.fetch (genFrameID (requiredOutputNr));
          if (fetched)
            return fetched;
          
          // Cache miss, need to calculate
          BuffHandle calculated[NrO];
          calculateResult (&calculated);
          
          // commit result to Cache
          current_.isCalculated (NrO, calculated, requiredOutputNr);
          
          return calculated[requiredOutputNr];
        }
    };
  
  struct NoCaching
    {
      void retrieveResult (BuffHandle requiredOutputNr)
        {
          return calculateResult (0);
        }
    };
  
  struct Process
    {
      BuffHandle calculateResult(BuffHandle* calculated)
        {
          uint nrI = this->getNrI();
          for (uint i = 0; i<nrI; ++i )
            {
              BuffHandle inID = predNode.pull(inNo); // invoke predecessor
              this->inBuff[i] = current_.getBuffer(inID);
              // now Input #i is ready...
            }
          uint nrO = this->getNrO();
          for (uint i = 0; i<nrO; ++i )
            {
              calculated[i] = this->allocateBuffer(this->getBuferType(i));  ///TODO: Null pointer when no caching!!!!!
              this->outBuff[i] = current_.getBuffer(calculated[i]);
              // now Output buffer for channel #i is available...
            }
            //
           // Invoke our own process() function
          this->wiring_.process (this->outBuff);
          
          this->feedCache();
          // Inputs no longer needed
          for (uint i = 0; i<nrI; ++i)
            current_.releaseBuffer(inID);  ////TODO.... better release /all/ buffers which are != requiredOutput
          
          return calculated[requiredOutputNr];
        }
    };
  
  struct NoProcess
    {
      BuffHandle calculateResult(BuffHandle* calculated)
        {
          uint nrO = this->getNrO();
          for (uint i = 0; i<nrO; ++i )
            {
              calculated[i] = this->retrieveInput(i);  ///TODO: Null pointer when no caching!!!!!
              this->outBuff[i] = current_.getBuffer(calculated[i]);
              // now Buffer containing Output channel #i is available...
            }
          
          this->feedCache();
          for (uint i=0; i < nrO; ++i)
            if (i!=requiredOutputNr)
              current_.releaseBuffer(i);
              
          return calculated[requiredOutputNr];
        };
      
    };
  
  
  
} // namespace engine
#endif
