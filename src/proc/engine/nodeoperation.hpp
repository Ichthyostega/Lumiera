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
   * 
   * \par assembling the implementation
   * This is the abstract base class of all concrete StateAdapter types.
   * Each ProcNode#pull() call creates such a StateAdapter on the stack,
   * with a concrete type according to the WiringDescriptor of the node
   * to pull. The concrete type is assembled by a chain of policy templates
   * on top of InvocationStateBase. For each of the possible configuratons
   * we define such a chain (see bottom of this header). The WiringFactory
   * defined in nodewiring.cpp actually drives the instantiation of all
   * those possible combinations
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
  
    
  
  struct Invocation
    : StateAdapter
    {
      WiringDescriptor const& wiring;
      BuffTable& buffTab;
      const uint outNr;
      
    protected:
      Invocation (State& callingProcess, WiringDescriptor const& w, BuffTable& b, uint o)
        : StateAdapter(callingProcess),
          wiring(w),
          buffTab(b),
          outNr(o)
        { }
    };

  template<uint SZ>
  class BuffTableStorage
    : public BuffTable
    {
      BuffHandle hTab_[SZ];
      BuffHandle::PBuff pTab_[SZ];
      
    protected:
      BuffTableStorage (WiringDescriptor const& wd)
        {
          const uint nrI = wd.getNrI();
          const uint nrO = wd.getNrO();
          REQUIRE (nrI+nrO <= SZ);
          
          this->outHandle = &hTab_[0];
          this->inHandle  = &hTab_[nrO];
          this->outBuff   = &pTab_[0];
          this->inBuff    = &pTab_[nrO];
        }
    };
  
  
  
  template<uint SZ> /////////////////TODO: rather template on the BufferProvider
  class InvocationImpl
    : private BuffTableStorage<SZ>,
      public Invocation
    {

    protected:
      InvocationImpl (State& callingProcess, WiringDescriptor const& w, const uint outCh)
        : BuffTableStorage<SZ>(w),
          Invocation(callingProcess, w, static_cast<BuffTable&>(*this), outCh)
        { }
      
      /** contains the details of Cache query and recursive calls
       *  to the predecessor node(s), eventually followed by the 
       *  ProcNode::process() callback
       */
      template<class Strategy>
      BuffHandle retrieve ()
        {
          return Strategy::step (*this);
        }
    };

    
    
    
  class OperationBase
    {
      
    };
    
  template<class NEXT>
  struct QueryCache : NEXT
    {
      BuffHandle
      step (Invocation& ivo)
        {
          BuffHandle fetched = this->current_.fetch (
                                 this->genFrameID (
                                   this->requiredOutputNr));
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
      step (Invocation& ivo)
        {
          this->createBuffTable();
          
          ASSERT (this->buffTab);
          ASSERT (0 < this->buffTabSize());
          ASSERT (this->nrO == this->nrI );
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
          return NEXT::step (ivo);
        }
    };
  
  
  template<class NEXT>
  struct ReadSource : NEXT
    {
      BuffHandle
      step (Invocation& ivo)
        {
          this->createBuffTable();
          
          ASSERT (this->buffTab);
          ASSERT (0 < this->buffTabSize());
          ASSERT (this->nrO+this->nrI <= this->buffTabSize());
          ASSERT (this->buffTab->inHandles = &this->buffTab->handles[this->nrO]);
          BuffHandle           *inH  = this->buffTab->inHandles;
          BuffHandle           *outH = this->buffTab->handles;
          BuffHandle::PBuff *inBuff  = this->buffTab->inBuffs;
          BuffHandle::PBuff *outBuff = this->buffTab->buffers;
          
          for (uint i = 0; i < this->nrI; ++i )
            {
              inBuff[i] = outBuff[i] =
                *(inH[i] = outH[i] = this->getSource(i)); // TODO: how to access source nodes???
              // now Input #i is ready...
            }
          return NEXT::step (ivo);
        }
    };
  
  
  template<class NEXT, class BUFFSRC>
  struct AllocOutput
    {
      BuffHandle 
      step (Invocation& ivo)
        {
          ASSERT (this->buffTab);
          ASSERT (this->nrO < this->buffTabSize());
          BuffHandle           *outH = this->buffTab->handles;
          BuffHandle::PBuff *outBuff = this->buffTab->buffers;
          
          for (uint i = 0; i < this->nrO; ++i )
            {
              outBuff[i] =
                 *(outH[i] = allocateBuffer (i));
              // now Output buffer for channel #i is available...
            }
          return NEXT::step (ivo);
        }
      
    private:
      BuffHandle const&
      allocateBuffer (uint outCh)
        {
          BUFFSRC::getBufferProvider(this).allocateBuffer(
                          this->getBufferDescriptor(outCh));
        }
    };
  
  
  enum OutBuffProvider { PARENT, CACHE };
  
  template<OutBuffProvider>
  struct OutBuffSource ;
  
  template<>
  struct OutBuffSource<PARENT>
    {
      static State& getBufferProvider (Invocation& thisState) { return thisState.parent_; }
    };
  
  template<>
  struct OutBuffSource<CACHE>
    {
      static State& getBufferProvider (Invocation& thisState) { return thisState.current_; }
    };
  
  
  template<class NEXT>
  struct ProcessData
    {
      BuffHandle 
      step (Invocation& ivo)
        {
          ASSERT (this->buffTab);
          ASSERT (this->nrO+this->nrI <= this->buffTabSize());
          ASSERT (this->validateBuffers());
          
           // Invoke our own process() function, providing the buffer array
          this->wiring_.processFunction (this->buffTab->buffers);
          
          return NEXT::step (ivo);
        }
    };
  
  template<class NEXT>
  struct FeedCache
    {
      BuffHandle 
      step (Invocation& ivo)
        {
          // declare all Outputs as finished
          this->current_.isCalculated(this->buffTab->handles, 
                                      this->nrO);
          
          return NEXT::step (ivo);
        }
    };
  
  template<class NEXT>
  struct ReleaseBuffers
    {
      BuffHandle 
      step (Invocation& ivo)
        {
          // all buffers besides the required Output no longer needed
          this->current_.releaseBuffers(this->buffTab->handles, 
                                        this->buffTabSize(), 
                                        this->requiredOutputNr);
          
          return this->buffTab->outH[this->requiredOutputNr];
        }
    };
  
  

  
  /* === declare the possible Assembly of these elementary steps === */
  
  template<class Config>
  struct Strategy ;

  
  template<char CACHE_Fl=0, char INPLACE_Fl=0>
  struct SelectBuffProvider;
  
  template<> struct SelectBuffProvider<CACHING>         : OutBuffSource<CACHE> { };
  template<> struct SelectBuffProvider<NOT_SET,INPLACE> : OutBuffSource<PARENT>{ };
  template<> struct SelectBuffProvider<CACHING,INPLACE> : OutBuffSource<CACHE> { };
  template<> struct SelectBuffProvider<>                : OutBuffSource<PARENT>{ };

  
  using lumiera::typelist::Config;
  
  template<char INPLACE_Fl>
  struct Strategy< Config<CACHING,PROCESS,INPLACE_Fl> >
    : QueryCache<
       PullInput<
        AllocOutput<SelectBuffProvider<CACHING,INPLACE_Fl>,
         ProcessData<
          FeedCache<
           ReleaseBuffers< 
            OperationBase > > > > > >
    { };
  
  template<char INPLACE_Fl>
  struct Strategy< Config<PROCESS,INPLACE_Fl> >
    : PullInput<
       AllocOutput<SelectBuffProvider<NOT_SET,INPLACE_Fl>,
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
       AllocOutput<OutBuffSource<CACHE>,
        ProcessData<                       // wiring_.processFunction is supposed to do just buffer copying here 
         ReleaseBuffers< 
          OperationBase > > > >  
    { };

  
    
  
} // namespace engine
#endif
