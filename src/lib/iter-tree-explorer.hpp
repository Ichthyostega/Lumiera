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
 ** \par Rationale
 ** The primary benefit of using the monad pattern is to separate the transforming operation completely from
 ** the mechanics of applying that operation and combining the results. More specifically, we rely on an iterator
 ** to represent an abstracted source of data and we expose the combined and transformed results again as such
 ** an abstracted data sequence. The transformation to apply can be selected at runtime (as a functor), and
 ** also the logic how to combine elements can be implemented elsewhere. The monad pattern defines a sane
 ** way of representing this partial evaluation state without requiring a container for intermediary
 ** results. This is especially helpful when
 ** - a flexible and unspecific source data structure needs to be processed
 ** - and this evaluation needs to be done asynchronously and in parallel (no locking, immutable data)
 ** - and a partial evaluation needs to be stored as continuation (not relying on the stack for partial results) 
 ** 
 ** \par preconfigured solutions
 ** This header provides some preconfigured applications of this pattern
 ** - the DefaultCombinator processes the source elements on demand, feeding them through
 **   the given functor and using the resulting iterator to deliver the result elements
 ** - Chained iterator uses similar building blocks just to get the "flattening" of
 **   a sequence of iterators into a single result iterator
 ** - the RecursiveExhaustingEvaluation is another kind of combination strategy,
 **   which recursively evaluates the given function and combines the results
 **   such as to produce classical depth-first and breadth-first search orders.
 ** - the more low-level RecursiveSelfIntegration combinator strategy actually
 **   delegates to the result set iterator implementation to perform the collecting
 **   and re-integrating of intermediary results. This approach is what we actually
 **   use in the proc::engine::Dispatcher
 ** 
 ** Alternatively, just the basic IterExplorer template can be used together with a custom
 ** "combinator strategy" and typically even a specific iterator or sequence to implement very specific
 ** and optimised data structure evaluation patterns. This strategy needs to define some way to hold onto
 ** the original source elements, feed them through the functor on demand and recombine the result sets
 ** into a new sequence to be delivered on demand.
 ** Actually this is what we utilise for the continuous render job generation within the scheduler.
 ** All the other preconfigured variants defined here where created as proof-of-concept, to document
 ** and verify this implementation technique as such.
 ** 
 ** @warning preferably use value semantics for the elements to be processed. Recall, C++ is not
 **      really a functional programming language, and there is no garbage collector. It might be
 **      tempting just to pass pointers through a whole evaluation chain. Indeed, you can do so,
 **      but make sure you understand the precise timing of the evaluation, expansion and
 **      re-integration steps with regards to memory management; an "explorer function"
 **      may pass a reference or pointer to some transient source, which is gone after
 **      incrementing the source iterator.
 ** @see IterExplorer_test
 ** @see iter-adapter.hpp
 ** @see itertools.hpp
 ** @see IterSource (completely opaque iterator)
 **
 */


#ifndef LIB_ITER_TREE_EXPLORER_H
#define LIB_ITER_TREE_EXPLORER_H


#include "lib/error.hpp"
#include "lib/meta/function.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/iter-stack.hpp"
#include "lib/meta/trait.hpp" ////////////////TODO
#include "lib/null-value.hpp" ////////////////TODO
#include "lib/util.hpp"

#include <boost/utility/enable_if.hpp>  ////////////////TODO
#include <stack>                        ////////////////TODO


namespace lib {
  
  namespace iter_explorer {
    
    template<class SRC, class FUN>
    class DefaultCombinator;
  }
  
  
  
  /**
   * Adapter to build a demand-driven tree expanding and exploring computation
   * based on a custom opaque _state core_. TreeExploer adheres to the _Monad_
   * pattern known from functional programming, insofar the _expansion step_ is
   * tied into the basic template by means of a function provided at usage site.
   * 
   * @todo WIP -- preliminary draft as of 11/2017
   */
  template<class SRC
          ,template<class,class> class _COM_ = iter_explorer::DefaultCombinator
          >
  class IterExplorer
    : public IterStateWrapper<typename SRC::value_type, SRC>
    {
      
      
    public:
      typedef typename SRC::value_type value_type;
      typedef typename SRC::reference reference;
      typedef typename SRC::pointer  pointer;
      
      /** Metafunction: the resulting type when binding ("flat mapping")
       *  a functor of type FUN. Basically the result of binding a function
       *  is again an IterExplorer (with an "expanded" state core type) */
      template<class FUN>
      struct FlatMapped
        {
          typedef IterExplorer<_COM_<IterExplorer,FUN>, _COM_> Type;
        };
      
      
      
      /** by default create an empty iterator */
      IterExplorer() { }
      
      
      /** wrap an iterator-like state representation
       *  to build it into a monad. The resulting entity
       *  is both an iterator yielding the elements generated
       *  by the core, and it provides the (monad) bind operator.
       */
      explicit
      IterExplorer (SRC const& iterStateCore)
        : IterStateWrapper<value_type, SRC> (iterStateCore)
        { }
      
      
      /** monad bind ("flat map") operator.
       *  Using a specific function to explore and work
       *  on the "contents" of this IterExplorer, with the goal
       *  to build a new IterExplorer combining the results of this
       *  function application. The enclosing IterExplorer instance
       *  provides a Strategy template _COM_, which defines how those
       *  results are actually to be combined. An instantiation of
       *  this "Combinator" strategy becomes the state core
       *  of the result iterator. 
       */
      template<class FUN>
      typename FlatMapped<FUN>::Type
      operator >>= (FUN explorer)
        {
          typedef _COM_<IterExplorer,FUN>         Combinator;       // instantiation of the combinator strategy
          typedef typename FlatMapped<FUN>::Type ResultsIter;       // result IterExplorer using that instance as state core
          
          return ResultsIter (
                  Combinator (explorer                              // build a new iteration state core
                             ,accessRemainingElements()));          // based on a copy of this iterator / sequence
        }
      
      
      
    private:
      IterExplorer const&
      accessRemainingElements()
        {
          return *this;
        }
    };
  
  
  
  
  
  
  
  
  
  
  namespace iter_explorer { ///< predefined "exploration strategies", policies and configurations
    
    using util::unConst;
    using lib::meta::enable_if;
    using lib::meta::disable_if;
    using std::function;
    using meta::_Fun;

    
    /**
     * Building block: just evaluate source elements.
     * This strategy will be tied into a "Combinator"
     * to hold the actual functor bound into the enclosing
     * IterExplorer monad to work on the contained elements.
     */
    template<class SIG>
    struct ExploreByFunction
      : function<SIG>
      {
        template<typename FUN>
        ExploreByFunction(FUN explorationFunctionDefinition)
          : function<SIG>(explorationFunctionDefinition)
          { }
        
        ExploreByFunction()  { } ///< by default initialised to bottom function
      };
    
    /**
     * Support for a special use case: an Iterator of Iterators, joining results.
     * In this case, already the source produces a sequence of Iterators, which
     * just need to be passed through to the output buffer unaltered. Using this
     * within the DefaultCombinator strategy creates a combined, flattened iterator
     * of all the source iterator's contents.  
     */
    template<class SIG>
    struct UnalteredPassThrough;
    
    template<class IT>
    struct UnalteredPassThrough<IT(IT)>
      {
        IT operator() (IT elm)  const { return elm; }
        bool operator! ()       const { return false; }  ///< identity function is always valid
      };
    
    
    
    /**
     * Building block: evaluate and combine a sequence of iterators.
     * This implementation helper provides two kinds of "buffers" (actually implemented
     * as iterators): A result buffer (iterator) which holds a sequence of already prepared
     * result elements, which can be retrieved through iteration right away. And a supply buffer
     * (iterator) holding raw source elements. When the result buffer is exhausted, the next source
     * element will be pulled from there and fed through the "evaluation strategy", which typically
     * is a function processing the source element and producing a new result buffer (iterator). 
     */
    template<class SRC, class FUN
            ,template<class> class  _EXP_  = ExploreByFunction   ///< Strategy: how to store and evaluate the function to apply on each element
            >
    class CombinedIteratorEvaluation
      {
        typedef typename _Fun<FUN>::Ret     ResultIter;
        typedef typename SRC::value_type    SrcElement;
        typedef _EXP_<ResultIter(SrcElement)> Explorer;
        
        
        SRC         srcSeq_;
        ResultIter results_;
        Explorer  explorer_;
        
      public:
        typedef typename ResultIter::value_type value_type;
        typedef typename ResultIter::reference  reference;
        typedef typename ResultIter::pointer    pointer;
        
        
        CombinedIteratorEvaluation() { }
        
        CombinedIteratorEvaluation(FUN explorerFunction)
          : srcSeq_()
          , results_()
          , explorer_(explorerFunction)
          { }
        
        // using standard copy operations
        
        
        void
        setSourceSequence (SRC const& followUpSourceElements)
          {
            REQUIRE (explorer_);
            srcSeq_ = followUpSourceElements;
          }
        
      private:
        bool
        findNextResultElement()
          {
            while (!results_ && srcSeq_)
              {
                results_ = explorer_(*srcSeq_);
                ++srcSeq_;
              }
            return bool(results_);
          
          }
        /* === Iteration control API for IterStateWrapper== */
        
        friend bool
        checkPoint (CombinedIteratorEvaluation const& seq)
        {
          return unConst(seq).findNextResultElement();
        }
        
        friend reference
        yield (CombinedIteratorEvaluation const& seq)
        {
          return *(seq.results_);
        }
        
        friend void
        iterNext (CombinedIteratorEvaluation & seq)
        {
          ++(seq.results_);
        }
      };
    
    /**
     * a generic "Combinator strategy" for IterExplorer.
     * This default / fallback solution doesn't assume anything beyond the
     * source and the intermediary result(s) to be Lumiera Forward Iterators.
     * @note the implementation stores the functor into a std::function object,
     *       which might cause heap allocations, depending on the function given.
     *       Besides, the implementation holds one instance of the (intermediary)
     *       result iterator (yielded by invoking the function) and a copy of the
     *       original IterExplorer source sequence, to get the further elements
     *       when the initial results are exhausted.
     */
    template<class SRC, class FUN>
    class DefaultCombinator
      : public CombinedIteratorEvaluation<SRC,FUN>
      {
        typedef typename _Fun<FUN>::Ret  ResultIter;
        
      public:
        DefaultCombinator() { }
        
        DefaultCombinator(FUN explorerFunction, SRC const& sourceElements)
          : CombinedIteratorEvaluation<SRC,FUN>(explorerFunction)
          { 
            this->setSourceSequence (sourceElements);
          }
      };
    
    
    /** Metafunction to detect an iterator yielding an iterator sequence */
    template<class IT>
    struct _is_iterator_of_iterators
      {
        typedef typename IT::value_type IteratorElementType;
        
        enum{ value = meta::can_IterForEach<IteratorElementType>::value };
      };
    
    
    template<class ITI, class SEQ>
    class ChainedIteratorImpl
      : public CombinedIteratorEvaluation<ITI, SEQ(SEQ)
                                        , UnalteredPassThrough
                                        >
      { };
    
    
    /**
     * Special iterator configuration for combining / flattening the
     * results of a sequence of iterators. This sequence of source iterators
     * is assumed to be available as "Iterator yielding Iterators".
     * The resulting class is a Lumiera Forward Iterator, delivering all the
     * elements of all source iterators in sequence.
     * @remarks this is quite similar to the IterExplorer monad, but without
     *        binding an exploration function to produce the result sequences.
     *        Rather, the result sequences are directly pulled from the source
     *        sequence, which thus needs to be an "Iterator of Iterators".
     *        Beyond that, the implementation relies on the same building
     *        blocks as used for the full-blown IterExplorer.
     * @param ITI iterator of iterators
     * @param SEQ type of the individual sequence (iterator).
     *            The value_type of this sequence will be the overall
     *            resulting value type of the flattened sequence
     */
    template<class ITI, class SEL = void>
    class ChainedIters;
    
    template<class ITI>
    class ChainedIters<ITI,     enable_if< _is_iterator_of_iterators<ITI>>
                      >
      : public IterStateWrapper<typename ITI::value_type::value_type 
                               ,ChainedIteratorImpl<ITI, typename ITI::value_type>
                               >
      {
      public:
        ChainedIters(ITI const& iteratorOfIterators)
          { //  note: default ctor on parent -> empty sequences
            this->stateCore().setSourceSequence (iteratorOfIterators);
          }
      };
    
    /**
     * Convenience specialisation: manage the sequence of iterators automatically.
     * @note in this case the \em first template parameter denotes the \em element sequence type;
     *       we use a IterStack to hold the sequence-of-iterators in heap storage.
     * @warning this specialisation will not be picked, if the \em value-type
     *       of the given iterator is itself an iterator
     */ 
    template<class SEQ>
    class ChainedIters<SEQ,     disable_if< _is_iterator_of_iterators<SEQ>>
                      > 
      : public IterStateWrapper<typename SEQ::value_type
                               ,ChainedIteratorImpl<IterStack<SEQ>, SEQ>
                               >
      { 
      public:
        typedef IterStack<SEQ> IteratorIterator;
        
        ChainedIters(IteratorIterator const& iteratorOfIterators)
          {
            this->stateCore().setSourceSequence (iteratorOfIterators);
          }
        
        /** empty result sequence by default */
        ChainedIters() { }
      };
    
    
    
    
    
    
    /**
     * A "Combinator strategy" allowing to expand and evaluate a
     * (functional) data structure successively and recursively.
     * Contrary to the DefaultCombinator, here the explorer is evaluated
     * repeatedly, feeding back the results until exhaustion. The concrete
     * exploration function needs to embody some kind of termination condition,
     * e.g. by returning an empty sequence at some point, otherwise infinite
     * recursion might happen. Another consequence of this repeated re-evaluation
     * is the requirement of the source sequence's element type to be compatible
     * to the result sequence's element type -- we can't \em transform the contents
     * of the source sequence into another data type, just explore and expand those
     * contents into sub-sequences based on the same data type. (While this contradicts
     * the full requirements for building a Monad, we can always work around that kind
     * of restriction by producing the element type of the target sequence by implicit
     * type conversion)
     * 
     * \par strategy requirements
     * To build a concrete combinator a special strategy template is required to define
     * the actual implementation logic how to proceed with the evaluation (i.e. how to
     * find the feed of the "next elements" and how to re-integrate the results of an
     * evaluation step into the already expanded sequence of intermediary results.
     * Moreover, this implementation strategy pattern is used as a data buffer
     * to hold those intermediary results. Together, this allows to create
     * various expansion patterns, e.g. depth-first or breadth-first.
     * - \c Strategy::getFeed() accesses the point from where
     *   to pull the next element to be expanded. This function <i>must not</i>
     *   yield an empty sequence, \em unless the overall exploration is exhausted
     * - \c Strategy::feedBack() re-integrates the results of an expansion step
     * 
     * @warning beware, when tempted to pass elements by reference (or pointer)
     *          through the explorer function, make sure you really understand the
     *          working mode of the #iterate function with regards to memory management.
     *          When ResultIter attempts just to store a pointer, after incrementing
     *          \c ++feed(), depending on the internals of the actual src iterator,
     *          this pointer might end up dangling. Recommendation is to let the
     *          Explorer either take arguments or return results by value (copy).
     */
    template<class SRC, class FUN
            ,template<class> class _BUF_
            >
    class RecursiveExhaustingEvaluation
      {
        typedef typename _Fun<FUN>::Ret   ResultIter;
        typedef typename _Fun<FUN>::Sig   Sig;
        typedef function<Sig>             Explorer;
        typedef _BUF_<ResultIter>         Buffer;
        
        Buffer  resultBuf_;
        Explorer  explore_;
        
        
      public:
        typedef typename ResultIter::value_type value_type;
        typedef typename ResultIter::reference reference;
        typedef typename ResultIter::pointer  pointer;
        
        RecursiveExhaustingEvaluation (Explorer fun, SRC const& src)
          : resultBuf_()
          , explore_(fun)
          { 
            resultBuf_.feedBack(
                initEvaluation (src));
          }
        
        RecursiveExhaustingEvaluation() { };
        
        // standard copy operations
        
        
      private:
        /** Extension point: build the initial evaluation state
         * based on the source sequence (typically an IterExplorer).
         * This is a tricky problem, since the source sequence is not
         * necessarily assignment compatible to the ResultIter type
         * and there is no general method to build a ResultIter.
         * The solution is to rely on a "builder trait", which
         * needs to be defined alongside with the concrete
         * ResultIter type. The actual builder trait will
         * be picked up through a free function (ADL). */
        ResultIter
        initEvaluation (SRC const& initialElements)
          {
            ResultIter startSet;
            return build(startSet).usingSequence(initialElements);
          }                                   // extension point: free function build (...)
        
        
        /** @note \c _BUF_::getFeed is required to yield a non-empty sequence,
         * until everything is exhausted. Basically the buffer- and re-integrated
         * result sequences can be expected to be pulled until finding the next
         * non-empty supply. This is considered hidden internal state and thus
         * concealed within this \em const and \em idempotent function.
         */
        ResultIter &
        feed()  const
          {
            return unConst(this)->resultBuf_.getFeed();
          }
        
        void
        iterate ()
          {
            REQUIRE (feed());
            ResultIter nextStep = explore_(*feed());
            ++ feed();
            resultBuf_.feedBack (nextStep);
          }
        
        
        /* === Iteration control API for IterStateWrapper== */
        
        friend bool
        checkPoint (RecursiveExhaustingEvaluation const& seq)
        {
          return bool(seq.feed());
        }
        
        friend reference
        yield (RecursiveExhaustingEvaluation const& seq)
        {
          reference result = *(seq.feed());
          return result;
        }
        
        friend void
        iterNext (RecursiveExhaustingEvaluation & seq)
        {
          seq.iterate();
        }
      };
    
    
    /**
     * Strategy building block for recursive exhausting evaluation.
     * Allows to create depth-fist or breadth-first evaluation patterns, just
     * by using a suitably intermediary storage container to hold the partially
     * evaluated iterators created at each evaluation step. Using a stack and
     * pushing results will create a depth-first pattern, while using a queue
     * will evaluate in layers (breadth-first). In both cases, the next
     * evaluation step will happen at the iterator returned by #getFeed. 
     * @warning uses an empty-iterator marker object to signal exhaustion
     *          - this marker \c IT() may be re-initialised concurrently
     *          - accessing this marker during app shutdown might access
     *            an already defunct object 
     */
    template< class IT
            , template<class> class _QUEUE_   ///< the actual container to use for storage of intermediary results
            >
    class EvaluationBufferStrategy
      {
        _QUEUE_<IT> intermediaryResults_;
        
        
        /** @return default constructed (=empty) iterator
         * @remarks casting away const is safe here, since all
         * you can do with an NIL iterator is to test for emptiness.
         */
        IT &
        emptySequence()
          {
            return unConst(NullValue<IT>::get());
          }                            // unsafe during shutdown 
        
        
      public:
        IT &
        getFeed ()
          {
            // fast forward to find the next non-empty result sequence
            while (intermediaryResults_ && ! *intermediaryResults_)
              ++intermediaryResults_;
            
            if (intermediaryResults_)
              return *intermediaryResults_;
            else
              return emptySequence();
          }
        
        void
        feedBack (IT const& newEvaluationResults)
          {
            intermediaryResults_.insert (newEvaluationResults);
          }
      };
    
    
    
    /**
     * concrete strategy for recursive \em depth-first evaluation.
     * Using heap allocated storage in a STL Deque (used stack-like)
     */
    template<class IT>
    struct DepthFirstEvaluationBuffer
      : EvaluationBufferStrategy<IT, IterStack>
      { };
    
    /**
     * concrete strategy for recursive \em breadth-first evaluation.
     * Using heap allocated storage in a STL Deque (used queue-like)
     */
    template<class IT>
    struct BreadthFirstEvaluationBuffer
      : EvaluationBufferStrategy<IT, IterQueue>
      { };
    
    
    
    /**
     * preconfigured IterExplorer "state core" resulting in
     * depth-first exhaustive evaluation 
     */
    template<class SRC, class FUN>
    struct DepthFirstEvaluationCombinator
      : RecursiveExhaustingEvaluation<SRC, FUN, DepthFirstEvaluationBuffer>
      {
        DepthFirstEvaluationCombinator() { }
        
        DepthFirstEvaluationCombinator(FUN explorerFunction, SRC const& sourceElements)
          : RecursiveExhaustingEvaluation<SRC, FUN, DepthFirstEvaluationBuffer> (explorerFunction,sourceElements)
          {  }
      };
    
    /**
     * preconfigured IterExplorer "state core" resulting in
     * breadth-first exhaustive evaluation 
     */
    template<class SRC, class FUN>
    struct BreadthFirstEvaluationCombinator
      : RecursiveExhaustingEvaluation<SRC, FUN, BreadthFirstEvaluationBuffer>
      {
        BreadthFirstEvaluationCombinator() { }
        
        BreadthFirstEvaluationCombinator(FUN explorerFunction, SRC const& sourceElements)
          : RecursiveExhaustingEvaluation<SRC, FUN, BreadthFirstEvaluationBuffer> (explorerFunction,sourceElements)
          {  }
      };
    
    
    
    
    /** 
     * IterExplorer "state core" for progressively expanding
     * an initial result set. This initial set can be conceived to hold the seed
     * or starting points of evaluation. Elements are consumed by an iterator, at
     * the front. Each element is fed to the "explorer function". This exploration
     * returns an expanded result sequence, which is immediately integrated into the
     * overall result sequence, followed by further exploration of the then-to-be first
     * element of the result sequence. All this exploration is driven on-demand, by
     * consuming the result sequence. Exploration will proceed until exhaustion,
     * in which case the exploration function will yield an empty result set.
     * 
     * This strategy is intended for use with the IterExplorer -- most prominently
     * in use for discovering render prerequisites and creating new render jobs for
     * the engine. The RecursiveSelfIntegration  strategy is a partially reduced
     * and hard-coded variation on the #RecursiveExhaustingEvaluation in depth-first
     * configuration.
     * This setup works in conjunction with a <i>special result sequence</i> type,
     * with the ability to re-integrate results yielded by partial evaluation.
     * But the working pattern is more similar to the #CombinedIteratorEvaluation,
     * where the focal point of further expansion is always at the front end of
     * further items yet to be consumed and expanded; thus the evaluation is
     * bound to proceed depth-first (i.e. it is \em not possible to "integrate"
     * any intermediary result with the \em whole result set, only with the part
     * reachable immediately at the evaluation front)
     * 
     * \par usage considerations
     * There needs to be a specific sequence or iterator type, which is used to
     * hold the result set(s). This custom type together with the Explorer function
     * are performing the actual expansion and re-integration steps. The latter is
     * accessed through the free function \c build(sequence) -- which is expected
     * to return a "builder trait" for manipulating the element set yielded by
     * the custom iterator type returned by the Explorer function.
     * 
     * @param SRC the initial result set sequence; this iterator needs to yield
     *            values of the special ResultIter type, which are then expanded
     *            until exhaustion by repeated calls to the Explorer function
     * @param FUN the Explorer function of type ResultIter -> ResultIter
     * @note the ResultIter type is defined implicitly through the result type
     *            of the Explorer function. Similarly the result value type
     *            is defined through the typedef \c ResultIter::value_type
     * @warning beware of dangling references; make sure you never pass a
     *            reference or pointer through the Explorer function unaltered. 
     *            Because some source iterator might expose a reference to a
     *            transient object just for the purpose of expanding it.
     *            The very reason of building iterator pipelines is to
     *            avoid heap allocation and to produce intermediaries
     *            on demand. So make sure in such a case that there is
     *            at least one real copy operation in the pipeline.
     */
    template<class SRC, class FUN>
    class RecursiveSelfIntegration
      {
          typedef typename _Fun<FUN>::Ret   ResultIter;
          typedef typename _Fun<FUN>::Sig   Sig;
          typedef typename SRC::value_type  Val;
          typedef function<Sig>             Explorer;
          
          SRC        srcSeq_;
          ResultIter outSeq_;
          Explorer  explore_;
          
        public:
          typedef typename ResultIter::value_type value_type;
          typedef typename ResultIter::reference reference;
          typedef typename ResultIter::pointer  pointer;
          
          RecursiveSelfIntegration (Explorer fun, SRC const& src)
            : srcSeq_(src)
            , outSeq_()
            , explore_(fun)
            { }
          
          RecursiveSelfIntegration() { };
          
          // standard copy operations
          
          
        private:
          /** ensure the next elements to be processed
           * will appear at outSeq_ head. When outSeq_
           * is still empty after this function,
           * we're done.
           * @note \em not calling this function after
           *       construction, because the typical user
           *       of this template will do that implicitly
           *       through invocation of #checkPoint */
          bool
          findNextResultElement()
            {
              while (!outSeq_ && srcSeq_)
                {
                  Val& nextElement(*srcSeq_);
                  build(outSeq_).wrapping(nextElement);    // extension point: free function build(...).wrapping(...)
                  ++srcSeq_;
                }
              return bool(outSeq_);
            }
          
          void
          iterate ()
            {
              REQUIRE (outSeq_);
              ResultIter nextSteps = explore_(*outSeq_);
              ++ outSeq_;
              build(outSeq_).usingSequence(nextSteps);     // extension point: free function build(...).usingSequence(...)
            }
          
          
          /* === Iteration control API for IterStateWrapper== */
          
          friend bool
          checkPoint (RecursiveSelfIntegration const& seq)
          {
            return unConst(seq).findNextResultElement();
          }
          
          friend reference
          yield (RecursiveSelfIntegration const& seq)
          {
            return *(seq.outSeq_);
          }
          
          friend void
          iterNext (RecursiveSelfIntegration & seq)
          {
            seq.iterate();
          }
      };
    
    
    
    
    /**
     * Helper template to bootstrap a chain of IterExplorers.
     * This is a "state core", which basically just wraps a given
     * source iterator and provides the necessary free functions
     * (iteration control API) to use this as iteration state
     * within IterExplorer.
     * @note to ease building such an initial version of the Iterator Monad,
     *       use the free function \link #exploreIter \endlink
     */
    template<class IT>
    class WrappedSequence
      : public IT
      {
      public:
        WrappedSequence()
          : IT()
          { }
        
        WrappedSequence(IT const& srcIter)
          : IT(srcIter)
          { }
        
        
        /* === Iteration control API for IterStateWrapper == */
        
        friend bool
        checkPoint (WrappedSequence const& sequence)
        {
          return bool(sequence);
        }
        
        friend typename IT::reference
        yield (WrappedSequence const& sequence)
        {
          return *sequence;
        }
        
        friend void
        iterNext (WrappedSequence & sequence)
        {
          ++sequence;
        }
      };
    
    
    template<class SRC>
    struct DepthFirst
      : IterExplorer<WrappedSequence<SRC>, DepthFirstEvaluationCombinator>
      {
        DepthFirst() { };
        DepthFirst(SRC const& srcSeq)
          : IterExplorer<WrappedSequence<SRC>, DepthFirstEvaluationCombinator> (srcSeq)
          { }
      };
    
    template<class SRC>
    struct BreadthFirst
      : IterExplorer<WrappedSequence<SRC>, BreadthFirstEvaluationCombinator>
      {
        BreadthFirst() { };
        BreadthFirst(SRC const& srcSeq)
          : IterExplorer<WrappedSequence<SRC>, BreadthFirstEvaluationCombinator> (srcSeq)
          { }
      };
    
    
  }//(End) namespace iter_explorer : predefined policies and configurations
  
  
  
  
  /* ==== convenient builder free functions ==== */
  
  template<class IT>
  inline IterExplorer<iter_explorer::WrappedSequence<IT>>
  exploreIter (IT const& srcSeq)
  {
    return IterExplorer<iter_explorer::WrappedSequence<IT>> (srcSeq);
  }
  
  
  template<class IT>
  inline iter_explorer::DepthFirst<IT>
  depthFirst (IT const& srcSeq)
  {
    return iter_explorer::DepthFirst<IT> (srcSeq);
  }
  
  
  template<class IT>
  inline iter_explorer::BreadthFirst<IT>
  breadthFirst (IT const& srcSeq)
  {
    return iter_explorer::BreadthFirst<IT> (srcSeq);
  }
  
  
  
  template<class IT>
  inline iter_explorer::ChainedIters<IT>
  iterChain(IT const& seq)
  {
    typename iter_explorer::ChainedIters<IT>::IteratorIterator sequenceOfIterators;
    
    sequenceOfIterators.push (seq);
    return iter_explorer::ChainedIters<IT>(sequenceOfIterators);
  }
  
  template<class IT>
  inline iter_explorer::ChainedIters<IT>
  iterChain(IT const& seq1, IT const& seq2)
  {
    typename iter_explorer::ChainedIters<IT>::IteratorIterator sequenceOfIterators;
    
    sequenceOfIterators.push (seq2);
    sequenceOfIterators.push (seq1);
    return iter_explorer::ChainedIters<IT>(sequenceOfIterators);
  }
  
  template<class IT>
  inline iter_explorer::ChainedIters<IT>
  iterChain(IT const& seq1, IT const& seq2, IT const& seq3)
  {
    typename iter_explorer::ChainedIters<IT>::IteratorIterator sequenceOfIterators;
    
    sequenceOfIterators.push (seq3);
    sequenceOfIterators.push (seq2);
    sequenceOfIterators.push (seq1);
    return iter_explorer::ChainedIters<IT>(sequenceOfIterators);
  }
  
  template<class IT>
  inline iter_explorer::ChainedIters<IT>
  iterChain(IT const& seq1, IT const& seq2, IT const& seq3, IT const& seq4)
  {
    typename iter_explorer::ChainedIters<IT>::IteratorIterator sequenceOfIterators;
    
    sequenceOfIterators.push (seq4);
    sequenceOfIterators.push (seq3);
    sequenceOfIterators.push (seq2);
    sequenceOfIterators.push (seq1);
    return iter_explorer::ChainedIters<IT>(sequenceOfIterators);
  }
  
  template<class IT>
  inline iter_explorer::ChainedIters<IT>
  iterChain(IT const& seq1, IT const& seq2, IT const& seq3, IT const& seq4, IT const& seq5)
  {
    typename iter_explorer::ChainedIters<IT>::IteratorIterator sequenceOfIterators;
    
    sequenceOfIterators.push (seq5);
    sequenceOfIterators.push (seq4);
    sequenceOfIterators.push (seq3);
    sequenceOfIterators.push (seq2);
    sequenceOfIterators.push (seq1);
    return iter_explorer::ChainedIters<IT>(sequenceOfIterators);
  }
  
  
  
  
} // namespace lib
#endif /* LIB_ITER_TREE_EXPLORER_H */
