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


#include "proc/state.hpp"
#include "proc/engine/procnode.hpp"
#include "proc/engine/buffhandle.hpp"
#include "proc/engine/bufftable.hpp"
#include "proc/engine/nodeinvocation.hpp"

#include "lib/meta/util.hpp"
#include "lib/meta/configflags.hpp"
#include "lib/frameid.hpp"




namespace engine {
namespace config {
  
  
  /**
   * Base class of all concrete invocation sequences.
   * Could contain a collection of functions used to build up the invocation sequence.
   * Currently contains just a marker used to detect the existence of an concrete
   * definition/specialisation for a given specific configuration.
   */
  class OperationBase
    {
      typedef lumiera::Yes_t is_defined;
      
      
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
                *(inH[i] = this->pullPredecessor(ivo,i)); // invoke predecessor
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
                *(inH[i] = outH[i] = this->getSource(ivo,i));
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
                 *(outH[i] = ivo.allocateBuffer (ivo.wiring.out[i].bufferType));
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
          ivo.wiring.procFunction (ivo.buffTab->outBuff);
          
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
    {                                             /////////////////       this would require BuffHandle to be a smart ref....
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
  
  
  
  template< char CACHE_Fl   =NOT_SET
          , char INPLACE_Fl =NOT_SET
          >
  struct SelectBuffProvider;
  
  template<> struct SelectBuffProvider<CACHING>         { typedef AllocBufferFromCache  Type; };
  template<> struct SelectBuffProvider<NOT_SET,INPLACE> { typedef AllocBufferFromParent Type; };
  template<> struct SelectBuffProvider<CACHING,INPLACE> { typedef AllocBufferFromCache  Type; };
  template<> struct SelectBuffProvider<>                { typedef AllocBufferFromParent Type; };
  
  
  template<class Config>
  struct Strategy ;
  
  using lumiera::typelist::Config;
  
  template<char INPLACE>
  struct Strategy< Config<CACHING,PROCESS,INPLACE> >
    : QueryCache<
       AllocBufferTable<
        PullInput<
         AllocOutput<
          ProcessData<
           FeedCache<
            ReleaseBuffers< 
             OperationBase > > > > > > >
    { };
  
  template<char INPLACE>
  struct Strategy< Config<PROCESS,INPLACE> >
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
  
  
  
  
}} // namespace engine::config
#endif
