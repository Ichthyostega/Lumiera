/*
  NODEINVOCATION.hpp  -  Organise the invocation state within a single pull() call

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

/** @file nodeinvocation.hpp
 ** Organise the state related to the invocation of s single ProcNode::pull() call
 ** This header defines part of the "glue" which holds together the render node network
 ** and enables to pull result frames from the nodes. Doing so requires some invocation
 ** local state to be maintained, especially a table of buffers used to carry out the
 ** calculations. Further, getting the input buffers filled requires to issue recursive
 ** \c pull() calls, which on the whole creates a stack-like assembly of local invocation
 ** state.
 ** The actual steps to be carried out for a \c pull() call are dependent on the configuration
 ** of the node to pull. Each node has been preconfigured by the builder with a Connectivity
 ** descriptor and a concrete type of a StateAdapter. The actual sequence of steps is defined
 ** in the header nodeoperation.hpp out of a set of basic operation steps. These steps all use
 ** the passed in Invocation object (a sub-interface of StateAdapter) to access the various
 ** aspects of the invocation state.
 ** 
 ** # composition of the Invocation State
 ** 
 ** For each individual ProcNode#pull() call, the WiringAdapter#callDown() builds an StateAdapter
 ** instance directly on the stack, managing the actual buffer pointers and state references. Using this
 ** StateAdapter, the predecessor nodes are pulled. The way these operations are carried out is encoded
 ** in the actual StateAdapter type known to the NodeWiring (WiringAdapter) instance. All of these actual
 ** StateAdapter types are built as implementing the engine::StateClosure interface.
 ** 
 ** @todo relies still on an [obsoleted implementation draft](\ref bufftable-obsolete.hpp)
 ** @see engine::ProcNode
 ** @see engine::StateProxy
 ** @see engine::FeedManifold
 ** @see nodewiring.hpp interface for building/wiring the nodes
 **
 */

#ifndef ENGINE_NODEINVOCATION_H
#define ENGINE_NODEINVOCATION_H


#include "steam/engine/proc-node.hpp"
#include "steam/engine/state-closure-obsolete.hpp"
#include "steam/engine/channel-descriptor.hpp"
#include "steam/engine/feed-manifold.hpp"




namespace steam {
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
    : public StateClosure_OBSOLETE
    {
    protected:
      StateClosure_OBSOLETE& parent_;
      StateClosure_OBSOLETE& current_;
      
      StateAdapter (StateClosure_OBSOLETE& callingProcess)
        : parent_ (callingProcess),
          current_(callingProcess.getCurrentImplementation())
        { }
      
      virtual StateClosure_OBSOLETE& getCurrentImplementation () { return current_; }
      
      
      
    public: /* === proxying the StateClosure interface === */
      
      virtual void releaseBuffer (BuffHandle& bh)       { current_.releaseBuffer (bh); }
      
      virtual void is_calculated (BuffHandle const& bh) { current_.is_calculated (bh); }
      
      virtual BuffHandle fetch (FrameID const& fID)     { return current_.fetch (fID); }
      
      virtual BuffTableStorage& getBuffTableStorage()   { return current_.getBuffTableStorage(); }

      // note: allocateBuffer()  is chosen specifically based on the actual node wiring
      
    };
  
  
  
  
  
  /**
   * Invocation context state.
   * A ref to this type is carried through the chain of NEXT::step() functions
   * which form the actual invocation sequence. The various operations in this sequence
   * access the context via the references in this struct, while also using the inherited
   * public State interface. The object instance actually used as Invocation is created
   * on the stack and parametrised according to the necessities of the invocation sequence
   * actually configured. Initially, this real instance is configured without FeedManifold,
   * because the invocation may be short-circuited due to Cache hit. Otherwise, when
   * the invocation sequence actually prepares to call the process function of this
   * ProcNode, a buffer table chunk is allocated by the StateProxy and wired in.
   */
  struct Invocation
    : StateAdapter
    {
      Connectivity const& wiring;
      const uint outNr;
      
      FeedManifold* feedManifold;
      
    protected:
      /** creates a new invocation context state, without FeedManifold */
      Invocation (StateClosure_OBSOLETE& callingProcess, Connectivity const& w, uint o)
        : StateAdapter(callingProcess),
          wiring(w), outNr(o),
          feedManifold(0)
        { }
      
    public:
      uint nrO()          const { return wiring.nrO; }
      uint nrI()          const { return wiring.nrI; }
      uint buffTabSize()  const { return nrO()+nrI(); }
      
      /** setup the link to an externally allocated buffer table */
      void setBuffTab (FeedManifold* b) { this->feedManifold = b; }
      
      bool
      buffTab_isConsistent ()
        {
          return (feedManifold)
              && (0 < buffTabSize())
              && (nrO()+nrI() <= buffTabSize())
              && (feedManifold->inBuff   == &feedManifold->outBuff[nrO()]  )
              && (feedManifold->inHandle == &feedManifold->outHandle[nrO()])
               ;
        }
      
      
    public:
      /** specialised version filling in the additional information, i.e
       *  the concrete node id and the channel number in question */
      virtual FrameID const&
      genFrameID ()
        {
          return current_.genFrameID(wiring.nodeID, outNr);
        }
      
      virtual FrameID const&
      genFrameID (NodeID const& nID, uint chanNo)
        {
          return current_.genFrameID (nID,chanNo);
        }
      
    };
    
    
                                                                                                    ////////////TICKET #249  this strategy should better be hidden within the BuffHandle ctor (and type-erased after creation)
  struct AllocBufferFromParent  ///< using the parent StateAdapter for buffer allocations
    : Invocation
    {
      AllocBufferFromParent (StateClosure_OBSOLETE& sta, Connectivity const& w, const uint outCh)
        : Invocation(sta, w, outCh) {}
      
      virtual BuffHandle
      allocateBuffer (const lumiera::StreamType* ty) { return parent_.allocateBuffer(ty); }          ////////////TODO: actually implement the "allocate from parent" logic!
    };
  
  struct AllocBufferFromCache   ///< using the global current StateClosure, which will delegate to Cache
    : Invocation
    {
      AllocBufferFromCache (StateClosure_OBSOLETE& sta, Connectivity const& w, const uint outCh)
        : Invocation(sta, w, outCh) {}
      
      virtual BuffHandle
      allocateBuffer (const lumiera::StreamType* ty) { return current_.allocateBuffer(ty); }
    };
  
  
  /**
   * The real invocation context state implementation. It is created
   * by the NodeWiring (Connectivity) of the processing node which
   * is pulled by this invocation, hereby using the internal configuration
   * information to guide the selection of the real call sequence
   * 
   * \par assembling the call sequence implementation
   * Each ProcNode#pull() call creates such a StateAdapter subclass on the stack,
   * with a concrete type according to the Connectivity of the node to pull.
   * This concrete type encodes a calculation Strategy, which is assembled
   * as a chain of policy templates on top of OperationBase. For each of the
   * possible configurations we define such a chain (see bottom of nodeoperation.hpp).
   * The WiringFactory defined in nodewiring.cpp actually drives the instantiation
   * of all those possible combinations.
   */
  template<class Strategy, class BufferProvider>
  class ActualInvocationProcess
    : public BufferProvider
    , private Strategy
    {
    public:
      ActualInvocationProcess (StateClosure_OBSOLETE& callingProcess, Connectivity const& w, const uint outCh)
        : BufferProvider(callingProcess, w, outCh)
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
  
  
  
  
}} // namespace steam::engine
#endif
