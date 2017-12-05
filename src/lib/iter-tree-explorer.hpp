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
 ** Based on the <b>Lumiera Forward Iterator</b> concept and using the basic IterAdaptor templates,
 ** these components allow to implement typical evaluation strategies, like e.g. depth-first or
 ** breadth-first exploration of a hierarchical structure. Since the access to this structure is
 ** abstracted through the underlying iterator, what we effectively get is a functional datastructure.
 ** The implementation is based on the IterStateWrapper, which is one of the basic helper templates
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
 ** The fundamental idea behind the implementation technique used here is the \em Monad pattern
 ** known from functional programming. A Monad is a (abstract) container created by using some specific functions.
 ** This is an rather abstract concept with a wide variety of applications (things like IO state, parsers, combinators,
 ** calculations with exception handling but also simple data structures like lists or trees). The key point with any
 ** monad is the ability to \em bind a function into the monad; this function will work on the \em internals of the
 ** monad and produce a modified new monad instance. In the simple case of a list, "binding" a function
 ** basically means to map the function onto the elements in the list.
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
 ** @todo WIP-WIP-WIP initial draft as of 11/2017
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
#include "lib/iter-stack.hpp"
#include "lib/util.hpp"
#include "lib/test/test-helper.hpp"///////////TODO Bug-o

#include <functional>
#include <utility>


namespace lib {
  
  using std::move;
  using std::forward;
  using std::function;
  using util::isnil;
  
  namespace iter_explorer {
    
    template<class CON>
    using iterator = typename meta::Strip<CON>::TypeReferred::iterator;
    template<class CON>
    using const_iterator = typename meta::Strip<CON>::TypeReferred::const_iterator;
    
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
        
        template<typename...ARGS>
        IterableDecorator (ARGS&& ...init)
          : COR(std::forward<ARGS>(init)...)
          { }
        
        IterableDecorator()                                     =default;
        IterableDecorator (IterableDecorator&&)                 =default;
        IterableDecorator (IterableDecorator const&)            =default;
        IterableDecorator& operator= (IterableDecorator&&)      =default;
        IterableDecorator& operator= (IterableDecorator const&) =default;
        
        explicit operator bool() const { return isValid(); }
        
        
        /* === lumiera forward iterator concept === */
        
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
    
  }//(End) namespace iter_explorer : predefined policies and configurations
  
  
  namespace { // TreeExplorer traits
    
    using meta::enable_if;
    using meta::disable_if;
    using meta::Yes_t;
    using meta::No_t;
    using meta::_Fun;
    using std::__and_;
    using std::__not_;
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
     * - moreover, we try to determine, if an explicitly typed functor accepts a value of the
     *   embedded source iterator (this is the "monadic" usage pattern), or if it rather accepts
     *   the iterator or state core itself (the"opaque state manipulation" usage pattern).
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
    struct _BoundFunctorTraits
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
                           "the expansion functor must accept the source iterator or state core as parameter");
            
            static auto build() { return [](ARG& arg) -> ARG& { return arg; }; }
          };
        
        /** adapt to a functor, which accepts the value type of the source sequence ("monadic" usage pattern) */
        template<class IT>
        struct ArgAccessor<IT,   enable_if<is_convertible<typename IT::value_type, Arg>>>
          {
            static auto build() { return [](auto& iter) { return *iter; }; }
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
    
  }//(End) TreeExplorer traits
  
  
  
  
  namespace iter_explorer {
    
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
        using _Traits = _BoundFunctorTraits<FUN,SRC>;
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
        Expander&
        expandChildren()
          {
            REQUIRE (this->checkPoint(), "attempt to expand an empty explorer");
            
            ResIter expanded{ 0 < depth()? expandChildren_(*expansions_)
                                         : expandChildren_(*this)};
            iterNext();         // consume current head element
            if (not isnil(expanded))
              expansions_.push (move(expanded));
            
            return *this;
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
            return 0 < depth()
                or this->isValid();
          }
        
        typename SRC::reference
        yield()  const
          {
            return 0 < depth()? **expansions_
                              : **this;
          }
        
        void
        iterNext()
          {
            if (0 < depth())
              {
                ++(*expansions_);
                while (0 < depth() and not *expansions_)
                  ++expansions_;
              }
            else
              ++(*this);
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
        using _Traits = _BoundFunctorTraits<FUN,SRC>;
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
  }
  
  
  
  
  /**
   * Adapter to build a demand-driven tree expanding and exploring computation
   * based on a custom opaque _state core_. TreeExploer adheres to the _Monad_
   * pattern known from functional programming, insofar the _expansion step_ is
   * tied into the basic template by means of a function provided at usage site.
   * This allows to separate the mechanics of evaluation and result combination
   * from the actual processing and thus to define tree structured computations
   * based on a not further disclosed, opaque source data structure.
   * 
   * @todo WIP -- preliminary draft as of 11/2017
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
      
      
      /** by default create an empty iterator */
      TreeExplorer() { }
      
      // default copy acceptable (unless prohibited by nested state core)
      
      
      /** wrap an iterator-like state representation
       *  to build it into a monad. The resulting entity
       *  is both an iterator yielding the elements generated
       *  by the core, and it provides the (monad) bind operator.
       */
      explicit
      TreeExplorer (SRC iterStateCore)
        : SRC{std::move (iterStateCore)}
        { }
      
      
      
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
      
    private:
    };
  
  
  
  
  
  
  
  
  /* ==== convenient builder free functions ==== */
  
  /** start building a TreeExplorer
   * by suitably wrapping the given iterable source.
   * @return a TreeEplorer, which is an Iterator to yield all the source elements,
   *         but may also be used to build an processing pipeline.
   */
  template<class IT>
  inline auto
  treeExplore (IT&& srcSeq)
  {
    using SrcIter = typename _DecoratorTraits<IT>::SrcIter;
    
    return TreeExplorer<SrcIter> {std::forward<IT>(srcSeq)};
  }
  
  
} // namespace lib
#endif /* LIB_ITER_TREE_EXPLORER_H */
