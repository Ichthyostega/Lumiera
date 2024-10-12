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
 ** Construction kit for establishing an invocation scheme for media calculations.
 ** 
 ** @see turnout.hpp
 ** @see node-builder.hpp
 ** @see NodeLinkage_test
 ** 
 ** @todo WIP-WIP-WIP as of 7/2024 prototyping how to build and invoke render nodes /////////////////////////TICKET #1367
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
//#include "lib/util-foreach.hpp"
//#include "lib/iter-adapter.hpp"
//#include "lib/meta/function.hpp"
//#include "lib/itertools.hpp"
//#include "lib/util.hpp"

//#include <utility>
#include <functional>
//#include <array>
#include <vector>


namespace steam {
namespace engine {
  
  using std::forward;
  using lib::Several;
  using lib::Depend;
  
  
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
      auto bound = std::max (_F::FAN_I, _F::FAN_O);
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
      
      enum{ N = MAN::inBuff::size()
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
        : FUN{forward<INIT> (funSetup)...}
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
   * Example base configuration for a Weaving-Pattern chain:
   * - use a simple processing function
   * - pass an input/output buffer array to this function
   * - map all »slots« directly without any re-ordering
   * - use a sufficiently sized FeedManifold as storage scheme
   */
  template<uint N, class FUN>
  struct Conf_DirectFunctionInvocation
    : util::MoveOnly
    {
      using Manifold = FeedManifold<N>;
      using Feed = SimpleFunctionInvocationAdapter<Manifold, FUN>;
      enum{ MAX_SIZ = N };
      
      std::function<Feed()> buildFeed;
    };
  
  
  
  
  template<class POL, class I, class E=I>
  using DataBuilder = lib::SeveralBuilder<I,E, POL::template Policy>;
  

/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Prototyping: how to assemble a Turnout
  template<uint N, class FUN>
  using SimpleDirectInvoke = SimpleWeavingPattern<Conf_DirectFunctionInvocation<N,FUN>>;
  
  template<class POL, uint N, class FUN>
  struct WeavingBuilder
    : util::MoveOnly
    {
      DataBuilder<POL, PortRef>   leadPort;
      DataBuilder<POL, BuffDescr> outTypes;
      
      using TypeMarker = std::function<BuffDescr(BufferProvider&)>;
      using ProviderRef = std::reference_wrapper<BufferProvider>;
      
      std::vector<TypeMarker> buffTypes;
      std::vector<ProviderRef> providers;
      
      uint resultSlot{0};
      
      Depend<EngineCtx> ctx;
      
      FUN fun_;
      
      WeavingBuilder(FUN&& init)
        : fun_{move(init)}
        { }
      
      WeavingBuilder
      attachToLeadPort(ProcNode& lead, uint portNr)
        {
          PortRef portRef; /////////////////////////////////////OOO TODO need Accessor on ProcNode!!!!!
          leadPort.emplace (portRef); 
          ENSURE (leadPort.size() < N);
          return move(*this);
        }
      
      template<class BU>
      WeavingBuilder
      appendBufferTypes(uint cnt)
        {
          while (cnt--)
            buffTypes.emplace_back([](BufferProvider& provider)
                                    { return provider.getDescriptor<BU>(); });
          ENSURE (buffTypes.size() < N);
          return move(*this);
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
          maybeFillDefaultProviders (buffTypes.size());
          uint i=0;
          for (auto& typeConstructor : buffTypes)
            outTypes.emplace (
              typeConstructor (providers[i]));
          
          ENSURE (leadPort.size() < N);
          ENSURE (outTypes.size() < N);
          
          using Product = Turnout<SimpleDirectInvoke<N,FUN>>;
          ///////////////////////////////OOO need a way to prepare SeveralBuilder-instances for leadPort and outDescr --> see NodeBuilder
          return Product{leadPort.build(), outTypes.build(), move(fun_)};
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
