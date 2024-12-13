/*
  NODE-BUILDER.hpp  -  Setup of render nodes connectivity

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
 ** 
 ** # Levels of connectivity building
 ** 
 ** The actual node connectivity is established by a process of gradual refinement,
 ** operating over several levels of abstraction. Each of these levels uses its associated
 ** builder and descriptor records to collect information, which is then emitted by a
 ** _terminal invocation_ to produce the result; the higher levels thereby rely on the
 ** lower levels to fill in and elaborate the details.
 ** - *Level-1* is the preparation of an actual frame processing operation; the Level-1-builder
 **   is in fact the implementation class sitting behind a Render Node's _Port._ It is called
 **   a _Turnout_ and contains a preconfigured »blue print« for the data structure layout
 **   used for the invocation; its purpose is to generate the actual data structure on the
 **   stack, holding all the necessary buffers and parameters ready for invoking the external
 **   library functions. Since the actual data processing is achieved by a _pull processing,_
 **   originating at the top level exit nodes and propagating down towards the data sources,
 **   all the data feeds at all levels gradually link together, forming a _TurnoutSystem._
 ** - *Level-2* generates the actual network of Render Nodes, which in turn will have the
 **   Turnout instances for Level-1 embedded into their internal ports. Conceptually, a
 **   _Port_ is where data production can be requested, and the processing will then
 **   retrieve its prerequisite data from the ports of the _Leads,_ which are the
 **   prerequisite nodes situated one level below or one step closer to the source.
 ** - *Level-3* establishes the processing steps and data retrieval links between them;
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
 ** levels of the build process will generate additional temporary data structures, refined gradually
 ** until the actual render node network can be emitted. Each builder level can thus be  outfitted
 ** with a custom allocator — typically an instance of lib::AllocationCluster. Notably the higher
 ** levels can be attached to a separate AllocationCluster instance, which will be discarded once
 ** the build process is complete, while Level-2 (and below) uses the allocator for the actual
 ** target data structure, which has to be retained while the render graph is used; more
 ** specifically until a complete segment of the timeline is superseded and has been re-built.
 ** @remark syntactically, the custom allocator specification is given after opening a top-level
 **         builder, by means of the builder function `.withAllocator<ALO> (args...)`
 ** 
 ** @todo WIP-WIP-WIP 10/2024 Node-Invocation is reworked from ground up -- some parts can not be
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


#include "lib/error.hpp"
//#include "lib/symbol.hpp"//////////////////////////////TODO RLY?
#include "lib/nocopy.hpp"
#include "steam/engine/weaving-pattern-builder.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/engine/turnout.hpp"
#include "lib/several-builder.hpp"
#include "lib/format-string.hpp"
#include "lib/index-iter.hpp"
#include "lib/test/test-helper.hpp"/////////////////////TODO TOD-oh

#include <utility>
#include <vector>


namespace steam {
namespace engine {
  namespace err = lumiera::error;
  
//  using lib::Literal;
  using util::_Fmt;
  using std::forward;
  using std::move;
  
  
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
  class NodeBuilder;
  
  template<class POL, class DAT>
  class PortBuilderRoot;

  
  
  
  template<class POL, class DAT = PatternDataAnchor>
  class NodeBuilder
    : util::MoveOnly
    {
      using PortData = DataBuilder<POL, Port>;
      using LeadRefs = DataBuilder<POL, ProcNodeRef>;
      
    protected:
      StrView symbol_;
      LeadRefs leads_;
      DAT patternData_;
      
    public:
      template<typename...INIT>
      NodeBuilder (StrView nodeSymbol, INIT&& ...alloInit)
        : symbol_{nodeSymbol}
        , leads_{forward<INIT> (alloInit)...}
        { }
      
      template<class BUILD, uint siz, class D0>
      NodeBuilder (NodeBuilder<POL,D0>&& pred, SizMark<siz>, BUILD&& entryBuilder)
        : symbol_{pred.symbol_}
        , leads_{move (pred.leads_)}
        , patternData_{move (pred.patternData_), forward<BUILD> (entryBuilder)}
        { }
      
      template<class P, class D0>
      friend class NodeBuilder;
      
      
      NodeBuilder
      addLead (ProcNode const& lead)
        {
          leads_.append (lead);
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
          return NodeBuilder<AllocatorPolicy>{symbol_, forward<INIT>(alloInit)...};
        }
      
      
      /************************************************************//**
       * Terminal: complete the ProcNode Connectivity defined thus far.
       */
      Connectivity
      build()
        {
          PortData ports;
          patternData_.collectEntries(ports);
          return Connectivity{ports.build()
                             ,leads_.build()
                             };
        }
    };
  
  /** Deduction Guide: help the compiler with deducing follow-up NodeBuilder parameters */
  template<class POL, class D0, uint siz, class BUILD>
  NodeBuilder (NodeBuilder<POL,D0>&&, SizMark<siz>, BUILD&&) -> NodeBuilder<POL, PatternData<D0,BUILD,siz>>;
  
  
  
  
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
      auto invoke (StrView portSpec, FUN fun);
      
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
      uint defaultPort_;
      
    public:
      
      template<class ILA, typename...ARGS>
      PortBuilder
      createBuffers (ARGS&& ...args)
        {
          UNIMPLEMENTED ("define builder for all buffers to use");
          return move(*this);
        }
      
      /** define the output slot to use as result
       * @remark default is to use the first one */
      PortBuilder
      asResultSlot (uint r)
        {
          weavingBuilder_.selectResultSlot(r);
          return move(*this);
        }
      
      /** connect the next input slot to existing lead-node given by index */
      PortBuilder
      connectLead (uint idx)
        {
          return connectLeadPort (idx, this->defaultPort_);
        }
      
      /** connect the next input slot to either existing or new lead-node" */
      PortBuilder
      conectLead (ProcNode& leadNode)
        {
          return connectLeadPort (leadNode, this->defaultPort_);
        }
      
      /** connect next input to lead-node, using a specific port-number */
      PortBuilder
      connectLeadPort (uint idx, uint port)
        {
          if (idx >= _Par::leads_.size())
            throw err::Logic{_Fmt{"Builder refers to lead-node #%d, yet only %d are currently defined."}
                                 % idx % _Par::leads_.size()
                            ,LERR_(INDEX_BOUNDS)
                            };
          weavingBuilder_.attachToLeadPort (_Par::leads_[idx], port);
          return move(*this);
        }
      
      /** connect next input to existing or new lead-node, with given port-number */
      PortBuilder
      connectLeadPort (ProcNode& leadNode, uint port)
        {
          uint knownEntry{0};
          for (auto& lead : lib::IndexIter{_Par::leads_})
            if (util::isSameObject (leadNode, lead))
              break;
            else
              ++knownEntry;
          if (knownEntry == _Par::leads_.size())
            _Par::addLead (leadNode);
          
          ENSURE (knownEntry < _Par::leads_.size());
          weavingBuilder_.attachToLeadPort (knownEntry, port);
          return move(*this);
        }
      
      /** use given port-index as default for all following connections */
      PortBuilder
      useLeadPort (uint defaultPort)
        {
          this->defaultPort_ = defaultPort;
          return move(*this);
        }
      
      
      /*************************************************************//**
       * Terminal: complete the Port wiring and return to the node level.
       */
      auto
      completePort()
        {
          weavingBuilder_.connectRemainingInputs (_Par::leads_, this->defaultPort_);
          weavingBuilder_.fillRemainingBufferTypes();
          return NodeBuilder{static_cast<NodeBuilder<POL,DAT>&&> (*this) // slice away PortBulder subclass data
                            ,weavingBuilder_.sizMark
                            ,weavingBuilder_.build()};
        }                 // chain to builder with extended patternData
      
    private:
      template<typename FUN>
      PortBuilder(_Par&& base, FUN&& fun, StrView portSpec)
        : _Par{move(base)}
        , weavingBuilder_{forward<FUN> (fun), _Par::symbol_, portSpec, _Par::leads_.policyConnect()}
        , defaultPort_{_Par::patternData_.size()}
        { }
      
      friend class PortBuilderRoot<POL,DAT>;
    };
  
  
  /**
   * @param qualifier a semantic distinction of the implementation function
   * @param fun invocation of the actual _data processing operation._
   * @remarks
   *  - a _»weaving pattern«_ is applied for the actual implementation, which amounts
   *    to a specific style how to route data input and output and how to actually integrate
   *    with the underlying media handling library, which exposes the processing functionality.
   *  - the standard case of this connectivity is to associate input and output connections
   *    directly with the »parameter slots« of the processing function; a function suitable
   *    for this pattern takes two arguments (input, output) — each of which is a std::array
   *    of buffer pointers, corresponding to the »parameter slots«
   *  - what is bound as \a FUN here thus typically is either an adapter function provided by
   *    the media-library plug-in, or it is a lambda directly invoking implementation functions
   *    of the underlying library, using a buffer type (size) suitable for this library and for
   *    the actual media frame data to be processed.
   *  - the `fun` is deliberately _taken by-value_ and then moved into a »prototype copy« within
   *    the generated `Turnout`, from which an actual copy is drawn anew for each node invocation.
   *  - notably this implies that the implementation code of a lambda will be _inlined_ into the
   *    actual invocation call, while possibly _creating a copy_ of value-captured closure data;
   *    this arrangement aims at exposing the actual invocation for the optimiser.
   */
  template<class POL, class DAT>
  template<typename FUN>
  auto
  PortBuilderRoot<POL,DAT>::invoke (StrView portSpec, FUN fun)
    {
      using WeavingBuilder_FUN = WeavingBuilder<POL, manifoldSiz<FUN>(), FUN>;
      return PortBuilder<POL,DAT, WeavingBuilder_FUN>{move(*this), move(fun), portSpec};
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
  prepareNode (StrView nodeSymbol)
  {
    return NodeBuilder<UseHeapAlloc>{nodeSymbol};
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
