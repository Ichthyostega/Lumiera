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
 ** During the Builder run, the Render Node network will be constructed by gradually
 ** refining the connectivity structure derived from interpreting the »high-level Model«
 ** from the current Session. At some point, it is essentially clear what data streams
 ** must be produced and what media processing functionality from external libraries
 ** will be utilised to achieve the goal. This is when the fluent builder notation
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
 ** 
 ** # Building Render Nodes
 ** 
 ** At Level-2, actual render nodes are generated. The NodeBuilder creates a suitably configured
 ** \ref Connectivity object, which can be dropped directly into a ProcNode. Managing the storage
 ** of those Render Nodes themselves is beyond the scope of the builder; so the user of the builder
 ** is responsible for the lifecycle of generated ProcNode objects.
 ** 
 ** ## Flavours of the processing function
 ** The binding to the actual data processing operations (usually supplied by an external library)
 ** is established by a **processing-functor** passed to configure the [Port builder](\PortBuilderRoot::invoke()).
 ** The supported signatures of this functor are quite flexible to allow for various flavours of invocation.
 ** Data types of parameters and buffers are picked up automatically (at compile time), based on the
 ** signature of the actual function supplied. The accepted variations are described in detail
 ** [here](\ref feed-manifold.hpp). Basically, a function can take parameters, input- and output-buffers,
 ** yet only the output-buffers are mandatory. Several elements of one kind can be passed as tuple.
 ** 
 ** ## Handling of Invocation Parameters
 ** Typically, a processing operation can be configured in various ways, by passing additional
 ** setup- and invocation parameters. This entails both technical aspects (like picking some specific
 ** data format), organisational concerns (like addressing a specific frame-number) and elements of
 ** artistic control, like choosing the settings of a media processing effect. Parameters will thus
 ** be collected from various sources, which leads to an additional binding step, where all these
 ** sources are retrieved and the actual parameter value or value tuple is produced. This specific
 ** _parameter binding_ is represented as a **parameter-functor**. Whenever the processing-function
 ** accepts a parameter argument, optionally a such parameter-functor can be installed; this functor
 ** is supplied with the \ref TurnoutSystem of the actual invocation, which acts as front-end to
 ** access contextual parameters.
 ** \par synthesised additional parameters
 ** As an extension for (rare and elaborate) special cases, a special evaluation scheme is provided,
 ** which relies on a »Param Agent Node« as entry point, to invoke additional functors and compute
 ** additional parameter values, which can then be used in a nested _delegate Node tree._
 ** This special scheme can be configured from the [nested Port builder](\ref PortBuilderRoot::computeParam()).
 ** See the detailed description in param-weaving-pattern.hpp
 ** 
 ** @todo WIP-WIP 1/2025 Node-Invocation is reworked from ground up -- some parts can not be
 **       spelled out completely yet, since we have to build this tightly interlocked system of
 **       code moving bottom up, and then filling in further details later working top-down.
 ** @todo as of 1/2025, the Layer-3 builder seems to be beyond the current integration scope.  //////////////TICKET #1389 : develop a processing builder
 ** 
 ** @see steam::engine::NodeFactory
 ** @see nodewiring.hpp
 ** @see node-basic-test.cpp
 ** 
 */


#ifndef ENGINE_NODE_BUILDER_H
#define ENGINE_NODE_BUILDER_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "steam/engine/weaving-pattern-builder.hpp"
#include "steam/engine/media-weaving-pattern.hpp"
#include "steam/engine/param-weaving-pattern.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/engine/turnout.hpp"
#include "lib/several-builder.hpp"
#include "lib/format-string.hpp"
#include "lib/index-iter.hpp"

#include <utility>
#include <vector>


namespace steam {
namespace engine {
  namespace err = lumiera::error;
  
  using util::_Fmt;
  using std::forward;
  using std::move;
  using std::ref;
  
  
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
  
  
  
  
  /**
   * Top-level builder to create a single [Render Node](\ref ProcNode)
   * @note
   *     - Entry-point for the Builder-DSL is \ref prepareNode()
   *     - A sequence of Ports is defined by `.preparePort()` ... `.completePort()`
   *     - the Build is completed with the NodeBuilder::build() terminal,
   *       which generates a \ref Connectivity object, that can be directly
   *       dropped into the constructor of ProcNode.
   */
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
      
      
      NodeBuilder&&
      addLead (ProcNode const& lead)
        {
          leads_.append (ref(lead));
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
       *            AllocationCluster; this is the most relevant usage pattern.
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
  
  
  
  
  /**
   * Nested DSL builder scope to define a single Port for the enclosing Node.
   * @note inherits from NodeBuilder and _slices away_ the subclass when done.
   */
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
      
      /** setup a »ParamAgentNode« to compute additional parameters
       *  and then delegate into an existing node invocation. */
      template<class SPEC>
      auto computeParam(SPEC&&);
      
      
    private:
      PortBuilderRoot(NodeBuilder<POL,DAT>&& anchor)
        : NodeBuilder<POL,DAT>{move(anchor)}
        { }
      
      friend PortBuilderRoot NodeBuilder<POL,DAT>::preparePort();
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
  
  
  
  /**
   * Nested DSL-Builder context to define a regular media processing Port
   * @remark relies on [delegate sub-builder](\ref WeavingBuilder) for
   *         technical details of data feed and parameter wiring.
   */
  template<class POL, class DAT, class WAB>
  class PortBuilder
    : public PortBuilderRoot<POL,DAT>
    {
      using _Par = PortBuilderRoot<POL,DAT>;
      
      WAB weavingBuilder_;
      uint defaultPort_;
      
    public:
      
      /** @remark this would allow to pass constructor arguments
       *   to »buffer inlay objects« automatically placed into media buffers.
       *   There is a corresponding hook in BufferProvider::getDescriptor(args...) 
       */
      template<class ILA, typename...ARGS>
      PortBuilder&&
      createBuffers (ARGS&& ...args)
        {
          UNIMPLEMENTED ("define builder for all buffers to use");
          return move(*this);
        }
      
      
      /** define the output slot number to use as result
       * @remark default is to use the first one */
      PortBuilder&&
      asResultSlot (uint r)
        {
          weavingBuilder_.selectResultSlot(r);
          return move(*this);
        }
      
      /** connect the next input slot to existing lead-node given by index
       * @note the port to use on this lead is implicitly defaulted to use the same port-number
       *       as the port which is currently about to be built; this is a common pattern, since
       *       when a top-level node exposes N different flavours, its predecessors will very
       *       likely also be configured to produce the pre-product for these flavours.
       */
      PortBuilder&&
      connectLead (uint idx)
        {
          return connectLeadPort (idx, this->defaultPort_);
        }
      
      /** connect the next input slot to either existing or new lead-node" */
      PortBuilder&&
      connectLead (ProcNode& leadNode)
        {
          return connectLeadPort (leadNode, this->defaultPort_);
        }
      
      /** connect next input to lead-node, using a specific port-number */
      PortBuilder&&
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
      PortBuilder&&
      connectLeadPort (ProcNode& leadNode, uint port)
        {
          uint knownEntry{0};
          for (auto& lead : lib::IndexIter{_Par::leads_})  // leads_ holds ref-wrappers
            if (util::isSameObject (leadNode, lead.get()))
              break;
            else
              ++knownEntry;
          if (knownEntry == _Par::leads_.size())
            _Par::addLead (leadNode);
          
          ENSURE (knownEntry < _Par::leads_.size());
          return connectLeadPort (knownEntry, port);
        }
      
      /** use given port-index as default for all following connections */
      PortBuilder&&
      useLeadPort (uint defaultPort)
        {
          this->defaultPort_ = defaultPort;
          return move(*this);
        }
      
      /**
       * Embed the explicitly given parameter-functor into the FeedPrototype,
       * so that it will be called on each Node invocation to generate parameters
       * to be passed into the actual processing function. The TurnoutSystem acts
       * as source for the base coordinates, typically the _absolute nominal Time._
       * @return adapted PortBuilder marked with the `FeedPrototype` holding \a PFX
       */
      template<class PFX>
      auto
      attachParamFun (PFX paramFunctor)
        {
          using AdaptedWeavingBuilder = typename WAB::template Adapted<PFX>;
          using AdaptedPortBuilder = PortBuilder<POL,DAT,AdaptedWeavingBuilder>;
          //
          return AdaptedPortBuilder{move(*this)
                                   ,weavingBuilder_.adaptParam (move (paramFunctor))
                                   };
        }
      
      template<class AUTO>
      auto
      attachAutomation (AUTO&& aFun)
        {
          return attachParamFun ([automation = forward<AUTO>(aFun)]
                                 (TurnoutSystem& turnoutSys)
                                    {
                                      return automation (turnoutSys.getNomTime());
                                    });
        }
      
      template<typename PAR>
      auto
      setParam (PAR paramVal)
        {
          return attachParamFun ([=](TurnoutSystem&) -> PAR
                                    {
                                      return paramVal;
                                    });
        }
      
      template<typename PAR, typename...PARS>
      auto
      setParam (PAR v1, PARS ...vs)
        {
          return attachParamFun ([=](TurnoutSystem&) -> tuple<PAR,PARS...>
                                    {
                                      return std::make_tuple (v1,vs...);
                                    });
        }
      
      template<typename GET>
      auto
      retrieveParam (GET&& getter)
        {
          return attachParamFun ([accessor=forward<GET>(getter)]
                                 (TurnoutSystem& turnoutSys)
                                    {
                                      return accessor.getParamVal (turnoutSys);
                                    });
        }
      
      
      
      /*************************************************************//**
       * Terminal: complete the Port wiring and return to the node level.
       * @remark this prepares a suitable Turnout instance for a port;
       *         but due to constraints with memory allocation, actual build
       *         is delayed and packaged as functor into a PatternData instance.
       */
      auto
      completePort()
        {
          weavingBuilder_.connectRemainingInputs (_Par::leads_, this->defaultPort_);
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
        { }               // ^^^ by default use next free port
      
      friend class PortBuilderRoot<POL,DAT>;
      
      /** cross-builder to adapt embedded WeavingBuilder type */
      template<class WABO>
      PortBuilder (PortBuilder<POL,DAT,WABO>&& prevBuilder, WAB&& adaptedWeavingBuilder)
        : _Par{move(prevBuilder)}
        , weavingBuilder_{move (adaptedWeavingBuilder)}
        , defaultPort_{prevBuilder.defaultPort_}
        { }
      
      template<class PX, class DX, class WX>
      friend class PortBuilder;
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
      using Prototype = typename FeedManifold<FUN>::Prototype;
      using WeavingBuilder_FUN = WeavingBuilder<POL, Prototype>;
      return PortBuilder<POL,DAT, WeavingBuilder_FUN>{move(*this), move(fun), portSpec};
    }
  
  
  
  
  
  /**
   * Nested sub-Builder analogous to \ref PortBuilder, but for building  a _»Param Agent Node«._
   * This will compute additional parameters and make them temporarily accessible through the
   * TurnoutSystem of the invocation, but only while delegating recursively to another
   * computation node, which can then draw upon these additional parameter values.
   * @tparam SPEC a ParamBuildSpec, which is a sub-builder to define the parameter-functors
   *              evaluated on each invocation to retrieve the actual parameter values
   */
  template<class POL, class DAT, class SPEC>
  class ParamAgentBuilder
    : public PortBuilderRoot<POL,DAT>
    {
      using _Par = PortBuilderRoot<POL,DAT>;
      
      using BlockBuilder = typename SPEC::BlockBuilder;
      using PostProcessor = function<void(TurnoutSystem&)>;

      BlockBuilder blockBuilder_;
      PostProcessor postProcessor_;
      Port* delegatePort_;
      uint defaultPortNr_;

    public:
      /** use a lead node designated by ID as delegate to invoke with the extended parameters.
       * @note the port to use on this lead is implicitly defaulted to use the same port-number
       *       as the port which is currently about to be built; this is a common pattern, since
       *       when a top-level node exposes N different flavours, its predecessors will very
       *       likely also be configured to produce the pre-product for these flavours.
       */
      ParamAgentBuilder&&
      delegateLead (uint idx)
        {
          return delegateLeadPort (idx, defaultPortNr_);
        }
      
      /** use the given node as delegate, but also possibly register it as lead node */
      ParamAgentBuilder&&
      delegateLead (ProcNode& leadNode)
        {
          return delegateLeadPort (leadNode, defaultPortNr_);
        }
      
      /** use a lead node and specific port as delegate to invoke with extended parameters */
      ParamAgentBuilder&&
      delegateLeadPort (uint idx, uint port)
        {
          if (idx >= _Par::leads_.size())
            throw err::Logic{_Fmt{"Builder refers to lead-node #%d, yet only %d are currently defined."}
                                 % idx % _Par::leads_.size()
                            ,LERR_(INDEX_BOUNDS)
                            };
          ProcNode& leadNode = _Par::leads_[idx];
          delegatePort_ = & leadNode.getPort (port);
          return move(*this);
        }
      
      /** use the specific port on the given node as delegate,
       *  while possibly also registering it as lead node. */
      ParamAgentBuilder&&
      delegateLeadPort (ProcNode& leadNode, uint port)
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
          return delegateLeadPort (knownEntry, port);
        }
      
      
      /**
       * Install a post-processing function for the parameters.
       * This functor will be invoked after the individual parameter values have been created
       * by invoking their respective parameter-functor; furthermore, the parameter data block
       * in current scope has already been linked with the TurnoustSystem, and thus the new
       * parameters are already accessible through this front-end and can be manipulated.
       * @remark the purpose is to enable coordinated adjustments on all parameters together,
       *         immediately before delegating to the nested node evaluation with these parameters.
       */
      ParamAgentBuilder&&
      installPostProcessor(PostProcessor pp)
        {
          postProcessor_ = move(pp);
        }
      
      
      /*********************************************************************//**
       * Terminal: complete the Param-Agent wiring and return to the node level.
       * @remark this prepares a suitable Turnout instance for a port; it will
       *         actually built later, together with other ports of this Node.
       */
      auto
      completePort()
        {
          if (not delegatePort_)
            throw err::Logic{"Building a ParamAgentNode requires a delegate node "
                             "to perform within the scope with extended parameters"
                            ,LERR_(BOTTOM_VALUE)};
          string portSpec = "Par+"+delegatePort_->procID.genProcSpec();
          ProcAttrib flags;
          flags.isProxy = true;
          flags.manifold = false;
          
          using WeavingPattern  = ParamWeavingPattern<SPEC>;
          using TurnoutWeaving  = Turnout<WeavingPattern>;
          using PortDataBuilder = DataBuilder<POL, Port>;
          
          return NodeBuilder ( static_cast<NodeBuilder<POL,DAT>&&> (*this) // slice away PortBulder subclass data
                             , SizMark<sizeof(TurnoutWeaving)>{}
                             ,// prepare a builder-λ to construct the actual Turnout-object
                              [procID = ProcID::describe(_Par::symbol_,portSpec,flags)
                              ,builder = move(blockBuilder_)
                              ,postProc = move(postProcessor_)
                              ,delegate = delegatePort_
                              ]
                              (PortDataBuilder& portData) mutable -> void
                                {
                                  portData.template emplace<TurnoutWeaving> (procID
                                                                            ,move(builder)
                                                                            ,move(postProc)
                                                                            ,*delegate
                                                                            );
                                });
        }                     // chain back up to Node-Builder with extended patternData
      
    private:
      ParamAgentBuilder(_Par&& base, BlockBuilder&& builder)
        : _Par{move(base)}
        , blockBuilder_{move(builder)}
        , delegatePort_{nullptr}
        , defaultPortNr_{_Par::patternData_.size()}
        { }                 // ^^^ by default use next free port
      
      friend class PortBuilderRoot<POL,DAT>;
    };
  
  
  /** @remarks
   * - this is an advanced setup for generating a complex set of _derived parameters,_
   *   which can then be used by all nodes within a complete subtree of the node-graph.
   * - such a setup is not necessary for simple parameters based on nominal timeline time.
   * - the purpose is either to avoid redundancy or to draw from additional contextual
   *   parameter sources (which must be accessible with the help of the processKey or
   *   some global service or plug-in)
   * - another special scenario could be to synthesise further data based on the consolidated
   *   set of current automation values, possibly together with contextual data; basically
   *   some kind of _parameter fusion_ that can not reasonably be pre-defined in the
   *   High-level-Model, but must really be computed late, directly from the render process.
   * - this function enters a nested port-builder, which will setup a »Param Weaving Pattern«
   * - at Node invocation time, this _Weaving Pattern_ will first evaluate all parameter-funcors,
   *   then consolidate the generated parameters into a local data block on the stack and link
   *   this data block into the TurnoutSystem of this invocation; after establishing this
   *   quite tricky and fragile setup, the invocation will recursively delegate to another
   *   Node-Port, which thus performs in this extended scope and can refer to all the
   *   additional parameters.
   * - To define the set of parameter-functors, you need to use a helper-builder based on
   *   \ref steam::engine::ParamBuildSpec, starting with \ref steam::engine::buildParamSpec().
   * - this generates a _Param Spec,_ which especially provides _accessor functors_ for each
   *   of the additional parameters; you need to bind these accessor functors into the
   *   parameter-functors of nested nodes which want to access the additional parameters.
   * - it is thus necessary first to build the _Param Spec,_ then to build the complete
   *   subtree of processing nodes for the actual processing (aka the _delegate tree_),
   *   and then finally create a _Param Agent Node_ with this builder, referring to the
   *   entry point into the processing tree as _delegate lead_ (see \ref ParamAgentBuilder)
   * @return a nested \ref ParamAgentBuilder to set up the desired wiring and delegate
   * @see NodeFeed_test::feedParamNode()
   */
  template<class POL, class DAT>
  template<class SPEC>
  auto
  PortBuilderRoot<POL,DAT>::computeParam(SPEC&& ref)
    {
      using ParamBuildSpec = std::decay_t<SPEC>;
      ParamBuildSpec spec {forward<SPEC>(ref)};                      // consumes the spec
      return ParamAgentBuilder<POL,DAT,ParamBuildSpec>{move(*this), spec.makeBlockBuilder()};
    }
  
  
  
  
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
      
      void //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1389 : return type
      requiredSources ()
        {
          UNIMPLEMENTED ("enumerate all source feeds required");
//        return move(*this);
        }
      
      void //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1389 : return type
      retrieve (void* streamType)
        {
          (void)streamType;
          UNIMPLEMENTED ("recursively define a predecessor feed");
//        return move(*this);
        }
      
      /****************************************************//**
       * Terminal: trigger the Level-3 build walk to produce a ProcNode network.
       */
      void //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1389 : return type
      build()
        {
          UNIMPLEMENTED("Level-3 build-walk");
        }
    };
  
  
  class LinkBuilder
    : util::MoveOnly
    {
    public:
      
      void //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1389 : return type
      from (void* procAsset)
        {
          (void)procAsset;
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
    (void)streamType;
    UNIMPLEMENTED("start a connectivity definition at Level-3");
    return LinkBuilder{};  //////////////////////////////////////////////////////////////////////////////////TICKET #1389 : this is placeholder code....
  }
  
  
}} // namespace steam::engine
#endif /*ENGINE_NODE_BUILDER_H*/
