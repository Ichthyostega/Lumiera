/*
  WEAVING-PATTERN-BUILDER.hpp  -  build an invocation pattern for media calculations

  Copyright (C)         Lumiera.org
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
 ** @note steam::engine::Turnout mixes-in the steam::engine::SimpleWeavingPattern, which in turn
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
 ** @see NodeLinkage_test
 ** 
 ** @todo WIP-WIP-WIP as of 10/2024 prototyping how to build and invoke render nodes /////////////////////////TICKET #1371
 ** 
 */


#ifndef STEAM_ENGINE_WEAVING_PATTERN_BUILDER_H
#define STEAM_ENGINE_WEAVING_PATTERN_BUILDER_H

//#include "steam/common.hpp"
//#include "steam/engine/channel-descriptor.hpp"
//#include "vault/gear/job.h"
#include "lib/several-builder.hpp"
#include "steam/engine/turnout.hpp"
#include "steam/engine/engine-ctx.hpp"
#include "steam/engine/buffer-provider.hpp"
#include "steam/engine/buffhandle-attach.hpp"  /////////////////OOO why do we need to include this? we need the accessAs<TY>() template function
#include "lib/test/test-helper.hpp"
//#include "lib/util-foreach.hpp"
//#include "lib/iter-adapter.hpp"
//#include "lib/meta/function.hpp"
//#include "lib/itertools.hpp"
#include "lib/util.hpp"

//#include <utility>
#include <functional>
//#include <array>
#include <vector>


namespace steam {
namespace engine {
  
  using std::forward;
  using lib::Several;
  using lib::Depend;
  using util::max;
  
  
  namespace {// Introspection helpers....
    
    using lib::meta::_Fun;
    using lib::meta::is_BinaryFun;
    using std::remove_reference_t;
    
    /** Helper to pick up the parameter dimensions from the processing function
     * @remark this is the rather simple yet common case that media processing
     *         is done by a function, which takes an array of input and output
     *         buffer pointers with a common type; this simple case is used
     *         7/2024 for prototyping and validate the design.
     * @tparam FUN a _function-like_ object, expected to accept two arguments,
     *         which both are arrays of buffer pointers (input, output).
     */
    template<class FUN>
    struct _ProcFun
      {
        static_assert(_Fun<FUN>()         , "something funktion-like required");
        static_assert(is_BinaryFun<FUN>() , "function with two arguments expected");
        
        using ArgI = remove_reference_t<typename _Fun<FUN>::Args::List::Head>;
        using ArgO = remove_reference_t<typename _Fun<FUN>::Args::List::Tail::Head>;
        
        template<class ARG>
        struct MatchBuffArray
          {
            static_assert(not sizeof(ARG), "processing function expected to take array-of-buffer-pointers");
          };
        template<class BUF, size_t N>
        struct MatchBuffArray<std::array<BUF*,N>>
          {
            using Buff = BUF;
            enum{ SIZ = N };
          };
        
        using BuffI = typename MatchBuffArray<ArgI>::Buff;
        using BuffO = typename MatchBuffArray<ArgO>::Buff;
        
        enum{ FAN_I = MatchBuffArray<ArgI>::SIZ
            , FAN_O = MatchBuffArray<ArgO>::SIZ
            };
      };
    
    
    /**
     * Pick a suitable size for the FeedManifold to accommodate the given function.
     * @remark only returning one of a small selection of sizes, to avoid
     *         excessive generation of template instances.
     * @todo 10/24 this is a premature safety guard;
     *       need to assess if there is actually a problem
     *       (chances are that the optimiser absorbs most of the combinatoric complexity,
     *       or that, to the contrary, other proliferation mechanisms cause more harm)
     */
    template<class FUN>
    inline constexpr uint
    manifoldSiz()
    {
      using _F = _ProcFun<FUN>;
      auto constexpr bound = std::max (_F::FAN_I, _F::FAN_O);
      static_assert (bound <= 10,
           "Limitation of template instances exceeded");
      return bound < 3? bound
           : bound < 6? 5
                      : 10;
    }
  }//(End)Introspection helpers.
  
  
  /**
   * Adapter to handle a simple yet common setup for media processing
   * - somehow we can invoke processing as a simple function
   * - this function takes two arrays: the input- and output buffers
   * @remark this setup is useful for testing, and as documentation example;
   *         actually the FeedManifold is mixed in as baseclass, and the
   *         buffer pointers are retrieved from the BuffHandles.
   * @tparam MAN a FeedManifold, providing arrays of BuffHandles
   * @tparam FUN the processing function
   */
  template<class MAN, class FUN>
  struct SimpleFunctionInvocationAdapter
    : MAN
    {
      using BuffI = typename _ProcFun<FUN>::BuffI;
      using BuffO = typename _ProcFun<FUN>::BuffO;
      
      enum{ N = MAN::STORAGE_SIZ
          , FAN_I = _ProcFun<FUN>::FAN_I
          , FAN_O = _ProcFun<FUN>::FAN_O
      };
      
      static_assert(FAN_I <= N and FAN_O <= N);
      
      using ArrayI = std::array<BuffI*, FAN_I>;
      using ArrayO = std::array<BuffO*, FAN_O>;
      
      
      FUN process;
      
      ArrayI inParam;
      ArrayO outParam;
      
      template<typename...INIT>
      SimpleFunctionInvocationAdapter (INIT&& ...funSetup)
        : process{forward<INIT> (funSetup)...}
        { }
      
      
      void
      connect (uint fanIn, uint fanOut)
        {
          REQUIRE (fanIn >= FAN_I and fanOut >= FAN_O);
          for (uint i=0; i<FAN_I; ++i)
            inParam[i] = & MAN::inBuff[i].template accessAs<BuffI>();
          for (uint i=0; i<FAN_O; ++i)
            outParam[i] = & MAN::outBuff[i].template accessAs<BuffO>();
        }
      
      void
      invoke()
        {
          process (inParam, outParam);
        }
    };
  
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
  template<uint N, class FUN>
  struct DirectFunctionInvocation
    : util::MoveOnly
    {
      using Manifold = FeedManifold<N>;
      using Feed = SimpleFunctionInvocationAdapter<Manifold, FUN>;
      enum{ MAX_SIZ = N };
      
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
   *               opaquely embedding all specific data typing.
   * @tparam siz   storage in bytes to hold data produced by \a BUILD
   */
  template<class PAR, class BUILD, uint siz>
  struct PatternData
    : PAR
    {
      BUILD buildEntry;
      
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
      template<class DAB>
      void
      collectEntries (DAB& dataBuilder, uint cntElm, uint maxSiz)
        {
          dataBuilder.reserve (cntElm, maxSiz);
        }
    };
  
  
  
  
  template<uint N, class FUN>
  using SimpleDirectInvoke = SimpleWeavingPattern<DirectFunctionInvocation<N,FUN>>;
  
  
  template<class POL, uint N, class FUN>
  struct WeavingBuilder
    : util::MoveOnly
    {
      using TurnoutWeaving = Turnout<SimpleDirectInvoke<N,FUN>>;
      static constexpr SizMark<sizeof(TurnoutWeaving)> sizMark{};
      
      using TypeMarker = std::function<BuffDescr(BufferProvider&)>;
      using ProviderRef = std::reference_wrapper<BufferProvider>;
      
      DataBuilder<POL, PortRef> leadPorts;
      std::vector<TypeMarker>   buffTypes;
      std::vector<ProviderRef>  providers;
      
      uint resultSlot{0};
      
      Depend<EngineCtx> ctx;
      
      FUN fun_;
      
      template<typename...INIT>
      WeavingBuilder(FUN&& init, INIT&& ...alloInit)
        : leadPorts{forward<INIT> (alloInit)...}
        , fun_{move(init)}
        { }
      
      WeavingBuilder
      attachToLeadPort(ProcNode& lead, uint portNr)
        {
          PortRef portRef{lead.getPort (portNr)};
          leadPorts.append (portRef);
          ENSURE (leadPorts.size() <= N);
          return move(*this);
        }
      
      template<class BU>
      WeavingBuilder
      appendBufferTypes(uint cnt)
        {
          while (cnt--)
            buffTypes.emplace_back([](BufferProvider& provider)
                                    { return provider.getDescriptor<BU>(); });
          ENSURE (buffTypes.size() <= N);
          return move(*this);
        }
      
      WeavingBuilder
      fillRemainingBufferTypes()
        {
          using FunSpec = _ProcFun<FUN>;
          auto constexpr FAN_O = FunSpec::FAN_O;
          using BuffO = typename FunSpec::BuffO;
          uint cnt = FAN_O - buffTypes.size();
          return appendBufferTypes<BuffO>(cnt);
        }
      
      WeavingBuilder
      selectResultSlot(uint idx)
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
          
          ENSURE (leadPorts.size() <= N);
          ENSURE (outTypes.size()  <= N);
          
          using PortDataBuilder = DataBuilder<POL, Port>;
          // provide a free-standing functor to build a suitable Port impl (≙Turnout)
          return [leads = move(leadPorts.build())
                 ,types = move(outTypes.build())
                 ,procFun = move(fun_)
                 ,resultIdx = resultSlot
                 ]
                 (PortDataBuilder& portData) mutable -> void
                   {
                     portData.template emplace<TurnoutWeaving> (move(leads)
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
