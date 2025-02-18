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
 ** it is a Weaving Pattern residing within some Node's Port). The use-case is to provide a set
 ** of additional parameter values, beyond what can be derived directly by a parameter-functor
 ** based on the _absolute-nominal-Time_ of the invocation. The necessity for such a setup may
 ** arise when additional context or external state must be combined with the nominal time into
 ** a tuple of data values, which shall then be consumed by several follow-up evaluations further
 ** down into a recursive invocation tree _for one single render job._ The solution provided by
 ** the Parameter Agent Node relies on placing those additional data values into a tuple, which
 ** is then stored directly in the render invocation stack frame, prior to descending into further
 ** recursive Node evaluations. Notably, parameter-functors within the scope of this evaluation tree
 ** can then access these additional parameters through the TurnoutSystem of the overall invocation.
 ** 
 ** ## Configuration
 ** In order to setup such an evaluation scheme involving a Parameter Agent as top entrance point,
 ** and a delegate Node tree below, in a first step, the evaluations for the additional parameter
 ** values must be established. This is achieved by defining a _Parameter Build Spec,_ which is
 ** a descriptor record with a builder-DSL notation to be assembled step by step. Internally,
 ** this ParamBuildSpec embeds a tuple of parameter-functors, one for each new parameter value.
 ** Furthermore, since the intended usage scheme calls for a lib::HeteroData »chain block« as
 ** storage for the extended parameter values, an _anchor type_ is embedded into the type signature
 ** of ParamBuildSpec; this _anchor_ is assumed to be the HeteroData-prefix-chain, where the new
 ** data block is assumed to be attached at the end. In the common use case, this prefix will be
 ** the default layout of a TurnoutSystem, which internally embeds a lib::HeteroData record,
 ** configured to hold some basic parameters, which are present in every render invocation:
 ** - the _absolute nominal Time_ in the timeline
 ** - a _process Key_
 ** 
 ** Since this is a well-known, fixed layout, the definition of a ParamBuildSpec can be started
 ** from a static convenience function, steam::engine::buildParamSpec(), which yields an (initially
 ** empty) ParamBuildSpec, anchored at the default layout of the TurnoutSystem. On this builder term,
 ** several «parameter slots» can be added successively, either
 ** - by embedding a fixed (constant) parameter value
 ** - or with a parameter-functor, which works on the basic default TurnoutSystem
 ** 
 ** Once a ParamBuildSpec is outfitted with all desired functors, it can be used to generate
 ** _accessor functors_ — which are the crucial ingredient for actually accessing the extended
 ** parameter values from the nested node tree. Please recall that also a regular Render Node Port
 ** can embed a parameter-functor, which works on the TurnoutSystem. Usually this param-functor
 ** will retrieve the _absolute nominal time_ from there, but in our case here, the purpose of
 ** the whole elaborate scheme is that such an parameter-functor embedded into some processing
 ** node down in the nested tree can access the extended parameters, which were generated
 ** initially by the Param Agent Node at top-level and reside in a storage block somewhere up
 ** the call stack, from where they are linked-in temporarily into the TurnoutSystem. Thus,
 ** after building the ParamBuildSpec, next some actual Render Nodes have to be built, which
 ** include the _accessor functors_ retrieved from the ParamBuildSpec. These are actually
 ** static type markers, as they do not actually refer to the ParamBuildSpec, but rather to
 ** the «slot number» in the structure (which is encoded into the type signature). Such an
 ** accessor functor can be applied to the extended TurnoutSystem, and will retrieve the
 ** corresponding extended parameter value from the chain-block linked temporarily into
 ** the TurnoutSystem.
 ** 
 ** Finally, when the nested delegate Node tree is complete, the actual Param Agent Node
 ** can be built through a special hook in the NodeBuilder: After opening the nested definition
 ** of a port, invoke steam::engine::PortBuilderRoot::computeParam(spec), passing the ParamBuildSpec
 ** record (with the actual functor instances embedded!). Furthermore, the link to the delegate
 ** Node tree must be added, followed by `.completePort()`. Optionally, a post-processing functor
 ** can be added, which will be invoked after all parameters are generated, but before delegating
 ** to the nested Node tree.
 ** 
 ** As should be clear from context, the so called »Param Agent Node« is actually a special port.
 ** Yet in practice, you'd typically set up a top-level node, where each port is configured with
 ** a suitable Param Agent setup, and then delegates to the processing chain to produce the
 ** content to produce for this port. Internally, this Param Agent setup is a Turnout (thus
 ** implementing the Port interface) with a special \ref ParamWeavingPattern. The latter
 ** incorporates all the steps necessary to establish this special extended parameter setup
 ** - it first invokes all the parameter-functors in the tuple
 ** - it forms a parameter tuple from the result values
 ** - this parameter-tuple is dropped off into a storage within the current stack frame
 ** - then this storage block (which is a valid HeteroData chain-block), will be linked
 **   temporarily with the current TurnoutSystem of the invocation
 ** - next an optional postprocessing-functor will be invoked
 ** - followed by a regular recursive invocation of the delegate tree
 ** - as a last step, the temporary storage block is disconnected from the TurnoutSystem
 ** - and the result buffer of the recursive delegate tree is returned as result.
 ** 
 ** @see node-builder.hpp
 ** @see weaving-pattern-builder.hpp
 ** @see \ref NodeFeed_test::feedParamNode() "elaborate demonstration example"
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
#include "lib/uninitialised-storage.hpp"
#include "lib/meta/variadic-rebind.hpp"
#include "lib/meta/tuple-helper.hpp"
#include "lib/meta/function.hpp"

#include <tuple>
#include <utility>


namespace steam {
namespace engine {
  
  using std::move;
  using std::forward;
  using std::function;
  using std::make_tuple;
  using std::tuple;
  using lib::meta::Tuple;
  using lib::meta::ElmTypes;
  
  
  /**
   * Specification record for the setup of a »Param Agent Node«.
   * @tparam ANCH the lib::HeteroData prefix-chain to use as anchor point
   * @tparam FUNZ a set of parameter-functors used to generate additional parameter values.
   * @note This template provides a builder-DSL notation, starting with the free function
   *       \ref buildParamSpec(). Further parameter «slots» can be added step by step.
   * @remark use the nested Accessor records to retrieve the generated parameter values
   *       and use the nested BlockBuilder as a »Prototype« in the ParamWeavingPattern,
   *       which actually can be configured through the NodeBuilder...
   */
  template<class ANCH, typename...FUNZ>
  struct ParamBuildSpec
    {
      using Functors = tuple<FUNZ...>;
      
      using ResTypes = typename ElmTypes<Functors>::template Apply<lib::meta::_FunRet>;
      using ParamTup = Tuple<ResTypes>;
      
      Functors functors_;
      
      ParamBuildSpec (Functors&& funz)
        : functors_{move (funz)}
        { }
      
      /** can be copied if all functors are copyable... */
      ParamBuildSpec clone() { return *this; }
      
      
      template<typename FUN>
      auto
      addSlot (FUN&& paramFun)
        {
          using FunN = std::decay_t<FUN>;
          return ParamBuildSpec<ANCH,FUNZ...,FunN>{std::tuple_cat (move(functors_)
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
       * which extends the HeteroData chain given by \a ANCH with the sequence of types derived from
       * the result-values of all functors stored in the ParamBuildSpec, i.e. the resulting param tuple.
       * @remark HeteroData defines a nested struct `Chain`, and with the help of `RebindVariadic`,
       *         the type sequence from the ParamTup can be used to instantiate this chain constructor.
       */
      using ChainCons = typename lib::meta::RebindVariadic<ANCH::template Chain, ParamTup>::Type;
      
      
      /** a (static) getter functor able to work on the full extended HeteroData-Chain
       * @remark the front-end of this chain resides in TurnoutSystem */
      template<size_t slot>
      struct Accessor
        {
          static auto&
          getParamVal (TurnoutSystem& turnoutSys)
            {
              using StorageAccessor = typename ChainCons::template Accessor<slot>;
              return turnoutSys.retrieveData (StorageAccessor());
            }
        };
      
      template<size_t idx>
      Accessor<idx>
      makeAccessor()
        {
          return Accessor<idx>{};
        }
      
      /**
       * @internal Helper: after the ParamBuildSpec is complete,
       *  it will be packaged into a BlockBuilder, which is then embedded
       *  as a kind of »Prototype« into the `Turnout<ParamWeavingPattern>`
       * @remark #emplateParamDataBlock is invoked on each Node activation.
       */
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
       * thereby invoking the embedded functors to drop-off the results into storage.
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
     * A tuple of parameter-functors is embedded and will be invoked on each activation,
     * to generate a tuple of parameter-values, which are placed into the local stack
     * frame and then made accessible through the TurnoutSystem. Then the delegatePort_
     * is invoked recursively; the additional parameter values are thus usable from
     * within all nodes contained therein, during this recursive evaluation.
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
      
      /** Storage data frame placed on the call stack */
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
      
      
      /** forwarding-ctor to used from within Turnout, to provide actual setup. */
      ParamWeavingPattern (BlockBuilder builder, PostProcessor postProc, Port& delegate)
        : blockBuilder_{move (builder)}
        , postProcess_{move (postProc)}
        , delegatePort_{delegate}
        { }
      
      
      /** Preparation: create a Feed data frame to use as local scope */
      Feed
      mount (TurnoutSystem&)
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
       *  Nodes within the nested scope can draw from this data.
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
          ENSURE (feed.outBuff);
        }
      
      /** clean-up: detach the parameter-data-block.
       * @return the output buffer produced by the recursive delegate call.
       */
      BuffHandle
      fix (Feed& feed, TurnoutSystem& turnoutSys)
        {
          turnoutSys.detachChainBlock(feed.block());
          return *feed.outBuff;
        }
      
      
      /** @internal expose data not dependent on the template params */
      friend auto
      _accessInternal(ParamWeavingPattern& patt)
      {
        return std::tie (patt.delegatePort_);
      }
    };
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_PARAM_WEAVING_PATTERN_H*/
