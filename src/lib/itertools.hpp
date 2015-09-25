/*
  ITERTOOLS.hpp  -  collection of tools for building and combining iterators 

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file itertools.hpp
 ** Helpers for working with iterators based on the pipeline model.
 ** Iterators abstract from the underlying data container and provide
 ** the contained data as a source to pull values from. Based on this
 ** model, we can build pipelines, with filters, valves, junction points
 ** and transforming facilities. The templates in this header enable such,
 ** based on the Lumiera Forward Iterator concept. They build on generic
 ** programming techniques, thus are intended to be combined at compile time
 ** using definitive type information. Contrast this to an iterator model
 ** as in Java's Commons-Collections, where Iterator is an Interface based
 ** on virtual functions. Thus, the basic problem to overcome is the lack
 ** of a single common interface, which could serve as foundation for
 ** type inference. As a solution, we use a "onion" approach, where a
 ** generic base gets configured with an active core, implementing
 ** the filtering or processing functionality, while the base class
 ** (IterTool) exposes the operations necessary to comply to the
 ** Forward Iterator Concept. 
 ** 
 ** \par filtering Iterator
 ** The FilterIter template can be used to build a filter into a pipeline,
 ** as it forwards only those elements from its source iterator, which pass
 ** the predicate evaluation. Anything acceptable as ctor parameter for a
 ** tr1::function object can be passed in as predicate, but of course the
 ** signature must be sensible. Please note, that -- depending on the
 ** predicate -- already the ctor or even a simple \c bool test might
 ** pull and exhaust the source iterator completely, in an attempt
 ** to find the first element passing the predicate test.
 ** 
 ** \par processing Iterator
 ** the TransformIter template can be used as processing (or transforming)
 ** step within the pipeline. It is created with a functor, which, when 
 ** pulling elements, is invoked for each element pulled from the 
 ** source iterator. The signature of the functor must match the
 ** desired value (output) type. 
 ** 
 ** @todo WIP WIP WIP
 ** @todo see Ticket #347
 ** 
 ** @see IterAdapter
 ** @see itertools-test.cpp
 ** @see contents-query.hpp
 */


#ifndef LIB_ITERTOOLS_H
#define LIB_ITERTOOLS_H


#include "lib/bool-checkable.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/trait.hpp"
#include "lib/wrapper.hpp"
#include "lib/util.hpp"

#include <functional>



namespace lib {
  
  using std::function;
  using util::unConst;
  
  using lib::meta::RefTraits;
  
  
  
  /** 
   * A neutral \em identity-function core,
   * also serving as point-of reference how any
   * core is intended to work. Any core is intended
   * to serve as inner part of an iterator tool template.
   * - it provides the trait typedefs
   * - it abstracts the "source"
   * - it abstracts the local operation to be performed
   * - the ctor of the core sets up the configuration.
   * @note cores should be copyable without much overhead
   */
  template<class IT>
  struct IdentityCore
    {
      IT source_;
      
      IdentityCore (IT const& orig)
        : source_(orig)
        { }
      
      IT&
      pipe ()
        {
          return source_;
        }
      
      IT const&
      pipe ()  const
        {
          return source_;
        }
      
      void
      advance ()
        {
          ++source_;
        }
      
      bool
      evaluate () const
        {
          return bool(source_);
        }
      
      typedef typename IT::pointer pointer;
      typedef typename IT::reference reference;
      typedef typename IT::value_type value_type;
    };
  
  
  /**
   * Standard functionality to build up any iterator tool.
   * IterTool exposes the frontend functions necessary to
   * comply to the Lumiera Forward Iterator concept.
   * The protected part provides the building blocks
   * to implement the actual processing/filter logic.
   */
  template<class CORE>
  class IterTool
    : public lib::BoolCheckable<IterTool<CORE> >
    {
      
    protected: /* iteration control */
      CORE core_;
      
      bool
      hasData()  const
        {
          return core_.evaluate()
              || unConst(this)->iterate();
        }        // skipping irrelevant results doesn't count as "mutation"
      
      bool
      iterate ()
        {
          if (!core_.pipe()) return false;
          
          do core_.advance();
          while (core_.pipe() && !core_.evaluate());
          return core_.pipe();
        }
      
      void
      _maybe_throw()  const
        {
          if (!isValid())
            _throwIterExhausted();
        }
      
      
      
    public:
      typedef typename CORE::pointer pointer;
      typedef typename CORE::reference reference;
      typedef typename CORE::value_type value_type;
      
      
      IterTool (CORE const& setup)
        : core_(setup)
        {
          hasData();
        }
      
      
      
      /* === lumiera forward iterator concept === */
      
      reference
      operator*() const
        {
          _maybe_throw();
          return *core_.pipe();
        }
      
      pointer
      operator->() const
        {
          _maybe_throw();
          return & *core_.pipe();
        }
      
      IterTool&
      operator++()
        {
          _maybe_throw();
          iterate();
          return *this;
        }
      
      bool
      isValid ()  const
        {
          return hasData();
        }
      
      bool
      empty ()    const
        {
          return not isValid();
        }
      
    };
  
  
  template<class CX>
  inline bool
  operator== (IterTool<CX> const& it1, IterTool<CX> const& it2)
  {
    return (!it1 && !it2 )
        || ( it1 &&  it2 && (*it1) == (*it2) )
        ;
  }
  
  template<class CX>
  inline bool
  operator!= (IterTool<CX> const& ito1, IterTool<CX> const& ito2)
  {
    return not (ito1 == ito2);
  }
  
  
  
  
  
  
  
  
  /**
   * Implementation of the filter logic.
   * This core stores a function object instance,
   * passing each pulled source element to this
   * predicate function for evaluation.
   * @note predicate is evaluated <i>at most once</i>
   *       for each value yielded by the source
   */
  template<class IT>
  struct FilterCore
    : IdentityCore<IT>
    {
      typedef IdentityCore<IT> Raw;
      typedef typename IT::reference Val;
      
      
      function<bool(Val)> predicate_;
      
      bool
      evaluate () const
        {
          return Raw::pipe()
              && currVal_isOK();
        }
      
      
      mutable bool cached_;
      mutable bool isOK_;
      
      bool
      currVal_isOK () const  ///< @return (maybe cached) result of filter predicate
        {
          return (cached_ && isOK_)
              || (cached_ = true
                 &&(isOK_ = predicate_(*Raw::pipe())));
        }
      
      void
      advance ()
        {
          cached_ = false;
          Raw::advance();
        }
      
      
      template<typename PRED>
      FilterCore (IT const& source, PRED prediDef)
        : Raw(source)
        , predicate_(prediDef) // induces a signature check
        , cached_(false)      //  not yet cached
        , isOK_()            //   some value
        { }
    };
  
  
  /**
   * Iterator tool filtering pulled data according to a predicate 
   */
  template<class IT>
  class FilterIter
    : public IterTool<FilterCore<IT> >
    {
      typedef FilterCore<IT> _Filter;
      typedef IterTool<_Filter> _Impl;
      
      static bool acceptAll(typename _Filter::Val) { return true; }
      
      
    public:
      FilterIter ()
        : _Impl(FilterCore<IT>(IT(), acceptAll))
        { }
      
      template<typename PRED>
      FilterIter (IT const& src, PRED filterPredicate)
        : _Impl(_Filter(src,filterPredicate))
        { }
      
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (FilterIter)
    };
  
  
  /** Build a FilterIter: convenience free function shortcut,
   *  picking up the involved types automatically.
   *  @param  filterPredicate to be invoked for each source element
   *  @return Iterator filtering contents of the source
   */
  template<class IT, typename PRED>
  inline FilterIter<IT>
  filterIterator (IT const& src, PRED filterPredicate)
  {
    return FilterIter<IT>(src,filterPredicate);
  }
  
  
  /** 
   * Helper: predicate returning \c true
   * whenever the argument value changes
   * during a sequence of invocations. 
   */ 
  template<typename VAL>
  class SkipRepetition
    {
      typedef wrapper::ItemWrapper<VAL> Item;
      
      Item prev_;
      
    public:
      bool
      operator() (VAL const& elm)
        {
           if (prev_ &&
              (*prev_ == elm))
             return false;
           
           // element differs from predecessor
           prev_ = elm;
           return true;
        }
      
      typedef bool result_type;
    };
  
  
  
  
  
  
  
  
  
  
  
  /**
   * Implementation of custom processing logic.
   * This core stores a function object instance
   * to treat each source element pulled.
   */
  template<class IT, class VAL>
  class TransformingCore
    {
      typedef typename IT::reference InType;
      typedef wrapper::ItemWrapper<VAL> Item;
      
      function<VAL(InType)> trafo_;
      
      IT   source_;
      Item treated_;
      
      void
      processItem ()
        {
          if (source_)
            treated_ = trafo_(*source_);
          else
            treated_.reset();
        }
      
      
      
    public:
      TransformingCore () ///< deactivated core
        : trafo_()
        , source_()
        , treated_()
        { }
      
      template<typename FUN>
      TransformingCore (IT const& orig, FUN processor)
        : trafo_(processor) // induces a signature check
        , source_(orig)
        {
          processItem();
        }
      
      Item const&
      pipe ()  const
        {
          return treated_;
        }
      
      void
      advance ()
        {
          ++source_;
          processItem();
        }
      
      bool
      evaluate () const
        {
          return bool(source_);
        }
      
      typedef typename RefTraits<VAL>::pointer pointer;
      typedef typename RefTraits<VAL>::reference reference;
      typedef typename RefTraits<VAL>::value_type value_type;
    };
  
  
  /**
   * Iterator tool treating pulled data by a custom transformation (function)
   * @param IT source iterator
   * @param VAL result (output) type
   */
  template<class IT, class VAL>
  class TransformIter
    : public IterTool<TransformingCore<IT,VAL> >
    {
      typedef TransformingCore<IT,VAL> _Trafo;
      typedef IterTool<_Trafo> _IteratorImpl;
      
    public:
      TransformIter ()
        : _IteratorImpl(_Trafo())
        { }
      
      template<typename FUN>
      TransformIter (IT const& src, FUN trafoFunc)
        : _IteratorImpl(_Trafo(src,trafoFunc))
        { }
      
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (TransformIter)
    };
  
  
  
  namespace { // Helper to pick up the produced value type automatically
    
    using lib::meta::FunctionSignature;
    
    template<typename SIG>
    struct _ProducedOutput
      {
        typedef typename FunctionSignature<function<SIG> >::Ret Type;
      };
    
    template<typename SIG>
    struct _ProducedOutput<function<SIG> >
      {
        typedef typename FunctionSignature<function<SIG> >::Ret Type;
      };
    
    template<typename FUN>
    struct _ProducedOutput<FUN*>
      {
        typedef typename FunctionSignature<function<FUN> >::Ret Type;
      };
  }
  
  
  /** Build a TransformIter: convenience free function shortcut,
   *  picking up the involved types automatically.
   *  @param  processingFunc to be invoked for each source element
   *  @return Iterator processing the source feed
   */
  template<class IT, typename FUN>
  inline TransformIter<IT, typename _ProducedOutput<FUN>::Type>
  transformIterator (IT const& src, FUN processingFunc)
  {
    typedef typename _ProducedOutput<FUN>::Type OutVal;
    return TransformIter<IT,OutVal>(src,processingFunc);
  }
  
  
  
  
  /* === utility functions === */
  
  template<class IT, class CON>
  inline void
  append_all (IT iter, CON& container)
  {
    for ( ; iter; ++iter )
      container.push_back (*iter);
  }
  
  
  template<class IT>
  inline typename IT::value_type
  pull_last (IT iter)
    {
      typedef typename IT::value_type Val;
      typedef wrapper::ItemWrapper<Val> Item;
      
      Item lastElm;
      
      while (iter)
        {
          lastElm = *iter;
          ++iter;
        }
      
      if (lastElm)
        return *lastElm;
      else
        throw lumiera::error::State ("attempt to retrieve the last element "
                                     "of an exhausted or empty iterator"
                                    ,lumiera::error::LUMIERA_ERROR_ITER_EXHAUST);
    }
  
  
  
  /** filters away repeated values
   *  emitted by source iterator */
  template<class IT>
  inline FilterIter<IT>
  filterRepetitions (IT const& source)
  {
    typedef typename IT::value_type Val;
    return filterIterator(source, SkipRepetition<Val>() );
  }
  
  
  
  
  
} // namespace lib
#endif
