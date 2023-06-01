/*
  ITER-TREE-EXPLORER.hpp  -  building blocks for iterator evaluation strategies

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file iter-tree-explorer.hpp
 ** Building tree expanding and backtracking evaluations within hierarchical scopes.
 ** Based on the *Lumiera Forward Iterator* concept and using the basic IterAdaptor templates,
 ** these components allow to implement typical evaluation strategies, like conditional expanding
 ** or depth-first exploration of a hierarchical structure. Since the access to this structure is
 ** abstracted through the underlying iterator, what we effectively get is a functional datastructure.
 ** The implementation is based on the idea of a "state core", which is wrapped right into the iterator
 ** itself (value semantics) -- similar to the IterStateWrapper, which is one of the basic helper templates
 ** provided by iter-adapter.hpp.
 ** 
 ** @remark as of 2017, this template, as well as the initial IterExplorer (draft from 2012) can be
 **         seen as steps towards designing a framework of building blocks for tree expanding and
 **         backtracking algorithms. Due to the nature of Lumiera's design, we repeatedly encounter
 **         this kind of computation pattern, when it comes to matching flexible configuration against
 **         a likewise hierarchical and rules based model. To keep the code base maintainable,
 **         we deem it crucial to reduce the inherent complexity in such algorithms by clearly
 **         separate the _mechanics of evaluation_ from the actual logic of the target domain.
 ** 
 ** # Iterators as Monad
 ** The fundamental idea behind the implementation technique used here is the _Monad pattern_
 ** known from functional programming. A Monad is a container holding some arbitrarily typed base value; the monad can
 ** be seen as "amplifying" and enhancing the contained base value by attaching additional properties or capabilities
 ** This is a rather detached concept with a wide variety of applications (things like IO state, parsers, combinators,
 ** calculations with exception handling but also simple data structures like lists or trees). The key point with any
 ** monad is the ability to _bind a function_ into the monad; this function will work on the _contained base values_
 ** and produce a modified new monad instance. In the simple case of a list, "binding" a function basically means
 ** to _map the function onto_ the elements in the list. (actually it means the `flatMap` operation)
 ** 
 ** ## Rationale
 ** The primary benefit of using the monad pattern is to separate the transforming operation completely from
 ** the mechanics of applying that operation and combining the results. More specifically, we rely on an iterator
 ** to represent an abstracted source of data and we expose the combined and transformed results again as such
 ** an abstracted data sequence. While the transformation to apply can be selected at runtime (as a functor),
 ** the monad pattern defines a sane way to represent partial evaluation state without requiring a container
 ** for intermediary results. This is especially helpful when
 ** - a flexible and unspecific source data structure needs to be processed
 ** - and this evaluation needs to be done asynchronously and in parallel (no locking, immutable data)
 ** - and a partial evaluation needs to be stored as continuation (not relying on the stack for partial results)
 ** 
 ** # A Pipeline builder
 ** Based on such concepts, structures and evaluation patterns, the TreeExplorer serves the purpose to provide
 ** building blocks to assemble a _processing pipeline_, where processing will happen _on demand,_ while iterating.
 ** TreeExplorer itself is both a Lumiera Forward Iterator based on some wrapped data source, and at the same time
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
 ** 
 ** In itself, the TreeExplorer is an iterator with implementation defined type (all operations being inlined).
 ** But it is possible to package this structure behind a conventional iteration interface with virtual functions.
 ** By invoking the terminal builder function TreeExplorer::asIterSource(), the iterator compound type, as created
 ** thus far, will be moved into a heap allocation, returning a front-end based on IterSource. In addition, the
 ** actually returned type, IterExplorerSource, exposes the `expandChildren()` operation as discussed above.
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
 **          move; if you captured it into a variable, be sure to capture the _result_ of the new
 **          builder operation as well and don't use the old variable anymore. Moreover, it should
 **          be ensured that any "state core" used within TreeExplorer has an efficient move ctor;
 **          including RVO, the compiler is typically able to optimise such move calls away altogether.
 ** 
 ** @see IterTreeExplorer_test
 ** @see iter-adapter.hpp
 ** @see itertools.hpp
 ** @see IterSource (completely opaque iterator)
 ** 
 ** @warning as of 4/2023 the alternative Monad-style iterator framework "iter-explorer" will be retracted
 **      and replaced by this design here, which will then be renamed into IterExplorer  //////////////////////////////TICKET #1276
 **
 */


#ifndef LIB_ITER_TREE_EXPLORER_H
#define LIB_ITER_TREE_EXPLORER_H


#include "lib/error.hpp"
#include "lib/meta/duck-detector.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/trait.hpp"
#include "lib/wrapper.hpp"        ////////////TODO : could be more lightweight by splitting FunctionResult into separate header. Relevant?
#include "lib/iter-adapter.hpp"
#include "lib/iter-source.hpp"   /////////////TICKET #493 : only using the IterSource base feature / interface here. Should really split the iter-source.hpp
#include "lib/iter-stack.hpp"
#include "lib/util.hpp"

#include <functional>
#include <utility>


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
     * Decorate a state or logic core to treat it as Lumiera Forward Iterator.
     * This Adapter does essentially the same as \ref IterStateWrapper, but here
     * the state core is not encapsulated opaque, but rather inherited, and thus
     * the full interface of the core remains publicly accessible.
     */
    template<typename T, class COR>
    class IterableDecorator
      : public COR
      {
        COR &      _core()       { return static_cast<COR&>       (*this); }
        COR const& _core() const { return static_cast<COR const&> (*this); }
        
      protected:
        void
        __throw_if_empty()  const
          {
            if (not isValid())
              throw lumiera::error::Invalid ("Can't iterate further",
                    lumiera::error::LUMIERA_ERROR_ITER_EXHAUST);
          }
        
        
      public:
        typedef T* pointer;
        typedef T& reference;
        typedef T  value_type;
        
        /** by default, pass anything down for initialisation of the core.
         * @note especially this allows move-initialisation from an existing core.
         * @remarks to prevent this rule from "eating" the standard copy operations,
         *          and the no-op default ctor, we need to declare them explicitly below.
         */
        template<typename...ARGS>
        IterableDecorator (ARGS&& ...init)
          : COR(std::forward<ARGS>(init)...)
          { }
        
        IterableDecorator()                                     =default;
        IterableDecorator (IterableDecorator&&)                 =default;
        IterableDecorator (IterableDecorator const&)            =default;
        IterableDecorator& operator= (IterableDecorator&&)      =default;
        IterableDecorator& operator= (IterableDecorator const&) =default;
        
        
        /* === lumiera forward iterator concept === */
        
        explicit operator bool() const { return isValid(); }
        
        reference
        operator*() const
          {
            __throw_if_empty();
            return _core().yield ();     // core interface: yield
          }
        
        pointer
        operator->() const
          {
            __throw_if_empty();
            return & _core().yield();    // core interface: yield
          }
        
        IterableDecorator&
        operator++()
          {
            __throw_if_empty();
            _core().iterNext();          // core interface: iterNext
            return *this;
          }
        
        bool
        isValid ()  const
          {
            return _core().checkPoint(); // core interface: checkPoint
          }
        
        bool
        empty ()    const
          {
            return not isValid();
          }
        
        
        
        ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (IterableDecorator);
        
        
        /// Supporting equality comparisons of equivalent iterators (same state core)...
        template<class T1, class T2>
        friend bool
        operator== (IterableDecorator<T1,COR> const& il, IterableDecorator<T2,COR> const& ir)
        {
          return (il.empty()   and ir.empty())
              or (il.isValid() and ir.isValid() and il._core() == ir._core());
        }
        
        template<class T1, class T2>
        friend bool
        operator!= (IterableDecorator<T1,COR> const& il, IterableDecorator<T2,COR> const& ir)
        {
          return not (il == ir);
        }
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
  
  
  
  
  namespace { // TreeExplorer traits
    
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
    using meta::can_IterForEach;
    using meta::can_STL_ForEach;
    using meta::ValueTypeBinding;
    using meta::has_TypeResult;
    
    
    META_DETECT_FUNCTION_ARGLESS(checkPoint);
    META_DETECT_FUNCTION_ARGLESS(iterNext);
    META_DETECT_FUNCTION_ARGLESS(yield);
    
    template<class SRC>
    struct is_StateCore
      : __and_< HasArglessFun_checkPoint<SRC>
              , HasArglessFun_iterNext<SRC>
              , HasArglessFun_yield<SRC>
              >
      { };
    
    template<class SRC>
    struct shall_wrap_STL_Iter
      : __and_<can_STL_ForEach<SRC>
              ,__not_<can_IterForEach<SRC>>
              >
      { };
    
    template<class SRC>
    struct shall_use_StateCore
      : __and_<__not_<can_IterForEach<SRC>>
              ,is_StateCore<SRC>
              >
      { };
    
    template<class SRC>
    struct shall_use_Lumiera_Iter
      : __and_<can_IterForEach<SRC>
              ,__not_<is_StateCore<SRC>>
              >
      { };
    
    
    /** the _value type_ yielded by a »state core« */
    template<class COR>
    struct CoreYield
      {
        using Res = remove_reference_t<decltype(std::declval<COR>().yield())>;
        
        using value_type = typename meta::ValueTypeBinding<Res>::value_type;
        using reference  = typename meta::ValueTypeBinding<Res>::reference;
        using pointer    = typename meta::ValueTypeBinding<Res>::pointer;
      };
    
    
    /** decide how to adapt and embed the source sequence into the resulting TreeExplorer */
    template<class SRC, typename SEL=void>
    struct _DecoratorTraits
      {
        static_assert (!sizeof(SRC), "Can not build TreeExplorer: Unable to figure out how to iterate the given SRC type.");
      };
    
    template<class SRC>
    struct _DecoratorTraits<SRC,   enable_if<is_StateCore<SRC>>>
      {
        using SrcVal  = typename CoreYield<SRC>::value_type;
        using SrcIter = iter_explorer::IterableDecorator<SrcVal, SRC>;
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
     */
    template<class SRC, class RES>
    struct _ExpanderTraits
      {
        using ResIter = typename _DecoratorTraits<RES>::SrcIter;
        using SrcYield = typename ValueTypeBinding<SRC>::value_type;
        using ResYield = typename ValueTypeBinding<ResIter>::value_type;
        static constexpr bool can_reconcile =
           has_TypeResult<common_type<SrcYield,ResYield>>();
        
        static_assert (can_reconcile,
                       "source iterator and result from the expansion must yield compatible values");
        static_assert (is_const_v<SrcYield> == is_const_v<ResYield>,
                       "source and expanded types differ in const-ness");
        
        // NOTE: unfortunately std::common_type decays (strips cv and reference)
        //       in C++20, there would be std::common_reference; for now we have to work around that
        using CommonType = conditional_t<is_const_v<SrcYield> or is_const_v<ResYield>
                                        , const common_type_t<SrcYield,ResYield>
                                        ,       common_type_t<SrcYield,ResYield>
                                        >;
        using value_type = typename ValueTypeBinding<CommonType>::value_type;
        using reference  = typename ValueTypeBinding<CommonType>::reference;
        using pointer    = typename ValueTypeBinding<CommonType>::pointer;
      };
    
  }//(End) TreeExplorer traits
  
  
  
  
  
  namespace iter_explorer { // Implementation of Iterator decorating layers...
    
    constexpr auto ACCEPT_ALL = [](auto){return true;};
    
    /**
     * @internal technical details of binding a functor into the TreeExplorer.
     * Notably, this happens when adapting an _"expansion functor"_ to allow expanding a given element
     * from the TreeExploer (iterator) into a sequence of child elements. A quite similar situation
     * arises when binding a _transformation function_ to be mapped onto each result element.
     * 
     * The TreeExplorer::expand() operation accepts various flavours of functors, and depending on
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
     * @tparam SRC (optional) but need to specify the source iterator type to apply when passing
     *             a generic lambda or template as FUN. Such a generic functor will be _instantiated_
     *             passing the type `SRC&` as argument. This instantiation may fail (and abort compilation),
     *             but when it succeeds, we can infer the result type `Res` from the generic lambda
     */
    template<class FUN, typename SRC =void>
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
        struct ArgAdapter<IT,   enable_if<__and_<is_convertible<typename IT::reference, Arg>
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
     * @internal Base of pipe processing decorator chain.
     * TreeExplorer allows to create a stack out of various decorating processors
     * - each decorator is itself a _"state core"_, adding some on-demand processing
     * - each wraps and adapts a source iterator, attaching to and passing-on the iteration logic
     * Typically each such layer is configured with actual functionality provided as lambda or functor.
     * Yet in addition to forming an iteration pipeline, there is kind of an internal interconnection
     * protocol, allowing the layers to collaborate; notably this allows to handle an expandChildren()
     * call, where some "expansion layer" consumes the current element and replaces it by an expanded
     * series of new elements. Other layers might need to sync to this operation, and thus it is passed
     * down the chain. For that reason, we need a dedicated BaseAdapter to adsorb such chained calls.
     * @remark when building the TreeExplorer, the to-be-wrapped source is fed down into its place
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
        
        void expandChildren() { }
        size_t depth()  const { return 0; }
      };
    
    
    /**
     * @internal Decorator for TreeExplorer adding the ability to "expand children".
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
     * the source iterator wrapped by this decorator.
     * @remark since we allow a lot of leeway regarding the actual form and definition of the
     *         _expansion functor_, there is a lot of minute technical details, mostly confined
     *         within the _FunTraits traits. For the same reason, we need to prepare two different
     *         bindings of the passed raw functor, one to work on the source sequence, and the other
     *         one to work on the result sequence of a recursive child expansions; these two sequences
     *         need not be implemented in the same way, which simplifies the definition of algorithms.
     * @tparam SRC the wrapped source iterator, typically a TreeExplorer or nested decorator.
     * @tparam FUN the concrete type of the functor passed. Will be dissected to find the signature
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
          : SRC{move (parentExplorer)}                   // NOTE: slicing move to strip TreeExplorer (Builder)
          , expandRoot_ {_FunTraits<FUN,SRC>    ::adaptFunctor (forward<FUN> (expandFunctor))}  // adapt to accept SRC&
          , expandChild_{_FunTraits<FUN,ResIter>::adaptFunctor (forward<FUN> (expandFunctor))}  // adapt to accept RES&
          , expansions_{}
          { }
        
        
        
        /** core operation: expand current head element */
        void
        expandChildren()
          {
            REQUIRE (this->checkPoint(), "attempt to expand an empty explorer");
            
            ResIter expanded{ hasChildren()? expandChild_(*expansions_)
                                           : expandRoot_(*this)};
            incrementCurrent();   // consume current head element (but don't clean-up)
            if (not isnil(expanded))
              expansions_.push (move(expanded));
            else
              dropExhaustedChildren(); // clean-up only here to preserve the logical depth
            
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
        using value_type = typename _Trait::value_type;
        using reference = typename _Trait::reference;
        using pointer = typename _Trait::pointer;
        
        
        bool
        checkPoint()  const
          {
            ENSURE (invariant());
            
            return hasChildren()
                or SRC::isValid();
          }
        
        reference
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
              ++expansions_;
          }
        
        bool
        hasChildren()  const
          {
            return 0 < depth();
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
     * @internal Decorator for TreeExplorer to map a transformation function on all results.
     * The transformation function is invoked on demand, and only once per item to be treated,
     * storing the treated result into an universal value holder buffer. The given functor
     * is adapted in a similar way as the "expand functor", so to detect and convert the
     * expected input on invocation.
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
        
        
        Transformer() =default;
        // inherited default copy operations
        
        template<typename FUN>
        Transformer (SRC&& dataSrc, FUN&& transformFunctor)
          : SRC{move (dataSrc)}                            // NOTE: slicing move to strip TreeExplorer (Builder)
          , trafo_{_FunTraits<FUN,SRC>::adaptFunctor (forward<FUN> (transformFunctor))}
          { }
        
        
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
     * @internal Decorator for TreeExplorer to filter elements based on a predicate.
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
     * @internal Decorator for TreeExplorer to cut iteration once a predicate ceases to be true.
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
     * This interface is used when packaging a TreeExplorer pipeline opaquely into IterSource.
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
     * @internal Decorator to package a whole TreeExplorer pipeline suitably to be handled through
     * an IterSource based front-end. Such packaging is performed by the TreeExplorer::asIterSource()
     * terminal builder function. In addition to [wrapping the iterator](\ref WrappedLumieraIter),
     * the `expandChildren()` operation is exposed as virtual function, to allow invocation through
     * the type-erased front-end, without any knowledge about the concrete implementation type
     * of the wrapped TreeIterator pipeline.
     */
    template<class SRC>
    class PackagedTreeExplorerSource
      : public WrappedLumieraIter<SRC>
      , public ChildExpandableSource<typename SRC::value_type>
      {
        using Parent = WrappedLumieraIter<SRC>;
        using Val = typename SRC::value_type;                             ///////////////////////////////////TICKET #1125 : get rid of Val
        
       ~PackagedTreeExplorerSource() { }
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
   * Iterator front-end to manage and operate a TreeExplorer pipeline opaquely.
   * In addition to the usual iterator functions, this front-end also exposes an
   * `expandChildren()`-function, to activate the _expansion functor_ installed
   * through TreeExplorer::expand().
   * @remarks A iterator pipeline is assembled through invocation of the builder functions
   *    on TreeExplorer -- this way creating a complex implementation defined iterator type.
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
      friend class TreeExplorer;
      
      template<class IT>
      IterExploreSource (IT&& opaqueSrcPipeline)
        : IterSource<VAL>::iterator {
            IterSource<VAL>::build (
              new iter_explorer::PackagedTreeExplorerSource<IT> {
                move (opaqueSrcPipeline)})}
      { }
      
      Expandable&
      expandableSource()  const
        {
          if (not this->source())
            throw error::State ("operating on a disabled default constructed TreeExplorer"
                               ,error::LUMIERA_ERROR_BOTTOM_VALUE);
          
          auto source = unConst(this)->source().get();
          return dynamic_cast<Expandable&> (*source);
        }
    };
  
  
  
  
  
  /* ======= TreeExplorer pipeline builder and iterator ======= */
  
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
   * TreeExplorer is meant to be used as *Builder* for a processing pipeline.
   * For this to work, it is essential to pick the `SRC` baseclass properly.
   *        - to build a TreeExplorer, use the treeExplore() free function,
   *          which cares to pick up and possibly adapt the given iteration source
   *        - to add processing layers, invoke the builder operations on TreeExplorer
   *          in a chained fashion, thereby binding functors or lambdas. Capture the
   *          final result with an auto variable.
   *        - the result is iterable in accordance to »Lumiera Forward Iterator«
   * 
   * @warning deliberately, the builder functions exposed on TreeExplorer will
   *          _move_ the old object into the new, augmented iterator. This is
   *          possibly dangerous, since one might be tempted to invoke such a
   *          builder function on an existing iterator variable captured by auto.
   * @todo if this turns out as a problem on the long run, we'll need to block
   *          the iterator operations on the builder (by inheriting protected)
   *          and provide an explicit `build()`-function, which removes the
   *          builder API and unleashes or slices down to the iterator instead.
   */
  template<class SRC>
  class TreeExplorer
    : public SRC
    {
      static_assert(can_IterForEach<SRC>::value, "Lumiera Iterator required as source");
      
      
    public:
      using value_type = typename meta::ValueTypeBinding<SRC>::value_type;
      using reference  = typename meta::ValueTypeBinding<SRC>::reference;
      using pointer    = typename meta::ValueTypeBinding<SRC>::pointer;
      
      /** pass-through ctor */
      using SRC::SRC;
      
      
      
      /* ==== Builder functions ==== */
      
      /** preconfigure this TreeExplorer to allow for _"expansion of children"_.
       * The resulting iterator exposes an `expandChildren()` function, which consumes
       * the current head element of this iterator and feeds it through the
       * _expansion functor_, which was provided to this builder function here.
       * The _expansion functor_ is expected to yield a sequence of "child" elements,
       * which will be integrated into the overall result sequence instead of the
       * consumed source element. Thus, repeatedly invoking `expand()` until exhaustion
       * generates a _depth-first evaluation_, since every child will be expanded until
       * reaching the leaf nodes of a tree like structure.
       * 
       * @param expandFunctor a "function-like" entity to perform the actual "expansion".
       *        There are two distinct usage patterns, as determined by the signature
       *        of the provided function or functor:
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
       *          to exist during the whole lifetime of this TreeExplorer.
       * @note there is limited support for generic lambdas, but only for the second case.
       *       The reason is, we can not "probe" a template or generic lambda for possible
       *       argument and result types. Thus, if you provide a generic lambda, TreeExplorer
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
          
          return TreeExplorer<ResIter> (ResCore {move(*this), forward<FUN>(expandFunctor)});
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
          
          return TreeExplorer<ResIter> (ResCore {move(*this)});
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
          
          return TreeExplorer<ResIter> (ResCore {move(*this)});
        }
      
      
      /** adapt this TreeExplorer to pipe each result value through a transformation function.
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
          
          return TreeExplorer<ResIter> (ResCore {move(*this), forward<FUN>(transformFunctor)});
        }
      
      
      /** adapt this TreeExplorer to iterate only as long as a condition holds true.
       * @return processing pipeline with attached [stop condition](\ref iter_explorer::StopTrigger)
       */
      template<class FUN>
      auto
      iterWhile (FUN&& whileCond)
        {
          iter_explorer::static_assert_isPredicate<FUN,SRC>();
          
          using ResCore = iter_explorer::StopTrigger<SRC>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return TreeExplorer<ResIter> (ResCore {move(*this), forward<FUN>(whileCond)});
        }
      
      
      /** adapt this TreeExplorer to iterate until a condition becomes first true.
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
          
          return TreeExplorer<ResIter> (ResCore { move(*this)
                                                ,[whileCond = forward<FUN>(untilCond)](ArgType val)
                                                  {
                                                    return not whileCond(val);
                                                  }
                                                });
        }
      
      
      /** adapt this TreeExplorer to filter results, by invoking the given functor to approve them.
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
          
          return TreeExplorer<ResIter> (ResCore {move(*this), forward<FUN>(filterPredicate)});
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
       * @see \ref IterTreeExplorer_test::verify_FilterChanges()
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
          
          return TreeExplorer<ResIter> (ResCore {move(*this), forward<FUN>(filterPredicate)});
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
       * - it must be behave like a default-constructible, copyable value object
       * @return augmented TreeExplorer, incorporating and adapting the injected layer
       */
      template<template<class> class LAY>
      auto
      processingLayer()
        {
          using ResCore = LAY<SRC>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return TreeExplorer<ResIter> (ResCore {move(*this)});
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
      
      
      /** _terminal builder_ to strip the TreeExplorer and expose the built Pipeline.
       * @return a »Lumiera Forward iterator« incorporating the complete pipeline logic.
       */
      SRC
      asIterator()
        {
          return SRC {move(*this)};
        }
    };
  
  
  
  
  
  
  
  
  /* ==== convenient builder free functions ==== */
  
  /** start building a TreeExplorer
   * by suitably wrapping the given iterable source.
   * @return a TreeEplorer, which is an Iterator to yield all the source elements,
   *         but may also be used to build an processing pipeline.
   * @warning if you capture the result of this call by an auto variable,
   *         be sure to understand that invoking any further builder operation on
   *         TreeExplorer will invalidate that variable (by moving it into the
   *         augmented iterator returned from such builder call).
   * 
   * # usage
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
   * The resulting TreeExplorer instance can directly be used as "Lumiera Forward Iterator".
   * However, typically you might want to invoke the builder functions to configure further
   * processing steps in a processing pipeline...
   * - to [filter](\ref TreeExplorer::filter) the results with a predicate (functor)
   * - to [transform](\ref TreeExplorer::transform) the yielded results
   * - to bind and configure a [child expansion operation](\ref TreeExplorer::expand), which
   *   can then be triggered by explicitly invoking [.expandChildren()](\ref iter_explorer::Expander::expandChildren)
   *   on the resulting pipeline; the generated child sequence is "flat mapped" into the results
   *   (a *Monad* style usage).
   * - to [package](\ref TreeExplorer::asIterSource) the pipeline built thus far behind an opaque
   *   interface and move the implementation into heap memory.
   * 
   * A typical usage might be...
   * \code
   * auto multiply  = [](int v){ return 2*v; };
   * 
   * auto ii = treeExplore (CountDown{7,4})
   *             .transform(multiply);
   * 
   * CHECK (14 == *ii);
   * ++ii;
   * CHECK (12 == *ii);
   * ++ii;
   * \endcode
   * In this example, a `CountDown` _state core_ is used, as defined in iter-tree-explorer-test.cpp
   */
  template<class IT>
  inline auto
  treeExplore (IT&& srcSeq)
  {
    using SrcIter = typename _DecoratorTraits<IT>::SrcIter;
    using Base = iter_explorer::BaseAdapter<SrcIter>;
    
    return TreeExplorer<Base> (std::forward<IT> (srcSeq));
  }
  
  /** synonym for #treeExplore.
   * @remark this might become an extension to C++20 pipelines
   */
  template<class IT>
  inline auto
  explore (IT&& srcSeq)
  {
    return treeExplore (std::forward<IT> (srcSeq));
  }
  
  
} // namespace lib
#endif /* LIB_ITER_TREE_EXPLORER_H */
