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
 ** This amounts to an tow-stage adaptation process. Firstly, the plug-in for an external library
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
 **   on managing the memory, so frameworks insisting on their own memory management will have
 **   to be broken up and side-stepped, in order to be usable with Lumiera).
 ** - when several and even mixed types of a kind must be given, e.g. several buffers or
 **   several parameters, then the processing functor should be written such as to
 **   accept a std::tuple or a std::array.
 ** 
 ** \par Implementation remarks
 ** A suitable storage layout is chosen at compile type, based on the given functor type.
 ** - essentially, FeedManifold is structured storage with some default-wiring.
 ** - the trait functions #hasInput() and #hasParam() should be used by downstream code
 **   to find out if some part of the storage is present and branch accordingly
 ** @todo 12/2024 figure out how constructor-arguments can be passed flexibly
 ** @todo staled since 2009, picked up in 2024 in an attempt to finish the node invocation.
 ** @todo WIP-WIP 2024 rename and re-interpret as a connection system
 ** @see NodeBase_test
 ** @see weaving-pattern-builder.hpp
 */


#ifndef ENGINE_FEED_MANIFOLD_H
#define ENGINE_FEED_MANIFOLD_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
//#include "steam/engine/proc-node.hpp"
#include "steam/engine/buffhandle.hpp"
#include "lib/uninitialised-storage.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/typeseq-util.hpp"
//#include "lib/several.hpp"

//#include <utility>
//#include <array>


////////////////////////////////TICKET   #826  will be reworked alltogether

namespace steam {
namespace engine {
  
//  using std::pair;
//  using std::vector;
  
  namespace {// Introspection helpers....
    
    using lib::meta::_Fun;
    using lib::meta::is_UnaryFun;
    using lib::meta::is_BinaryFun;
    using lib::meta::is_TernaryFun;
    using std::remove_reference_t;
    using lib::meta::enable_if;
    using std::void_t;
    using std::__and_;
    using std::__not_;
    
    template<class X,  typename SEL=void>
    struct is_Structured
      : std::false_type
      { };
    template<class TUP>
    struct is_Structured<TUP,   void_t<std::tuple_size<TUP>>>
      : std::true_type
      { };
    
    template<typename V>
    struct is_Value
      : __and_<__not_<std::is_pointer<V>>
              ,__not_<std::is_reference<V>>
              ,__not_<is_Structured<V>>
              ,std::is_default_constructible<V>
              ,std::is_copy_assignable<V>
              >
      { };
    
    template<typename B>
    struct is_Buffer
      : __and_<__not_<std::is_pointer<B>>
              ,__not_<std::is_reference<B>>
              ,std::is_default_constructible<B>
              ,__not_<_Fun<B>>
              >
      { };
    
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
        static_assert(0 <  _Fun<FUN>::ARITY , "function with at least one argument expected");
        static_assert(3 >= _Fun<FUN>::ARITY , "function with up to three arguments accepted");
        
        using Sig  = typename _Fun<FUN>::Sig;
        
        template<class SIG, size_t i>
        using _Arg = typename lib::meta::Pick<typename _Fun<SIG>::Args, i>::Type;
        
        
        template<class ARG,  typename SEL =void>
        struct _ArgTrait
          {
            static_assert(not sizeof(ARG), "processing function expected to take parameters, "
                                           "buffer-poiinters or tuples or arrays of these");
          };
        template<class PAR>
        struct _ArgTrait<PAR,   enable_if<is_Value<PAR>>>
          {
            using Buff = PAR; ////////////////////////////////OOO not correct, remove this!
            enum{ SIZ = 1 };
          };
        template<class BUF>
        struct _ArgTrait<BUF*,  enable_if<is_Buffer<BUF>>>
          {
            using Buff = BUF;
            enum{ SIZ = 1 };
          };
        template<class BUF, size_t N>
        struct _ArgTrait<std::array<BUF*,N>>
          {
            using Buff = BUF;
            enum{ SIZ = N };
          };
        
        
        template<class SIG,  typename SEL =void>
        struct _Case
          {
            static_assert(not sizeof(SIG), "use case could not be determined from function stignature");
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
                , SLOT_I = is_Value<_Arg<SIG,0>>()? 1 : 0
            };
          };
        template<class SIG>
        struct _Case<SIG,   enable_if<is_TernaryFun<SIG>>>
          {
            enum{ SLOT_O = 2
                , SLOT_I = 1
            };
          };
        
        using SigI = _Arg<FUN,_Case<Sig>::SLOT_I>;
        using SigO = _Arg<FUN,_Case<Sig>::SLOT_O>;
        
        using BuffI = typename _ArgTrait<SigI>::Buff;
        using BuffO = typename _ArgTrait<SigO>::Buff;
        
        enum{ FAN_I = _ArgTrait<SigI>::SIZ
            , FAN_O = _ArgTrait<SigO>::SIZ
            , SLOT_I = _Case<Sig>::SLOT_I
            , SLOT_O = _Case<Sig>::SLOT_O
            , MAXSZ = std::max (uint(FAN_I), uint(FAN_O))           /////////////////////OOO required temporarily until the switch to tuples
            };
        
        static constexpr bool hasInput() { return SLOT_I != SLOT_O; }
        static constexpr bool hasParam() { return 0 < SLOT_I; }
      };
    
  }//(End)Introspection helpers.
  
  template<class FUN>
  struct FeedManifold_StorageSetup
    : util::NonCopyable
    {
      
    };
  
    /**
     * Adapter to connect input/output buffers to a processing functor backed by an external library.
     * Essentially, this is structured storage tailored specifically to a given functor signature.
     * Tables of buffer handles are provided for the downstream code to store results received from
     * preceding odes or to pick up calculated data after the invocation. From these BuffHandle entries,
     * buffer pointers are retrieved and packaged suitably for use by the wrapped invocation functor.
     * This setup is used by a »weaving pattern« within the invocation of a processing node for the
     * purpose of media processing or data calculation.
     * 
     * @todo WIP-WIP-WIP 7/24 now reworking the old design in the light of actual render engine requirements...
     */
  template<class FUN>
  struct FoldManifeed
    : util::NonCopyable
    {
      enum{ STORAGE_SIZ = _ProcFun<FUN>::MAXSZ };
      using BuffS = lib::UninitialisedStorage<BuffHandle,STORAGE_SIZ>;
      
      BuffS inBuff;
      BuffS outBuff;
    };
  
  template<class FUN>
  struct FeedManifold
    : FeedManifold_StorageSetup<FUN>
    {
      using _Trait = _ProcFun<FUN>;
      
      static constexpr bool hasInput() { return _Trait::hasInput(); }
      static constexpr bool hasParam() { return _Trait::hasParam(); }
    };
  
  
  
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
      
      using ArrayI = typename _ProcFun<FUN>::SigI;
      using ArrayO = typename _ProcFun<FUN>::SigO;
      
      
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
          REQUIRE (fanIn == FAN_I and fanOut == FAN_O);  //////////////////////////OOO this distinction is a left-over from the idea of fixed block sizes
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
  
  
  
}} // namespace steam::engine
#endif /*ENGINE_FEED_MANIFOLD_H*/
