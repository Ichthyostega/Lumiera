/*
  ITER-EXPLORER.hpp  -  building blocks for iterator evaluation strategies 

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file iter-explorer.hpp
 ** Helper template(s) for establishing various evaluation strategies for hierarchical data structures.
 ** Based on the <b>Lumiera Forward Iterator</b> concept and using the basic IterAdaptor templates,
 ** these components allow to implement typical evaluation strategies, like e.g. depth-first or
 ** breadth-first exploration of a hierarchical structure. Since the access to this structure is
 ** abstracted through the underlying iterator, what we effectively get is a functional datastructure.
 ** The implementation is based on the IterStateWrapper, which is one of the basic helper templates
 ** provided by iter-adapter.hpp.
 ** 
 ** \par Iterators as Monad
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
 ** @see IterExplorer_test.cpp
 ** @see iter-adapter.hpp
 ** @see itertools.hpp
 ** @see IterSource (completely opaque iterator)
 **
 */


#ifndef LIB_ITER_EXPLORER_H
#define LIB_ITER_EXPLORER_H


#include "lib/error.hpp"
//#include "lib/bool-checkable.hpp"
#include "lib/meta/function.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/util.hpp"

//#include <boost/type_traits/remove_const.hpp>


namespace lib {

  
  
  namespace { // internal helpers
    
    using std::tr1::function;
    using meta::FunctionSignature;
    
    /**
     * Helper to dissect an arbitrary function signature,
     * irrespective if the parameter is given as function reference,
     * function pointer, member function pointer or functor object.
     * The base case assumes a (language) function reference.
     */
    template<typename SIG>
    struct _Fun
      {
        typedef typename FunctionSignature<function<SIG> >::Ret Ret;
        typedef typename FunctionSignature<function<SIG> >::Args Args;
      };
    /** Specialisation for using a function pointer */
    template<typename SIG>
    struct _Fun<SIG*>
      {
        typedef typename FunctionSignature<function<SIG> >::Ret Ret;
        typedef typename FunctionSignature<function<SIG> >::Args Args;
      };
    /** Specialisation for passing a functor */
    template<typename SIG>
    struct _Fun<function<SIG> >
      {
        typedef typename FunctionSignature<function<SIG> >::Ret Ret;
        typedef typename FunctionSignature<function<SIG> >::Args Args;
      };
    
    
    
    
  }

  namespace iter_explorer {
    
    template<class SRC, class FUN>
    class DefaultCombinator;
  }

  

  /**
   * Adapter for using an Iterator in the way of a Monad
   * This allows to "bind" or "flatMap" a functor, thereby creating
   * a derived version of the iterator, yielding the (flattened) combination
   * of all the results generated by this bound functor. The rationale is
   * to apply some exploration or expansion pattern on the elements of the
   * source sequence (source iterator) -- while completely separating out
   * the \em mechanics how to treat and combine individual elements.
   * 
   * \par implementation approach
   * IterExplorer is a thin wrapper, based on IterStateWrapper; thus the assumption
   * is for the actual elements to be generated by a "state core", which is embedded
   * right into each instance. To provide the monad bind operator \c >>= , we need
   * the help of a strategy template, the so called \em Combinator. This strategy
   * contains the details how to combine the results of various iterators and
   * to join them together into a single new IterExplorer instance. Thus, obviously
   * this Combinator strategy template relies on additional knowledge about the
   * source iterator's implementation. Such is unavoidable, since C++ isn't a
   * functional programming language.
   * 
   * When invoking the bind (flat map) operator, a suitably represented \em functor
   * is embedded into an instance of the Combinator template. Moreover, a copy of
   * the current IterExplorer is embedded alongside. The resulting object becomes
   * the state core of the new IterExplorer object returned as result.
   * So the result \em is an iterator, but -- when "pulled" -- it will in turn
   * pull from the source iterator and feed the provided elements through the
   * \em exploration functor, which this way has been \em bound into the resulting
   * monad. The purpose of the Combinator strategy is to join together various
   * resulting iterators, thereby "flattening" one level of "monad packaging"
   * (hence the name "flat map operator").
   * 
   * @remarks figuring out what happens here just from the code might lead to confusion.
   *          You really need to grasp the monad concept first, which is a design pattern
   *          commonly used in functional programming. For the practical purpose in question
   *          here, you might consider it a "programmable semicolon": it links together a
   *          chain of operations detailed elsewhere, and provided in a dynamic fashion
   *          (as functor at runtime). More specifically, these operations will expand
   *          and explore a dynamic source data set on the fly. Notably this allows
   *          us to handle such an "exploration" in a flexible way, without the need
   *          to allocate heap memory to store intermediary results, but also without
   *          using the stack and recursive programming.
   * @warning be sure to consider the effects of copying iterators on any hidden state
   *          referenced by the source iterator(s). Basically any iterator must behave
   *          sane when copied while in iteration: IterExplorer first evaluates the
   *          head element of the source (the explorer function should build an 
   *          independent, new result sequence based on this first element).
   *          Afterwards, the source is \em advanced and then \em copied 
   *          into the result iterator.
   * @param SRC the source sequence or iterator to wrap
   * @param _COM_ "Combinator" strategy template. When binding (\c >>= ) a function,
   *          an instance of that strategy becomes the new SRC for the created new
   *          IterExplorer. This instantiation of the strategy gets as type parameters
   *          - this IterExplorer's instance type
   *          - the type of the function bound with \c >>= 
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
      IterExplorer<_COM_<IterExplorer,FUN>, _COM_>
      operator >>= (FUN explorer)
        {
          typedef _COM_<IterExplorer,FUN>         Combinator;       // instantiation of the combinator strategy
          typedef IterExplorer<Combinator, _COM_> ResultsIter;      // result IterExplorer using that instance as state core
          
          Combinator combinator(explorer);                          // build a new iteration state core
          if (this->isValid())                                      // if source iterator isn't empty...
            {
              combinator.startWith( explorer(accessHeadElement())); // immediately apply the function to the first element
              combinator.followUp (accessRemainingElements());      // and provide state to allow for continued exploration later
            }
          return ResultsIter (combinator);                                    
        }
      
      
      
    private:
      reference
      accessHeadElement()
        {
          return this->operator* ();
        }
      
      IterExplorer&
      accessRemainingElements()
        {
          this->operator++ ();
          return *this;
        }
    };
      
      
    
    
  namespace iter_explorer { ///< predefined policies and configurations
    
    using util::unConst;
    
    /**
     * Building block: evaluating source elements.
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
        setResultSequence (ResultIter firstExplorationResult)
          {
            results_ = firstExplorationResult;
          }
        
        void
        setSourceSequence (SRC & followUpSourceElements)
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
     * This fallback solution doesn't assume anything beyond the source
     * and the intermediary result(s) being a Lumiera Forward Iterators.
     * @note the implementation stores the functor into a std::function object,
     *       which might cause heap allocations, depending on given function.
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
        
        DefaultCombinator(FUN explorerFunction)
          : CombinedIteratorEvaluation<SRC,FUN>(explorerFunction)
          { }
        
        
        void
        startWith (ResultIter firstExplorationResult)
          {
            this->setResultSequence (firstExplorationResult);
          }
        
        void
        followUp (SRC & followUpSourceElements)
          {
            this->setSourceSequence (followUpSourceElements);
          }
      };
    
    
    /**
     * Special configuration for combining / flattening the results
     * of a sequence of iterators 
     */
    template<class SEQ>
    class ChainedIters
      {
        /////////////TODO unimplemented
      };
    
    /**
     * Helper template to bootstrap a chain of IterExplorers.
     * This is a "state corer", which basically just wraps a given
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
      
  }//(End) namespace iter_explorer : predefined policies and configurations
  
  
  
  
  template<class IT>
  IterExplorer<iter_explorer::WrappedSequence<IT> >
  exploreIter (IT const& srcSeq)
  {
    return IterExplorer<iter_explorer::WrappedSequence<IT> > (srcSeq);
  }
    
    
  
  
  
  
  
  
} // namespace lib
#endif