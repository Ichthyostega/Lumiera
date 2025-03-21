/*
  ITER-EXPLORER.hpp  -  building blocks for iterator evaluation strategies

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file iter-explorer.hpp
 ** Building tree expanding and backtracking evaluations within hierarchical scopes.
 ** Based on the *Lumiera Forward Iterator* concept and using the basic IterAdapter templates,
 ** these components allow to implement typical evaluation strategies, like conditional expanding
 ** or depth-first exploration of a hierarchical structure. Since the access to this structure is
 ** abstracted through the underlying iterator, what we effectively get is a functional datastructure.
 ** The implementation is based on the idea of a "state core", which is wrapped right into the iterator
 ** itself (value semantics) -- similar to the IterStateWrapper, which is one of the basic helper templates
 ** provided by iter-adapter.hpp.
 ** 
 ** @remark historically, this template, as well as the initial IterExplorer (draft from 2012) can be
 **         seen as steps towards a framework of building blocks for tree expanding and backtracking
 **         algorithms — driven by a persistent need for this computation pattern, which lies in the
 **         nature of Lumiera's design: matching flexible configuration against a likewise hierarchical
 **         and rules based model.
 ** 
 ** # Background: Iterators as Monad
 ** The fundamental idea behind the implementation technique used here is the _Monad pattern_
 ** known from functional programming. A Monad is a container holding some arbitrarily typed base value; the monad can
 ** be seen as „amplifying“ and enhancing the contained base value by attaching additional properties or capabilities
 ** This is a rather detached concept with a wide variety of applications (things like IO state, parsers, combinators,
 ** calculations with exception handling but also simple data structures like lists or trees). The key point with any
 ** monad is the ability to _bind a function_ into the monad; this function will work on the _contained base values_
 ** and produce a modified new monad instance. In the simple case of a list, "binding" a function basically means
 ** to _map the function onto_ the elements in the list. (strictly speaking, it means the `flatMap` operation)
 ** 
 ** # A Pipeline builder
 ** Based on such concepts, structures and evaluation patterns, the IterExplorer serves the purpose to provide
 ** building blocks to assemble a _processing pipeline_, where processing will happen _on demand,_ while iterating.
 ** IterExplorer itself is both a Lumiera Forward Iterator based on some wrapped data source, and at the same time
 ** it is a builder to chain up processing steps to work on the data pulled from that source. These processing steps
 ** are attached as _decorators_ wrapping the source, in the order the corresponding builder functions were invoked.
 ** - the *expand operation* installs a function to consume one element and replace it by the sequence of elements
 **   (``children'') produced by that _»expansion functor«_. But this expansion does not happen automatically and
 **   on each element, rather it is triggered by issuing a dedicated `expandChildren()` call on the processing
 **   pipeline. Thus, binding the expansion functor has augmented the data source with the ability to explore
 **   some part in more detail _when required_.
 ** - the *transform operation* installs a function to be mapped onto each element retrieved from the underlying source
 ** - in a similar vein, the *filter operation* binds a predicate to decide about using or discarding data
 ** - in concert, expand- and transform operation allow to build hierarchy evaluation algorithms without exposing
 **   any knowledge regarding the concrete hierarchy used and explored as data source.
 ** - further special convenience adaptors and _terminal functions_ are provided.
 ** 
 ** In itself, the IterExplorer is an iterator with implementation defined type (all operations being inlined).
 ** But it is possible to package this structure behind a conventional iteration interface with virtual functions.
 ** By invoking the terminal builder function IterExplorer::asIterSource(), the iterator compound type, as created
 ** thus far, will be moved into a heap allocation, returning a front-end based on IterSource. In addition, the
 ** actually returned type, IterExplorerSource, exposes the `expandChildren()` operation as discussed above.
 ** 
 ** ## Rationale
 ** This design leads to a complete separation of the transforming operation from the mechanics how to apply that
 ** operation and combine the results. More specifically, we rely on an iterator to represent an abstracted source
 ** of data and we expose the combined and transformed results again as such an abstracted data sequence. Thereby,
 ** a trend towards separation of concerns is introduced; the data source remains opaque, while the manipulation
 ** to apply can be selected at runtime or written inline as Lambda. The iterator itself is a self-contained
 ** value and represents partial evaluation state without requiring a container for intermediary results.
 ** 
 ** ## some implementation details
 ** The builder operations assemble a heavily nested type, each builder call thereby adding yet another layer of
 ** subclassing. The templates involved into this build process are _specialised_, as driven by the actual functor
 ** type bound into each builder step; this functor is investigated and possibly adapted, according to its input
 ** type, while its output type determines the value type used in the pipeline "downstream". A functor with an
 ** input (argument) type incompatible or unsuitable to the existing pipeline will produce that endless sway
 ** of template error messages we all love so much. When this happens, please look at the static assertion
 ** error message typically to be found below the first template-instantiation stack sequence of messages.
 ** 
 ** @warning all builder operations work by _moving_ the existing pipeline built thus far into the parent
 **          of the newly built subclass object. The previously existing pipeline is defunct after that
 **          move; if you captured it into a variable, be sure to capture the _result_ of the new builder
 **          operation as well and don't use the old variable anymore. An insidious trap can be to store
 **          references to source iterator state _from within_ the pipeline. Moreover, it should be ensured
 **          that any "state core" used within IterExplorer has an efficient move ctor; including RVO,
 **          the compiler is typically able to optimise such move calls away altogether.
 ** 
 ** @see IterExplorer_test
 ** @see iter-adapter.hpp
 ** @see itertools.hpp
 ** @see IterSource (completely opaque iterator)
 **
 */


#ifndef LIB_ITER_EXPLORER_H
#define LIB_ITER_EXPLORER_H


#include "lib/error.hpp"
#include "lib/uninitialised-storage.hpp"
#include "lib/meta/duck-detector.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/trait.hpp"
#include "lib/wrapper.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/iter-source.hpp"   /////////////TICKET #493 : only using the IterSource base feature / interface here. Should really split the iter-source.hpp
#include "lib/iter-stack.hpp"
#include "lib/util.hpp"

#include <functional>
#include <optional>
#include <utility>


//Forward declaration to allow a default result container for IterExplorer::effuse
namespace std {
  template<typename T, class A> class vector;
  template<typename K, typename CMP, class A> class set;
}


namespace lib {
  
  using std::move;
  using std::forward;
  using std::function;
  using util::isnil;
  
  namespace error = lumiera::error;
  
  namespace iter_explorer { // basic iterator wrappers...
    
    template<class CON>
    using iterator = typename meta::Strip<CON>::TypeReferred::iterator;
    template<class CON>
    using const_iterator = typename meta::Strip<CON>::TypeReferred::const_iterator;
    
    /**
     * Adapt STL compliant container.
     * @note the container itself is _not_ included in the resulting iterator,
     *       it is just assumed to stay alive during the entire iteration.
     */
    template<class CON>
    struct StlRange
      : RangeIter<iterator<CON>>
      {
        StlRange() =default;
        StlRange (CON& container)
          : RangeIter<iterator<CON>> {begin(container), end(container)}
          { }
       // standard copy operations acceptable
      };
    
    template<class CON>
    struct StlRange<const CON>
      : RangeIter<const_iterator<CON>>
      {
        StlRange() =default;
        StlRange (CON const& container)
          : RangeIter<const_iterator<CON>> {begin(container), end(container)}
          { }
       // standard copy operations acceptable
      };
    
    template<class CON>
    struct StlRange<CON &>
      : StlRange<CON>
      {
        using StlRange<CON>::StlRange;
      };
    
    
    /**
     * Adapt an IterSource to make it iterable. As such, lib::IterSource is meant
     * to be iterable, while only exposing a conventional VTable-based _iteration interface_.
     * To support this usage, the library offers some builders to attach an iterator adapter.
     * Two flavours need to be distinguished:
     * - we get a _reference_ to something living elsewhere; all we know is it's iterable.
     * - we get a pointer, indicating that we must take ownership and manage the lifetime.
     *   The iterable entity in this case can be assumed to be heap allocated, featuring a
     *   virtual destructor.
     * The generated front-end has identical type in both cases; it is based on a shared_ptr,
     * just a different deleter function is used in both cases. This design makes sense,
     * since the protocol requires us to invoke virtual function IterSource::disconnect()
     * when use count goes to zero.
     */
    template<class ISO>
    class IterSourceIter
      : public ISO::iterator
      {
        using Iterator = typename ISO::iterator;
        
      public:
        IterSourceIter()  =default;
        // standard copy operations
        
        /** link to existing IterSource (without memory management) */
        IterSourceIter (ISO& externalSource)
          : Iterator{ISO::build (externalSource)}
          { }
        
        /** own and manage a heap allocated IterSource */
        IterSourceIter (ISO* heapObject)
          : Iterator{heapObject? ISO::build (heapObject)
                               : Iterator()}
          { }
        
        using Source = ISO;
        
        Source&
        source()
          {
            REQUIRE (ISO::iterator::isValid());
            return static_cast<ISO&> (*ISO::iterator::source());
          }
      };
    
  }//(End) namespace iter_explorer : basic iterator wrappers
  
  
  
  
  namespace { // IterExplorer traits
    
    using meta::enable_if;
    using meta::disable_if;
    using meta::Yes_t;
    using meta::No_t;
    using meta::_Fun;
    using std::__and_;
    using std::__not_;
    using std::is_const_v;
    using std::is_base_of;
    using std::common_type;
    using std::common_type_t;
    using std::conditional_t;
    using std::is_convertible;
    using std::remove_reference_t;
    using meta::is_StateCore;
    using meta::can_IterForEach;
    using meta::can_STL_ForEach;
    using meta::ValueTypeBinding;
    using meta::has_TypeResult;
    
    
    template<class SRC>
    struct shall_wrap_STL_Iter
      : __and_<can_STL_ForEach<SRC>
              ,__not_<can_IterForEach<SRC>>
              >
      { };
    
    template<class SRC>
    struct shall_use_Lumiera_Iter
      : __and_<can_IterForEach<SRC>
              ,__not_<is_StateCore<SRC>>
              >
      { };
    
    
    
    /**
     * @internal Type-selector template to adapt for IterExplorer:
     *           decide how to adapt and embed the source sequence */
    template<class SRC, typename SEL=void>
    struct _DecoratorTraits
      {
        static_assert (!sizeof(SRC), "Can not build IterExplorer: Unable to figure out how to iterate the given SRC type.");
      };
    
    template<class SRC>
    struct _DecoratorTraits<SRC,   enable_if<is_StateCore<SRC>>>
      {
        using SrcRaw  = typename lib::meta::Strip<SRC>::Type;
        using SrcVal  = typename meta::RefTraits<iter::CoreYield<SrcRaw>>::Value;
        using SrcIter = lib::IterableDecorator<lib::CheckedCore<SrcRaw>>;
      };
    
    template<class SRC>
    struct _DecoratorTraits<SRC,   enable_if<shall_use_Lumiera_Iter<SRC>>>
      {
        using SrcIter = remove_reference_t<SRC>;
        using SrcVal  = typename SrcIter::value_type;
      };
    
    template<class SRC>
    struct _DecoratorTraits<SRC,   enable_if<shall_wrap_STL_Iter<SRC>>>
      {
        static_assert (not std::is_rvalue_reference<SRC>::value,
                       "container needs to exist elsewhere during the lifetime of the iteration");
        using SrcIter = iter_explorer::StlRange<SRC>;
        using SrcVal  = typename SrcIter::value_type;
      };
    
    template<class ISO>
    struct _DecoratorTraits<ISO*,  enable_if<is_base_of<IterSource<typename ISO::value_type>, ISO>>>
      {
        using SrcIter = iter_explorer::IterSourceIter<ISO>;
        using SrcVal  = typename ISO::value_type;
      };
    
    template<class ISO>
    struct _DecoratorTraits<ISO*&, enable_if<is_base_of<IterSource<typename ISO::value_type>, ISO>>>
      : _DecoratorTraits<ISO*>
      { };
    
    template<class ISO>
    struct _DecoratorTraits<ISO&,  enable_if<is_base_of<IterSource<typename ISO::value_type>, ISO>>>
      : _DecoratorTraits<ISO*>
      { };
    
    
    
    /**
     * helper to derive a suitable common type when expanding children
     * @tparam SRC source iterator fed into the Expander
     * @tparam RES result type of the expansion function
     * @note this also implies the decision, if the common result
     *       can be exposed by-ref or must be delivered as value,
     *       which may have further ramification down the pipeline.
     */
    template<class SRC, class RES>
    struct _ExpanderTraits
      {
        using ResIter = typename _DecoratorTraits<RES>::SrcIter;
        using SrcYield = iter::Yield<SRC>;
        using ResYield = iter::Yield<ResIter>;
        using _CommonT = meta::CommonResultYield<SrcYield,ResYield>;
        static constexpr bool can_reconcile = _CommonT::value;
        static constexpr bool isRefResult = _CommonT::isRef;
        
        static_assert (can_reconcile,
                       "source iterator and result from the expansion must yield compatible values");
        static_assert (is_const_v<SrcYield> == is_const_v<ResYield>,
                       "source and expanded types differ in const-ness");
        
        using YieldRes   = typename _CommonT::ResType;
        using value_type = typename _CommonT::value_type;
        using reference  = typename _CommonT::reference;
        using pointer    = typename _CommonT::pointer;
      };
    
  }//(End) IterExplorer traits
  
  
  
  
  
  namespace iter_explorer { // Implementation of Iterator decorating layers...
    
    constexpr auto ACCEPT_ALL = [](auto){return true;};
    constexpr auto IDENTITY   = [](auto it){return *it;};
    
    /**
     * @internal technical details of binding a functor into the IterExplorer.
     * Notably, this happens when adapting an _"expansion functor"_ to allow expanding a given element
     * from the TreeExploer (iterator) into a sequence of child elements. A quite similar situation
     * arises when binding a _transformation function_ to be mapped onto each result element.
     * 
     * The IterExplorer::expand() operation accepts various flavours of functors, and depending on
     * the signature of such a functor, an appropriate adapter will be constructed here, allowing to
     * write a generic Expander::expandChildren() operation. The following details are handled here:
     * - detect if the passed functor is generic, or a regular "function-like" entity.
     * - in case it is generic (generic lambda), we assume it actually accepts a reference to
     *   the source iterator type `SRC`. Thus we instantiate a templated functor with this
     *   argument type to find out about its result type (and this instantiation may fail)
     * - moreover, we try to determine, if an explicitly typed functor accepts a value as yielded
     *   by the embedded source iterator (this is the "monadic" usage pattern), or if it rather
     *   accepts the iterator or state core itself (the "opaque state manipulation" usage pattern).
     * - we generate a suitable argument accessor function and build the function composition
     *   of this accessor and the provided _expansion functor_.
     * - the resulting, combined functor is stored into a std::function, thereby abstracting
     *   from the actual adapting mechanism. This allows to combine different kinds of functors
     *   within the same processing step; and in addition, it allows the processing step to
     *   remain agnostic with respect to the adaptation and concrete type of the functor/lambda.
     * @tparam FUN either the signature, or something _"function-like"_ passed as functor to be bound
     * @tparam SRC source type to feed to the function to be adapted.
     * @remark Especially need to specify the source iterator type to apply when passing a generic lambda
     *         or template as FUN. Such a generic functor will be _instantiated_ passing the type `SRC&`
     *         as argument. This instantiation may fail (and abort compilation), but when it succeeds,
     *         the result type `Res` can be inferred from the generic lambda.
     */
    template<class FUN, typename SRC>
    struct _FunTraits
      {
        /** handle all regular "function-like" entities */
        template<typename F, typename SEL =void>
        struct FunDetector
          {
            using Sig = typename _Fun<F>::Sig;
          };
        
        /** handle a generic lambda, accepting a reference to the `SRC` iterator */
        template<typename F>
        struct FunDetector<F,  disable_if<_Fun<F>> >
          {
            using Arg = typename std::add_lvalue_reference<SRC>::type;
            using Ret = decltype(std::declval<F>() (std::declval<Arg>()));
            using Sig = Ret(Arg);
          };
        
        
        using Sig = typename FunDetector<FUN>::Sig;
        using Arg = typename _Fun<Sig>::Args::List::Head;  // assuming function with a single argument
        using Res = typename _Fun<Sig>::Ret;
        static_assert (meta::is_UnaryFun<Sig>());
        
        
        
        /** adapt to a functor, which accesses the source iterator or embedded "state core" */
        template<class ARG, class SEL =void>
        struct ArgAdapter
          {
            using FunArgType = remove_reference_t<Arg>;
            static_assert (std::is_convertible<ARG, FunArgType>::value,
                           "the bound functor must accept the source iterator or state core as parameter");
            
            static decltype(auto)
            wrap (FUN&& rawFunctor)  ///< actually pass-through the raw functor unaltered
              {
                return forward<FUN> (rawFunctor);
              }
          };
        
        /** adapt to a functor, which accepts the value type of the source sequence ("monadic" usage pattern) */
        template<class IT>
        struct ArgAdapter<IT,   enable_if<__and_<is_convertible<iter::Yield<IT>, Arg>
                                                 ,__not_<is_convertible<IT, Arg>>>>>        // need to exclude the latter, since IterableDecorator
          {                                                                                //  often seems to accept IT::value_type (while in fact it doesn't)
            static auto
            wrap (function<Sig> rawFun)          ///< adapt by dereferencing the source iterator
              {
                return [rawFun](IT& srcIter) -> Res { return rawFun(*srcIter); };
              }
          };
        
        /** adapt to a functor collaborating with an IterSource based iterator pipeline */
        template<class IT>
        struct ArgAdapter<IT,   enable_if<__and_< is_base_of<IterSource<typename IT::value_type>, typename IT::Source>
                                                 , is_base_of<IterSource<typename IT::value_type>, remove_reference_t<Arg>>
                                                 > >>
          {
            using Source = typename IT::Source;
            
            static auto
            wrap (function<Sig> rawFun)       ///< extract the (abstracted) IterSource
              {
                return [rawFun](IT& iter) -> Res { return rawFun(iter.source()); };
              }
          };
        
        
        
        /** builder to create a nested/wrapping functor, suitably adapting the arguments */
        static auto
        adaptFunctor (FUN&& rawFunctor)
          {
            return function<Res(SRC&)> {ArgAdapter<SRC>::wrap (forward<FUN> (rawFunctor))};
          }
      };
    
    
    template<typename FUN, typename SRC>
    inline void
    static_assert_isPredicate()
    {
      using Res = typename _FunTraits<FUN,SRC>::Res;
      static_assert(std::is_constructible<bool, Res>::value, "Functor must be a predicate");
    }
    
    
    /**
     * @internal derive suitable result value types when reducing elements into an accumulator.
     */
    template<class SRC, class FUN>
    struct _ReduceTraits
      {
        using Result = typename iter_explorer::_FunTraits<FUN,SRC>::Res;
        using ResVal = typename lib::meta::RefTraits<Result>::Value;
      };
    
    
    
    
    
    /**
     * @internal Base of pipe processing decorator chain.
     * IterExplorer allows to create a stack out of various decorating processors
     * - each decorator is itself a _"state core"_, adding some on-demand processing
     * - each wraps and adapts a source iterator, attaching to and passing-on the iteration logic
     * Typically each such layer is configured with actual functionality provided as lambda or functor.
     * Yet in addition to forming an iteration pipeline, there is kind of an internal interconnection
     * protocol, allowing the layers to collaborate; notably this allows to handle an expandChildren()
     * call, where some "expansion layer" consumes the current element and replaces it by an expanded
     * series of new elements. Other layers might need to sync to this operation, and thus it is passed
     * down the chain. For that reason, we need a dedicated BaseAdapter to adsorb such chained calls.
     * @remark when building the IterExplorer, the to-be-wrapped source is fed down into its place
     *         within BaseAdapter. For that reason, it is not sufficient just to lift the copy ctors
     *         of the base (as inheriting the base class ctors would do). Rather, we need dedicated
     *         further copy ctors to clone and move from the _undecorated base type._
     */
    template<class SRC>
    struct BaseAdapter
      : SRC
      {
        BaseAdapter() = default;
        BaseAdapter(SRC const& src) : SRC{src}                { }
        BaseAdapter(SRC && src)     : SRC{forward<SRC> (src)} { }
        
        void expandChildren() { }                             ///< collaboration: recurse into nested scope
        size_t depth()  const { return 0; }                   ///< collaboration: number of nested scopes
        
        using TAG_IterExplorer_BaseAdapter = SRC;             ///< @internal for \ref _BaseDetector
      };
    
    
    /**
     * @internal Decorator for IterExplorer adding the ability to "expand children".
     * The expandChildren() operation is the key element of a depth-first evaluation: it consumes
     * one element and performs a preconfigured _expansion functor_ on that element to yield
     * its "children". These are given in the form of another iterator, which needs to be
     * compatible to the source iterator ("compatibility" boils down to both iterators
     * yielding a compatible value type). Now, this _sequence of children_ effectively
     * replaces the expanded source element in the overall resulting sequence; which
     * means, the nested sequence was _flattened_ into the results. Since this expand()
     * operation can again be invoked on the results, the implementation of such an evaluation
     * requires a stack datastructure, so the nested iterator from each expand() invocation
     * can be pushed to become the new active source for iteration. Thus the primary purpose
     * of this Expander (decorator) is to integrate those "nested child iterators" seamlessly
     * into the overall iteration process; once a child iterator is exhausted, it will be
     * popped and iteration continues with the previous child iterator or finally with
     * the source iterator wrapped by this decorator. The source pipeline is only pulled
     * once the expanded children are exhausted.
     * @remark since we allow a lot of leeway regarding the actual form and definition of the
     *         _expansion functor_, there is a lot of minute technical details, mostly confined
     *         within the _FunTraits traits. For the same reason, we need to prepare two different
     *         bindings of the passed raw functor, one to work on the source sequence, and the other
     *         one to work on the result sequence of a recursive child expansions; these two sequences
     *         need not be implemented in the same way, which simplifies the definition of algorithms.
     * @tparam SRC the wrapped source iterator, typically a IterExplorer or nested decorator.
     * @tparam FUN the concrete type of the functor passed. Will be dissected to find the signature
     * @note the _return type_ of #yield depends _both_ on the return type produced from the original
     *         sequence and the return type of the sequence established through the expand functor.
     *         An attempt is made to _reconcile these_ and this attempt may fail (at compile time).
     *         The reason is, any further processing downstream can not tell if data was produced
     *         by the original sequence of the expansion sequence. Notably, if one of these two
     *         delivers results by-value, then the Expander will _always_ deliver all results
     *         by-value, because it would not be possible to expose a reference to some value
     *         that was just delivered temporarily from a source iterator.
     */
    template<class SRC, class RES>
    class Expander
      : public SRC
      {
        static_assert(can_IterForEach<SRC>::value, "Lumiera Iterator required as source");
        
        using _Trait = _ExpanderTraits<SRC,RES>;
        using ResIter = typename _Trait::ResIter;
        using RootExpandFunctor = function<RES(SRC&)>;
        using ChldExpandFunctor = function<RES(ResIter&)>;
        
        RootExpandFunctor expandRoot_;
        ChldExpandFunctor expandChild_;
        
        IterStack<ResIter> expansions_;
        
      public:
        Expander() =default;
        // inherited default copy operations
        
        template<typename FUN>
        Expander (SRC&& parentExplorer, FUN&& expandFunctor)
          : SRC{move (parentExplorer)}                   // NOTE: slicing move to strip IterExplorer (Builder)
          , expandRoot_ {_FunTraits<FUN,SRC>    ::adaptFunctor (forward<FUN> (expandFunctor))}  // adapt to accept SRC&
          , expandChild_{_FunTraits<FUN,ResIter>::adaptFunctor (forward<FUN> (expandFunctor))}  // adapt to accept RES&
          , expansions_{}
          { }
        
        
        
        /** core operation: expand current head element */
        void
        expandChildren()
          {
            REQUIRE (this->checkPoint(), "attempt to expand an empty explorer");
            REQUIRE (invariant());
            
            ResIter expanded{ hasChildren()? expandChild_(*expansions_)
                                           : expandRoot_(*this)};
            if (not isnil(expanded))
              expansions_.push (move(expanded)); // note: source of expansion retained
            else
              iterNext();  // expansion unsuccessful, thus consume source immediately
            
            ENSURE (invariant());
          }
        
        /** diagnostics: current level of nested child expansion */
        size_t
        depth()  const
          {
            return expansions_.size();
          }
        
        /** lock into the current child sequence.
         * This special feature turns the current child sequence into the new root,
         * thereby discarding everything else in the expansions stack, including the
         * original root sequence.
         */
        void
        rootCurrent()
          {
            if (not hasChildren()) return;
            static_cast<SRC&> (*this) = move (*expansions_);
            expansions_.clear();
          }
        
        
      public: /* === Iteration control API for IterableDecorator === */
        
        /** @note result type bindings based on a common type of source and expanded result */
        using YieldRes   = typename _Trait::YieldRes;
        using value_type = typename _Trait::value_type;
        using reference  = typename _Trait::reference;
        using pointer    = typename _Trait::pointer;
        
        
        bool
        checkPoint()  const
          {
            ENSURE (invariant());
            
            return hasChildren()
                or SRC::isValid();
          }
        
        YieldRes
        yield()  const
          {
            return hasChildren()? **expansions_
                                : **this;
          }
        
        void
        iterNext()
          {
            incrementCurrent();
            dropExhaustedChildren();
            ENSURE (invariant());
          }
        
      private:
        bool
        invariant()  const
          {
            return not hasChildren()
                or expansions_->isValid();
          }
        
        void
        incrementCurrent()
          {
            if (hasChildren())
              ++(*expansions_);
            else
              ++(*this);
          }
        
        
      protected:
        bool
        hasChildren()  const
          {
            return 0 < depth();
          }
        
        /** @internal accessor for downstream layers to allow close collaboration */
        ResIter&
        accessCurrentChildIter()
          {
            REQUIRE (hasChildren());
            return *expansions_;
          }
        
        void
        dropExhaustedChildren()
          {
            while (not invariant())
              {
                ++expansions_;      // pop expansion stack (to reinstate invariant)
                incrementCurrent(); // consume source of innermost expansion
              }
          }
      };
    
    
    
    /**
     * @internal extension to the Expander decorator to perform expansion automatically on each iteration step.
     * @todo as of 12/2017, this is more like a proof-of concept and can be seen as indication, that there might
     *       be several flavours of child expansion. Unfortunately, most of these conceivable extensions would
     *       require a flexibilisation of Expander's internals and thus increase the complexity of the code.
     *       Thus, if we ever encounter the need of anything beyond the basic expansion pattern, we should
     *       rework the design of Expander and introduce building blocks to define the evaluation strategy.
     */
    template<class SRC>
    class AutoExpander
      : public SRC
      {
        static_assert(is_StateCore<SRC>::value, "need wrapped state core as predecessor in pipeline");
        
      public:
        /** pass through ctor */
        using SRC::SRC;
        
        void
        iterNext()
          {
            SRC::__throw_if_empty();
            SRC::expandChildren();
          }
      };
    
    
    
    /**
     * @internal extension to the Expander decorator to perform expansion delayed on next iteration.
     */
    template<class SRC>
    class ScheduledExpander
      : public SRC
      {
        static_assert(is_StateCore<SRC>::value, "need wrapped state core as predecessor in pipeline");
        
        bool shallExpand_ = false;
        
      public:
        /** pass through ctor */
        using SRC::SRC;
        
        void
        expandChildren()
          {
            shallExpand_ = true;
          }
        
        void
        iterNext()
          {
            if (shallExpand_)
              {
                SRC::__throw_if_empty();
                SRC::expandChildren();
                shallExpand_ = false;
              }
            else
              SRC::iterNext();
          }
      };
    
    
    
    /**
     * @internal Decorator for IterExplorer to map a transformation function on all results.
     * The transformation function is invoked on demand, and only once per item to be treated,
     * storing the treated result into an universal value holder buffer. The given functor
     * is adapted in a similar way as the "expand functor", so to detect and convert the
     * expected input on invocation.
     * @note the result-type of the #yield() function _must be_ `reference`, even when
     *       the TransformFunctor produces a value; otherwise we can not provide a safe
     *       `operator->` on any iterator downstream. This is also the reason why the
     *       ItemWrapper is necessary, precisely _because we want to support_ functions
     *       producing a value; it provides a safe location for this value to persist.
     * @warning handling a transformer function which exposes references can be dangerous.
     *       For this reason, Transformer attempts to »dis-engage« on each copy / assignment,
     *       in order to provoke a re-invocation of the transformer function, which hopefully
     *       picks up references to the copied / moved / swapped location. Be careful though!
     */
    template<class SRC, class RES>
    class Transformer
      : public SRC
      {
        static_assert(can_IterForEach<SRC>::value, "Lumiera Iterator required as source");
        
        using TransformFunctor = function<RES(SRC&)>;
        using TransformedItem = wrapper::ItemWrapper<RES>;
        
        TransformFunctor trafo_;
        TransformedItem treated_;
        
      public:
        using value_type = typename meta::ValueTypeBinding<RES>::value_type;
        using reference  = typename meta::ValueTypeBinding<RES>::reference;
        using pointer    = typename meta::ValueTypeBinding<RES>::pointer;
        
        
        template<typename FUN>
        Transformer (SRC&& dataSrc, FUN&& transformFunctor)
          : SRC{move (dataSrc)}                            // NOTE: slicing move to strip IterExplorer (Builder)
          , trafo_{_FunTraits<FUN,SRC>::adaptFunctor (forward<FUN> (transformFunctor))}
          { }
        
        Transformer() =default;
        Transformer (Transformer const& o)
          : SRC{o}
          , trafo_{o.trafo_}
          , treated_{/* deliberately empty: force re-engage */}
          { }
        Transformer (Transformer && o)
          : SRC{move (o)}
          , trafo_{move (o.trafo_)}
          , treated_{/* deliberately empty: force re-engage */}
          { }
        Transformer&
        operator= (Transformer changed)
          {
            swap (*this,changed);
            return *this;
          }
        friend void
        swap (Transformer& t1, Transformer& t2)
          {
            using std::swap;
            t1.treated_.reset();
            t2.treated_.reset();
            swap (t1.trafo_, t2.trafo_);
          }
        
        
        /** refresh state when other layers manipulate the source sequence
         * @remark expansion replaces the current element by a sequence of
         *         "child" elements. Since we cache our transformation, we
         *         need to ensure possibly new source elements get processed
         */
        void
        expandChildren()
          {
            treated_.reset();
            SRC::expandChildren();
          }
        
      public: /* === Iteration control API for IterableDecorator === */
        
        bool
        checkPoint()  const
          {
            return bool(srcIter());
          }
        
        reference
        yield()  const
          {
            return unConst(this)->invokeTransformation();
          }
        
        void
        iterNext()
          {
            ++ srcIter();
            treated_.reset();
          }
        
      private:
        SRC&
        srcIter()  const
          {
            return unConst(*this);
          }
        
        reference
        invokeTransformation ()
          {
            if (not treated_)  // invoke transform function once per src item
              treated_ = trafo_(srcIter());
            return *treated_;
          }
      };
    
    
    
    /**
     * @internal Decorator for IterExplorer to group consecutive elements into fixed sized chunks.
     * One group of elements is always prepared eagerly, and then the next one on iteration.
     * The group is packaged into a std::array, returning a _reference_ into the internal buffer.
     * If there are leftover elements at the end of the source sequence, which are not sufficient
     * to fill a full group, these can be retrieved through the special API getRestElms(), which
     * returns an iterator.
     */
    template<class SRC, class RES, uint grp>
    class Grouping
      : public SRC
      {
        static_assert(can_IterForEach<SRC>::value, "Lumiera Iterator required as source");
        
      protected:
        using Group = std::array<RES, grp>;
        using Iter  = typename Group::iterator;
        struct Buffer
          : lib::UninitialisedStorage<RES,grp>
          {
            Group& group(){ return *this;          }
            Iter begin()  { return group().begin();}
            Iter end()    { return group().end();  }
          };
        Buffer buff_;
        uint   pos_{0};
        
        
      public:
        using value_type = Group;
        using reference  = Group&;
        using pointer    = Group*;

        Grouping() =default;
        // inherited default copy operations
        
        Grouping (SRC&& dataSrc)
          : SRC{move (dataSrc)}
          {
            pullGroup(); // initially pull to establish the invariant
          }
        
        
        /**
         * Iterate over the Elements in the current group.
         * @return a Lumiera Forward Iterator with value type RES
         */
        auto
        getGroupedElms()
          {
            ENSURE (buff_.begin()+pos_ <= buff_.end());
                   // Array iterators are actually pointers
            return RangeIter{buff_.begin(), buff_.begin()+pos_};
          }
        
        /**
         * Retrieve the tail elements produced by the source,
         * which did not suffice to fill a full group.
         * @remark getRest() is NIL during regular iteration, but
         *         possibly yields elements when checkPoint() = false;
         */
        auto
        getRestElms()
          {
            return checkPoint()? RangeIter<Iter>()
                               : getGroupedElms();
          }
        
        /** refresh state when other layers manipulate the source sequence.
         * @note possibly pulls to re-establish the invariant */
        void
        expandChildren()
          {
            SRC::expandChildren();
            pullGroup();
          }
        
      public: /* === Iteration control API for IterableDecorator === */
        
        bool
        checkPoint()  const
          {
            return pos_ == grp;
          }
        
        reference
        yield()  const
          {
            return unConst(buff_).group();
          }
        
        void
        iterNext()
          {
            pullGroup();
          }
        
        
      protected:
        SRC&
        srcIter()  const
          {
            return unConst(*this);
          }
        
        /** @note establishes the invariant:
         *        source has been consumed to fill a group */
        void
        pullGroup ()
          {
            for (pos_=0
                ; pos_<grp and srcIter()
                ; ++pos_,++srcIter()
                )
              buff_.group()[pos_] = *srcIter();
          }
      };
    
    
    
    /**
     * @internal Decorator for IterExplorer to group consecutive elements controlled by some
     * grouping value \a GRP and compute an aggregate value \a AGG for each such group as
     * iterator yield. The first group is consumed eagerly, each further group on iteration;
     * thus when the aggregate for the last group appears as result, the source iterator has
     * already been exhausted. The aggregate is default-initialised at start of each group
     * and then the computation functor \a FAGG is invoked for each consecutive element marked
     * with the same _grouping value_ — and this grouping value itself is obtained by invoking
     * the functor \a FGRP on each source value. All computation are performed on-the-fly. No
     * capturing or reordering of the source elements takes place, rather groups are formed
     * based on the changes of the grouping value over the source iterator's result sequence.
     * @tparam AGG data type to collect the aggregate; must be default constructible and assignable
     * @tparam GRP value type to indicate a group
     * @note while `groupFun` is adapted, the `aggFun` is _not adapted_ to the source iterator,
     *       but expected always to take the _value type_ of the preceding iterator, i.e. `*srcIter`.
     *       This limitation was deemed acceptable (adapting a function with several arguments would
     *       require quite some nasty technicalities). The first argument of this `aggFun` refers
     *       to the accumulator by value, and thereby also implicitly defines the aggregate result type.
     * @warning the Aggregator \a AGG *must not capture references* to upstream internal state, because
     *       the overall pipeline will be moved into final location _after the initial ctor call._
     */
    template<class SRC, typename AGG, class GRP>
    class GroupAggregator
      : public SRC
      {
        static_assert(can_IterForEach<SRC>::value, "Lumiera Iterator required as source");
        
      protected:
        using SrcValue   = typename meta::ValueTypeBinding<SRC>::value_type;
        using Grouping   = function<GRP(SRC&)>;
        using Aggregator = function<void(AGG&, SrcValue&)>;
        
        std::optional<AGG> agg_{};
        
        Grouping   grouping_;
        Aggregator aggregate_;
        
      public:
        using value_type = typename meta::RefTraits<AGG>::Value;
        using reference  = typename meta::RefTraits<AGG>::Reference;
        using pointer    = typename meta::RefTraits<AGG>::Pointer;

        GroupAggregator() =default;
        // inherited default copy operations
        
        template<class FGRP, class FAGG>
        GroupAggregator (SRC&& dataSrc, FGRP&& groupFun, FAGG&& aggFun)
          : SRC{move (dataSrc)}
          , grouping_{_FunTraits<FGRP,SRC>::adaptFunctor (forward<FGRP> (groupFun))}
          , aggregate_{forward<FAGG> (aggFun)}
          {
            pullGroup(); // initially pull to establish the invariant
          }
        
        
      public: /* === Iteration control API for IterableDecorator === */
        bool
        checkPoint()  const
          {
            return bool(agg_);
          }
        
        reference
        yield()  const
          {
            return *unConst(this)->agg_;
          }
        
        void
        iterNext()
          {
            if (srcIter())
              pullGroup();
            else
              agg_ = std::nullopt;
          }
        
        
      protected:
        SRC&
        srcIter()  const
          {
            return unConst(*this);
          }
        
        /** @note establishes the invariant:
         *        source has been consumed up to the beginning of next group */
        void
        pullGroup()
          {
            GRP group = grouping_(srcIter());
            agg_ = AGG{};
            do{
                aggregate_(*agg_, *srcIter());
                ++ srcIter();
              }
            while (srcIter() and group == grouping_(srcIter()));
          }
      };
    
    
    
    
    /**
     * @internal Decorator for IterExplorer to filter elements based on a predicate.
     * Similar to the Transformer, the given functor is adapted as appropriate. However,
     * we require the functor's result type to be convertible to bool, to serve as approval test.
     * The filter predicate and thus the source iterator is evaluated _eagerly_, to establish the
     * *invariant* of this class: _if a "current element" exists, it has already been approved._
     */
    template<class SRC>
    class Filter
      : public SRC
      {
        static_assert(can_IterForEach<SRC>::value, "Lumiera Iterator required as source");
        
      protected:
        using FilterPredicate = function<bool(SRC&)>;
        
        FilterPredicate predicate_;
        
      public:
        
        Filter() =default;
        // inherited default copy operations
        
        template<typename FUN>
        Filter (SRC&& dataSrc, FUN&& filterFun)
          : SRC{move (dataSrc)}
          , predicate_{_FunTraits<FUN,SRC>::adaptFunctor (forward<FUN> (filterFun))}
          {
            pullFilter(); // initially pull to establish the invariant
          }
        
        
        /** refresh state when other layers manipulate the source sequence.
         * @note possibly pulls to re-establish the invariant */
        void
        expandChildren()
          {
            SRC::expandChildren();
            pullFilter();
          }
        
      public: /* === Iteration control API for IterableDecorator === */
        
        bool
        checkPoint()  const
          {
            return bool(srcIter());
          }
        
        typename SRC::reference
        yield()  const
          {
            return *srcIter();
          }
        
        void
        iterNext()
          {
            ++ srcIter();
            pullFilter();
          }
        
        
      protected:
        SRC&
        srcIter()  const
          {
            return unConst(*this);
          }
        
        bool
        isDisabled()  const
          {
            return not bool{predicate_};
          }
        
        /** @note establishes the invariant:
         *        whatever the source yields as current element,
         *        has already been approved by our predicate */
        void
        pullFilter ()
          {
            if (isDisabled()) return;
            while (srcIter() and not predicate_(srcIter()))
              ++srcIter();
          }
      };
    
    
    
    /**
     * @internal Special variant of the \ref Filter Decorator to allow for dynamic remoulding.
     * This covers a rather specific use case, where we want to remould or even exchange the
     * Filter predicate in the middle of an ongoing iteration. Such a remoulding can be achieved
     * by binding the existing (opaque) filter predicate into a new combined lambda, thereby
     * capturing it _by value._ After building, this remoulded version can be assigned to the
     * original filter functor, under the assumption that both are roughly compatible. Moreover,
     * since we wrap the actual lambda into an adapter, allowing for generic lambdas to be used
     * as filter predicates, this setup allows for a lot of leeway regarding the concrete predicates.
     * @note whenever the filter is remoulded, the invariant is immediately
     *       [re-established](\ref Filter::pullFilter() ), possibly forwarding the sequence
     *       to the next element approved by the new version of the filter.
     * @remarks filter predicates can be specified in a wide variety of forms, and will be adapted
     *       automatically. This flexibility also holds for any of the additional clauses provided
     *       for remoulding the filter. Especially this means that functors of different kinds can
     *       be mixed and combined.
     */
    template<class SRC>
    class MutableFilter
      : public Filter<SRC>
      {
        using _Filter = Filter<SRC>;
        static_assert(can_IterForEach<SRC>::value, "Lumiera Iterator required as source");
        
      public:
        MutableFilter() =default;
        // inherited default copy operations
        
        template<typename FUN>
        MutableFilter (SRC&& dataSrc, FUN&& filterFun)
          : _Filter{move (dataSrc), forward<FUN> (filterFun)}
          { }
        
        
      public: /* === API to Remould the Filter condition underway === */
        
        /** remould existing predicate to require in addition the given clause to hold */
        template<typename COND>
        void
        andFilter (COND&& conjunctiveClause)
          {
            remouldFilter (forward<COND> (conjunctiveClause)
                          ,[](auto first, auto chain)
                             {
                               return [=](auto& val)
                                         {
                                           return first(val)
                                              and chain(val);
                                         };
                             });
          }
        
        /** remould existing predicate to require in addition the negation of the given clause to hold */
        template<typename COND>
        void
        andNotFilter (COND&& conjunctiveClause)
          {
            remouldFilter (forward<COND> (conjunctiveClause)
                          ,[](auto first, auto chain)
                             {
                               return [=](auto& val)
                                         {
                                           return first(val)
                                              and not chain(val);
                                         };
                             });
          }
        
        /** remould existing predicate to require either the old _OR_ the given new clause to hold */
        template<typename COND>
        void
        orFilter (COND&& disjunctiveClause)
          {
            remouldFilter (forward<COND> (disjunctiveClause)
                          ,[](auto first, auto chain)
                             {
                               return [=](auto& val)
                                         {
                                           return first(val)
                                               or chain(val);
                                         };
                             });
          }
        
        /** remould existing predicate to require either the old _OR_ the negation of a new clause to hold */
        template<typename COND>
        void
        orNotFilter (COND&& disjunctiveClause)
          {
            remouldFilter (forward<COND> (disjunctiveClause)
                          ,[](auto first, auto chain)
                             {
                               return [=](auto& val)
                                         {
                                           return first(val)
                                               or not chain(val);
                                         };
                             });
          }
        
        /** remould existing predicate to negate the meaning of the existing clause */
        void
        flipFilter()
          {
            auto dummy = [](auto){ return false; };
            remouldFilter (dummy
                          ,[](auto currentFilter, auto)
                             {
                               return [=](auto& val)
                                         {
                                           return not currentFilter(val);
                                         };
                             });
          }
        
        /** replace the existing predicate with the given, entirely different predicate */
        template<typename COND>
        void
        setNewFilter (COND&& entirelyDifferentPredicate)
          {
            remouldFilter (forward<COND> (entirelyDifferentPredicate)
                          ,[](auto, auto chain)
                             {
                               return [=](auto& val)
                                         {
                                           return chain(val);
                                         };
                             });
          }
        
        /** discard filter predicates and disable any filtering */
        void
        disableFilter()
          {
            _Filter::predicate_ = nullptr;
          }
        
        
      private:
        /** @internal boilerplate to remould the filter predicate in-place
         * @param additionalClause additional functor object to combine
         * @param buildCombinedClause a _generic lambda_ (important!) to define
         *        how exactly the old and the new predicate are to be combined
         * @note the actual combination logic is handed in as generic lambda, which
         *       essentially is a template class, and this allows to bind to any kind
         *       of function objects or lambdas. This combination closure requires a
         *       specific setup: when invoked with the existing and the new functor
         *       as argument, it needs to build a new _likewise generic_ lambda
         *       to perform the combined evaluation.
         * @warning the handed-in lambda `buildCombinedClause` must capture its
         *       arguments, the existing functors _by value._ This is the key piece
         *       in the puzzle, since it effectively moves the existing functor into
         *       a new heap allocated storage.
         */
        template<typename COND, class COMB>
        void
        remouldFilter (COND&& additionalClause, COMB buildCombinedClause)
          {
            static_assert_isPredicate<COND,SRC>();
            
            if (_Filter::isDisabled())
              _Filter::predicate_ = ACCEPT_ALL;
            
            _Filter::predicate_ = buildCombinedClause (_Filter::predicate_   // pick up the existing filter predicate
                                                      ,_FunTraits<COND,SRC>::adaptFunctor (forward<COND> (additionalClause))
                                                      );                   //   wrap the extension predicate in a similar way
            _Filter::pullFilter();                                        //    then pull to re-establish the Invariant
          }
      };                                                                /////////////////////////////////////TICKET #1305 shouldn't we use std::move(_Filter::predicate_) ???
    
    
    
    /**
     * @internal Decorator for IterExplorer to cut iteration once a predicate ceases to be true.
     * Similar to Filter, the given functor is adapted as appropriate, yet is required to yield
     * a bool convertible result. The functor will be evaluated whenever the »exhausted« state
     * of the resulting iterator is checked, on each access and before iteration; this evaluation
     * is not cached (and thus could also detect ongoing state changes by side-effect).
     * @note usually an _exhausted iterator will be abandoned_ — however, since the test is
     *       not cached, the iterator might become active again, if for some reason the
     *       condition becomes true again (e.g. as result of `expandChildern()`)
     */
    template<class SRC>
    class StopTrigger
      : public IterStateCore<SRC>
      {
        static_assert(can_IterForEach<SRC>::value, "Lumiera Iterator required as source");
        
        using Core = IterStateCore<SRC>;
        using Cond = function<bool(SRC&)>;
        
        Cond whileCondition_;
        
      public:
        
        StopTrigger() =default;
        // inherited default copy operations
        
        template<typename FUN>
        StopTrigger (SRC&& dataSrc, FUN&& condition)
          : Core{move (dataSrc)}
          , whileCondition_{_FunTraits<FUN,SRC>::adaptFunctor (forward<FUN> (condition))}
          { }
        
        
        /** adapt the iteration control API for IterableDecorator:
         *  check the stop condition first and block eventually */
        bool
        checkPoint()  const
          {
            return Core::checkPoint()
               and whileCondition_(Core::srcIter());
          }
      };
    
    
    
    
    /**
     * Interface to indicate and expose the ability for _child expansion_.
     * This interface is used when packaging a IterExplorer pipeline opaquely into IterSource.
     * @remark the depth() call indicates the depth of the child expansion tree. This information
     *       can be used by a "downstream" consumer to react according to a nested scope structure.
     * @todo expandChildren() should not return the value pointer.
     *       This is just a workaround to cope with the design mismatch in IterSource;
     *       the fact that latter just passes around a pointer into the implementation is
     *       ugly, dangerous and plain silly.    ////////////////////////////////////////////////////////////TICKET #1125
     *       Incidentally, this is also the sole reason why this interface is templated with `VAL`
     */
    template<typename VAL>
    class ChildExpandableSource
      {
      protected:
         ~ChildExpandableSource() { }    ///< @note mix-in interface, not meant to handle objects
      public:
          virtual VAL* expandChildren()  =0;
          virtual size_t depth()  const  =0;
      };
    
    /**
     * @internal Decorator to package a whole IterExplorer pipeline suitably to be handled through
     * an IterSource based front-end. Such packaging is performed by the IterExplorer::asIterSource()
     * terminal builder function. In addition to [wrapping the iterator](\ref WrappedLumieraIter),
     * the `expandChildren()` operation is exposed as virtual function, to allow invocation through
     * the type-erased front-end, without any knowledge about the concrete implementation type
     * of the wrapped TreeIterator pipeline.
     */
    template<class SRC>
    class PackagedIterExplorerSource
      : public WrappedLumieraIter<SRC>
      , public ChildExpandableSource<typename SRC::value_type>
      {
        using Parent = WrappedLumieraIter<SRC>;
        using Val = typename SRC::value_type;                             ///////////////////////////////////TICKET #1125 : get rid of Val
        
       ~PackagedIterExplorerSource() { }
      public:
        using Parent::Parent;
        
        virtual Val*
        expandChildren()  override
          {
            Parent::wrappedIter().expandChildren();
            return Parent::wrappedIter()?  & *Parent::wrappedIter()       ///////////////////////////////////TICKET #1125 : trickery to cope with the misaligned IterSource design
                                        : nullptr;
          }
        
        virtual size_t
        depth()  const override
          {
            return Parent::wrappedIter().depth();
          }
      };
    
  }//(End)Iterator decorating layer implementation
  
  
  
  
  
  /**
   * Iterator front-end to manage and operate a IterExplorer pipeline opaquely.
   * In addition to the usual iterator functions, this front-end also exposes an
   * `expandChildren()`-function, to activate the _expansion functor_ installed
   * through IterExplorer::expand().
   * @remarks A iterator pipeline is assembled through invocation of the builder functions
   *    on IterExplorer — thereby creating a complex implementation defined iterator type.
   *    This front-end manages such a pipeline in heap allocated storage (by shared_ptr), while
   *    exposing only a simple conventional interface (templated to the resulting value type `VAL`).
   *    This allows to pass it over interfaces as "unspecified data source", without disclosing
   *    the details of the implementation.
   * @warning this lightweight front-end handle in itself is copyable and default constructible,
   *    but any copies will hold onto the same implementation back-end. The effect of competing
   *    manipulations through such copies is _undefined_ (it depends on arbitrary intrinsics of
   *    the implementation). Recommendation is, at any time, to use only one single instance
   *    for iteration and discard it when done.
   */
  template<typename VAL>
  struct IterExploreSource
    : IterSource<VAL>::iterator
    {
      using Expandable = iter_explorer::ChildExpandableSource<VAL>;
      
      
      IterExploreSource()  =default;
      // inherited default copy operations
      
      
      void
      expandChildren()
        {
          VAL* changedResult = expandableSource().expandChildren();
          this->resetPos (changedResult);                                 ///////////////////////////////////TICKET #1125 : trickery to cope with the misaligned IterSource design
        }
        
      size_t
      depth()  const
        {
          return expandableSource().depth();
        }
      
      
    private:
      template<class SRC>
      friend class IterExplorer;
      
      template<class IT>
      IterExploreSource (IT&& opaqueSrcPipeline)
        : IterSource<VAL>::iterator {
            IterSource<VAL>::build (
              new iter_explorer::PackagedIterExplorerSource<IT> {
                move (opaqueSrcPipeline)})}
      { }
      
      Expandable&
      expandableSource()  const
        {
          if (not this->source())
            throw error::State ("operating on a disabled default constructed IterExplorer"
                               ,error::LUMIERA_ERROR_BOTTOM_VALUE);
          
          auto source = unConst(this)->source().get();
          return dynamic_cast<Expandable&> (*source);
        }
    };
  
  
  
  
  
  /* ======= IterExplorer pipeline builder and iterator ======= */
  
  /**
   * Adapter to build a demand-driven tree expanding and exploring computation
   * based on a custom opaque _state core_. TreeExploer adheres to the _Monad_
   * pattern known from functional programming, insofar the _expansion step_ is
   * tied into the basic template by means of a function provided at usage site.
   * This allows to separate the mechanics of evaluation and result combination
   * from the actual processing and thus to define tree structured computations
   * based on an opaque source data structure not further disclosed.
   * @tparam SRC a suitably adapted _source iterator_ or _state core_, wrapped
   *             into an instance of the iter_explorer::BaseAdapter template
   * 
   * \par usage
   * IterExplorer is meant to be used as *Builder* for a processing pipeline.
   * For this to work, it is essential to pick the `SRC` baseclass properly.
   *        - to build a IterExplorer, use the \ref explore() free function,
   *          which cares to pick up and possibly adapt the given iteration source
   *        - to add processing layers, invoke the builder operations on IterExplorer
   *          in a chained fashion, thereby binding functors or lambdas. Capture the
   *          final result with an auto variable.
   *        - the result is iterable in accordance to »Lumiera Forward Iterator«
   * 
   * @warning deliberately, the builder functions exposed on IterExplorer will
   *          _move_ the old object into the new, augmented iterator. This is
   *          possibly dangerous, since one might be tempted to invoke such a
   *          builder function on an existing iterator variable captured by auto.
   * @todo if this turns out as a problem on the long run, we'll need to block
   *          the iterator operations on the builder (by inheriting protected)
   *          and provide an explicit `build()`-function, which removes the
   *          builder API and unleashes or slices down to the iterator instead.
   */
  template<class SRC>
  class IterExplorer
    : public SRC
    {
      static_assert(can_IterForEach<SRC>::value, "Lumiera Iterator required as source");
      
      
    public:
      using value_type = typename meta::ValueTypeBinding<SRC>::value_type;
      using reference  = typename meta::ValueTypeBinding<SRC>::reference;
      using pointer    = typename meta::ValueTypeBinding<SRC>::pointer;
      
      using TAG_IterExplorer_Src = SRC; ///< @internal for \ref _PipelineDetector
      
      /** pass-through ctor */
      using SRC::SRC;
      
      
      
      /* ==== Builder functions ==== */
      
      /** preconfigure this IterExplorer to allow for _»expansion of children«_.
       * The resulting iterator exposes an `expandChildren()` function, which must be
       * invoked explicitly and consumes then the current head element of this iterator
       * and feeds it through the _expansion functor_, which was provided to this builder
       * function here. This _expansion functor_ is expected to yield a compatible sequence
       * of "child" elements, which will be integrated into the overall result sequence
       * instead of the consumed source element. Thus, repeatedly invoking `expand()`
       * until exhaustion generates a _depth-first evaluation_, since every child
       * will be expanded until reaching the leaf nodes of a tree like structure.
       * The result-type of the compound will be  chosen appropriately
       * (which may imply to return by-value instead of by-reference)
       * 
       * @param expandFunctor a "function-like" entity to perform the actual "expansion".
       *        There are two distinct usage patterns, as determined by the signature
       *        of the provided callable, function or functor:
       *        - _"monad style"_: the functor takes a _value_ from the sequence and
       *          produces a new sequence, iterator or collection of compatible values
       *        - _"opaque state manipulation"_: the functor accepts the concrete source
       *          iterator type, or even a "state core" type embedded therein. It yields
       *          a new sequence, state core or collection representing the "children".
       *          Obviously, the intention here is to allow hidden collaboration between
       *          the expansion functor and the embedded opaque "data source". For that
       *          reason, the functor may take its argument by reference, and a produced
       *          new "child state core" may likewise collaborate with that original
       *          data source or state core behind the scenes; the latter is guaranteed
       *          to exist during the whole lifetime of this IterExplorer.
       * @warning be cautions when relying on stored references into the wrapped state core,
       *       because the IterExplorer pipeline as a whole is meant to be movable; either
       *       take those references only after the pipeline is »engaged« and placed at its
       *       final storage location, or ensure a way to „refresh“ this information on move.
       * @note there is limited support for generic lambdas, but only for the second case.
       *       The reason is, we can not "probe" a template or generic lambda for possible
       *       argument and result types. Thus, if you provide a generic lambda, IterExplorer
       *       tries to pass it a `SrcIter &` (reference to the embedded original iterator).
       *       For any other cases, please provide a lambda or functor with a single, explicitly
       *       typed argument. Obviously, argument and result type should also make sense for
       *       the desired evaluation pattern, otherwise you'll get all kinds of nasty
       *       template compilation failures (have fun!)
       * @return processing pipeline with attached [Expander](\ref iter_explorer::Expander) decorator
       */
      template<class FUN>
      auto
      expand (FUN&& expandFunctor)
        {
          using ExpandedChildren = typename iter_explorer::_FunTraits<FUN,SRC>::Res;
          
          using ResCore = iter_explorer::Expander<SRC, ExpandedChildren>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return IterExplorer<ResIter> (ResCore {move(*this), forward<FUN>(expandFunctor)});
        }
      
      
      /** extension functionality to be used on top of expand(), to perform expansion automatically.
       * When configured, child elements will be expanded on each iteration step; it is thus not
       * necessary to invoke `expandChildren()` (and doing so would have no further effect than
       * just iterating). Thus, on iteration, each current element will be fed to the _expand functor_
       * and the results will be integrated depth first.
       * @return processing pipeline with attached [AutoExpander](\ref iter_explorer::AutoExpander) decorator
       * @warning iteration will be infinite, unless the _expand functor_ provides some built-in
       *       termination condition, returning an empty child sequence at that point. This would
       *       then be the signal for the internal combination mechanism to return to visiting the
       *       results of preceding expansion steps, eventually exhausting all data source(s).
       */
      auto
      expandAll()
        {
          using ResCore = iter_explorer::AutoExpander<SRC>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return IterExplorer<ResIter> (ResCore {move(*this)});
        }
      
      /** shortcut notation to invoke \ref expand(expandFunctor) followed by \ref expandAll() */
      template<class FUN>
      auto
      expandAll (FUN&& expandFunctor)
        {
          return this->expand (forward<FUN> (expandFunctor))
                      .expandAll();
        }
      
      
      /** extension functionality to be used on top of expand(), to perform expansion on next iteration.
       * When configured, an expandChildren() call will not happen immediately, but rather in place of
       * the next iteration step. Basically child expansion _is kind of a special iteration step,_ and
       * thus all we need to do is add another layer with a boolean state flag, which catches the
       * expandChildren() and iterNext() calls and redirects appropriately.
       * @warning expandAll and expandOnIteration are not meant to be used at the same time.
       *          Recommendation is to use expandOnIteration() right above (after) the expand()
       *          definition, since interplay with intermingled layers can be complex.
       */
      auto
      expandOnIteration()
        {
          using ResCore = iter_explorer::ScheduledExpander<SRC>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return IterExplorer<ResIter> (ResCore {move(*this)});
        }
      
      
      /** adapt this IterExplorer to pipe each result value through a transformation function.
       * Several "layers" of mapping can be piled on top of each other, possibly mixed with the
       * other types of adaptation, like the child-expanding operation, or a filter. Obviously,
       * when building such processing pipelines, the input and output types of the functors
       * bound into the pipeline need to be compatible or convertible. The transformation
       * functor supports the same definition styles as described for #expand
       * - it can be pure functional, src -> res
       * - it can accept the underlying source iterator and exploit side-effects
       * @return processing pipeline with attached [Transformer](\ref iter_explorer::Transformer) decorator
       */
      template<class FUN>
      auto
      transform (FUN&& transformFunctor)
        {
          using Product = typename iter_explorer::_FunTraits<FUN,SRC>::Res;
          
          using ResCore = iter_explorer::Transformer<SRC, Product>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return IterExplorer<ResIter> (ResCore {move(*this), forward<FUN>(transformFunctor)});
        }
      
      
      /** adapt this IterExplorer to group result elements into fixed size chunks, packaged as std::array.
       * The first group of elements is pulled eagerly at construction, while further groups are formed
       * on consecutive iteration. Iteration ends when no further full group can be formed; this may
       * leave out some leftover elements, which can then be retrieved by iteration through the
       * special API [getRestElms()](\ref iter_explorer::Grouping::getRestElms).
       * @return processing pipeline with attached [Grouping](\ref iter_explorer::Grouping) decorator
       * @warning yields a reference into the internal buffer, changed on next iteration.
       */
      template<uint grp>
      auto
      grouped()
        {
          using Value   = typename meta::ValueTypeBinding<SRC>::value_type;
          using ResCore = iter_explorer::Grouping<SRC, Value, grp>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return IterExplorer<ResIter> (ResCore {move(*this)});
        }
      
      
      /** adapt this IterExplorer to group elements by a custom criterium and aggregate the group members.
       * The first group of elements is pulled eagerly at construction, further groups are formed on each
       * iteration. Aggregation is done by a custom functor, which takes an _aggregator value_ as first
       * argument and the current element (or iterator) as second argument. Downstream, the aggregator
       * value computed for each group is yielded on iteration.
       * @param groupFun a functor to derive a grouping criterium from the source sequence; consecutive elements
       *        yielding the same grouping value will be combined / aggregated
       * @param aggFun a functor to compute contribution to the aggregate value. Signature `void(AGG&, Val&)`,
       *        where the type AGG implicitly also defines the _value_ to use for accumulation and the result value,
       *        while Val must be assignable from the _source value_ provided by the preceding iterator in the pipeline.
       */
      template<class FGRP, class FAGG>
      auto
      groupedBy (FGRP&& groupFun, FAGG&& aggFun)
        {
          using GroupVal = typename iter_explorer::_FunTraits<FGRP,SRC>::Res;
          
          static_assert (meta::is_BinaryFun<FAGG>());
          using ArgType1  = typename _Fun<FAGG>::Args::List::Head;
          using Aggregate = typename meta::RefTraits<ArgType1>::Value;
          
          using ResCore = iter_explorer::GroupAggregator<SRC, Aggregate, GroupVal>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return IterExplorer<ResIter> (ResCore {move(*this)
                                                ,forward<FGRP> (groupFun)
                                                ,forward<FAGG> (aggFun)});
        }
      
      /** simplified grouping to sum / combine all values in a group */
      template<class FGRP>
      auto
      groupedBy (FGRP&& groupFun)
        {
          using Value   = typename meta::ValueTypeBinding<SRC>::value_type;
          return groupedBy (forward<FGRP> (groupFun)
                           ,[](Value& agg, Value const& val){ agg += val; }
                           );
        }
      
      
      /** adapt this IterExplorer to iterate only as long as a condition holds true.
       * @return processing pipeline with attached [stop condition](\ref iter_explorer::StopTrigger)
       */
      template<class FUN>
      auto
      iterWhile (FUN&& whileCond)
        {
          iter_explorer::static_assert_isPredicate<FUN,SRC>();
          
          using ResCore = iter_explorer::StopTrigger<SRC>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return IterExplorer<ResIter> (ResCore {move(*this), forward<FUN>(whileCond)});
        }
      
      
      /** adapt this IterExplorer to iterate until a condition becomes first true.
       * @return processing pipeline with attached [stop condition](\ref iter_explorer::StopTrigger)
       */
      template<class FUN>
      auto
      iterUntil (FUN&& untilCond)
        {
          iter_explorer::static_assert_isPredicate<FUN,SRC>();
          
          using ResCore = iter_explorer::StopTrigger<SRC>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          using ArgType = typename iter_explorer::_FunTraits<FUN,SRC>::Arg;
          
          return IterExplorer<ResIter> (ResCore { move(*this)
                                                ,[whileCond = forward<FUN>(untilCond)](ArgType val)
                                                  {
                                                    return not whileCond(val);
                                                  }
                                                });
        }
      
      
      /** adapt this IterExplorer to filter results, by invoking the given functor to approve them.
       * The previously created source layers will be "pulled" to fast-forward immediately to the
       * next element confirmed this way by the bound functor. If none of the source elements
       * is acceptable, the iterator will transition to exhausted state immediately.
       * @return processing pipeline with attached [Filter](\ref iter_explorer::Filter) decorator
       */
      template<class FUN>
      auto
      filter (FUN&& filterPredicate)
        {
          iter_explorer::static_assert_isPredicate<FUN,SRC>();
          
          using ResCore = iter_explorer::Filter<SRC>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return IterExplorer<ResIter> (ResCore {move(*this), forward<FUN>(filterPredicate)});
        }
      
      
      /** attach a special filter adapter, allowing to change the filter predicate while iterating.
       * While otherwise this filter layer behaves exactly like the [standard version](\ref #filter),
       * it exposes a special API to augment or even completely switch the filter predicate while
       * in the middle of iterator evaluation. Of course, the underlying iterator is not re-evaluated
       * from start (our iterators can not be reset), and so the new filter logic takes effect starting
       * from the current element. Whenever the filter is remoulded, it is immediately re-evaluated,
       * possibly causing the underlying iterator to be pulled until an element matching the condition
       * is found.
       * @return processing pipeline with attached [special MutableFilter](\ref iter_explorer::MutableFilter) decorator
       * @see \ref IterIterExplorer_test::verify_FilterChanges()
       * @see \ref iter_explorer::MutableFilter::andFilter()
       * @see \ref iter_explorer::MutableFilter::andNotFilter()
       * @see \ref iter_explorer::MutableFilter::orFilter()
       * @see \ref iter_explorer::MutableFilter::orNotFilter()
       * @see \ref iter_explorer::MutableFilter::flipFilter()
       * @see \ref iter_explorer::MutableFilter::setNewFilter()
       */
      template<class FUN>
      auto
      mutableFilter (FUN&& filterPredicate)
        {
          iter_explorer::static_assert_isPredicate<FUN,SRC>();
          
          using ResCore = iter_explorer::MutableFilter<SRC>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return IterExplorer<ResIter> (ResCore {move(*this), forward<FUN>(filterPredicate)});
        }
      
      
      auto
      mutableFilter()
        {
          return mutableFilter (iter_explorer::ACCEPT_ALL);
        }
      
      
      
      /** builder function to attach a _custom extension layer._
       * Any template in compliance with the general construction scheme can be injected through the template parameter.
       * - it must take a first template parameter SRC and inherit from this source iterator
       * - towards layers on top, it must behave like a _state core,_ either by redefining the state core API functions,
       *   as defined by \ref IterStateWrapper, or by inheriting them from a lower layer.
       * - it is bound to play well with the other layers; especially it needs to be aware of `expandChildren()` calls,
       *   which for the consumer side behave like `iterNext()` calls. If a layer needs to do something special for
       *   `iterNext()`, it needs to perform a similar action for `expandChildren()`.
       * - it must allow to be handled as default-constructible, copyable value object
       * @return augmented IterExplorer, incorporating and adapting the injected layer
       */
      template<template<class> class LAY>
      auto
      processingLayer()
        {
          using ResCore = LAY<SRC>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return IterExplorer<ResIter> (ResCore {move(*this)});
        }
      
      
      
      /** preconfigured transformer to pass pointers down the pipeline */
      auto
      asPtr()
        {
          using Val = typename meta::ValueTypeBinding<SRC>::value_type;
          static_assert (not std::is_pointer_v<Val>);
          return IterExplorer::transform ([](Val& ref){ return &ref; });
        }
      
      /** preconfigured transformer to dereference pointers into references */
      auto
      derefPtr()
        {
          using Ptr = typename meta::ValueTypeBinding<SRC>::value_type;
          return IterExplorer::transform ([](Ptr ptr){ return *ptr; });
        }
      
      
      /** preconfigured decorator to materialise, sort and deduplicate all source elements.
       * @warning uses heap storage to effuse the source pipeline immediately
       */
      template<template<typename> class SET =std::set>
      auto
      deduplicate()
        {
          using Value   = typename meta::ValueTypeBinding<SRC>::value_type;
          using ResCore = ContainerCore<SET<Value>>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          SET<Value> buffer;
          for (auto& val : *this)
            buffer.emplace (val);
          // »piggiy-back« the collected data into the result iterator
          return IterExplorer<ResIter>{ResCore{move (buffer)}};
        }
      
      
      
      /** _terminal builder_ to package the processing pipeline as IterSource.
       * Invoking this function moves the whole iterator compound, as assembled by the preceding
       * builder calls, into heap allocated memory and returns an [iterator front-end](\ref IterExploreSource).
       * Any iteration and manipulation on that front-end is passed through virtual function calls into
       * the back-end, thereby concealing all details of the processing pipeline.
       * @return an front-end handle object, which is an "Lumiera Forward Iterator",
       *         while holding onto a heap allocated [abstracted data source](\ref lib::IterExplorer).
       */
      IterExploreSource<value_type>
      asIterSource()
        {
          return IterExploreSource<value_type> {move(*this)};
        }
      
      
      /** _terminal builder_ to strip the IterExplorer and expose the built Pipeline.
       * @return a »Lumiera Forward iterator« incorporating the complete pipeline logic.
       */
      SRC
      asIterator()
        {
          return SRC {move(*this)};
        }
      
      /**
       * _terminal builder_ to invoke a functor for side effect on the complete pipeline.
       * @note exhausts and discards the pipeline itself
       */
      template<class FUN>
      void
      foreach (FUN&& consumer)
        {
          auto consumeFun = iter_explorer::_FunTraits<FUN,SRC>::adaptFunctor (forward<FUN> (consumer));
          SRC& pipeline = *this;
          for ( ; pipeline; ++pipeline)
            consumeFun (pipeline);
        }
      
      
      /**
       * _terminal builder_ to sum up or reduce values from the pipeline.
       * In the general case a _fold-left_ operation is performed; default values for the
       * joining operation and the initial value however allow to fall back on summation of values.
       * @param accessor a functor working on the pipeline result values or the iterator
       * @param junctor  (optional) binary operation, joining the sum with the next result of the junctor
       * @param seedVal  (optional) initial value to start accumulation from
       * @return accumulation of all results from the pipeline, combined with the junctor
       */
      template<class FUN
              ,typename COMB =decltype(std::plus<>())
              ,typename VAL  =typename iter_explorer::_ReduceTraits<SRC,FUN>::ResVal>
      VAL
      reduce (FUN&& accessor
             ,COMB junctor  =COMB()
             ,VAL seedVal   =VAL())
        {
          auto accessVal = iter_explorer::_FunTraits<FUN,SRC>::adaptFunctor (forward<FUN> (accessor));
          
          VAL sum{move(seedVal)};
          IterExplorer::foreach ([&](SRC& srcIter){ sum = junctor (sum, accessVal(srcIter)); });
          return sum;
        }
      
      /** simplified _terminal builder_ to [reduce](\ref #reduce) by numeric sum. */
      auto
      resultSum()
        {
          return IterExplorer::reduce ([](const reference val){ return val; });
        }
      
      /** simplified _terminal builder_ to count number of elements from this sequence. */
      size_t
      count()
        {
          return IterExplorer::reduce ([](auto){ return size_t(1); });
        }
      
      /** simplified _terminal builder_ to check if any result yields `true` (short-circuit) */
      bool
      has_any()
        {
          static_assert (std::is_constructible<bool,value_type>());
          SRC& pipeline = *this;
          for ( ; pipeline; ++pipeline)
            if (*pipeline)
              return true;
          return false;
        }
      
      /** simplified _terminal builder_ to check if all results yields `true` (short-circuit) */
      bool
      and_all()
        {
          static_assert (std::is_constructible<bool,value_type>());
          SRC& pipeline = *this;
          for ( ; pipeline; ++pipeline)
            if (not *pipeline)
              return false;
          return true;
        }
      
      
      /** _terminal builder_ to pour and materialise all results from this Pipeline.
       * @tparam CON a STL compliant container to store generated values (defaults to `vector`)
       * @return new instance of the target container, filled with all values
       *         pulled from this Pipeline until exhaustion.
       */
      template<template<typename> class CON =std::vector>
      auto
      effuse()
        {
          CON<value_type> con{};
          this->effuse (con);
          return con;
        }
      
      template<class CON>
      auto
      effuse (CON&& sink) -> CON
        {
          CON con{move(sink)};
          this->effuse (con);
          return con;
        }
      
      /** _terminal builder to fill an existing container with all results from this Pipeline */
      template<class CON>
      void
      effuse (CON& con)
        {
          for (auto& val : *this)
            con.push_back (val);
        }
    };
  
  
  namespace {// internal logic to pick suitable pipeline adapters...
    
    /** Detect or otherwise add BaseAdapter.
     * @remark in addition to just iteration, IterExplorer uses an internal wiring for some
     *   of the additional processing layers, which works by calling to the base class.
     *   When building a new pipeline, a BaseAdapter is added on top of the raw iterator
     *   to absorb these internal calls (and do nothing); however, if the raw iterator
     *   in fact is already a pipeline built by IterExplorer, than no BaseAdapter is
     *   required and rather both pipelines can be connected together
     */
    template<class SRC, class SEL =void>
    struct _BaseDetector
      {
        using BaseAdapter = iter_explorer::BaseAdapter<SRC>;
      };
    
    template<class SRC>                // used when type tag is present on some baseclass
    struct _BaseDetector<SRC, std::void_t<typename SRC::TAG_IterExplorer_BaseAdapter> >
      {
        using BaseAdapter = SRC;
      };
    
    
    /** Detect and remove typical adapter layers added by a preceding IterExplorer usage */
    template<class SRC, class SEL =void>
    struct _UnstripAdapter
      {
        using RawIter = SRC;
      };
    
    template<class COR>                // used when actually a CheckedCore was attached
    struct _UnstripAdapter<COR, std::void_t<typename COR::TAG_CheckedCore_Raw> >
      {
        using RawIter = typename COR::TAG_CheckedCore_Raw;
      };
    
    
    /** Detect if given source was already built by IterExplorer;
     * @remark when building on-top of an existing pipeline, some adapter can be stripped
     *   to simplify the type hierarchy; for this purpose, IterExplorer places a tag type
     *   into its own products, which point to a suitable source type below the adapter.
     */
    template<class SRC, class SEL =void>
    struct _PipelineDetector
      {
        using RawIter = SRC;
      };
    template<class SRC>
    struct _PipelineDetector<SRC, std::void_t<typename SRC::TAG_IterExplorer_Src> >
      {
        using _SrcIT  = typename SRC::TAG_IterExplorer_Src;
        using RawIter = typename _UnstripAdapter<_SrcIT>::RawIter;
      };
  }//(End)internal adapter logic
  
  
  
  
  
  
  
  /* ==== convenient builder free functions ==== */
  
  /** start building a IterExplorer
   * by suitably wrapping the given iterable source.
   * @return a TreeEplorer, which is an Iterator to yield all the source elements,
   *         but may also be used to build up a complex processing pipeline.
   * @warning if you capture the result of this call by an auto variable,
   *         be sure to understand that invoking any further builder operation on
   *         IterExplorer will invalidate that variable (by moving it into the
   *         augmented iterator returned from such builder call).
   * @todo this framework should be retrofitted to fit in with C++20 pipelines
   * 
   * # Usage
   * 
   * This function starts a *Builder* expression. It picks up the given source,
   * which can be something "sequence-like" or "iterable", and will automatically
   * be wrapped and adapted.
   * - from a STL container, we retrieve a pair of STL iterators (`begin()`, `end()`)
   * - a "Lumiera Forward Iterator" is copied or moved into the wrapper and used as
   *   data source, when pulling results on demand, until exhaustion
   * - a _State Core_ object is copied or moved into the wrapper and adapted to
   *   be iterated as "Lumiera Forward Iterator". Any object with suitable extension
   *   points and behaviour can be used, as explained [here](\ref lib::IterStateWrapper).
   * 
   * The resulting IterExplorer instance can directly be used as "Lumiera Forward Iterator".
   * However, typically you might want to invoke the builder functions to configure further
   * processing steps in a processing pipeline...
   * - to [filter](\ref IterExplorer::filter) the results with a predicate (functor)
   * - to [transform](\ref IterExplorer::transform) the yielded results
   * - to bind and configure a [child expansion operation](\ref IterExplorer::expand), which
   *   can then be triggered by explicitly invoking [.expandChildren()](\ref iter_explorer::Expander::expandChildren)
   *   on the resulting pipeline; the generated child sequence is "flat mapped" into the results
   *   (a *Monad* style usage).
   * - to [package](\ref IterExplorer::asIterSource) the pipeline built thus far behind an opaque
   *   interface and move the implementation into heap memory.
   * 
   * A typical usage might be...
   * \code
   * auto multiply  = [](int v){ return 2*v; };
   * 
   * auto ii = explore (CountDown{7,4})
   *             .transform(multiply);
   * 
   * CHECK (14 == *ii);
   * ++ii;
   * CHECK (12 == *ii);
   * ++ii;
   * \endcode
   * In this example, a `CountDown` _state core_ is used, as defined in iter-explorer-test.cpp
   */
  template<class IT>
  inline auto
  explore (IT&& srcSeq)
  {
    using RawIter = typename _PipelineDetector<IT>::RawIter;        // possibly strip an underlying IterExplorer
    using SrcIter = typename _DecoratorTraits<RawIter>::SrcIter;    // then decide how to adapt the source / iterator
    using Base    = typename _BaseDetector<SrcIter>::BaseAdapter;   // detect if a BaseAdapter exists or must be added
    
    return IterExplorer<Base> (std::forward<IT> (srcSeq));
  }
  
  
} // namespace lib
#endif /* LIB_ITER_EXPLORER_H */
