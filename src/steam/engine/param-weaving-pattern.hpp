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
#include "lib/meta/function.hpp"
#include "lib/meta/variadic-helper.hpp"
#include "lib/meta/tuple-helper.hpp"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
//#include "vault/gear/job.h"
//#include "steam/engine/exit-node.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/linked-elements.hpp"
#include "lib/several.hpp"
//#include "lib/util-foreach.hpp"
//#include "lib/iter-adapter.hpp"
//#include "lib/meta/function.hpp"
//#include "lib/itertools.hpp"
//#include "lib/util.hpp"      ////////OOO wegen manifoldSiz<FUN>()

//#include <stack>
#include <tuple>
#include <utility>


namespace steam {
namespace engine {
  
  using std::move;
  using std::forward;
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
      
      /** @internal the _chain constructor type_ is a type rebinding meta function (nested struct),
       * which extends the HeteroData chain given by \a ANK with the sequence of types derived from
       * the result-values of all functors stored in the ParamBuildSpec, i.e. the resulting param tuple.
       * @remark HeteroData defines a nested struct `Chain`, and with the help of `RebindVariadic`,
       *         the type sequence from the ParamTup can be used to instantiate this Chain context.
       */
      using ChainCons = typename lib::meta::RebindVariadic<ANK::template Chain, ParamTup>::Type;
      
      typename ChainCons::NewFrame
      buildParamDataBlock (TurnoutSystem& turnoutSys)
        {
          return std::apply ([&](auto&&... paramFun)
                                  {  //    invoke parameter-functors and build NewFrame from results
                                    return ChainCons::build (paramFun (turnoutSys) ...);
                                  }
                            ,functors_);
        }
      
      
      template<size_t slot>
      class Slot
        : util::MoveOnly
        {
          ParamBuildSpec& spec_;
          
          Slot (ParamBuildSpec& spec)
            : spec_{spec}
            { }
          friend class ParamBuildSpec;
          
        public:
          auto
          invokeParamFun (TurnoutSystem& turnoutSys)
            {
              return std::get<slot> (spec_.functors_) (turnoutSys);
            }
          
          /** a getter functor able to work on the full extended HeteroData-Chain
           * @remark the front-end of this chain resides in TurnoutSystem */
          using Accessor = typename ChainCons::template Accessor<slot>;
          static auto makeAccessor() { return Accessor{}; }
          
          static auto&
          getParamVal (TurnoutSystem& turnoutSys)
            {
              return turnoutSys.get (makeAccessor());
            }
        };
      
      template<size_t idx>
      Slot<idx>
      slot()
        { return *this; }
    };
  
  auto
  buildParamSpec()
    {
      return ParamBuildSpec<TurnoutSystem::FrontBlock>{tuple<>{}};
    }
  
  
    /**
     * Implementation for a _Weaving Pattern_ to conduct extended parameter evaluation.
     */
  template<class INVO>
  struct ParamWeavingPattern
    : INVO
    {
      using Feed = typename INVO::Feed;
      
      static_assert (_verify_usable_as_InvocationAdapter<Feed>());
      
      Several<PortRef>   leadPort;
      Several<BuffDescr> outTypes;
      
      uint resultSlot{0};
      
      /** forwarding-ctor to provide the detailed input/output connections */
      template<typename...ARGS>
      ParamWeavingPattern (Several<PortRef>&&   pr
                          ,Several<BuffDescr>&& dr
                          ,uint resultIdx
                          ,ARGS&& ...args)
        : INVO{forward<ARGS>(args)...}
        , leadPort{move(pr)}
        , outTypes{move(dr)}
        , resultSlot{resultIdx}
        { }
      
      
      Feed
      mount (TurnoutSystem& turnoutSys)
        {
          ENSURE (leadPort.size() <= INVO::FAN_I);
          ENSURE (outTypes.size() <= INVO::FAN_O);
          return INVO::buildFeed (turnoutSys);
        }
      
      void
      pull (Feed& feed, TurnoutSystem& turnoutSys)
        {
          if constexpr (Feed::hasInput())
            for (uint i=0; i<leadPort.size(); ++i)
              {
                BuffHandle inputData = leadPort[i].get().weave (turnoutSys);
                feed.inBuff.createAt(i, move(inputData));
              }
        }
      
      void
      shed (Feed& feed, OptionalBuff outBuff)
        {
          for (uint i=0; i<outTypes.size(); ++i)
              {
                BuffHandle resultData =
                  i == resultSlot and outBuff? *outBuff
                                             : outTypes[i].lockBuffer();
                feed.outBuff.createAt(i, move(resultData));
              }
          feed.connect();
        }
      
      void
      weft (Feed& feed)
        {
          feed.invoke();                 // process data
        }
      
      BuffHandle
      fix (Feed& feed)
        {
          if constexpr (Feed::hasInput())
            for (uint i=0; i<leadPort.size(); ++i)
              {
                feed.inBuff[i].release();
              }
          for (uint i=0; i<outTypes.size(); ++i)
              {
                feed.outBuff[i].emit();    // state transition: data ready
                if (i != resultSlot)
                  feed.outBuff[i].release();
              }
          ENSURE (resultSlot < INVO::FAN_O, "invalid result buffer configured.");
          return feed.outBuff[resultSlot];
        }
      
    };
  
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_PARAM_WEAVING_PATTERN_H*/
