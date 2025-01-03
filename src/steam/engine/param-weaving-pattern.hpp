/*
  PARAM-WEAVING-PATTERN.hpp  -  Pattern to carry out special parameter computations

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file param-weaving-pattern.hpp
 ** Construction kit to establish a set of parameters pre-computed prior to invocation
 ** of nested nodes. This arrangement is also known as »Parameter Agent Node« (while actually
 ** it is a Weaving Patter residing within some Node's Port). The use-case is to provide a set
 ** of additional parameter values, beyond what can be derived directly by a parameter-functor
 ** based on the _absolute-nominal-Time_ of the invocation. The necessity for such a setup may
 ** arise when additional context or external state must be combined with the nominal time into
 ** a tuple of data values, which shall then be consumed by several follow-up evaluations further
 ** down into a recursive invocation tree _for one single render job._ The solution provided by
 ** the Parameter Agent Node relies on placing those additional data values into a tuple stored
 ** directly in the render invocation stack frame, prior to descending into further recursive
 ** Node evaluations. Notably, parameter-functors within the scope of this evaluation tree can
 ** then access these additional parameters through the TurnoutSystem of the overall invocation.
 ** 
 ** @see node-builder.hpp
 ** @see weaving-pattern-builder.hpp
 ** @see NodeFeed_test::feedParamNode()
 ** @see \ref proc-node.hpp "Overview of Render Node structures"
 ** 
 ** @warning WIP as of 12/2024 first complete integration round of the Render engine ////////////////////////////TICKET #1367
 ** 
 */


#ifndef STEAM_ENGINE_PARAM_WEAVING_PATTERN_H
#define STEAM_ENGINE_PARAM_WEAVING_PATTERN_H

#include "steam/common.hpp"
#include "steam/engine/turnout.hpp"
#include "steam/engine/turnout-system.hpp"
#include "steam/engine/feed-manifold.hpp" ////////////TODO wegdamit
#include "lib/uninitialised-storage.hpp"
#include "lib/meta/variadic-helper.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/meta/function.hpp"
#include "lib/several.hpp"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
//#include "lib/time/timevalue.hpp"
//#include "lib/util.hpp"      ////////OOO wegen manifoldSiz<FUN>()

//#include <stack>
#include <tuple>
#include <utility>


namespace steam {
namespace engine {
  
  using std::move;
  using std::forward;
  using std::function;
  using std::make_tuple;
  using std::tuple;
  using lib::Several;////TODO RLY?
  
  
  template<class ANK, typename...FUNZ>
  struct ParamBuildSpec
    {
      using Functors = tuple<FUNZ...>;
      
      using ResTypes = typename lib::meta::ElmTypes<Functors>::template Apply<lib::meta::_FunRet>;
      using ParamTup = lib::meta::Tuple<ResTypes>;
      
      Functors functors_;
      
      ParamBuildSpec (Functors&& funz)
        : functors_{move (funz)}
        { }
      
      template<typename FUN>
      auto
      addSlot (FUN&& paramFun)
        {
          return ParamBuildSpec<ANK,FUNZ...,FUN>{std::tuple_cat (move(functors_)
                                                                ,make_tuple (forward<FUN>(paramFun)))};
        }
      
      template<typename PAR>
      auto
      addValSlot (PAR paramVal)
        {
          return addSlot ([paramVal](TurnoutSystem&){ return paramVal; });
        }
      
      
      /** intended for unit-testing: invoke one of the embedded param-functors */
      template<size_t slot>
      auto
      invokeParamFun (TurnoutSystem& turnoutSys)
        {
          return std::get<slot> (functors_) (turnoutSys);
        }
      
      /** @internal the _chain constructor type_ is a type rebinding meta function (nested struct),
       * which extends the HeteroData chain given by \a ANK with the sequence of types derived from
       * the result-values of all functors stored in the ParamBuildSpec, i.e. the resulting param tuple.
       * @remark HeteroData defines a nested struct `Chain`, and with the help of `RebindVariadic`,
       *         the type sequence from the ParamTup can be used to instantiate this Chain context.
       */
      using ChainCons = typename lib::meta::RebindVariadic<ANK::template Chain, ParamTup>::Type;
      
      
      /** a (static) getter functor able to work on the full extended HeteroData-Chain
       * @remark the front-end of this chain resides in TurnoutSystem */
      template<size_t slot>
      struct Accessor
        : util::MoveOnly
        {
          static auto&
          getParamVal (TurnoutSystem& turnoutSys)
            {
              using StorageAccessor = typename ChainCons::template Accessor<slot>;
              return turnoutSys.get (StorageAccessor());
            }
        };
      
      template<size_t idx>
      Accessor<idx>
      makeAccessor()
        {
          return Accessor<idx>{};
        }
      
      class BlockBuilder
        : util::MoveOnly
        {
          Functors functors_;
          
        public:
          /** invoke all parameter-functors and _drop off_ the result into a »chain-block« (non-copyable) */
          typename ChainCons::NewFrame
          buildParamDataBlock (TurnoutSystem& turnoutSys)
            {
              return std::apply ([&](auto&&... paramFun)
                                      {  //    invoke parameter-functors and build NewFrame from results
                                        return ChainCons::build (paramFun (turnoutSys) ...);
                                      }
                                ,functors_);
            }
          
          /** invoke all parameter-functors and package all results by placement-new into a »chain-block« */
          void
          emplaceParamDataBlock (void* storage, TurnoutSystem& turnoutSys)
            {
              std::apply ([&](auto&&... paramFun)
                              {  //    invoke parameter-functors and build NewFrame from results
                                ChainCons::emplace (storage, paramFun (turnoutSys) ...);
                              }
                        ,functors_);
            }
          
        private:
          BlockBuilder (Functors&& funz)
            : functors_{move (funz)}
            { }
          
          friend class ParamBuildSpec;
        };
      
      /**
       * Terminal Builder: (destructively) transform this ParamBuildSpec
       * into a BlockBuilder, which can then be used to create a Parameter data block,
       * thereby invoking the embedded functors and drop-off the results into storage.
       */
      BlockBuilder
      makeBlockBuilder()
        {
          return BlockBuilder (move (functors_));
        }
    };
  
  inline auto
  buildParamSpec()
    {
      return ParamBuildSpec<TurnoutSystem::FrontBlock>{tuple<>{}};
    }
  
  
    /**
     * Implementation for a _Weaving Pattern_ to conduct extended parameter evaluation.
     */
  template<class SPEC>
  struct ParamWeavingPattern
    : util::MoveOnly
    {
      using Functors = typename SPEC::Functors;
      using DataBlock = typename SPEC::ChainCons::NewFrame;
      using BlockBuilder = typename SPEC::BlockBuilder;
      using PostProcessor = function<void(TurnoutSystem&)>;
      
      BlockBuilder blockBuilder_;
      PostProcessor postProcess_;
      Port&        delegatePort_;
      
      struct Feed
        : util::NonCopyable
        {
          lib::UninitialisedStorage<DataBlock> buffer;
          OptionalBuff outBuff;
          
          DataBlock& block() { return buffer[0]; }
          
          void
          emplaceParamDataBlock (BlockBuilder& builder, TurnoutSystem& turnoutSys)
            {
              builder.emplaceParamDataBlock (&block(), turnoutSys);
            }
        };
      
      
      /** forwarding-ctor to provide the detailed input/output connections */
      ParamWeavingPattern (BlockBuilder builder, PostProcessor postProc, Port& delegate)
        : blockBuilder_{move (builder)}
        , postProcess_{move (postProc)}
        , delegatePort_{delegate}
        { }
      
      
      /** Preparation: create a Feed data frame to use as local scope */
      Feed
      mount (TurnoutSystem& turnoutSys)
        {
          return Feed{};
        }
      
      /** Invoke the parameter-functors to create the basic parameter data */
      void
      pull (Feed& feed, TurnoutSystem& turnoutSys)
        {
          feed.emplaceParamDataBlock (blockBuilder_, turnoutSys);
        }
      
      /** Link the param-data-block into the current TurnoutSystem,
       *  possibly post-process the param data. From this point on,
       *  nodes within the nested scope can draw from this data.
       */
      void
      shed (Feed& feed, TurnoutSystem& turnoutSys, OptionalBuff outBuff)
        {
          turnoutSys.attachChainBlock(feed.block());
          feed.outBuff = outBuff;
          if (postProcess_)
            postProcess_(turnoutSys);
        }
      
      /** recursively invoke the delegate port, while the generated
       *  parameter-data is indirectly reachable through the TurnoutSystem
       */
      void
      weft (Feed& feed, TurnoutSystem& turnoutSys)
        {
          feed.outBuff = delegatePort_.weave (turnoutSys, feed.outBuff);
        }
      
      /** clean-up: detach the parameter-data-block.
       * @return the output buffer produced by the recursive delegate call.
       */
      BuffHandle
      fix (Feed& feed, TurnoutSystem& turnoutSys)
        {
          turnoutSys.detachChainBlock(feed.block());
          return feed.outBuff;
        }
      
    };
  
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_PARAM_WEAVING_PATTERN_H*/
