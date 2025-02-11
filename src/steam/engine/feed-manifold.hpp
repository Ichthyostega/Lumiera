/*
  FEED-MANIFOLD.hpp  -  data feed connection system for render nodes

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file feed-manifold.hpp
 ** Adapter to connect parameters and data buffers to an external processing function.
 ** The Lumiera Render Engine relies on a »working substrate« of _Render Nodes,_ interconnected
 ** in accordance to the structure of foreseeable computations. Yet the actual media processing
 ** functionality is provided by external libraries — while the engine is arranged in a way to
 ** remain _agnostic_ regarding any details of actual computation. Those external libraries are
 ** attached into the system by means of a _library plugin,_ which cares to translate the external
 ** capabilities into a representation as _Processing Assets._ These can be picked up and used in
 ** the Session, and will eventually be visited by the _Builder_ as part of the effort to establish
 ** the aforementioned »network of Render Nodes.« At this point, external functionality must actually
 ** be connected to internal structures: this purpose is served by the FeedManifold.
 ** 
 ** This amounts to an two-stage adaptation process. Firstly, the plug-in for an external library
 ** has to wrap-up and package the library functions into an _invocation functor_ — which thereby
 ** creates a _low-level Specification_ of the functionality to invoke. This functor is picked up
 ** and stored as a prototype within the associated render node. More specifically, each node can
 ** offer several [ports for computation](\ref steam::engine::Port). This interface is typically
 ** implemented by a [Turnout](\ref turnout.hpp), which in turn is based on some »weaving pattern«
 ** performed around and on top of a FeedManifold instance, which is created anew on the stack for
 ** each invocation. This invocation scheme implies that the FeedManifold is tailored specifically
 ** for a given functor, matching the expectations indicated by the invocation functor's signature:
 ** - A proper invocation functor may accept _one to three arguments;_
 ** - in _must accept_ one or several **output** buffers,
 ** - optionally it _can accept_ one or several **input** buffers,
 ** - optionally it _can accept_ also one or several **parameters** to control specifics.
 ** - the order of these arguments is fixed to the sequence: _parameters, inputs, outputs._
 ** - Parameters are assumed to have _value semantics._ They must be copyable and default-constructible.
 ** - Buffers are always passed _by pointer._ The type of the pointee is picked up and passed-through.
 ** - such a pointee or buffer-type is assumed to be default constructible, since the engine will have
 **   to construct result buffers within its internal memory management scheme. The library-plugin
 **   might have to create a wrapper type in cases where the external library requires to use a
 **   specific constructor function for buffers (if this requirement turns out as problematic,
 **   there is leeway to pass constructor arguments to such a wrapper — yet Lumiera will insist
 **   on managing the memory, so frameworks enforcing their own memory management will have
 **   to be broken up and side-stepped, in order to be usable with Lumiera).
 ** - when several and even mixed types of a kind must be given, e.g. several buffers or
 **   several parameters, then the processing functor should be written such as to
 **   accept a std::tuple or a std::array.
 ** 
 ** \par Implementation remarks
 ** A suitable storage layout is chosen at compile type, based on the given functor type.
 ** - essentially, FeedManifold is structured storage with some default-wiring.
 ** - the trait functions #hasInput() and #hasParam() should be used by downstream code
 **   to find out if some part of the storage is present and branch accordingly...
 ** @remark in the first draft version of the Render Engine from 2009/2012, there was an entity
 **         called `BuffTable`, which however provided additional buffer-management capabilities.
 **         This name describes well the basic functionality, which can be hard to see with all
 **         the additional meta-programming related to the flexible functor signature. When it
 **         comes to actual invocation, we collect input buffers from predecessor nodes and
 **         we prepare output buffers, and then we pass both to a processing function.
 ** @see NodeBase_test
 ** @see weaving-pattern-builder.hpp
 ** @see \ref lib::meta::ElmTypes in variadic-helper.hpp "uniform processing of »tuple-like« data"
 */


#ifndef ENGINE_FEED_MANIFOLD_H
#define ENGINE_FEED_MANIFOLD_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "steam/engine/buffhandle.hpp"
#include "lib/uninitialised-storage.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/trait.hpp"
#include "lib/meta/typeseq-util.hpp"
#include "lib/meta/variadic-helper.hpp"
#include "lib/meta/generator.hpp"
#include "lib/test/test-helper.hpp"

#include <tuple>


namespace steam {
namespace engine {
  
  namespace {// Introspection helpers....
    
    using lib::meta::_Fun;
    using lib::meta::enable_if;
    using lib::meta::is_UnaryFun;
    using lib::meta::is_BinaryFun;
    using lib::meta::is_TernaryFun;
    using lib::meta::is_Structured;
    using lib::meta::forEachIDX;
    using lib::meta::ElmTypes;
    using lib::meta::NullType;
    using lib::meta::Tagged;
    using std::is_pointer;
    using std::is_reference;
    using std::is_convertible;
    using std::is_constructible;
    using std::is_copy_constructible;
    using std::remove_pointer_t;
    using std::tuple_element_t;
    using std::add_pointer_t;
    using std::__and_;
    using std::__not_;
    
    
    template<typename V>
    struct is_Value
      : __and_<__not_<is_pointer<V>>
              ,__not_<is_reference<V>>
              ,std::is_default_constructible<V>
              ,std::is_copy_assignable<V>
              >
      { };
    
    template<typename B>
    struct is_Buffer
      : __and_<is_pointer<B>
              ,__not_<_Fun<B>>
              ,std::is_default_constructible<remove_pointer_t<B>>
              >
      { };
    
    
    
    
    /**
     * Trait template to analyse and adapt to the given processing function.
     * The detection logic encoded here attempts to figure out the meaning of
     * the function arguments by their arrangement and type. As a base rule,
     * the arguments are expected in the order: Parameters, Input, Output
     * - a single argument function can only be a data generator
     * - a binary function can either be a processor input -> output,
     *   or accept parameters at «slot-0» and provide output at «slot-1»
     * - a ternary function is expected to accept Parameters, Input, Output.
     * @tparam FUN a _function-like_ object, expected to accept 1 - 3 arguments,
     *         which all may be simple types, tuples or arrays.
     * @note »Buffers« are always accepted by pointer, which allows
     *         to distinguish parameter and data «slots«
     * @see VariadicHelper_test::rebuild_variadic()
     */
    template<class FUN>
    struct _ProcFun
      {
        static_assert(_Fun<FUN>()           , "something funktion-like required");
        static_assert(0 <  _Fun<FUN>::ARITY , "function with at least one argument expected");
        static_assert(3 >= _Fun<FUN>::ARITY , "function with up to three arguments accepted");
        
        using Sig  = typename _Fun<FUN>::Sig;
        
        template<size_t i>
        using _Arg = typename lib::meta::Pick<typename _Fun<Sig>::Args, i>::Type;
        
        template<size_t i, template<class> class COND>
        using AllElements = typename ElmTypes<_Arg<i>>::template AndAll<COND>;
        
        template<size_t i>
        static constexpr bool nonEmpty   =   ElmTypes<_Arg<i>>::SIZ;
        template<size_t i>
        static constexpr bool is_BuffSlot =  AllElements<i, is_Buffer>();
        template<size_t i>
        static constexpr bool is_ParamSlot = AllElements<i, is_Value>();

        /**
         * Detect use-case as indicated by the function signature
         */
        template<class SIG,  typename SEL =void>
        struct _Case
          {
            static_assert (not sizeof(SIG), "use case could not be determined from function signature");
          };
        template<class SIG>
        struct _Case<SIG,   enable_if<is_UnaryFun<SIG>>>
          {
            enum{ SLOT_O = 0
                , SLOT_I = 0
            };
          };
        template<class SIG>
        struct _Case<SIG,   enable_if<is_BinaryFun<SIG>>>
          {
            enum{ SLOT_O = 1
                , SLOT_I = (nonEmpty<0> and is_BuffSlot<0>)? 0 : 1
            };
          };
        template<class SIG>
        struct _Case<SIG,   enable_if<is_TernaryFun<SIG>>>
          {
            enum{ SLOT_O = 2
                , SLOT_I = 1
            };
          };
        
        using SigI = _Arg<_Case<Sig>::SLOT_I>;
        using SigO = _Arg<_Case<Sig>::SLOT_O>;
        using SigP = _Arg< 0>;
        using ArgI = typename ElmTypes<SigI>::Seq;
        using ArgO = typename ElmTypes<SigO>::Seq;
        using ArgP = typename ElmTypes<SigP>::Seq;
        
        // Metaprogramming helper for Buffer types (sans pointer)
        using ElmsI = ElmTypes<typename ElmTypes<SigI>::template Apply<remove_pointer_t>>;
        using ElmsO = ElmTypes<typename ElmTypes<SigO>::template Apply<remove_pointer_t>>;
        
        enum{ FAN_I = ElmTypes<SigI>::SIZ
            , FAN_O = ElmTypes<SigO>::SIZ
            , FAN_P = ElmTypes<SigP>::SIZ
            , SLOT_I = _Case<Sig>::SLOT_I
            , SLOT_O = _Case<Sig>::SLOT_O
            , SLOT_P =  0
            };
        
        static constexpr bool hasInput() { return SLOT_I != SLOT_O; }
        static constexpr bool hasParam() { return 0 < SLOT_I; }
        
        /* ========== |consistency checks| ========== */
        static_assert (nonEmpty<SLOT_O> or nonEmpty<SLOT_I> or nonEmpty<SLOT_P>
                      ,"At least one slot of the function must accept data");
        static_assert (is_BuffSlot<SLOT_O>, "Output slot of the function must accept buffer pointers");
        static_assert (is_BuffSlot<SLOT_I>, "Input slot of the function must accept buffer pointers");
        static_assert (is_ParamSlot<SLOT_P> or not hasParam()
                      ,"Param slot must accept value data");
      };
    
    
    /**
     * Trait template to handle an _associated parameter functor._
     * In those cases, where the basic processing function is classified such
     * as to accept parameter(s), it may be desirable to _generate_ those parameters
     * at invocation — be it as a fixed parametrisation chosen for this usage, or even
     * by evaluation of an _Automation function_ for some parameters.
     * @tparam FUN type of the underlying _processing function_
     */
    template<class FUN>
    struct _ParamFun
      {
        using _Proc = _ProcFun<FUN>;
        
        static constexpr bool hasParam() { return _Proc::hasParam(); }
        
        using Param = std::conditional_t<hasParam(), typename _Proc::SigP, std::tuple<>>;
        
        template<class PF>
        using Res = typename _Fun<PF>::Ret;
        template<class PF>
        using SigP = add_pointer_t<typename _Fun<PF>::Sig>;
        
        template<class PF>
        using isSuitable  = __and_<is_constructible<Param, Res<PF>>
                                  ,std::is_invocable<PF, TurnoutSystem&>
                                  >;
        
        template<class PF>
        using isConfigurable = __and_<is_constructible<bool, PF&>
                                     ,__not_<is_convertible<PF&, SigP<PF>>>
                                     >;   // non-capture-λ are convertible via function-ptr to bool
                                         //  yet we want to detect a real built-in bool-conversion.
        template<class PF>
        static bool
        isActivated (PF const& paramFun)
          {
            if constexpr (isSuitable<PF>())
              { if constexpr (isConfigurable<PF>())
                  return bool(paramFun);
                else
                  return true;
              }
            return false;
          }
        
        
        template<class PF>
        static constexpr bool isParamFun()  { return isSuitable<PF>();                          }
        template<class PF>
        static constexpr bool canActivate() { return isSuitable<PF>() and isConfigurable<PF>(); }
      };
    
     /// a function of total void
    struct _Disabled
      {
        void operator() (void) const {/*I do make a difference, I really do!*/}
      };
    
  }//(End)Introspection helpers.
  
  
  
  template<class FUN, class PAM =_Disabled>
  class FeedPrototype;
  
  
  /**
   * Configuration context for a FeedManifold.
   * This type-rebinding helper provides a storage configuration
   * specifically tailored to serve the invocation of \a FUN
   * @note storage segments for input and parameters are only present
   *       when the given function is classified by _ProcFun<FUN>
   *       as handling input and / or parameters.
   * @remark since BuffHandle is not default-constructible, but must be
   *       retrieved from a BufferProvider rather, a chunk of uninitialised
   *       storage is used to accept the `BuffHandle`s allocated and populated
   *       with results from preceding Nodes.
   */
  template<class FUN>
  struct _StorageSetup
    {
      using _Trait = _ProcFun<FUN>;
      
      static constexpr bool hasInput() { return _Trait::hasInput(); }
      static constexpr bool hasParam() { return _Trait::hasParam(); }
      
      enum{ FAN_P = hasParam()? _Trait::FAN_P : 0
          , FAN_I = hasInput()? _Trait::FAN_I : 0
          , FAN_O = _Trait::FAN_O
          };
      
      template<size_t fan>
      using BuffS = lib::UninitialisedStorage<BuffHandle, fan>;
                                           // ^^^ can not be default-constructed
      using BuffI = BuffS<FAN_I>;
      using BuffO = BuffS<FAN_O>;
      
      using Param = std::conditional_t<hasParam(), typename _Trait::SigP, std::tuple<>>;
      using ArgI  = std::conditional_t<hasInput(), typename _Trait::SigI, std::tuple<>>;
      using ArgO  = typename _Trait::SigO;
      
      
      /** FeedManifold building block: hold parameter data */
      struct ParamStorage
        {
          Param param;
          
          ParamStorage()
            : param{}
            { }
          
          template<typename...INIT>
          ParamStorage (INIT&& ...paramInit)
            : param{forward<INIT> (paramInit)...}
            { }                                                                 /////////////////////////////TICKET #1392 : pick up actual param and compute cache key
        };
      
      /** FeedManifold building block: hold input buffer pointers */
      struct BufferSlot_Input
        {
          BuffI inBuff;
          ArgI  inArgs{};
        };
      
      /** FeedManifold building block: hold output buffer pointers */
      struct BufferSlot_Output
        {
          BuffO outBuff;
          ArgO  outArgs{};
        };
      
      template<typename F>
      using enable_if_hasParam  = typename lib::meta::enable_if_c<_ProcFun<F>::hasParam()>::type;
      
      template<class X>
      using NotProvided = Tagged<NullType, X>;
      
      template<bool yes, class B>
      using Provide_if = std::conditional_t<yes, B, NotProvided<B>>;
      
      using FeedOutput =                         BufferSlot_Output;
      using FeedInput  = Provide_if<hasInput(),  BufferSlot_Input>;
      using FeedParam  = Provide_if<hasParam(),  ParamStorage>;
      
      /**
       * Data Storage block for the FeedManifold
       * Flexibly configured based on the processing function.
       * @remark mixed-in as base-class
       */
      struct Storage
        : util::NonCopyable
        , FeedOutput
        , FeedInput
        , FeedParam
        {
          FUN process;
          
          template<typename F>
          Storage (F&& fun)
            : process{forward<F> (fun)}
            { }
          
          template<typename F,         typename =enable_if_hasParam<F>>
          Storage (Param p, F&& fun)
            : FeedParam{move (p)}
            , process{forward<F> (fun)}
            { }
        };
    };
  
  
  /**
   * Adapter to connect input/output buffers to a processing functor backed by an external library.
   * Essentially, this is structured storage tailored specifically to a given functor signature.
   * Tables of buffer handles are provided for the downstream code to store results received from
   * preceding nodes or to pick up calculated data after invocation. From these BuffHandle entries,
   * buffer pointers are retrieved and packaged suitably for use by the wrapped invocation functor.
   * This setup is intended for use by a »weaving pattern« within the invocation of a processing node
   * for the purpose of media processing or data calculation.
   * 
   * # Interface exposed to down-stream code
   * Data fields are typed to suit the given functor \a FUN, and are present only when needed
   * - `param` holds a parameter value or tuple of values, as passed to the constructor
   * - `inBuff` and `outBuff` are chunks of \ref UninitialisedStorage with suitable dimension
   *   to hold an array of \ref BuffHandle to organise input- and output-buffers
   * - the constants `FAN_P`, `FAN_I` and `FAN_O` reflect the number of individual elements
   *   connected for parameters, inputs and outputs respectively.
   * - `inBuff.array()` and `outBuff.array()` expose the storage for handles as std::array,
   *   with suitable dimension, subscript-operator and iteration. Note however that the
   *   storage itself is _uninitialised_ and existing handles must be _emplaced_ by
   *   invoking copy-construction e.g. `outBuff.createAt (idx, givenHandle)`
   * - after completely populating all BuffHandle slots this way, FeedManifold::connect()
   *   will pick up buffer pointers and transfer them into the associated locations in
   *   the input and output arguments `inArgs` and `outArgs`
   * - finally, FeedManifold::invoke() will trigger the stored processing functor,
   *   passing `param`, `inArgs` and `outArgs` as appropriate.
   * The `constexpr` functions #hasInput() and #hasParam() can be used to find out
   * if the functor was classified to take inputs and / or parameters.
   * @note destructors of parameter values will be invoked, but nothing will be done
   *       for the BuffHandle elements; the caller is responsible to perform the
   *       buffer management protocol, i.e. invoke BuffHandle::emit()
   *       and BuffHandle::release()
   */
  template<class FUN>
  struct FeedManifold
    : _StorageSetup<FUN>::Storage
    {
      using _T = _ProcFun<FUN>;
      using _S = _StorageSetup<FUN>;
      using _F = typename _S::Storage;
      
      /** pass-through constructor */
      using _S::Storage::Storage;
      
      using ArgI  = typename _S::ArgI;
      using ArgO  = typename _S::ArgO;
      using Param = typename _S::Param;
      enum{ FAN_I = _S::FAN_I
          , FAN_O = _S::FAN_O
          , FAN_P = _S::FAN_P
          };
      
      static constexpr bool hasInput() { return _S::hasInput(); }
      static constexpr bool hasParam() { return _S::hasParam(); }
      
      
      /**
       * cross-builder: _Prototype_ can be used to attach parameter-provider-functors
       * and then to create several further FeedManifold instances.
       */
      using Prototype = FeedPrototype<FUN>;
      
      
      
      template<size_t i, class ARG>
      auto&
      accessArg (ARG& arg)
        {
          if constexpr (is_Structured<ARG>())
            return std::get<i> (arg);
          else
            return arg;
        }
      
      using TupI = typename _T::ElmsI::Tup;
      using TupO = typename _T::ElmsO::Tup;
      
      
      void
      connect()
        {
          if constexpr (hasInput())
          {
            forEachIDX<TupI> ([&](auto i)
                                {
                                  using BuffI = tuple_element_t<i, TupI>;
                                  accessArg<i> (_F::inArgs) = & _F::inBuff[i].template accessAs<BuffI>();
                                });
          }
          // always wire output buffer(s)
          {
            forEachIDX<TupO> ([&](auto i)
                                {
                                  using BuffO = tuple_element_t<i, TupO>;
                                  accessArg<i> (_F::outArgs) = & _F::outBuff[i].template accessAs<BuffO>();
                                });
          }
        }
      
      void
      invoke()
        {
          if constexpr (hasParam())
            if constexpr (hasInput())
              _F::process (_F::param, _F::inArgs, _F::outArgs);
            else
              _F::process (_F::param, _F::outArgs);
          else
            if constexpr (hasInput())
              _F::process (_F::inArgs, _F::outArgs);
            else
              _F::process (_F::outArgs);
        }
    };
  
  
  
  /**
   * Builder-Prototype to create FeedManifold instances.
   * This »Prototype« becomes part of the Turnout / WeavingPattern
   * and holds processing- and parameter-functor instances as configuration.
   * The Processing-Functor will be copied into the actual FeedManifold instance
   * for each Node invocation.
   * @tparam FUN type of the data processing-functor
   * @tparam PAM type of an optional parameter-setup functor (defaults to deactivated)
   * 
   * # Usage
   * The Prototype is typically first built solely from a processing-functor.
   * It can even be constructed as type only, by `FeedManifold<FUN>::Prototype`.
   * In this form, any parameter handling will be _disabled._ However, by adding a
   * parameter-functor with the **cross-builder-API**, a _new instance_ of the prototype
   * is created _as a replacement_ of the old one (note: we move the processing functor).
   * This adds a parameter-functor to the configuration, which will then be invoked
   * _whenever a new FeedManifold instance_ [is created](\ref #buildFeed); the result of
   * this parameter-functor invocation should be a parameter value, which can be passed
   * into the constructor of FeedManifold, together with a copy of the proc-functor.
   * @see NodeBase_test::verify_FeedPrototype()
   */
  template<class FUN, class PAM>
  class FeedPrototype
    : util::MoveOnly
    {
      using _Proc = _ProcFun<FUN>;
      using _Trait = _ParamFun<FUN>;
      
      FUN procFun_;
      PAM paramFun_;
      
    public:
      using Feed = FeedManifold<FUN>;
      enum{ FAN_I = Feed::FAN_I
          , FAN_O = Feed::FAN_O
          , FAN_P = Feed::FAN_P
      };
      using ElmsI = typename _Proc::ElmsI;
      using ElmsO = typename _Proc::ElmsO;
      
      template<template<class> class META>
      using OutTypesApply = typename ElmsO::template Apply<META>;
      
      
      /** setup with processing-functor only */
      FeedPrototype (FUN&& proc)
        : procFun_{move (proc)}
        , paramFun_{}
        { }
      
      FeedPrototype (FUN&& proc, PAM&& par)
        : procFun_{move (proc)}
        , paramFun_{move (par)}
        { }
      // default move acceptable : pass pre-established setup
      
      static constexpr bool hasParam()    { return _Trait::hasParam(); }
      static constexpr bool hasParamFun() { return _Trait::template isParamFun<PAM>();  }
      static constexpr bool canActivate() { return _Trait::template canActivate<PAM>(); }
      
      /** @return runtime test: actually usable parameter-functor available to invoke? */
      bool isActivated()  const           { return _Trait::isActivated(paramFun_); }
      
      
      
      /************************************************************//**
       * create suitable Feed(Manifold) for processing a Node invocation
       */
      Feed
      buildFeed (TurnoutSystem& turnoutSys)
        {
          if constexpr (hasParamFun())
            if (isActivated())
              return Feed(paramFun_(turnoutSys), procFun_);
          return Feed{procFun_};
        }
      
      
      
      /* ======= cross-builder API ======= */
      
      using ProcFun = FUN;
      using ParamFun = PAM;
      
      template<typename PFX>
      using Adapted = FeedPrototype<FUN,PFX>;
      
      template<typename FUX>
      using Decorated = FeedPrototype<FUX,PAM>;
      
      /** is the given functor suitable as parameter functor for this Feed? */
      template<typename PFX>
      static constexpr bool isSuitable()
        {
          return hasParam() and _Trait::template isParamFun<PFX>();
        }
      
      /**
       * Cross-Builder to add configuration with a given parameter-functor.
       * @return new FeedPrototype instance outfitted with the current
       *         processing-functor and the given other param-functor
       * @warning the current instance is likely **defunct** after this call,
       *         and should not be used any more, due to the move-construct.
       * @remark together with the move-ctor of FeedPrototype this helper
       *         can be used to configure a Prototype in several steps.
       */
      template<typename PFX>
      auto
      moveAdaptedParam (PFX otherParamFun =PFX{})
        {
          using OtherParamFun = std::decay_t<PFX>;
          return Adapted<OtherParamFun>{move(procFun_), move(otherParamFun)};
        }
      
      template<typename FUX>
      auto
      moveDecoratedProc (FUX adaptedProcFun)
        {
          using AugmentedProcFun = std::decay_t<FUX>;
          return Decorated<AugmentedProcFun>{move(adaptedProcFun), move(paramFun_)};
        }
      
      template<typename TRA>
      auto
      moveTransformedParam (TRA paramTransformer)
        {
          static_assert (_Trait::hasParam(), "Processing-functor with parameters expected");
          using SigP = lib::meta::_FunArg<TRA>;
          using SigI = typename _Proc::SigI;
          using SigO = typename _Proc::SigO;
          if constexpr (_Proc::hasInput())
            {
              return moveDecoratedProc([procFun = move(procFun_)
                                       ,transform = move(paramTransformer)
                                       ]
                                       (SigP par, SigI in, SigO out)
                                        {
                                          return procFun (transform(par), in, out);
                                        });
            }
          else
            {
              return moveDecoratedProc([procFun = move(procFun_)
                                       ,transform = move(paramTransformer)
                                       ]
                                       (SigP par, SigO out)
                                        {
                                          return procFun (transform(par), out);
                                        });
            }
        }
      
      
      /** build a clone-copy of this prototype, holding the same functors
       * @note possible only if both proc-functor and param-functor are copyable
       */
      enable_if<__and_<is_copy_constructible<FUN>
                      ,is_copy_constructible<PAM>>, FeedPrototype>
      clone()  const
        {
          return FeedPrototype{FUN(procFun_), PAM(paramFun_)};
        }
      
      /**
       * Change the current parameter-functor setup by assigning some value.
       * @param paramFunDef something that is assignable to \a PAM
       * @note  possible only if the param-functor accepts this kind of assignment;
       *        especially when \a PAM was defined to be a `std::function`, then
       *        the param-functor can not only be reconfigured, but also disabled.
       */
      template<typename PFX =PAM,      typename = enable_if<std::is_assignable<PAM,PFX>>>
      FeedPrototype&&
      assignParamFun (PFX&& paramFunDef =PAM{})
        {
          paramFun_ = forward<PFX> (paramFunDef);
          return move(*this);
        }
    };
  
}} // namespace steam::engine
#endif /*ENGINE_FEED_MANIFOLD_H*/
