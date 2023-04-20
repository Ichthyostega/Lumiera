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
 ** as in Java's Collections, where Iterator is an Interface based on
 ** virtual functions. Thus, the basic problem to overcome is the lack
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
 ** std::function object can be passed in as predicate, but of course the
 ** signature must be sensible. Please note, that -- depending on the
 ** predicate -- already the ctor or even a simple `bool` test might
 ** pull and exhaust the source iterator completely, in an attempt
 ** to find the first element passing the predicate test.
 ** 
 ** \par extensible Filter
 ** Based on the FilterIter, this facility allows to elaborate the filter
 ** function while in the middle of iteration. The new augmented filter
 ** will be in effect starting with the current element, which might even
 ** be filtered away now due to a more restrictive condition. However,
 ** since this is still an iterator, any "past" elements are already
 ** extracted and gone and can thus not be subject to changed filtering.
 ** The ExtensibleFilterIter template provides several _builder functions_
 ** to elaborate the initial filter condition, like adding conjunctive or
 ** disjunctive clauses, flip the filter's meaning or even replace it
 ** altogether by a completely different filter function.
 ** 
 ** \par processing Iterator
 ** the TransformIter template can be used as processing (or transforming)
 ** step within the pipeline. It is created with a functor, which, when 
 ** pulling elements, is invoked for each element pulled from the 
 ** source iterator. The signature of the functor must match the
 ** desired value (output) type. 
 ** 
 ** @see iter-adapter.hpp
 ** @see itertools-test.cpp
 ** @see event-log.hpp
 */


#ifndef LIB_ITERTOOLS_H
#define LIB_ITERTOOLS_H


#include "lib/iter-adapter.hpp"
#include "lib/meta/function.hpp"
#include "lib/meta/trait.hpp"
#include "lib/wrapper.hpp"
#include "lib/util.hpp"

#include <functional>
#include <utility>



namespace lib {
  
  using std::forward;
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
      
      IdentityCore (IT&& orig)
        : source_{forward<IT>(orig)}
        { }
      IdentityCore (IT const& orig)
        : source_{orig}
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
   * The protected part provides the _iteration control_
   * building blocks to drive the processing/filter logic,
   * which is implemented in the specific core for each tool.
   */
  template<class CORE>
  class IterTool
    {
      
    protected: /* == iteration control == */
      CORE core_;
      
      bool
      hasData()  const
        {
          return core_.evaluate()
              || unConst(this)->iterate();
        }     // to skip irrelevant results doesn't count as "mutation"
      
      bool
      iterate ()
        {
          if (!core_.pipe()) return false;
          
          do core_.advance();
          while (core_.pipe() && !core_.evaluate());
          
          return bool{core_.pipe()};
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
      
      
      IterTool (CORE&& setup)
        : core_{std::move(setup)}
        {
          hasData();
        }
      
      explicit
      operator bool() const
        {
          return isValid();
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
      using Raw = IdentityCore<IT>;
      using Val = typename IT::reference;
      
      
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
      FilterCore (IT&& source, PRED prediDef)
        : Raw{forward<IT>(source)}
        , predicate_(prediDef) // induces a signature check
        , cached_(false)      //  not yet cached
        , isOK_(false)       //   not yet relevant
        { }
      
      template<typename PRED>
      FilterCore (IT const& source, PRED prediDef)
        : Raw{source}
        , predicate_(prediDef)
        , cached_(false)
        , isOK_(false)
        { }
    };
  
  
  /**
   * Iterator tool filtering pulled data according to a predicate 
   */
  template<class IT>
  class FilterIter
    : public IterTool<FilterCore<IT>>
    {
      typedef FilterCore<IT> _Filter;
      typedef IterTool<_Filter> _Impl;
      
    public:
      static bool acceptAll(typename _Filter::Val) { return true; }
      
      
      FilterIter ()
        : _Impl{FilterCore<IT>(IT(), acceptAll)}
        { }
      
      template<typename PRED>
      FilterIter (IT const& src, PRED filterPredicate)
        : _Impl{_Filter(src, filterPredicate)}
        { }
      
      template<typename PRED>
      FilterIter (IT&& src, PRED filterPredicate)
        : _Impl{_Filter(forward<IT>(src), filterPredicate)}
        { }
      
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (FilterIter)
    };
  
  
  /** Build a FilterIter: convenience free function shortcut,
   *  picking up the involved types automatically.
   *  @param  filterPredicate to be invoked for each source element
   *  @return Iterator filtering contents of the source
   */
  template<class IT, typename PRED>
  inline auto
  filterIterator (IT const& src, PRED filterPredicate)
  {
    return FilterIter<IT>{src, filterPredicate};
  }
  
  template<class IT, typename PRED>
  inline auto
  filterIterator (IT&& src, PRED filterPredicate)
  {
    using SrcIT  = typename std::remove_reference<IT>::type;
    return FilterIter<SrcIT>{forward<IT>(src), filterPredicate};
  }
  
  
  /** 
   * Additional capabilities for FilterIter,
   * allowing to extend the filter condition underway.
   * This wrapper enables remoulding of the filer functor
   * while in the middle of iteration. When the filter is
   * modified, current head of iteration gets re-evaluated
   * and possible fast-forwarded to the next element
   * satisfying the now extended filter condition.
   * @note changing the condition modifies a given iterator in place.
   *       Superficially this might look as if the storage remains
   *       the same, but in fact we're adding a lambda closure,
   *       which the runtime usually allocates on the heap,
   *       holding the previous functor and a second functor
   *       for the added clause.
   * @warning the addition of disjunctive and negated clauses might
   *       actually weaken the filter condition. Yet still there is
   *       _no reset of the source iterator,_ i.e. we don't
   *       re-evaluate from start, but just from current head.
   *       Which means we might miss elements in the already consumed
   *       part of the source sequence, which theoretically would
   *       pass the now altered filter condition.
   * @see IterTools_test::verify_filterExtension
   */
  template<class IT>
  class ExtensibleFilterIter
    : public FilterIter<IT>
    {
      using _Filter = FilterCore<IT>;
      using     Val = typename _Filter::Val;
      
      void
      reEvaluate()
        {
          this->core_.cached_ = false;
          this->hasData(); // re-evaluate head element
        }
      
    public:
      ExtensibleFilterIter() { }
      
      template<typename PRED>
      ExtensibleFilterIter (IT&& src, PRED initialFilterPredicate)
        : FilterIter<IT>{forward<IT>(src), initialFilterPredicate}
        { }
      template<typename PRED>
      ExtensibleFilterIter (IT const& src, PRED initialFilterPredicate)
        : FilterIter<IT>{src, initialFilterPredicate}
        { }
      
      ExtensibleFilterIter (IT&& src)
        : ExtensibleFilterIter{forward<IT>(src), FilterIter<IT>::acceptAll}
        { }
      
      // standard copy operations acceptable
      
      
      /** access the unfiltered source iterator
       *  in current state */
      IT&
      underlying()
        {
          return this->core_.source_;
        }
      
      
      template<typename COND>
      ExtensibleFilterIter&
      andFilter (COND conjunctiveClause)
        {
          function<bool(Val)>& filter = this->core_.predicate_;
          
          filter = [=](Val val)
                      {
                        return filter(val)
                           and conjunctiveClause(val);
                      };
          reEvaluate();
          return *this;
        }
      
      template<typename COND>
      ExtensibleFilterIter&
      andNotFilter (COND conjunctiveClause)
        {
          function<bool(Val)>& filter = this->core_.predicate_;
          
          filter = [=](Val val)
                      {
                        return filter(val)
                           and not conjunctiveClause(val);
                      };
          reEvaluate();
          return *this;
        }
      
      template<typename COND>
      ExtensibleFilterIter&
      orFilter (COND disjunctiveClause)
        {
          function<bool(Val)>& filter = this->core_.predicate_;
          
          filter = [=](Val val)
                      {
                        return filter(val)
                            or disjunctiveClause(val);
                      };
          reEvaluate();
          return *this;
        }
      
      template<typename COND>
      ExtensibleFilterIter&
      orNotFilter (COND disjunctiveClause)
        {
          function<bool(Val)>& filter = this->core_.predicate_;
          
          filter = [=](Val val)
                      {
                        return filter(val)
                            or not disjunctiveClause(val);
                      };
          reEvaluate();
          return *this;
        }
      
      
      template<typename COND>
      ExtensibleFilterIter&
      setNewFilter (COND entirelyDifferentPredicate)
        {
          this->core_.predicate_ = entirelyDifferentPredicate;
          reEvaluate();
          return *this;
        }
      
      ExtensibleFilterIter&
      flipFilter ()
        {
          function<bool(Val)>& filter = this->core_.predicate_;
          
          filter = [=](Val val)
                      {
                        return not filter(val);
                      };
          reEvaluate();
          return *this;
        }
    };
  
  
  
  /**
   * Helper: predicate returning `true`
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
   * Implementation of a _singleton value_ holder,
   * which discards the contained value once "iterated"
   */
  template<typename VAL>
  class SingleValCore
    {
      typedef wrapper::ItemWrapper<VAL> Item;
      
      Item theValue_;
      
    public:
      SingleValCore() { } ///< passive and empty
      
      SingleValCore (VAL&& something)
        : theValue_{forward<VAL> (something)}
        { }
      
      Item const&
      pipe ()  const
        {
          return theValue_;
        }
      
      void
      advance ()
        {
          theValue_.reset();
        }
      
      bool
      evaluate () const
        {
          return theValue_.isValid();
        }
      
      typedef typename std::remove_reference<VAL>::type * pointer;
      typedef typename std::remove_reference<VAL>::type & reference;
      typedef typename std::remove_reference<VAL>::type   value_type;
    };
  
  
  /**
   * Pseudo-Iterator to yield just a single value.
   * When incremented, the value is destroyed and
   * the Iterator transitions to _exhausted state_.
   * @remark as such might look nonsensical, but proves
   *   useful when a function yields an iterator, while
   *   producing an explicit value in some special case.
   * @tparam VAL anything, value or reference to store
   */
  template<class VAL>
  class SingleValIter
    : public IterTool<SingleValCore<VAL>>
    {
      using _ValHolder = SingleValCore<VAL>;
      using _IteratorImpl = IterTool<_ValHolder> ;
      
    public:
      SingleValIter ()
        : _IteratorImpl{_ValHolder{}}
        { }
      
      SingleValIter (VAL&& something)
        : _IteratorImpl{_ValHolder{forward<VAL>(something)}}
        { }
      
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (SingleValIter)
    };
  
  
  
  /** Build a SingleValIter: convenience free function shortcut,
   *  to pick up just any value and wrap it as Lumiera Forward Iterator.
   *  @return Iterator to yield the value once
   *  @warning be sure to understand that we literally pick up and wrap anything
   *           provided as argument. If you pass a reference, we wrap a reference.
   *           If you want to wrap a copy, you have to do the copy yourself inline
   */
  template<class VAL>
  inline auto
  singleValIterator (VAL&& something)
  {
    return SingleValIter<VAL>{forward<VAL>(something)};
  }
  
  template<class VAL>
  inline auto
  singleValIterator (VAL const& ref)
  {
    return SingleValIter<VAL>{ref};
  }
  
  /** not-anything-at-all iterator */
  template<class VAL>
  inline auto
  nilIterator()
  {
    return SingleValIter<VAL>();
  }
  

  
  
  
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
      TransformingCore (IT&& orig, FUN processor)
        : trafo_(processor) // induces a signature check
        , source_(forward<IT> (orig))
        {
          processItem();
        }
      
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
   * @tparam IT source iterator
   * @tparam VAL result (output) type
   */
  template<class IT, class VAL>
  class TransformIter
    : public IterTool<TransformingCore<IT,VAL>>
    {
      using _Trafo = TransformingCore<IT,VAL>;
      using _IteratorImpl = IterTool<_Trafo> ;
      
    public:
      TransformIter ()
        : _IteratorImpl(_Trafo())
        { }
      
      template<typename FUN>
      TransformIter (IT&& src, FUN trafoFunc)
        : _IteratorImpl{_Trafo(forward<IT>(src), trafoFunc)}
        { }
      template<typename FUN>
      TransformIter (IT const& src, FUN trafoFunc)
        : _IteratorImpl{_Trafo(src, trafoFunc)}
        { }
      
      ENABLE_USE_IN_STD_RANGE_FOR_LOOPS (TransformIter)
    };
  
  
  
  
  
  /** Build a TransformIter: convenience free function shortcut,
   *  picking up the involved types automatically.
   *  @tparam processingFunc to be invoked for each source element
   *  @return Iterator processing the source feed
   */
  template<class IT, typename FUN>
  inline auto
  transformIterator (IT const& src, FUN processingFunc)
  {
    using OutVal = typename lib::meta::_Fun<FUN>::Ret;
    return TransformIter<IT,OutVal>{src,processingFunc};
  }
  
  template<class IT, typename FUN>
  inline auto
  transformIterator (IT&& src, FUN processingFunc)
  {
    using SrcIT  = typename std::remove_reference<IT>::type;
    using OutVal = typename lib::meta::_Fun<FUN>::Ret;
    return TransformIter<SrcIT,OutVal>{forward<IT>(src), processingFunc};
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
  inline auto
  filterRepetitions (IT const& source)
  {
    using Val   = typename IT::value_type;
    return filterIterator (source, SkipRepetition<Val>());
  }
  
  template<class IT>
  inline auto
  filterRepetitions (IT&& source)
  {
    using SrcIT = typename std::remove_reference<IT>::type;
    using Val   = typename SrcIT::value_type;
    return filterIterator (forward<IT>(source), SkipRepetition<Val>() );
  }
  
  
  
  
  
} // namespace lib
#endif
