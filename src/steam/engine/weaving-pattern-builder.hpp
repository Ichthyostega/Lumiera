/*
  WEAVING-PATTERN-BUILDER.hpp  -  build an invocation pattern for media calculations

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file weaving-pattern-builder.hpp
 ** Construction kit to establish an invocation scheme for media calculations.
 ** Adapters and configuration is provided to invoke the actual _media processing function_
 ** in accordance to a wiring scheme as implied by the _signature_ of the bound function.
 ** - the function takes one to three arguments
 ** - these are related to the parameters, the input and the output (always in that order)
 ** - the specification of at least one output buffer is mandatory
 ** - a function may omit input and / or the parameter «slot»
 ** - multiple items of the same kind (output, input, parameter) can be packaged
 **   into a heterogeneous tuple, or given as an array of identically typed elements;
 **   yet a single value can be accepted directly as function argument.
 ** - input/output buffers are recognisable as pointers, while parameters are value data.
 ** - pointers and parameter values are typed, which is used internally to ensure passing
 **   the right value to the corresponding item and to ensure suitable memory allocations.
 ** @note steam::engine::Turnout mixes-in the steam::engine::MediaWeavingPattern, which in turn
 **       inherits from a FeedManifold given as template parameter. So this constitutes an
 **       **extension point** where other, more elaborate invocation schemes could be integrated.
 ** 
 ** # Preparing a FeedManifold and handling invocation parameters
 ** 
 ** Detection of the processing function signature with all possible variations as detailed above
 ** is the responsibility of the [FeedManifold template](\ref feed-manifold.hpp). For each distinct
 ** signature, a suitable data layout is generated, including storage to hold the processing-functor
 ** itself (which is embedded as a clone-copy to expose the actual invocation to the optimiser in
 ** the C++ compiler). The WeavingBuilder defined here is used to build a Port implementation and
 ** thus a specific »Weaving Pattern«, which — at the actual Node invocation — will in turn build
 ** the concrete FeedManifold instance into local stack memory. For this reason, the Port can be
 ** understood as the Level-1 builder, whereas the Port / Weaving Builder is classified as Level-2
 ** and a processing and link-builder operating on top of the former is designated as Level-3.
 ** 
 ** The actual type of the FeedManifold, including all the specifics of the data layout, becomes
 ** embedded into the Port implementation (≙Weaving Pattern) by means of a FeedPrototype instance.
 ** Furthermore, a parameter-functor can be installed there, to generate actual parameter data
 ** whenever the FeedPrototype generates a new FeedManifold instance for the next render invocation.
 ** The parameter data (and a copy of the processing-functor) is stored alongside in this generation
 ** step, and thus available in local stack memory during an extended (possibly recursive) render
 ** invocation sequence.
 ** 
 ** Invocation parameters are a crucial ingredient for each invocation, yet the responsibility for
 ** the parameter-functor to produce these parameters lies in a different part of the system than
 ** the responsibility for configuring the processing functor. The reason is simply that the
 ** setup of actual parameters is an essential part of the user's work on the edit in the Session.
 ** The control flow for parameter thus traces back into the session, while on the other hand the
 ** processing-functor must be configured by an external media-library adapter Plug-in. So this
 ** creates the challenge that in actual use the PortBuilder will be passed through several realms.
 ** Firstly, the external library binding will be invoked to set up a processing-functor, and then,
 ** in a separate step, the same PortBuilder instance, unfinished at that point, will be passed to
 ** the code responsible for configuring parameters and _Parameter Automation._ Only after that,
 ** the _terminal builder operation_ WeavingBuilder::build() will be invoked, and the control
 ** flow in the Lumiera Builder subsystem proceeds to outfitting the next Render Node.
 ** This intricate sequence of configuration step translates into the necessity to build the
 ** FeedPrototype first in its basic form, without a parameter-functor. The second configuration
 ** step performed later will then have to re-shape the FeedPrototype to add a parameter-functor.
 ** This amounts to a move-copy, thereby changing the FeedPrototype's template arguments to
 ** the full signature, including the type of the parameter functor. In this final shape,
 ** it can be integrated into a Turnout instance and dropped off into the PatternData, which
 ** is used to record configuration for the actual storage allocation and node generation step
 ** performed later.
 ** 
 ** 
 ** # Interplay of NodeBuider, PortBuilder and WeavingBuilder
 ** 
 ** The steam::engine::WeavingBuilder defined here serves as the low-level builder and adapter
 ** to prepare the wiring and invocation. The builder-API allows to setup the wiring of input
 ** and output-»slots« and control some detail aspects like caching. However, without defining
 ** any connections explicitly, a simple 1:1 wiring scheme is employed
 ** - each _input slot_ of the function gets an input buffer, which is filled by _pulling_
 **   (i.e. invoking) a predecessor node (a so called »lead«).
 ** - for each _output slot_ a buffer is allocated for the processing function to drop off
 **   the calculated media data
 ** - only one of these output buffers is used as actual result, while the other buffers
 **   are just discarded (but may possibly be fed to the frame cache).
 ** 
 ** Each [Processing Node](\ref ProcNode) represents one specific processing functionality on a
 ** logical level; yet such a node may be able to generate several „flavours“ of this processing,
 ** which are represented as *ports* on this node. Actually, each such port stands for one specific
 ** setup of a function invocation, with appropriate _wiring_ of input and output connections.
 ** For example, an audio filtering function may be exposed on port-#1 for stereo sound, while
 ** port-#2 may process the left, and port-#3 the right channel in isolation. It is entirely
 ** up to the library-adapter-plug-in to decide what processing functions to expose, and in
 ** which flavours. The WeavingBuilder is used to generate a single \ref Turnout object,
 ** which corresponds to the invocation of a single port and thus one flavour of processing.
 ** 
 ** At one architectural level above, the \ref NodeBuilder exposes the ability to set up a
 ** ProcNode, complete with several ports and connected to possibly several predecessor nodes.
 ** Using a sequence of NodeBuilder invocations, the _processing node graph_ can be built gradually,
 ** starting from the source (predecessors) and moving up to the _exit nodes,_ which produce the
 ** desired calculation results. The NodeBuilder offers a function to define the predecessor nodes
 ** (also designated as _lead nodes_), and it offers an [entrance point](\ref NodeBuilder::preparePort)
 ** to descend into a \ref PortBuilder, allowing to add the port definitions for this node step by step.
 ** 
 ** On the implementation level, the PortBuilder inherits from the NodeBuilder and embeds a
 ** WeavingBuilder instance. Moreover, the actual parametrisations of the NodeBuilder template
 ** are chained to create a _functional data structure._ This intricate setup is necessary because
 ** the actual data structure of the node graph comprises several small descriptor arrays and
 ** interconnected pointers, which are all placed into consecutive chunks of memory, using a
 ** custom allocator, the AllocationCluster. The lib::Several is used as front-end to access
 ** these small collections of related objects, and the associated lib::SeveralBuilder provides
 ** the low-level memory allocation and object creation functionality. The purpose of this
 ** admittedly quite elaborate scheme is to generate a compact data structure, with high
 ** cache locality and without wasting too much memory. Since the exact number of elements
 ** and the size of those elements can be deduced only after the builder-API usage has
 ** been completed, the aforementioned functional datastructure is used to collect the
 ** parametrisation information for all ports, while delaying the actual object creation.
 ** With this technique, it is possible to generate all descriptors or entries of one
 ** kind in a single run, and placed optimally and compact into the memory allocation.
 ** 
 ** @see turnout.hpp
 ** @see node-builder.hpp
 ** @see NodeLink_test
 ** 
 ** @todo WIP-WIP-WIP as of 10/2024 prototyping how to build and invoke render nodes /////////////////////////TICKET #1371
 ** 
 */


#ifndef STEAM_ENGINE_WEAVING_PATTERN_BUILDER_H
#define STEAM_ENGINE_WEAVING_PATTERN_BUILDER_H

#include "lib/error.hpp"
#include "lib/several-builder.hpp"
#include "steam/engine/proc-id.hpp"
#include "steam/engine/engine-ctx.hpp"
#include "steam/engine/weaving-pattern.hpp"
#include "steam/engine/buffer-provider.hpp"
#include "steam/engine/buffhandle-attach.hpp"  /////////////////OOO why do we need to include this? we need the accessAs<TY>() template function
#include "lib/meta/tuple-helper.hpp"
//#include "lib/test/test-helper.hpp" ////////////////////////////OOO TODO added for test
#include "lib/format-string.hpp"
#include "lib/iter-zip.hpp"
#include "lib/util.hpp"

#include <functional>
#include <utility>
#include <vector>
#include <string>


namespace steam {
namespace engine {
  namespace err = lumiera::error;
  
  using StrView = std::string_view;
  using std::forward;
  using lib::Depend;
  using lib::izip;
  using util::_Fmt;
  using util::max;
  
  
  
  template<class POL, class I, class E=I>
  using DataBuilder = lib::SeveralBuilder<I,E, POL::template Policy>;
  
  template<uint siz>
  using SizMark = std::integral_constant<uint,siz>;
  
  
  
  /**
   * Recursive functional data structure to collect weaving pattern data
   * and finally to emplace a Turnout instance into the data storage
   * for each port, as specified by preceding builder-API invocations.
   * @tparam PAR   recursive layering for preceding entries
   * @tparam BUILD a builder functor to emplace one Turnout instance,
   *               thereby opaquely embedding all specific data typing.
   * @tparam siz   storage in bytes to hold data produced by \a BUILD
   */
  template<class PAR, class BUILD, uint siz>
  struct PatternData
    : PAR
    {
      BUILD buildEntry;
      
      uint size() { return 1 + PAR::size(); }
      
      template<class DAB>
      void
      collectEntries (DAB& dataBuilder, uint cntElm =0, uint maxSiz =0)
        {
          PAR::collectEntries (dataBuilder, cntElm+1, max (siz,maxSiz));
          buildEntry (dataBuilder);
        }
      
      
      PatternData(PAR&& predecessor, BUILD&& entryBuilder)
        : PAR{move (predecessor)}
        , buildEntry{move (entryBuilder)}
        { }
    };
  
  /**
   * Data recursion end: prime the port data storage
   * by reserving appropriate storage to hold all known Turnout elements.
   */
  struct PatternDataAnchor
    {
      uint size() { return 0; }
      
      template<class DAB>
      void
      collectEntries (DAB& dataBuilder, uint cntElm, uint maxSiz)
        {
          dataBuilder.reserve (cntElm, maxSiz);
        }
    };
  
  
  
  
  
  
  /**
   * A low-level Builder to prepare and adapt for a specific node invocation.
   * In this context, »weaving« refers to the way parameters and results of an
   * processing function are provided, combined and forwarded within the setup
   * for an actual Render Node invocation. When the invocation happens, a kind
   * of preconfigured _blue print_ or invocation plan is executed; the purpose
   * of the build at »Level-2« (≙the purpose of this code) is to preconfigure
   * this invocation scheme. Using a _low level builder_ as controlled by the
   * actual NodeBuilder and PortBuilder allows to introduce extension points
   * and helps to abstract away internal technical details of the invocation.
   * @tparam POL allocation and context configuration policy
   * @tparam FUN function or invocation adapter to invoke
   */
  template<class POL, class FUN>
  struct WeavingBuilder
    : util::MoveOnly
    {
      using Prototype = typename FeedManifold<FUN>::Prototype;
      using WeavingPattern = MediaWeavingPattern<Prototype>;
      using TurnoutWeaving = Turnout<WeavingPattern>;
      static constexpr SizMark<sizeof(TurnoutWeaving)> sizMark{};
      static constexpr uint FAN_I = Prototype::FAN_I;
      static constexpr uint FAN_O = Prototype::FAN_O;

      
      using TypeMarker = std::function<BuffDescr(BufferProvider&)>;
      using ProviderRef = std::reference_wrapper<BufferProvider>;
      
      DataBuilder<POL, PortRef> leadPorts;
      std::vector<TypeMarker>   buffTypes;
      std::vector<ProviderRef>  providers;
      
      uint resultSlot{0};
      
      Depend<EngineCtx> ctx;
      
      StrView nodeSymb_;
      StrView portSpec_;
      FUN fun_;
      
      template<typename...INIT>
      WeavingBuilder(FUN&& init, StrView nodeSymb, StrView portSpec, INIT&& ...alloInit)
        : leadPorts{forward<INIT> (alloInit)...}
        , buffTypes{fillDefaultBufferTypes()}
        , nodeSymb_{nodeSymb}
        , portSpec_{portSpec}
        , fun_{move(init)}
        { }
      
      WeavingBuilder&&
      attachToLeadPort (ProcNode& lead, uint portNr)
        {
          if (leadPorts.size() >= FAN_I)
            throw err::Logic{_Fmt{"Builder: attempt to add further input, "
                                  "but all %d »input slots« of the processing function are already connected."}
                                 % FAN_I
                            };
          PortRef portRef{lead.getPort (portNr)};
          leadPorts.append (portRef);
          return move(*this);
        }
      
      
      WeavingBuilder&&
      connectRemainingInputs (DataBuilder<POL, ProcNodeRef>& knownLeads, uint defaultPort)
        {
          REQUIRE (leadPorts.size() <= FAN_I);
          uint cnt = FAN_I - leadPorts.size();
          if (FAN_I > knownLeads.size())
            throw err::Logic{_Fmt{"Builder: attempt to auto-connect %d further »input slots«, "
                                  "but this ProcNode has only %d predecessor nodes, while the "
                                  "given processing function expects %d inputs."}
                                 % cnt % knownLeads.size() % FAN_I
                            };
          while (cnt--)
            attachToLeadPort (knownLeads[leadPorts.size()], defaultPort);
          return move(*this);
        }
      
      WeavingBuilder&&
      selectResultSlot (uint idx)
        {
          this->resultSlot = idx;
          return move(*this);
        }
      
      
      auto
      build()
        {
          // discard excess storage prior to allocating the output types sequence
          leadPorts.shrinkFit();
          
          maybeFillDefaultProviders (buffTypes.size());
          REQUIRE (providers.size() == buffTypes.size());
          auto outTypes = DataBuilder<POL, BuffDescr>{leadPorts.policyConnect()}
                                     .reserve (buffTypes.size());
          
          for (auto& [i,typeConstructor] : izip(buffTypes))
            outTypes.append (
              typeConstructor (providers[i]));
          
          ENSURE (leadPorts.size() == FAN_I);
          ENSURE (outTypes.size()  == FAN_O);
          
          using PortDataBuilder = DataBuilder<POL, Port>;
          // provide a free-standing functor to build a suitable Port impl (≙Turnout)
          return [leads = move(leadPorts.build())
                 ,types = move(outTypes.build())
                 ,procFun = move(fun_)
                 ,resultIdx = resultSlot
                 ,procID = ProcID::describe (nodeSymb_,portSpec_)
                 ]
                 (PortDataBuilder& portData) mutable -> void
                   {
                     portData.template emplace<TurnoutWeaving> (procID
                                                               ,move(leads)
                                                               ,move(types)
                                                               ,resultIdx
                                                               ,move(procFun)
                                                               );
                   };
        }
      
      
      
    private: /* ====== WeavingBuilder implementation details ====== */
      void
      maybeFillDefaultProviders (size_t maxSlots)
        {
          for (uint i=providers.size(); i < maxSlots; ++i)
            providers.emplace_back (ctx().mem);
        }
      
      /**
       * @internal configuration builder for buffer descriptors
       * @tparam BU target type of the buffer (without pointer)
       * The FeedPrototype can generate for the given \a FUN a
       * type sequence of output buffer types, which are used
       * to instantiate this template and then later to work
       * on specific output buffer slots.
       */
      template<typename BU>
      struct BufferDescriptor
        {
          /**
           * Setup the constructor function for the default BufferDescriptors.
           * @return a functor that can be applied to the actual BufferProviders
           *         at the point when everything for this port is configured.
           */
          TypeMarker
          makeBufferDescriptor()  const
            {
              return [](BufferProvider& provider)
                        { return provider.getDescriptor<BU>(); };
            }
        };
      
      using OutTypesDescriptors = typename Prototype::template OutTypesApply<BufferDescriptor>;
      using OutDescriptorTup = lib::meta::Tuple<OutTypesDescriptors>;
      
      /** A tuple of BufferDescriptor instances for all output buffer types */
      static constexpr OutDescriptorTup outDescriptors{};
      
      /** @internal pre-initialise the buffTypes vector with a default configuration.
       *  @remarks In the _terminal step,_ the buffTypes will be transformed into a
       *           sequence of BufferDescriptor entries, which can later be used
       *           by the node invocation to prepare a set of output buffers.
       *         - each slot holds a function<BufferDescripter(BufferProvider&)>
       *         - these can be used to configure specific setup for some buffers
       *         - the default BufferDescriptor will just default-construct the
       *           designated «output slot» of the media processing-function.
       */
      static auto
      fillDefaultBufferTypes()
        {
          std::vector<TypeMarker> defaultBufferTypes;
          defaultBufferTypes.reserve (std::tuple_size_v<OutDescriptorTup>);
          lib::meta::forEach(outDescriptors
                            ,[&](auto& desc)
                                {
                                  defaultBufferTypes.emplace_back(
                                      desc.makeBufferDescriptor());
                                });
          return defaultBufferTypes;
        }
    };
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_WEAVING_PATTERN_BUILDER_H*/
