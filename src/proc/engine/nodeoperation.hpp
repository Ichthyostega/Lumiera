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
      
      
      
    public: /* === proxying the State interface === */
      
      virtual void releaseBuffer (BuffHandle& bh)       { current_.releaseBuffer (bh); }
      
      virtual void is_calculated (BuffHandle const& bh) { current_.is_calculated (bh); }
      
      virtual BuffHandle fetch (FrameID const& fID)     { return current_.fetch (fID); }
      
      // note: allocateBuffer()  is choosen specifically based on the actual node wiring
      
    };
  
  
  
  
  
  /**
   * Invocation context state.
   * A ref to this type is carried through the chain of NEXT::step() functions
   * which form the actual invocation sequence. The various operations in this sequence
   * access the context via the references in this struct, while also using the inherited
   * public State interface. The object instance actually used as Invocation is created
   * on the stack and parametrized according to the necessities of the invocation sequence
   * actually configured. Initially, this real instance is configured without BuffTable,
   * because the invocation may be short-circuited due to Cache hit. Otherwise, when
   * the invocation sequence actually prepares to call the process function of this
   * ProcNode, a buffer table chunk is allocated by the StateProxy and wired in.
   */
  struct Invocation
    : StateAdapter
    {
      WiringDescriptor const& wiring;
      const uint outNr;
      
      BuffTable* buffTab;
      
    protected:
      /** creates a new invocation context state, without BuffTable */
      Invocation (State& callingProcess, WiringDescriptor const& w, uint o)
        : StateAdapter(callingProcess),
          wiring(w), outNr(o),
          buffTab(0)
        { }
      
      const uint nrO()          const { return wiring.getNrO(); }
      const uint nrI()          const { return wiring.getNrI(); }
      const uint buffTabSize()  const { return nrO()+nrI(); }
      
      /** setup the link to an externally allocated buffer table */
      void setBuffTab (BuffTable* b) { this->buffTab = b; }
      
      bool
      buffTab_isConsistent ()
        {
          return (buffTab)
              && (0 < buffTabSize())
              && (nrO()+nrI() <= buffTabSize())
              && (buffTab->inBuff   == &buffTab->outBuff[nrO()]  )
              && (buffTab->inHandle == &buffTab->outHandle[nrO()])
               ;
        }
      
      
    };
    
    
    struct AllocBufferFromParent  ///< using the parent StateAdapter for buffer allocations
      : Invocation
      {
        AllocBufferFromParent (State& sta, WiringDescriptor const& w, const uint outCh) 
          : Invocation(sta, w, outCh) {}
        
        virtual BuffHandle
        allocateBuffer (BufferDescriptor const& bd) { return parent_.allocateBuffer(bd); }
      };
    
    struct AllocBufferFromCache   ///< using the global current State, which will delegate to Cache
      : Invocation
      {
        AllocBufferFromCache (State& sta, WiringDescriptor const& w, const uint outCh) 
          : Invocation(sta, w, outCh) {}
        
        virtual BuffHandle
        allocateBuffer (BufferDescriptor const& bd) { return current_.allocateBuffer(bd); }
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
   * possible configuratons we define such a chain (see bottom of nodeoperation.hpp).
   * The WiringFactory defined in nodewiring.cpp actually drives the instantiation
   * of all those possible combinations.
   */
  template<class Strategy, class BufferProvider>
  class ActualInvocationProcess
    : public BufferProvider
    {
    public:
      ActualInvocationProcess (State& callingProcess, WiringDescriptor const& w, const uint outCh)
        : Invocation(callingProcess, w, outCh)
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
          return NEXT::step (ivo); // note: passing down a ref to the ProcessInvocation
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
                 *(outH[i] = ivo.allocateBuffer (ivo.wiring.out[i].bufferType);
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
          ivo.wiring.processFunction (ivo.buffTab->outBuff);
          
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
          ivo.is_calculated(ivo.buffTab->outHandle, 
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
          this->releaseBuffers(ivo.buffTab->outHandle, 
                               ivo.buffTabSize(), 
                               ivo.outNr);
          
          return ivo.buffTab->outHandle[ivo.outNr];
        }
    };
  
  
  
  
  /* === declare the possible Assembly of these elementary steps === */
  
  template<char CACHE_Fl=0, char INPLACE_Fl=0>
  struct SelectBuffProvider;
  
  template<> struct SelectBuffProvider<CACHING>         : AllocBufferFromCache { };
  template<> struct SelectBuffProvider<NOT_SET,INPLACE> : AllocBufferFromParent{ };
  template<> struct SelectBuffProvider<CACHING,INPLACE> : AllocBufferFromCache { };
  template<> struct SelectBuffProvider<>                : AllocBufferFromParent{ };

  
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
  
  
  
  
} // namespace engine
#endif
