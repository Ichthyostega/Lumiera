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
 ** to _map the function onto_ the elements in the list.
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
    using std::is_base_of;
    using std::is_convertible;
    using std::remove_reference_t;
    using meta::can_IterForEach;
    using meta::can_STL_ForEach;
    
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
        
        using value_type = typename meta::TypeBinding<Res>::value_type;
        using reference  = typename meta::TypeBinding<Res>::reference;
        using pointer    = typename meta::TypeBinding<Res>::pointer;
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
    
  }//(End) TreeExplorer traits
  
  
  
  
  
  namespace iter_explorer { // Implementation of Iterator decorating layers...
    
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
     * - the resulting, combined functor is stored into a std::function, but wired in a way to
     *   keep the argument-accepting front-end still generic (templated `operator()`). This
     *   special adapter supports the case when the _expansion functor_ yields a child sequence
     *   type different but compatible to the original source sequence embedded in TreeExplorer.
     * @tparam FUN something _"function-like"_ passed as functor to be bound
     * @tparam SRC the source iterator type to apply when attempting to use a generic lambda as functor
     */
    template<class FUN, typename SRC>
    struct _BoundFunctor
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
        struct ArgAccessor
          {
            using FunArgType = remove_reference_t<Arg>;
            static_assert (std::is_convertible<ARG, FunArgType>::value,
                           "the bound functor must accept the source iterator or state core as parameter");
            
            static auto build() { return [](ARG& arg) -> ARG& { return arg; }; }
          };
        
        /** adapt to a functor, which accepts the value type of the source sequence ("monadic" usage pattern) */
        template<class IT>
        struct ArgAccessor<IT,   enable_if<is_convertible<typename IT::value_type, Arg>>>
          {
            static auto build() { return [](auto& iter) { return *iter; }; }
          };
        
        /** adapt to a functor collaborating with an IterSource based iterator pipeline */
        template<class IT>
        struct ArgAccessor<IT,   enable_if<__and_< is_base_of<IterSource<typename IT::value_type>, typename IT::Source>
                                                 , is_base_of<IterSource<typename IT::value_type>, remove_reference_t<Arg>>
                                                 > >>
          {
            using Source = typename IT::Source;
            
            static auto build() { return [](auto& iter) -> Source& { return iter.source(); }; }
          };
        
        
        /** holder for the suitably adapted _expansion functor_ */
        struct Functor
          {
            function<Sig> boundFunction;
            
            template<typename ARG>
            Res
            operator() (ARG& arg)
              {
                auto accessArg = ArgAccessor<ARG>::build();
                
                return boundFunction (accessArg (arg));
              }
          };
      };
    
    
    
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
     *         within BaseAdapter. For that reason, we need to lift the copy ctors of the base.
     *         Just inheriting the base class ctors won't do that, at least not in C++14.
     */
    template<class SRC>
    struct BaseAdapter
      : SRC
      {
        BaseAdapter() = default;
        BaseAdapter(SRC const& src) : SRC(src) { }
        BaseAdapter(SRC && src)     : SRC(src) { }
        
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
     * operation can again invoked on the results, the implementation of such an evaluation
     * requires a stack datastructure, so the nested iterator from each expand() invocation
     * can be pushed to become the new active source for iteration. Thus the primary purpose
     * of this Expander (decorator) is to integrate those "nested child iterators" seamlessly
     * into the overall iteration process; once a child iterator is exhausted, it will be
     * popped and iteration continues with the previous child iterator or finally with
     * the source iterator wrapped by this decorator.
     * @remark since we allow a lot of leeway regarding the actual form and definition of the
     *         _expansion functor_, there is a lot of minute technical details, mostly confined
     *         within the _BoundFunctorTraits.
     * @tparam SRC the wrapped source iterator, typically a TreeExplorer or nested decorator.
     * @tparam FUN the concrete type of the functor passed. Will be dissected to find the signature
     */
    template<class SRC, class FUN>
    class Expander
      : public SRC
      {
        static_assert(can_IterForEach<SRC>::value, "Lumiera Iterator required as source");
        using _Traits = _BoundFunctor<FUN,SRC>;
        using ExpandFunctor = typename _Traits::Functor;
        
        using ResIter = typename _DecoratorTraits<typename _Traits::Res>::SrcIter;
        static_assert (std::is_convertible<typename ResIter::value_type, typename SRC::value_type>::value,
                       "the iterator from the expansion must yield compatible values");
        
        ExpandFunctor expandChildren_;
        IterStack<ResIter> expansions_;
        
      public:
        Expander() =default;
        // inherited default copy operations
        
        Expander (SRC&& parentExplorer, FUN&& expandFunctor)
          : SRC{move (parentExplorer)}                           // NOTE: slicing move to strip TreeExplorer (Builder)
          , expandChildren_{forward<FUN> (expandFunctor)}
          , expansions_{}
          { }
        
        
        /** core operation: expand current head element */
        void
        expandChildren()
          {
            REQUIRE (this->checkPoint(), "attempt to expand an empty explorer");
            
            ResIter expanded{ hasChildren()? expandChildren_(*expansions_)
                                           : expandChildren_(*this)};
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
        
        
      public: /* === Iteration control API for IterableDecorator === */
        
        bool
        checkPoint()  const
          {
            ENSURE (invariant());
            
            return hasChildren()
                or SRC::isValid();
          }
        
        typename SRC::reference
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
                or *expansions_;
          }
        
        bool
        hasChildren()  const
          {
            return 0 < depth();
          }
        
        void
        incrementCurrent()
          {
            if (hasChildren())
              ++(*expansions_);
            else
              ++(*this);
          }
        
        void
        dropExhaustedChildren()
          {
            while (not invariant())
              ++expansions_;
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
    template<class SRC, class FUN>
    class Transformer
      : public SRC
      {
        static_assert(can_IterForEach<SRC>::value, "Lumiera Iterator required as source");
        using _Traits = _BoundFunctor<FUN,SRC>;
        using Res = typename _Traits::Res;
        
        using TransformFunctor = typename _Traits::Functor;
        using TransformedItem = wrapper::ItemWrapper<Res>;
        
        TransformFunctor trafo_;
        TransformedItem treated_;
        
      public:
        using value_type = typename meta::TypeBinding<Res>::value_type;
        using reference  = typename meta::TypeBinding<Res>::reference;
        using pointer    = typename meta::TypeBinding<Res>::pointer;
        
        
        Transformer() =default;
        // inherited default copy operations
        
        Transformer (SRC&& dataSrc, FUN&& transformFunctor)
          : SRC{move (dataSrc)}
          , trafo_{forward<FUN> (transformFunctor)}
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
    template<class SRC, class FUN>
    class Filter
      : public SRC
      {
        static_assert(can_IterForEach<SRC>::value, "Lumiera Iterator required as source");
        using _Traits = _BoundFunctor<FUN,SRC>;
        using Res = typename _Traits::Res;
        static_assert(std::is_constructible<bool, Res>::value, "Functor must be a predicate");
        
        using FilterPredicate = typename _Traits::Functor;
        
        FilterPredicate predicate_;
        
      public:
        
        Filter() =default;
        // inherited default copy operations
        
        Filter (SRC&& dataSrc, FUN&& filterFun)
          : SRC{move (dataSrc)}
          , predicate_{forward<FUN> (filterFun)}
          {
            pullFilter(); // initially pull to establish the invariant
          }
        
        
        /** refresh state when other layers manipulate the source sequence.
         * @note possibly pulls to re-establish the invariant */
        void
        expandChildren()
          {
            pullFilter();
            SRC::expandChildren();
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
        
      private:
        SRC&
        srcIter()  const
          {
            return unConst(*this);
          }
        
        /** @note establishes the invariant:
         *        whatever the source yields as current element,
         *        has already been approved by our predicate */
        void
        pullFilter ()
          {
            while (srcIter() and not predicate_(srcIter()))
              ++srcIter();
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
   * 
   * \param usage
   *        - to build a TreeExplorer, use the treeExplore() free function,
   *          which cares to pick up an possibly adapt the given iteration source
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
      using value_type = typename meta::TypeBinding<SRC>::value_type;
      using reference  = typename meta::TypeBinding<SRC>::reference;
      using pointer    = typename meta::TypeBinding<SRC>::pointer;
      
      /** pass-through ctor */
      using SRC::SRC;
      
      
      
      /* ==== Builder functions ==== */
      
      /** preconfigure this TreeExplorer to allow for _"expansion of children"_.
       * The resulting iterator exposes an `expand()` function, which consumes
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
       */
      template<class FUN>
      auto
      expand (FUN&& expandFunctor)
        {
          using ResCore = iter_explorer::Expander<SRC, FUN>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return TreeExplorer<ResIter> (ResCore {move(*this), forward<FUN>(expandFunctor)});
        }
      
      
      /** extension functionality to be used on top of expand(), to perform expansion automatically.
       * When configured, child elements will be expanded on each iteration step; it is thus not
       * necessary to invoke `expandChildren()` (and doing so would have no further effect than
       * just iterating). Thus, on iteration, each current element will be fed to the _expand functor_
       * and the results will be integrated depth first.
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
       */
      template<class FUN>
      auto
      transform (FUN&& transformFunctor)
        {
          using ResCore = iter_explorer::Transformer<SRC, FUN>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return TreeExplorer<ResIter> (ResCore {move(*this), forward<FUN>(transformFunctor)});
        }
      
      
      /** adapt this TreeExplorer to filter results, by invoking the given functor to approve them.
       * The previously created source layers will be "pulled" to fast-forward immediately to the
       * next element confirmed this way by the bound functor. If none of the source elements
       * is acceptable, the iterator will transition to exhausted state immediately.
       */
      template<class FUN>
      auto
      filter (FUN&& filterPredicate)
        {
          using ResCore = iter_explorer::Filter<SRC, FUN>;
          using ResIter = typename _DecoratorTraits<ResCore>::SrcIter;
          
          return TreeExplorer<ResIter> (ResCore {move(*this), forward<FUN>(filterPredicate)});
        }
      
      
      /** _terminal builder_ to package the processing pipeline as IterSource.
       * Invoking this function moves the whole iterator compound, as assembled by the preceding
       * builder calls, into heap allocated memory and returns a [iterator front-end](\ref IterExploreSource).
       * Any iteration and manipulation on that front-end is passed through virtual function calls into
       * the back-end, thereby concealing all details of the processing pipeline.
       */
      IterExploreSource<value_type>
      asIterSource()
        {
          return IterExploreSource<value_type> {move(*this)};
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
   */
  template<class IT>
  inline auto
  treeExplore (IT&& srcSeq)
  {
    using SrcIter = typename _DecoratorTraits<IT>::SrcIter;
    using Base = iter_explorer::BaseAdapter<SrcIter>;
    
    return TreeExplorer<Base> (std::forward<IT> (srcSeq));
  }
  
  
} // namespace lib
#endif /* LIB_ITER_TREE_EXPLORER_H */