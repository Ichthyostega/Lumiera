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


#include "proc/state.hpp"
#include "proc/engine/procnode.hpp"
#include "proc/engine/buffhandle.hpp"
#include "proc/engine/bufftable.hpp"
#include "proc/engine/nodewiringconfig.hpp"
#include "lib/appconfig.hpp"




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
    protected:
      State& parent_;
      State& current_;
      
      StateAdapter (State& callingProcess) 
        : parent_ (callingProcess),
          current_(callingProcess.getCurrentImplementation())
        { }
      
      virtual State& getCurrentImplementation () { return current_; }

      typedef State& (StateAdapter::*BufferProvider);
      
      
    public: /* === proxying the State interface === */
      
      virtual void releaseBuffer (BuffHandle& bh)       { current_.releaseBuffer (bh); }
      
      virtual void is_calculated (BuffHandle const& bh) { current_.is_calculated (bh); }
      
      virtual BuffHandle fetch (FrameID const& fID)     { return current_.fetch (fID); }
      
      
      // note: allocateBuffer()  is choosen specifically based on the actual node wiring
      
      
    protected: /* === API for use by the node operation pull() call === */
      
      BufferDescriptor
      getBufferDescriptor (uint chan)
        {
          TODO ("determine BufferDescriptor based on WiringDescriptor channel info");
        }
    };
  
    
  /**
   * Invocation context state.
   * A ref to this type is carried through the chain of NEXT::step() functions
   * which form the actual invocation sequence. The various operations in this sequence
   * access the context via the references in this struct, and also using the inherited
   * public State intercace. The object instance actually used as Invocation is created
   * on the stack and parametrized according to the necessities of the invocation
   * sequence actually configured. This real object instance comes in two flavours:
   * - a intended/partial invocation (which may be short-circuited due to Cache hit)
   * - a complete invocation, including storage for managing the buffer handles.
   */
  struct Invocation
    : StateAdapter
    {
      WiringDescriptor const& wiring;
      BuffTable& buffTab;
      const uint outNr;
      
    protected:
      /** creates a new invocation context state, 
       *  configuring it as intended/partial invocation without BuffTable */
      Invocation (State& callingProcess, WiringDescriptor const& w, BufferProvider buffSrc, uint o)
        : StateAdapter(callingProcess),
          wiring(w), outNr(o),
          bufferProvider_(buffSrc)
        { }
      
      /** used to build a complete invocation, including a buffer table,
          based on an existing intended/partial invocation */
      Invocation (Invocation const& currInvocation, BuffTable& localBuffTab)
        : StateAdapter(currInvocation),
          wiring(currInvocation.wiring),
          buffTab(localBuffTab),
          outNr(currInvocation.outNr),
          bufferProvider_(currInvocation.bufferProvider_)
        { }
      
      /** configuration of the actual entity in charge of allocating Buffers.
       *  depending on the configured call sequence, this may be a parent StateAdapter,
       *  or the backend in case we want to fade the results into the Cache  */
      const BufferProvider bufferProvider_;
      
    public:
      BuffHandle const&
      allocateBuffer (uint outChannel)
        {
          return this->*bufferProvider_.allocateBuffer(
                   this->getBufferDescriptor(outChannel));
        }
    };

  
  
  /**
   * The real invocation context state implementation. It is created
   * by the NodeWiring (WiringDescriptor) of the processing node which
   * is pulled by this invocation, hereby using the internal configuration
   * information to guide the selecton of the real call sequence 
   * 
   * \par assembling the call sequence implementation
   * Each ProcNode#pull() call creates such a StateAdapter subclass on the stack,
   * with a concrete type according to the WiringDescriptor of the node to pull.
   * This concrete type encodes a calculation Strategy, which is assembled
   * as a chain of policy templates on top of OperationBase. For each of the
   * possible configuratons we define such a chain (see bottom of this header).
   * The WiringFactory defined in nodewiring.cpp actually drives the instantiation
   * of all those possible combinations.
   */
  template<class Strategy, StateAdapter::BufferProvider buffPro>
  class ActualInvocationOrder
    : public Invocation
    {
    public:
      InvocationImpl (State& callingProcess, WiringDescriptor const& w, const uint outCh)
        : Invocation(callingProcess, w, buffPro, outCh)
        { }
      
      /** contains the details of Cache query and recursive calls
       *  to the predecessor node(s), eventually followed by the 
       *  ProcNode::process() callback
       */
      BuffHandle retrieve ()
        {
          return Strategy::step (*this);
        }
    };

  
  /** 
   * Extends the invocation context state by a table of buffer pointers.
   * This extension is used whenever the call sequence goes into actually
   * invoking a process() function, which necessiates to build a array of
   * buffer pointers and to allocate buffers to hold the data to be worked on.
   * It is implemented as decorating an already created "partial invocation",
   * just adding the allocation of a Table Chunk for organizing the buffers.
   */
  class ProcessInvocation
    : protected BuffTableChunk,
      public Invocation
    {
    public:
      ProcessInvocation (Invocation const& currInvocation)
        : BuffTableChunk(currInvocation.wiring, currInvocation.getBuffTableStorage()),
          Invocation(currInvocation, static_cast<BuffTable&>(*this))
        { }
    };
    
    
  /**
   * Collection of functions used to build up the invocation sequence.
   */
  class OperationBase
    {
      
    };
    
  template<class NEXT>
  struct QueryCache : NEXT
    {
      BuffHandle
      step (Invocation& ivo)
        {
          BuffHandle fetched = ivo.fetch (
                                 this->genFrameID (ivo));
          if (fetched)
            return fetched;
          else
            return NEXT::step (ivo);
        }
    };
          
  
  template<class NEXT>
  struct PullInput : NEXT
    {
      BuffHandle
      step (Invocation& started_invocation)
        {
          ProcessInvocation ivo (started_invocation);
          
          ASSERT (ivo.buffTab);
          ASSERT (0 < ivo.buffTabSize());
          ASSERT (ivo.nrO() == ivo.nrI() );
          ASSERT (ivo.nrO()+ivo.nrI() <= ivo.buffTabSize());
          ASSERT (ivo.buffTab->inHandles == &ivo.buffTab->handles[ivo.nrO()]);
          BuffHandle           *inH = ivo.buffTab->inHandles;
          BuffHandle::PBuff *inBuff = ivo.buffTab->inBuffs;
          
          for (uint i = 0; i < ivo.nrI(); ++i )
            {
              inBuff[i] =
                *(inH[i] = this->pullPredecessor(ivo,i)); // invoke predecessor
              // now Input #i is ready...
            }
          return NEXT::step (ivo); // note: passing down a ref to the ProcessInvocation
        }
    };
  
  
  template<class NEXT>
  struct ReadSource : NEXT
    {
      BuffHandle
      step (Invocation& started_invocation)
        {
          ProcessInvocation ivo (started_invocation);
          
          ASSERT (ivo.buffTab);
          ASSERT (0 < ivo.buffTabSize());
          ASSERT (ivo.nrO()+ivo.nrI() <= ivo.buffTabSize());
          ASSERT (ivo.buffTab->inHandles == &ivo.buffTab->handles[ivo.nrO()]);
          BuffHandle           *inH  = ivo.buffTab->inHandles;
          BuffHandle           *outH = ivo.buffTab->handles;
          BuffHandle::PBuff *inBuff  = ivo.buffTab->inBuffs;
          BuffHandle::PBuff *outBuff = ivo.buffTab->buffers;
          
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
          BuffHandle           *outH = ivo.buffTab->handles;
          BuffHandle::PBuff *outBuff = ivo.buffTab->buffers;
          
          for (uint i = 0; i < ivo.nrO(); ++i )
            {
              outBuff[i] =
                 *(outH[i] = ivo.allocateBuffer (i));
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
          ASSERT (ivo.nrO()+ivo.nrI() <= ivo.buffTabSize());
          ASSERT (this->validateBuffers(ivo));
          
           // Invoke our own process() function, providing the buffer array
          ivo.wiring.processFunction (ivo.buffTab->buffers);
          
          return NEXT::step (ivo);
        }
    };
  
  template<class NEXT>
  struct FeedCache : NEXT
    {
      BuffHandle 
      step (Invocation& ivo)
        {
          // declare all Outputs as finished
          ivo.isCalculated(ivo.buffTab->handles, 
                           ivo.nrO());
          
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
          this->releaseBuffers(ivo.buffTab->handles, 
                               ivo.buffTabSize(), 
                               ivo.outNr);
          
          return ivo.buffTab->outH[this->requiredOutputNr];
        }
    };
  
  

  
  /* === declare the possible Assembly of these elementary steps === */
  
  template<StateAdapter::BufferProvider buffPro>
  struct OutBuffSource{ static const StateAdapter::BufferProvider VALUE = buffPro; };
  
  template<char CACHE_Fl=0, char INPLACE_Fl=0>
  struct SelectBuffProvider;
  
  template<> struct SelectBuffProvider<CACHING>         : OutBuffSource<CACHE> { };
  template<> struct SelectBuffProvider<NOT_SET,INPLACE> : OutBuffSource<PARENT>{ };
  template<> struct SelectBuffProvider<CACHING,INPLACE> : OutBuffSource<CACHE> { };
  template<> struct SelectBuffProvider<>                : OutBuffSource<PARENT>{ };

  
  template<class Config>
  struct Strategy ;

  using lumiera::typelist::Config;
  
  template<char INPLACE>
  struct Strategy< Config<CACHING,PROCESS,INPLACE> >
    : QueryCache<
       PullInput<
        AllocOutput<
         ProcessData<
          FeedCache<
           ReleaseBuffers< 
            OperationBase > > > > > >
    { };
  
  template<char INPLACE>
  struct Strategy< Config<PROCESS,INPLACE> >
    : PullInput<
       AllocOutput<
        ProcessData<
          ReleaseBuffers< 
           OperationBase > > > > 
    { };
  
  template<>
  struct Strategy< Config<> >
    : ReadSource<
       ReleaseBuffers< 
        OperationBase > >  
    { };
  
  template<>
  struct Strategy< Config<INPLACE> > : Strategy< Config<> >  { };
  
  template<>
  struct Strategy< Config<CACHING> >
    : ReadSource<
       AllocOutput<
        ProcessData<                       // wiring_.processFunction is supposed to do just buffer copying here 
         ReleaseBuffers< 
          OperationBase > > > >  
    { };

  
    
  
} // namespace engine
#endif
