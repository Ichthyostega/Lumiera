/*
  NODEOPERATION.hpp  -  Specify how the nodes call each other and how processing is organized

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

/** @file nodeoperation.hpp
 ** Chunks of operation for invoking the rendernodes.
 ** This header defines part of the "glue" which holds together the render node network
 ** and enables to pull a result frames from the nodes. Especially, the aspect of
 ** buffer management and cache query is covered here. Each node has been preconfigured by
 ** the builder with a WiringDescriptor and a concrete type of a StateAdapter, including
 ** a specific Configuration, because the node can be built to
 ** - participate in the Caching or ignore the cache
 ** - actually process a result or just pull frames from a source
 ** - employ in-Place calculations or use separate in/out buffers
 ** Additionally, each node may have a given number of input/output pins, expecting to
 ** be provided with buffers holding a specific kind of data.
 ** 
 ** \par composition of the invocation Strategy
 ** For each individual ProcNode#pull() call, the WiringAdapter#callDown() builds an Invocation state
 ** instance directly on the stack, holding references to the actual buffer pointers and state. Using this
 ** StateAdapter, the predecessor nodes are pulled. The way these operations are carried out is encoded
 ** in the actual type of Strategy, which is defined at the bottom of this header. Each Strategy is a chain
 ** of elementary operations invoking each other (\c NEXT::step(invocation) ). Notably, all those possible
 ** configurations are pre-built while compiling (it's a small number below 32 configuration instance).
 ** To be able to select the Strategy for each configuration, we need a Factory (ConfigSelector defined in
 ** nodewiring-config.hpp). which is actually instantiated and used in nodewiring.cpp, which is the object
 ** file holding all those instantiations.
 ** 
 ** @see engine::ProcNode
 ** @see engine::Invocation
 ** @see engine::State
 ** @see engine::NodeFactory
 ** @see nodewiring-config.hpp
 ** @see nodewiring.hpp interface for building/wiring the nodes
 ** 
 */

#ifndef ENGINE_NODEOPERATION_H
#define ENGINE_NODEOPERATION_H


#include "steam/state.hpp"
#include "steam/engine/procnode.hpp"
#include "steam/engine/channel-descriptor.hpp"
#include "steam/engine/bufftable-obsolete.hpp"
#include "steam/engine/nodeinvocation.hpp"

#include "lib/meta/util.hpp"
#include "lib/meta/configflags.hpp"
#include "lib/frameid.hpp"




namespace proc {
namespace engine {
namespace config {
  
  
  /**
   * Base class of all concrete invocation sequences.
   * Provides a collection of functions used to build up the invocation sequence.
   * Additionally providing a marker used to detect the existence of an concrete
   * definition/specialisation for a given specific configuration.
   */
  struct OperationBase
    {
      typedef lib::meta::Yes_t is_defined;
      
      BuffHandle
      getSource (Invocation& ivo, uint chanNo)
        {
          UNIMPLEMENTED ("retrieve source data provided by the vault/scheduler");
        }
      
      BuffHandle
      pullPredecessor (Invocation& ivo, uint chanNo)
        {
          UNIMPLEMENTED ("invoke pull() on the denoted predecessor node");
        }
      
      void
      releaseBuffers(BuffHandle* table, uint slotCnt, uint slot_to_retain) //////////////TODO this is going to be implemented rather by smart-handle, Ticket #249
        {
          UNIMPLEMENTED ("release all buffers with the exception of the desired output");
        }
      
      bool
      validateBuffers (Invocation& ivo)
        {
          UNIMPLEMENTED ("Do a final, specifically tailored validation step on the buffers prior to invoking the process function");
        }
    };
  
  
  template<class NEXT>
  struct QueryCache : NEXT
    {
      BuffHandle
      step (Invocation& ivo)
        {
          BuffHandle fetched = ivo.fetch (ivo.genFrameID());
          if (fetched)
            return fetched;
          else
            return NEXT::step (ivo);
        }
    };
  
  
  template<class NEXT>
  struct AllocBufferTable : NEXT
    {
      BuffHandle
      step (Invocation& ivo)
        {
          BuffTableChunk buffTab (ivo.wiring, ivo.getBuffTableStorage());
          ivo.setBuffTab(&buffTab);
          ASSERT (ivo.buffTab);
          ASSERT (ivo.buffTab_isConsistent());
          
          return NEXT::step (ivo);
        }
    };
  
  
  template<class NEXT>
  struct PullInput : NEXT
    {
      BuffHandle
      step (Invocation& ivo)
        {
          BuffHandle        *   inH = ivo.buffTab->inHandle;
          BuffHandle::PBuff *inBuff = ivo.buffTab->inBuff;
          
          for (uint i = 0; i < ivo.nrI(); ++i )
            {
              inBuff[i] = 
               &*(inH[i] = this->pullPredecessor(ivo,i)); // invoke predecessor
              // now Input #i is ready...
            }
          return NEXT::step (ivo);
        }
    };
  
  
  template<class NEXT>
  struct ReadSource : NEXT
    {
      BuffHandle
      step (Invocation& ivo)
        {
          BuffHandle           *inH  = ivo.buffTab->inHandle;
          BuffHandle           *outH = ivo.buffTab->outHandle;
          BuffHandle::PBuff *inBuff  = ivo.buffTab->inBuff;
          BuffHandle::PBuff *outBuff = ivo.buffTab->outBuff;
          
          ASSERT (ivo.nrO() == ivo.nrI() );
          
          for (uint i = 0; i < ivo.nrI(); ++i )
            {
              inBuff[i] = outBuff[i] =
               &*(inH[i] = outH[i] = this->getSource(ivo,i));
              // now Input #i is ready...
            }
          return NEXT::step (ivo);
        }
    };
  
  
  template<class NEXT>
  struct AllocOutput : NEXT
    {
      BuffHandle 
      step (Invocation& ivo)
        {
          ASSERT (ivo.buffTab);
          ASSERT (ivo.nrO() < ivo.buffTabSize());
          BuffHandle           *outH = ivo.buffTab->outHandle;
          BuffHandle::PBuff *outBuff = ivo.buffTab->outBuff;
          
          for (uint i = 0; i < ivo.nrO(); ++i )
            {
              outBuff[i] =
                &*(outH[i] = ivo.allocateBuffer (ivo.wiring.out[i].bufferType));
              // now Output buffer for channel #i is available...
            }
          return NEXT::step (ivo);
        }
    };
  
  
  template<class NEXT>
  struct ProcessData : NEXT
    {
      BuffHandle 
      step (Invocation& ivo)
        {
          ASSERT (ivo.buffTab);
          ASSERT (ivo.buffTab_isConsistent());
          ASSERT (this->validateBuffers(ivo));
          
           // Invoke our own process() function,
          //  providing the array of outBuffer+inBuffer ptrs
          (*ivo.wiring.procFunction) (*ivo.buffTab->outBuff);
          
          return NEXT::step (ivo);
        }
    };
  
  
  template<class NEXT>
  struct FeedCache : NEXT
    {
      BuffHandle 
      step (Invocation& ivo)
        {
          for (uint i = 0; i < ivo.nrO(); ++i )
            {
              // declare all Outputs as finished
              ivo.is_calculated(ivo.buffTab->outHandle[i]);
            }
          
          return NEXT::step (ivo);
        }
    };
  
  
  template<class NEXT>
  struct ReleaseBuffers : NEXT                     /////////////////TODO: couldn't this be done automatically by BuffTab's dtor??
    {                                             /////////////////       this would require BuffHandle to be a smart ref....     --> ///TICKET #249
      BuffHandle 
      step (Invocation& ivo)
        {
          // all buffers besides the required Output no longer needed
          this->releaseBuffers(ivo.buffTab->outHandle,
                               ivo.buffTabSize(),
                               ivo.outNr);
          
          return ivo.buffTab->outHandle[ivo.outNr];
        }
    };
  
  
  
  
  
  /* =============================================================== */
  /* === declare the possible Assembly of these elementary steps === */
  
  enum Cases
    { 
      CACHING = 1,
      PROCESS,
      INPLACE,
      
      NOT_SET   = 0,
      NUM_Cases = INPLACE
    };
  
  
  using lib::meta::Config;
                                              ///////////////////////TODO: selecting this way isn't especially readable,
                                              ///////////////////////////: but BufferProvider selection is going to be solved differently anyway, see Ticket #249
  template<class CONF>
  struct SelectBuffProvider                                          { typedef AllocBufferFromParent Type; };
  template<uint PROC_ign, uint INPLA_ign>
  struct SelectBuffProvider< Config<CACHING, PROC_ign, INPLA_ign>>   { typedef AllocBufferFromCache  Type; };
  
  
  template<class Config>
  struct Strategy ;
  
  
  template<uint INPLACE_ign>
  struct Strategy< Config<CACHING,PROCESS,INPLACE_ign>>
    : QueryCache<
       AllocBufferTable<
        PullInput<
         AllocOutput<
          ProcessData<
           FeedCache<
            ReleaseBuffers< 
             OperationBase > > > > > > >
    { };
  
  template<uint INPLACE_ign>
  struct Strategy< Config<PROCESS,INPLACE_ign>>
    : AllocBufferTable<
       PullInput<
        AllocOutput<
         ProcessData<
          ReleaseBuffers< 
           OperationBase > > > > >
    { };
  
  template<>
  struct Strategy< Config<> >
    : AllocBufferTable<
       ReadSource<
        ReleaseBuffers< 
         OperationBase > > >  
    { };
  
  template<>
  struct Strategy< Config<INPLACE> > : Strategy< Config<> >  { };
  
  template<>
  struct Strategy< Config<CACHING> >
    : AllocBufferTable<
       ReadSource<
        AllocOutput<
         ProcessData<                       // wiring_.processFunction is supposed to do just buffer copying here 
          ReleaseBuffers< 
           OperationBase > > > > >  
    { };
  
  
  
  
}}} // namespace proc::engine::config
#endif
