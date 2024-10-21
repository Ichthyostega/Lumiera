/*
  NODE-BUILDER.hpp  -  Setup of render nodes connectivity

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
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

/** @file node-builder.hpp
 ** Specialised shorthand notation for building the Render Node network.
 ** During the Builder run, the render nodes network will be constructed by gradually
 ** refining the connectivity structure derived from interpreting the »high-level model«
 ** from the current Session. At some point, it is essentially clear what data streams
 ** must be produced and what media processing functionality from external libraries
 ** will be utilised to achieve this goal. This is when the fluent builder notation
 ** defined in this header comes into play, allowing to package the fine grained and
 ** in part quite confusing details of parameter wiring and invocation preparation into
 ** some goal oriented building blocks, that can be combined and directed with greater
 ** clarity by the control structure to govern the build process.
 ** 
 ** # Levels of connectivity building
 ** 
 ** The actual node connectivity is established by a process of gradual refinement,
 ** operating over several levels of abstraction. Each of these levels uses its associated
 ** builder and descriptor records to collect information, which is then emitted by a
 ** _terminal invocation_ to produce the result; the higher levels thereby rely on the
 ** lower levels to fill in and elaborate the details.
 ** - Level-1 is the preparation of an actual frame processing operation; the Level-1-builder
 **   is in fact the implementation class sitting behind a Render Node's _Port._ It is called
 **   a _Turnout_ and contains a preconfigured »blue print« for the data structure layout
 **   used for the invocation; its purpose is to generate the actual data structure on the
 **   stack, holding all the necessary buffers and parameters ready for invoking the external
 **   library functions. Since the actual data processing is achieved by a _pull processing,_
 **   originating at the top level exit nodes and propagating down towards the data sources,
 **   all the data feeds at all levels gradually link together, forming a _TurnoutSystem._
 ** - Level-2 generates the actual network of Render Nodes, which in turn will have the
 **   Turnout instances for Level-1 embedded into its internal ports. Conceptually, a
 **   _Port_ is where data production can be requested, and the processing will then
 **   retrieve its prerequisite data from the ports of the _Leads,_ which are the
 **   prerequisite nodes situated one level below or one step closer to the source.
 ** - Level-3 establishes the processing steps and data retrieval links between them;
 **   at this level, thus the outline of possible processing pathways is established.
 **   After spelling out the desired connectivity at a high level, the so called »Level-3 build
 **   walk« is triggered by invoking the [terminal builder operation](\ref ProcBuilder::build()
 **   on the [processing builder](\ref ProcBuilder) corresponding to the topmost node. This
 **   build walk will traverse the connectivity graph depth-first, and then start invoking the
 **   Level-2 builder operations bottom-up to generate and wire up the corresponding Render Nodes.
 ** 
 ** ## Using custom allocators
 ** 
 ** Since the low-level-Model is a massive data structure comprising thousands of nodes, each with
 ** specialised parametrisation for some media handling library, and a lot of cross-linking pointers,
 ** it is important to care for efficient usage of memory with good locality. Furthermore, the higher
 ** levels of the build process will generate additional temporary data structures, which is gradually
 ** refined until the actual render node network can be emitted. Each builder level can thus be
 ** outfitted with a custom allocator — typically an instance of lib::AllocationCluster. Notably
 ** the higher levels can be attached to a separate AllocationCluster instance, which will be
 ** discarded when the build process is complete, while Level-2 (and below) uses the allocator
 ** for the actual target data structure, which will be retained and until a complete segment
 ** of the timeline is superseded and has been re-built.
 ** @remark syntactically, the custom allocator specification is given after opening a top-level
 **         builder, by means of the builder function `.withAllocator<ALO> (args...)`
 ** 
 ** @todo WIP-WIP-WIP 7/2024 Node-Invocation is reworked from ground up -- some parts can not be
 **       spelled out completely yet, since we have to build this tightly interlocked system of
 **       code moving bottom up, and then filling in further details later working top-down.
 ** 
 ** @see steam::engine::NodeFactory
 ** @see nodewiring.hpp
 ** @see node-basic-test.cpp
 ** 
 */


#ifndef ENGINE_NODE_BUILDER_H
#define ENGINE_NODE_BUILDER_H


#include "steam/engine/weaving-pattern-builder.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/engine/turnout.hpp"
#include "lib/several-builder.hpp"
#include "lib/nocopy.hpp"

#include <utility>
#include <vector>


namespace steam {
namespace engine {
  
  using std::move;
  using std::forward;
  
  
  namespace { // default policy configuration to use heap allocator
    
    struct UseHeapAlloc
      {
        template<class I, class E=I>
        using Policy = lib::allo::HeapOwn<I,E>;
      };
    //
  }//(End) policy
  
  
  /**
   * A builder to collect working data.
   * Implemented through a suitable configuration of lib::SeveralBuilder,
   * with a policy configuration parameter to define the allocator to use.
   */
  template<class POL, class I, class E=I>
  using DataBuilder = lib::SeveralBuilder<I,E, POL::template Policy>;
  
  
  template<class POL, class DAT>
  class PortBuilderRoot;
  
  template<class POL, class DAT = PatternDataAnchor>
  class NodeBuilder
    : util::MoveOnly
    {
      using PortData = DataBuilder<POL, Port>;
      using LeadRefs = DataBuilder<POL, ProcNodeRef>;
      
    protected:
      PortData ports_; ///////////////////////////////////////OOO obsolete and replaced by patternData_
      LeadRefs leads_;
      
      DAT patternData_;
      
    public:
      template<typename...INIT>
      NodeBuilder (INIT&& ...alloInit)
        : ports_{forward<INIT> (alloInit)...}    /////////////OOO obsolete and replaced by patternData_
        , leads_{forward<INIT> (alloInit)...}
        { }
      
      template<class BUILD, uint siz, class D0>
      NodeBuilder (NodeBuilder<POL,D0>&& pred, BUILD&& entryBuilder)
        : ports_{}                               /////////////OOO obsolete and replaced by patternData_
        , leads_{move (pred.leads_)}
        , patternData_{move (pred.patternData_), forward<BUILD> (entryBuilder)}
        { }
      
      
      NodeBuilder
      addLead (ProcNode const& lead)
        {
          UNIMPLEMENTED ("append the given predecessor node to the sequence of leads");
          return move(*this);
        }
      
      
      /** recursively enter detailed setup of a single processing port */
      PortBuilderRoot<POL,DAT> preparePort();
      
      
      /**
       * cross-builder function to specify usage of a dedicated *node allocator*
       * @tparam ALO (optional) spec for the allocator to use
       * @tparam INIT (optional) initialisation arguments for the allocator
       * @remarks this is a front-end to the extension point for allocator specification
       *          exposed through lib::SeveralBuilder::withAllocator(). The actual meaning
       *          of the given parameters and the choice of the actual allocator happens
       *          through resolution of partial template specialisations of the extension
       *          point lib::allo::SetupSeveral. Some notable examples
       *          - withAllocator<ALO>() attaches to a _monostate_ allocator type.
       *          - `withAllocator<ALO> (ALO<X> allo)` uses a C++ standard allocator
       *            instance `allo`, dedicated to produce objects of type `X`
       *          - `withAllocator (AllocationCluster&)` attaches to a specific
       *            AllocationCluster; this is the most relevant usage pattern
       */
      template<template<typename> class ALO =std::void_t, typename...INIT>
      auto
      withAllocator (INIT&& ...alloInit)
        {
          using AllocatorPolicy = lib::allo::SetupSeveral<ALO,INIT...>;
          return NodeBuilder<AllocatorPolicy>{forward<INIT>(alloInit)...};
        }
      
      
      /************************************************************//**
       * Terminal: complete the ProcNode Connectivity defined thus far.
       */
      Connectivity
      build()
        {
          return Connectivity{ports_.build()
                             ,leads_.build()
                             ,NodeID{}}; //////////////////////////////////////OOO what's the purpose of the NodeID??
        }
    };
  
  
  
  template<class POL, class DAT>
  class PortBuilderRoot
    : protected NodeBuilder<POL,DAT>
    {
    public:
      NodeBuilder<POL,DAT>
      completePort()
        {
          static_assert(not sizeof(POL),
            "can not build a port without specifying a processing function");
        }
      
      /** setup standard wiring to adapt the given processing function.
       * @return a PortBuilder specialised to wrap the given \a FUN */
      template<typename FUN>
      auto invoke (FUN&& fun);
      
      /** specify an `InvocationAdapter` to use explicitly. */
      template<class ADA, typename...ARGS>
      auto adaptInvocation(ARGS&& ...args);
      
      
    private:
      PortBuilderRoot(NodeBuilder<POL>&& anchor)
        : NodeBuilder<POL>{move(anchor)}
        { }
      
      friend PortBuilderRoot NodeBuilder<POL>::preparePort();
    };
  
  /**
   * @remark while _logically_ this builder-function _descends_ into the
   *  definition of a port, for the implementation we _wrap_ the existing
   *  NodeBuilder and layer a PortBuilder subclass „on top“ — thereby shadowing
   *  the enclosed original builder temporarily; the terminal builder operation
   *  PortBuilder::completePort() will unwrap and return the original NodeBuilder.
   */
  template<class POL, class DAT>
  inline PortBuilderRoot<POL, DAT>
  NodeBuilder<POL,DAT>::preparePort ()
  {
    return PortBuilderRoot<POL,DAT>{move(*this)};
  }
  
  
  
  template<class POL, class DAT, class WAB>
  class PortBuilder
    : public PortBuilderRoot<POL,DAT>
    {
      using _Par = PortBuilderRoot<POL,DAT>;
      
      WAB weavingBuilder_;
      
    public:
      
      template<class ILA, typename...ARGS>
      PortBuilder
      createBuffers (ARGS&& ...args)
        {
          UNIMPLEMENTED ("define builder for all buffers to use");
          return move(*this);
        }
      
      PortBuilder
      asResultSlot (uint r)
        {
          UNIMPLEMENTED ("define the output slot to use as result (default is the first one)");
          return move(*this);
        }
      
      PortBuilder
      connectLead (uint idx)
        {
          UNIMPLEMENTED ("connect the next input slot to existing lead-node given by index");
          return move(*this);
        }
      
      PortBuilder
      conectLead (ProcNode& leadNode)
        {
          UNIMPLEMENTED ("connect the next input slot to either existing or new lead-node");
          return move(*this);
        }
      
      PortBuilder
      connectLeadPort (uint idx, uint port)
        {
          UNIMPLEMENTED ("connect next input to lead-node, using a specific port-number");
          return move(*this);
        }
      
      PortBuilder
      connectLeadPort (ProcNode& leadNode, uint port)
        {
          UNIMPLEMENTED ("connect next input to existing or new lead-node, with given port-number");
          return move(*this);
        }
      
      PortBuilder
      useLeadPort (uint defaultPort)
        {
          UNIMPLEMENTED ("use given port-index as default for all following connections");
          return move(*this);
        }
      
      
      /*************************************************************//**
       * Terminal: complete the Port wiring and return to the node level.
       */
      NodeBuilder<POL,DAT>  ////////////////////////////////////////OOO need to extend and evolve the DAT parameter here
      completePort()
        {
          //////////////////////////////////////////////////////////OOO need to provide all links to lead nodes here
          weavingBuilder_.fillRemainingBufferTypes();
          _Par::ports_.append(weavingBuilder_.build());
          return static_cast<NodeBuilder<POL,DAT>&&> (*this);
        }                  // slice away the subclass
      
    private:
      template<typename FUN>
      PortBuilder(_Par&& base, FUN&& fun)
        : _Par{move(base)}
        , weavingBuilder_{forward<FUN> (fun)}
        { }
      
      friend class PortBuilderRoot<POL,DAT>;
    };
  
  
  template<class POL, class DAT>
  template<typename FUN>
  auto
  PortBuilderRoot<POL,DAT>::invoke (FUN&& fun)
    {
      using WeavingBuilder_FUN = WeavingBuilder<POL, manifoldSiz<FUN>(), FUN>;
      return PortBuilder<POL,DAT, WeavingBuilder_FUN>{move(*this), forward<FUN> (fun)};
    }
/*
  template<class POL>
  template<class ADA, typename...ARGS>
  auto
  PortBuilderRoot<POL>::adaptInvocation(ARGS&& ...args)
    {
      return move(*this);
    }
  */
  
  /**
   * Entrance point for building actual Render Node Connectivity (Level-2)
   * @note when using a custom allocator, the first follow-up builder function
   *       to apply should be `withAllocator<ALO>(args...)`, prior to adding
   *       any further specifications and data elements.
   */
  inline auto
  prepareNode()
  {
    return NodeBuilder<UseHeapAlloc>{};
  }
  
  
  
  
  
  class ProcBuilder
    : util::MoveOnly
    {
    public:
      
      void //////////////////////////////////////////////////////////OOO return type
      requiredSources ()
        {
          UNIMPLEMENTED ("enumerate all source feeds required");
//        return move(*this);
        }
      
      void //////////////////////////////////////////////////////////OOO return type
      retrieve (void* streamType)
        {
          UNIMPLEMENTED ("recursively define a predecessor feed");
//        return move(*this);
        }
      
      /****************************************************//**
       * Terminal: trigger the Level-3 build walk to produce a ProcNode network.
       */
      void //////////////////////////////////////////////////////////OOO return type
      build()
        {
          UNIMPLEMENTED("Level-3 build-walk");
        }
    };
  
  
  class LinkBuilder
    : util::MoveOnly
    {
    public:
      
      void //////////////////////////////////////////////////////////OOO return type
      from (void* procAsset)
        {
          UNIMPLEMENTED ("recursively enter definition of processor node to produce this feed link");
//        return move(*this);
        }
    };
  
  
  
  /**
   * Entrance point for defining data flows and processing steps.
   */
  inline auto
  retrieve(void* streamType)
  {
    UNIMPLEMENTED("start a connectivity definition at Level-3");
    return LinkBuilder{};  ///////////////////////////////////////////////////////////////////OOO this is placeholder code; should at least open a ticket
  }
  
  
}} // namespace steam::engine
#endif /*ENGINE_NODE_BUILDER_H*/
