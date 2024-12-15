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
 ** in accordance to a fixed _wiring scheme:_
 ** - the function takes two arguments
 ** - these are an array of input and output buffer pointers
 ** - buffer sizes or types are assumed to be uniform over all »slots«
 ** - yet the input side my use another type than the output side
 ** @todo as of 10/2024, this scheme is established as prototype to explore how processing nodes
 **       can be build, connected and invoked; the expectation is however that this simple scheme
 **       is suitable to adapt and handle many common cases of invoking media processing functions,
 **       because the given _functor_ is constructed within a plug-in tailored to a specific
 **       media processing library (e.g. FFmpeg) and thus can be a lambda to forward to the
 **       actual function.
 ** @note steam::engine::Turnout mixes-in the steam::engine::MediaWeavingPattern, which in turn
 **       inherits from an *Invocation Adapter* given as template parameter. So this constitutes
 **       an *extension point* where other, more elaborate invocation schemes could be integrated.
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
 ** up to the library-adapter-plug-in what processing functions to expose, and in which flavours.
 ** The WeavingBuilder is used to generate a single \ref Turnout object, which corresponds to
 ** the invocation of a single port and thus one flavour of processing.
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

//#include "steam/common.hpp"
#include "lib/error.hpp"
#include "lib/symbol.hpp"
//#include "steam/engine/channel-descriptor.hpp"
//#include "vault/gear/job.h"
#include "lib/several-builder.hpp"
#include "steam/engine/proc-id.hpp"
#include "steam/engine/engine-ctx.hpp"
#include "steam/engine/weaving-pattern.hpp"
#include "steam/engine/buffer-provider.hpp"
#include "steam/engine/buffhandle-attach.hpp"  /////////////////OOO why do we need to include this? we need the accessAs<TY>() template function
#include "lib/test/test-helper.hpp" ////////////////////////////OOO TODO added for test
#include "lib/format-string.hpp"
//#include "lib/util-foreach.hpp"
//#include "lib/iter-adapter.hpp"
//#include "lib/meta/function.hpp"
//#include "lib/itertools.hpp"
#include "lib/util.hpp"

//#include <utility>
#include <functional>
//#include <array>
#include <vector>
#include <string>


namespace steam {
namespace engine {
  namespace err = lumiera::error;
  
  using StrView = std::string_view;
  using std::forward;
//  using lib::Literal;
  using lib::Several;
  using lib::Depend;
  using util::_Fmt;
  using util::max;
  
  
  
  /**
   * Typical base configuration for a Weaving-Pattern chain:
   * - use a simple processing function
   * - pass an input/output buffer array to this function
   * - map all »slots« directly without any re-ordering
   * - use a sufficiently sized FeedManifold as storage scheme
   * @remark actual media handling plug-ins may choose to
   *         employ more elaborate _invocation adapters_
   *         specifically tailored to the library's needs.
   */
  template<class FUN>
  struct DirectFunctionInvocation
    : util::MoveOnly
    {
      enum{ MAX_SIZ = _ProcFun<FUN>::MAXSZ };
      using Manifold = FeedManifold<FUN>;
      using Feed = SimpleFunctionInvocationAdapter<Manifold, FUN>;
      
      std::function<Feed()> buildFeed;
      
      /** when building the Turnout, prepare the _invocation adapter_
       * @note processing function \a fun is bound by value into the closure,
       *       so that each invocation will create a copy of that function,
       *       embedded (and typically inlined) into the invocation adapter.
       */
      DirectFunctionInvocation(FUN fun)
        : buildFeed{[=]{ return Feed{fun}; }}
        { }
    };
  
  
  
  
  template<class POL, class I, class E=I>
  using DataBuilder = lib::SeveralBuilder<I,E, POL::template Policy>;
  
  template<uint siz>
  using SizMark = std::integral_constant<uint,siz>;
  

/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1371 : Prototyping: how to assemble a Turnout
  
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
  
  
  
  
  template<class FUN>
  using SimpleDirectInvoke = MediaWeavingPattern<DirectFunctionInvocation<FUN>>;
  
  
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
   * @tparam N   maximum number of input and output slots
   * @tparam FUN function or invocation adapter to invoke
   */
  template<class POL, class FUN>
  struct WeavingBuilder
    : util::MoveOnly
    {
      using FunSpec = _ProcFun<FUN>;
      using TurnoutWeaving = Turnout<SimpleDirectInvoke<FUN>>;
      static constexpr SizMark<sizeof(TurnoutWeaving)> sizMark{};
      static constexpr uint FAN_I = FunSpec::FAN_I;
      static constexpr uint FAN_O = FunSpec::FAN_O;

      
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
      
      template<class BU>
      WeavingBuilder&&
      appendBufferTypes (uint cnt)
        {
          if (buffTypes.size()+cnt > FAN_O)
            throw err::Logic{_Fmt{"Builder: attempt add %d further output buffers, "
                                  "while %d of %d possible outputs are already connected."}
                                 % cnt % buffTypes.size() % FAN_O
                            };
          while (cnt--)
            buffTypes.emplace_back([](BufferProvider& provider)
                                    { return provider.getDescriptor<BU>(); });
          ENSURE (buffTypes.size() <= FAN_O);
          return move(*this);
        }
      
      WeavingBuilder&&
      fillRemainingBufferTypes()
        {
          using BuffO = typename FunSpec::BuffO;
          uint cnt = FAN_O - buffTypes.size();
          return appendBufferTypes<BuffO>(cnt);
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
          uint i=0;
          for (auto& typeConstructor : buffTypes)
            outTypes.append (
              typeConstructor (providers[i++]));
          
          ENSURE (leadPorts.size() == FunSpec::FAN_I);
          ENSURE (outTypes.size()  == FunSpec::FAN_O);
          
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
      
      private:
        void
        maybeFillDefaultProviders (size_t maxSlots)
          {
            for (uint i=providers.size(); i < maxSlots; ++i)
              providers.emplace_back (ctx().mem);
          }
    };
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : (End)Prototyping: how to assemble a Turnout
  
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_WEAVING_PATTERN_BUILDER_H*/
