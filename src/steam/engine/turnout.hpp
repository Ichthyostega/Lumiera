/*
  TURNOUT.hpp  -  Organise the invocation state within a single pull() call

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file turnout.hpp
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
 ** @warning as of 4/2023 a complete rework of the Dispatcher is underway ///////////////////////////////////////////TICKET #1275
 ** 
 */


#ifndef STEAM_ENGINE_TURNOUT_H
#define STEAM_ENGINE_TURNOUT_H

#include "steam/common.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/engine/turnout-system.hpp"
#include "steam/engine/feed-manifold.hpp"
#include "steam/engine/state-closure.hpp"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
#include "steam/engine/channel-descriptor.hpp"
//#include "vault/gear/job.h"
//#include "steam/engine/exit-node.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/linked-elements.hpp"
//#include "lib/util-foreach.hpp"
//#include "lib/iter-adapter.hpp"
#include "lib/meta/function.hpp"
//#include "lib/itertools.hpp"
//#include "lib/util.hpp"

//#include <utility>
#include <array>
//#include <stack>


namespace steam {
namespace engine {
  
  
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
  /**
   * Adapter to shield the ProcNode from the actual buffer management,
   * allowing the processing function within ProcNode to use logical
   * buffer IDs. StateAdapter is created on the stack for each pull()
   * call, using setup/wiring data preconfigured by the builder.
   * Its job is to provide the actual implementation of the Cache
   * push / fetch and recursive downcall to render the source frames.
   */
  class StateAdapter
    : public StateClosure
    {
    protected:
      StateClosure& parent_;
      StateClosure& current_;
      
      StateAdapter (StateClosure& callingProcess)
        : parent_ (callingProcess),
          current_(callingProcess.getCurrentImplementation())
        { }
      
      virtual StateClosure& getCurrentImplementation () { return current_; }
      
      
      
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
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
  struct Invocation
    : StateAdapter
    {
      Connectivity const& wiring;
      const uint outNr;
      
      FeedManifold* feedManifold;
      
    protected:
      /** creates a new invocation context state, without FeedManifold */
      Invocation (StateClosure& callingProcess, Connectivity const& w, uint o)
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
      AllocBufferFromParent (StateClosure& sta, Connectivity const& w, const uint outCh)
        : Invocation(sta, w, outCh) {}
      
      virtual BuffHandle
      allocateBuffer (const lumiera::StreamType* ty) { return parent_.allocateBuffer(ty); }          ////////////TODO: actually implement the "allocate from parent" logic!
    };
  
  struct AllocBufferFromCache   ///< using the global current StateClosure, which will delegate to Cache
    : Invocation
    {
      AllocBufferFromCache (StateClosure& sta, Connectivity const& w, const uint outCh)
        : Invocation(sta, w, outCh) {}
      
      virtual BuffHandle
      allocateBuffer (const lumiera::StreamType* ty) { return current_.allocateBuffer(ty); }
    };
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
  
  
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
      ActualInvocationProcess (StateClosure& callingProcess, Connectivity const& w, const uint outCh)
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
  
    /**
     * Definition to emulate a _Concept_ for the *Invocation Adapter*.
     * For each Proc-Asset, the corresponding Library Adapter must provide
     * such adapters to access the input and result buffers and finally to
     * invoke the processing functions from this library.
     * - `connect(fanIn,fanOut)` access the _Feed Manifold_ and link the buffers
     * - `invoke()` invoke the processing function, passing the connected buffers
     */
    template<class ADA>
    constexpr void
    _verify_usable_as_InvocationAdapter()
    {
      ASSERT_MEMBER_FUNCTOR (&ADA::connect, void(uint, uint));
      ASSERT_MEMBER_FUNCTOR (&ADA::invoke, void());
    }
  
  
  struct WeavingPatternBase
      //////////////////////////////OOO non-copyable? move-only??
    {
      using Feed = FeedManifold<0>;
      
      Feed mount() { return Feed{}; }
      void pull (Feed&, TurnoutSystem&) { /* NOP */ }
      void shed (Feed&)                 { /* NOP */ }
      void weft (Feed&)                 { /* NOP */ }
      void fix  (Feed&)                 { /* NOP */ }
    };
  
  
  template<class MAN, class FUN>
  struct InvocationAdapter
    : MAN
    {
      static constexpr auto N = MAN::inBuff::size();
      
      FUN process;
      
      void
      connect (uint fanIn, uint fanOut)
        {
          REQUIRE (fanIn <= N and fanOut <= N);
          UNIMPLEMENTED ("wire up all input/output buffers");
        }
      
      void
      invoke()
        {
          process();
        }
    };
  
  
  template<class PAR, uint N, class FUN>
  struct SimpleWeavingPattern
    : PAR
    {
      using Manifold = FeedManifold<N>;
      using Feed = InvocationAdapter<Manifold, FUN>;
      
      static_assert (_verify_usable_as_InvocationAdapter<Feed>());
      
      uint fanIn{0};
      uint fanOut{0};
      
      template<class X>
      using Storage = lib::UninitialisedStorage<X,N>;
      
      
      Storage<PortRef> leadPort;
      Storage<BufferDescriptor> inDescr;
      Storage<BufferDescriptor> outDescr;
      
      //////////////////////////////////////////OOO builder must set-up those descriptors
      
      Feed
      mount()
        {
          return Feed{};
        }
      
      void
      pull (Feed& feed, TurnoutSystem& turnoutSys)
        {
          for (uint i=0; i<fanIn; ++i)
            {
              BuffHandle inputData = leadPort[i].weave (turnoutSys);
              feed.inBuff.createAt(i, move(inputData));
            }
        }
      
      void
      shed (Feed&)
        {
          /* NOP */
        }
      
      void
      weft (Feed&)
        {
          /* NOP */
        }
      
      void
      fix (Feed&)
        {
          /* NOP */
        }
    };
  
  
  /**
   * Processing structure to activate a Render Node and produce result data.
   * @tparam PAT a _Weaving Pattern,_ which defines in detail how data is retrieved,
   *             combined and processed to yield the results; actually this implementation
   *             is assembled from several building blocks, in accordance to the specific
   *             situation as established by the _Builder_ for a given render node.
   */
  template<class PAT>
  class Turnout
    : public Port
    , public PAT
    {
      using Feed = typename PAT::Feed;
    public:
      
      /**
       * Entrance point to the next recursive step of media processing.
       * @param turnoutSys anchor context with parameters and services
       * @return a BuffHandle exposing the generated result data
       */
      BuffHandle
      weave (TurnoutSystem& turnoutSys)  override
        {
          Feed feed = PAT::mount();
          PAT::pull(feed, turnoutSys);
          PAT::shed(feed);
          PAT::weft(feed);
          PAT::fix (feed);
          return feed.result();
        }
    };
  
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_TURNOUT_H*/
