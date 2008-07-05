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

/** @file nodeoperation.hpp
 ** Chunks of operation for invoking the rendernodes.
 ** This header defines the "glue" which holds together the render node network
 ** and enables to pull a result frames from the nodes. Especially, the aspect of
 ** buffer management is covered here. Each node has been preconfigured by the builder
 ** with a WiringDescriptor and a concrete type of a StateAdapter. These concrete
 ** StateAdapter objects are assembled out of the building blocks defined in this header,
 ** depending on the desired mode of operation. Any node can be built to 
 ** - participate in the Caching or ignore the cache
 ** - actually process a result or just pull frames from a source
 ** - employ in-Place calculations or use separate in/out buffers
 ** Additionally, each node may have a given number of input/output pins, expecting to
 ** be provided with buffers holding a specific kind of data.
 ** 
 ** \par composition of the StateAdapter
 ** For each individual ProcNode#pull() call, the WiringAdapter#callDown() builds an StateAdapter
 ** instance directly on the stack, holding the actual buffer pointers and state references. Using this
 ** StateAdapter, the predecessor nodes are pulled. The way these operations are carried out is encoded
 ** in the actual StateAdapter type known to the NodeWiring (WiringAdapter) instance. All of these actual
 ** StateAdapter types are built as implementing the engine::State interface, on top of the InvocationStateBase
 ** and inheriting from a chain of strategy classes (single inheritance, mostly \em no virtual functions).
 **
 ** @see engine::ProcNode
 ** @see engine::StateProxy
 ** @see nodewiring.hpp interface for building/wiring the nodes
 **
 */

#ifndef ENGINE_NODEOPERATION_H
#define ENGINE_NODEOPERATION_H


#include "proc/engine/procnode.hpp"
#include "proc/engine/nodewiringconfig.hpp"
#include "lib/appconfig.hpp"




namespace engine {
  
  

  /** 
   * (abstract) base class of all concrete StateAdapter types.
   * Defines the skeleton for the node operation/calculation
   */
  class InvocationStateBase
    : public State
    {
      
    };
  
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
    
  template<class NEXT>
  struct QueryCache : NEXT
    {
      BuffHandle step () // brauche: current state
        {
          BuffHandle fetched = current_.fetch (genFrameID (requiredOutputNr));
          if (fetched)
            return fetched;
          else
            return NEXT::step();
        }
    };
          
  
  template<class NEXT>
  struct PullInput : NEXT
    {
      BuffHandle step ()
        {
          this->createBuffTable();
          
          ASSERT (this->buffTab);
          ASSERT (0 < this->buffTabSize());
          ASSERT (this->nrO+this->nrI <= this->buffTabSize());
          ASSERT (this->buffTab->inHandles = &this->buffTab->handles[this->nrO]);
          BuffHandle           *inH = this->buffTab->inHandles;
          BuffHandle::PBuff *inBuff = this->buffTab->inBuffs;
          
          for (uint i = 0; i < this->nrI; ++i )
            {
              inBuff[i] =
                *(inH[i] = this->pullPredecessor(i)); // invoke predecessor
              // now Input #i is ready...
            }
          return NEXT::step();
        }
    };
  
  template<class NEXT>
  struct AllocOutputFromCache
    {
      BuffHandle step ()
        {
          ASSERT (this->buffTab);
          ASSERT (this->nrO < this->buffTabSize());
          BuffHandle           *outH = this->buffTab->handles;
          BuffHandle::PBuff *outBuff = this->buffTab->buffers;
          
          for (uint i = 0; i < this->nrO; ++i )
            {
              outBuff[i] =
                 *(outH[i] = this->allocateBuffer(i));
              // now Output buffer for channel #i is available...
            }
          return NEXT::step();
        }
    };
  
  template<class NEXT>
  struct ProcessData
    {
      BuffHandle step ()
        {
          ASSERT (this->buffTab);
          ASSERT (this->nrO+this->nrI <= this->buffTabSize());
          ASSERT (this->validateBuffers());
          
           // Invoke our own process() function, providing the buffer array
          this->wiring_.processFunction (this->buffTab->buffers);
          
          return NEXT::step();
        }
    };
  
  template<class NEXT>
  struct FeedCache
    {
      BuffHandle step ()
        {
          // declare all Outputs as finished
          this->current_.isCalculated(this->buffTab->handles, 
                                      this->nrO);
          
          return NEXT::step();
        }
    };
  
  template<class NEXT>
  struct ReleaseBuffers
    {
      BuffHandle step ()
        {
          // all buffers besides the required Output no longer needed
          this->current_.releaseBuffers(this->buffTab->handles, 
                                        this->buffTabSize(), 
                                        this->requiredOutputNr);
          
          return this->buffTab->outH[this->requiredOutputNr];
        }
    };
  
  template<class NEXT>
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
  

  
  /* === declare the possible Assembly of these elementary steps === */
  
  template<class Config>
  struct Strategy
    {
      BuffHandle step () { NOTREACHED; }
    };
  
  template<>
  struct Strategy< Config<CACHE,CALCULATE,IN_OUT> >
    : QueryCache <
       PullInput<
        AllocOutputFromCache<
         ProcessData<
          FeedCache<
           ReleaseBuffers< 
            InvocationStateBase > > > > > >
    { };
  
  template<>
  struct Strategy< Config<NOCACHE,CALCULATE,IN_OUT> >
    : PullInput<
       AllocOutputFromParent<
        ProcessData<
          ReleaseBuffers< 
           InvocationStateBase > > > > 
    { };
  
  
  // At Application startup: build table of all possible operation configs
  namespace {
  
    bool
    determine_if_case_is_possible (Bits& caseFlags)
    {
      ////////////////////////////////////////////////TODO: Henne oder Ei?
    }
  
    void
    build_table_of_possible_configs ()
    {
      registerPossibleCases (&determine_if_case_is_possible);
    }
  
  
    using namespace lumiera;
    LifecycleHook schedule_ (ON_BASIC_INIT, &build_table_of_possible_configs);         

  }
  
  
} // namespace engine
#endif
